#ifndef SIMPLECONTROLS_NOTIFY_H
#define SIMPLECONTROLS_NOTIFY_H

#include <glib-object.h>
#include <libnotify/notification.h>
#include <libnotify/notify.h>
#include <stdarg.h>
#include <stdio.h>
#include "ctk/types/string.h"
#include "settings.h"

typedef NotifyNotification NotificationState;

inline static NotificationState* notify_new() {
    notify_init("SimpleControls");
    return null;
}

inline static void notify_free(NotificationState* state) {
    if (state != null) {
        g_object_unref(G_OBJECT(state));
    }
    notify_uninit();
}

inline static void notify(NotificationState* state, const CStr* title, const CStr* message) {
    if (state == null) {
        state = notify_notification_new(title->buffer, message->buffer, null);
        notify_notification_set_hint(state, "replace-id", g_variant_new_string(NOTIFY_KEY.buffer));
        notify_notification_set_hint(state, "x-dunst-stack-tag", g_variant_new_string(NOTIFY_KEY.buffer));
    } else {
        notify_notification_update(state, title->buffer, message->buffer, null);
    }

    notify_notification_show(state, null);
}

inline static void notifyf(NotificationState* state, const CStr* title, const Str* message_format, ...) {
    va_list args;
    va_start(args, message_format);
    CString* message = cstring_from_args(message_format, args);
    notify(state, title, cstring_slice(message));
    cstring_free(message);
    va_end(args);
}

#endif
