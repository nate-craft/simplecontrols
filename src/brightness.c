#include "brightness.h"
#include "ctk/file/file.h"
#include "ctk/file/path.h"
#include "ctk/io.h"
#include "ctk/types/string.h"
#include "notify.h"
#include "settings.h"

const Str BRIGHTNESS_HELP_MSG =
    str("\nInvalid brightness command!\n\n"
        "Arguments: \n"
        "\n"
        "  --higher     increments the brightness by 5%\n"
        "  --lower      decrements the brightness by 5\n");

i32 get_brightness_reading(const Str* path_uri);
i32 get_brightness(i32 max_brightness);
i32 get_raw_max_brightness();
void set_brightness(f32 brightness_precise, i32 max_brightness);

void control_brightness(NotificationState* state, Str* flag_str) {
    i32 max_brightness = get_raw_max_brightness();
    i32 brightness = get_brightness(max_brightness);

    if (str_equals_str(flag_str, &str("--higher"))) {
        if (brightness + BRIGHTNESS_DELTA > BRIGHTNESS_MAX) {
            set_brightness(max_brightness, max_brightness);
            notifyf(state, &cstr("Brightness"), &str("%d%%"), BRIGHTNESS_MAX);
        } else {
            set_brightness(brightness + BRIGHTNESS_DELTA, max_brightness);
            notifyf(state, &cstr("Brightness"), &str("%d%%"), brightness + BRIGHTNESS_DELTA);
        }
    } else if (str_equals_str(flag_str, &str("--lower"))) {
        if (brightness - BRIGHTNESS_DELTA < (BRIGHTNESS_DELTA + 1)) {
            set_brightness(BRIGHTNESS_MIN, max_brightness);
            notify(state, &cstr("Brightness"), &cstr("Minimum"));
        } else {
            set_brightness(brightness - BRIGHTNESS_DELTA, max_brightness);
            notifyf(state, &cstr("Brightness"), &str("%d%%"), brightness - BRIGHTNESS_DELTA);
        }
    } else {
        print(&str("%s\n"), &BRIGHTNESS_HELP_MSG);
    }
}

i32 get_brightness_reading(const Str* path_uri) {
    Path* path = path_from_str(path_uri);
    File file = file_from_path(path);
    String* buffer = string_empty();

    file_open(&file, FILE_READ);
    file_read_line(&file, buffer);

    buffer->buffer[buffer->length] = '\0';
    i32 as_int = strtol(buffer->buffer, null, INT_BASE);

    file_close(&file);
    string_free(buffer);

    return as_int;
}

i32 get_raw_max_brightness() {
    return get_brightness_reading(&URI_MAX_BRIGHTNESS);
}

i32 get_brightness(i32 max_brightness) {
    i32 brightness_raw = get_brightness_reading(&URI_BRIGHTNESS);
    i32 translated = ((f32) brightness_raw) / ((f32) max_brightness) * 100.0;
    return ((translated + 2) / 5) * 5;  // NOLINT
}

void set_brightness(f32 brightness, i32 max_brightness) {
    i32 raw = brightness / 100.0 * ((f32) max_brightness);
    Path* path = path_from_str(&URI_BRIGHTNESS);
    File file = file_from_path(path);

    file_open(&file, FILE_OVERWRITE);
    file_write(&file, &str("%d"), raw);
    file_close(&file);
}
