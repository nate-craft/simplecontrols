#include "audio.h"
#include <pulse/introspect.h>
#include <pulse/mainloop.h>
#include <pulse/pulseaudio.h>
#include <pulse/volume.h>
#include "ctk/io.h"
#include "ctk/types/string.h"
#include "notify.h"
#include "settings.h"

const Str AUDIO_HELP_MSG =
    str("\nInvalid audio command!\n\n"
        "Arguments: \n"
        "\n"
        "  --mute       toggles output mute status\n"
        "  --mute-mic   toggles input mute status\n"
        "  --higher     increments the output volume by 5\n"
        "  --lower      decrements the output volume by 5\n");

typedef enum { AUDIO_INCREMENT, AUDIO_DECREMENT, AUDIO_MUTE_OUTPUT, AUDIO_MUTE_INPUT } AudioFlag;
typedef struct {
    Notifier* state;
    Str* flag_str;
    pa_mainloop* loop;
    CStr* default_sink;
    CStr* default_source;
    bool mute_sink;
    bool mute_source;
    i32 volume_sink;
    i32 channels;
} PulseData;

// Helper Functions
pa_cvolume get_pac_volume(i32 volume, i32 channels);
void modify_volume(Notifier* state, PulseData* data, pa_context* context, AudioFlag flag);
void pulse_free(PulseData* data);
// Callback Function Order
void handle_pulse_state(pa_context* context, void* given_data);
void handle_pulse_server_callback(pa_context* context, const pa_server_info* info, void* userdata);
void handle_pulse_sink_callback(pa_context* context, const pa_sink_info* info, i32 eol, void* userdata);
void handle_pulse_source_callback(pa_context* context, const pa_source_info* info, i32 eol, void* userdata);
void handle_pulse_command(PulseData* data, pa_context* context);
void handle_pulse_operation_callback(pa_context* context, int success, void* userdata);

void control_volume(Notifier* state, Str* flag_str) {
    pa_mainloop* loop = pa_mainloop_new();
    pa_mainloop_api* pulse = pa_mainloop_get_api(loop);
    pa_context* ctx = pa_context_new(pulse, "SimpleControls");

    pa_context_connect(ctx, null, PA_CONTEXT_NOFLAGS, null);
    PulseData data = {state, flag_str, loop, null, null, false, false, -1, 1};
    pa_context_set_state_callback(ctx, handle_pulse_state, &data);

    int ret = 1;
    pa_mainloop_run(loop, &ret);
    pulse_free(&data);
    pa_context_unref(ctx);
    pa_mainloop_free(loop);
}

void handle_pulse_state(pa_context* context, void* given_data) {
    PulseData* data = (PulseData*) given_data;

    switch (pa_context_get_state(context)) {
        case PA_CONTEXT_READY:
            pa_operation_unref(pa_context_get_server_info(context, handle_pulse_server_callback, data));
            break;
        case PA_CONTEXT_FAILED:
            print(&str("PulseAudio connection failed without reason. Please report bug to Github!"));
            pa_mainloop_quit(data->loop, 0);
            return;
        case PA_CONTEXT_TERMINATED:
            pa_mainloop_quit(data->loop, 0);
            return;
        default:
            return;
    }
}

void pulse_free(PulseData* data) {
    if (data->default_sink != null) {
        cstr_free_deep(data->default_sink);
    }
    if (data->default_source != null) {
        cstr_free_deep(data->default_source);
    }
}

void handle_pulse_server_callback(pa_context* context, const pa_server_info* info, void* userdata) {
    PulseData* data = (PulseData*) userdata;
    if (info->default_source_name != null) {
        data->default_source = cstr_owned(info->default_source_name);
    }
    data->default_sink = cstr_owned(info->default_sink_name);

    pa_context_get_sink_info_by_name(context, data->default_sink->buffer, handle_pulse_sink_callback, data);
}

// NOLINTNEXTLINE
void handle_pulse_sink_callback(pa_context* context, const pa_sink_info* info, i32 eol, void* userdata) {
    if (eol > 0) {
        return;
    }

    PulseData* data = (PulseData*) userdata;

    data->mute_sink = info->mute;
    data->volume_sink = (f32) info->volume.values[0] * 100.0 / (f32) PA_VOLUME_NORM;
    data->volume_sink = ((data->volume_sink + 2) / 5) * 5;  // NOLINT

    if (data->default_source != null) {
        pa_operation_unref(pa_context_get_source_info_by_name(context, data->default_source->buffer,
                                                              handle_pulse_source_callback, data));
    } else {
        handle_pulse_command(data, context);
    }
}

// NOLINTNEXTLINE
void handle_pulse_source_callback(pa_context* context, const pa_source_info* info, i32 eol, void* userdata) {
    PulseData* data = (PulseData*) userdata;
    if (eol > 0) {
        return;
    }
    data->mute_source = info->mute;
    handle_pulse_command(data, context);
}

void handle_pulse_command(PulseData* data, pa_context* context) {
    if (str_equals_str(data->flag_str, &str("--higher"))) {
        modify_volume(data->state, data, context, AUDIO_INCREMENT);
    } else if (str_equals_str(data->flag_str, &str("--lower"))) {
        modify_volume(data->state, data, context, AUDIO_DECREMENT);
    } else if (str_equals_str(data->flag_str, &str("--mute"))) {
        if (data->mute_sink) {
            pa_operation_unref(pa_context_set_sink_mute_by_name(context, data->default_sink->buffer, 0,
                                                                handle_pulse_operation_callback, null));
            notifyf(data->state, &cstr("Volume"), &str("%d%%"), data->volume_sink);
        } else {
            pa_operation_unref(pa_context_set_sink_mute_by_name(context, data->default_sink->buffer, 1,
                                                                handle_pulse_operation_callback, null));
            notify(data->state, &cstr("Volume"), &cstr("Muted"));
        }
    } else if (str_equals_str(data->flag_str, &str("--mute-mic"))) {
        if (data->mute_source && data->default_source != null) {
            pa_operation_unref(pa_context_set_source_mute_by_name(context, data->default_source->buffer, 0,
                                                                  handle_pulse_operation_callback, null));
            notify(data->state, &cstr("Microphone"), &cstr("Enabled"));
        } else if (data->default_source != null) {
            pa_operation_unref(pa_context_set_source_mute_by_name(context, data->default_source->buffer, 1,
                                                                  handle_pulse_operation_callback, null));
            notify(data->state, &cstr("Microphone"), &cstr("Disabled"));
        } else {
            print(&str("%s\n"), &str("No audio input device could be found!"));
            pa_context_disconnect(context);
        }
    } else {
        print(&str("%s\n"), &AUDIO_HELP_MSG);
        pa_context_disconnect(context);
    }
}

// NOLINTNEXTLINE
void handle_pulse_operation_callback(pa_context* context, int success, void* userdata) {
    if (!success) {
        print(&str("Could not run PulseAudio operation. Please submit bug to Github!\n"));
    }

    pa_context_disconnect(context);
}

void modify_volume(Notifier* state, PulseData* data, pa_context* context, AudioFlag flag) {
    i32 volume = data->volume_sink;
    bool muted = data->mute_sink;

    if (muted) {
        notifyf(state, &cstr("Volume"), &str("%d%%"), volume);
        pa_context_disconnect(context);
        return;
    }

    i32 volume_new;

    if (flag == AUDIO_INCREMENT) {
        if (volume + VOL_DELTA > VOL_MAX) {
            volume_new = VOL_MAX;
        } else {
            volume_new = volume + VOL_DELTA;
        }
    } else {
        if (volume - VOL_DELTA < VOL_DELTA) {
            volume_new = 0;
        } else {
            volume_new = volume - VOL_DELTA;
        }
    }

    pa_cvolume pac_volume = get_pac_volume(volume_new, data->channels);
    if (data->default_sink->buffer != null) {
        notifyf(state, &cstr("Volume"), &str("%d%%"), volume_new);
        pa_operation_unref(pa_context_set_sink_volume_by_name(context, data->default_sink->buffer, &pac_volume,
                                                              handle_pulse_operation_callback, data));
    }
}

pa_cvolume get_pac_volume(i32 volume, i32 channels) {
    pa_volume_t pa_volume = (pa_volume_t) (PA_VOLUME_NORM * volume / 100.0);
    pa_cvolume pac_volume;
    pa_cvolume_set(&pac_volume, channels, pa_volume);

    return pac_volume;
}
