#include "audio.h"
#include "brightness.h"
#include "ctk/io.h"
#include "ctk/types/string.h"

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
        "  --lower      decrements the output volume by 5\n");

int main(i32 argc, c8** argv) {
    if (argc < 3) {
        print(&str("%s\n"), &HELP_MSG);
        return 1;
    }

    Str type_str = str_from_chars(argv[1]);
    Str flag_str = str_from_chars(argv[2]);

    if (str_equals_str(&type_str, &str("audio"))) {
        control_volume(&flag_str);
    } else if (str_equals_str(&type_str, &str("brightness"))) {
        control_brightness(&flag_str);
    } else {
        print(&str("%s\n"), &HELP_MSG);
    }

    return 0;
}
