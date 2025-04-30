#ifndef SIMPLECONTROLS_NOTIFY_H
#define SIMPLECONTROLS_NOTIFY_H

#include <glib-object.h>
#include <libnotify/notification.h>
#include <libnotify/notify.h>
#include <stdarg.h>
#include <stdio.h>
#include "ctk/types/string.h"
#include "settings.h"

typedef struct {
    NotifyNotification* state;
    bool enabled;
} Notifier;

inline static Notifier notify_new(bool enabled) {
    notify_init("SimpleControls");
    return (Notifier) {.state = null, .enabled = enabled};
}

inline static void notify_free(Notifier* notifier) {
    if (!notifier->enabled) {
        return;
    }

    if (notifier->state != null) {
        g_object_unref(G_OBJECT(notifier->state));
    }
    notify_uninit();
}

inline static void notify(Notifier* notifier, const CStr* title, const CStr* message) {
    if (!notifier->enabled) {
        return;
    }

    if (notifier->state == null) {
        notifier->state = notify_notification_new(title->buffer, message->buffer, null);
        notify_notification_set_hint(notifier->state, "replace-id", g_variant_new_string(NOTIFY_KEY.buffer));
        notify_notification_set_hint(notifier->state, "x-dunst-stack-tag", g_variant_new_string(NOTIFY_KEY.buffer));
    } else {
        notify_notification_update(notifier->state, title->buffer, message->buffer, null);
    }

    notify_notification_show(notifier->state, null);
}

inline static void notifyf(Notifier* notifier, const CStr* title, const Str* message_format, ...) {
    if (!notifier->enabled) {
        return;
    }

    va_list args;
    va_start(args, message_format);
    CString* message = cstring_from_args(message_format, args);
    notify(notifier, title, cstring_slice(message));
    cstring_free(message);
    va_end(args);
}

#endif
