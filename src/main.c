#include "audio.h"
#include "brightness.h"
#include "ctk/io.h"
#include "ctk/types/string.h"
#include "notify.h"

typedef enum { BRIGHTNESS_INCREMENT, BRIGHTNESS_DECREMENT } BrightnessFlag;

const Str HELP_MSG =
    str("\nInvalid command!\n\n"
        "simplecontrols brightness \n"
        "\n"
        "  --higher     increments the brightness by 5%\n"
        "  --lower      decrements the brightness by 5%\n"
        "\n"
        "simplecontrols audio \n"
        "\n"
        "  --mute       toggles output mute status\n"
        "  --mute-mic   toggles input mute status\n"
        "  --higher     increments the output volume by 5\n"
        "  --lower      decrements the output volume by 5\n"
        "\n"
        "  --silent     does not notify changes");

int main(i32 argc, c8** argv) {
    if (argc < 3) {
        print(&str("%s\n"), &HELP_MSG);
        return 1;
    }

    Str type_str = str_from_chars(argv[1]);
    Str flag_str = str_from_chars(argv[2]);
    bool silent = argc >= 4 && str_equals_chars(&str("--silent"), (const u8*) argv[3]);
    Notifier state = notify_new(!silent);

    if (str_equals_str(&type_str, &str("audio"))) {
        control_volume(&state, &flag_str);
    } else if (str_equals_str(&type_str, &str("brightness"))) {
        control_brightness(&state, &flag_str);
    } else {
        print(&str("%s\n"), &HELP_MSG);
    }

    notify_free(&state);

    return 0;
}
