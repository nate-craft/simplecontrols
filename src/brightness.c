#include "brightness.h"
#include "ctk/io.h"
#include "settings.h"

const Str BRIGHTNESS_HELP_MSG =
    str("\nInvalid brightness command!\n\n"
        "Arguments: \n"
        "\n"
        "  --higher     increments the brightness by 5%\n"
        "  --lower      decrements the brightness by 5\n");

const CStr CMD_GET_BRIGHTNESS = cstr("light -G");
const CStr CMD_NOTIFY_BRIGHTNESS_MIN = cstr("dunstify -r 9999 \"Brightness\" \"Minimum\"");
const Str CMD_FMT_INCREMENT_BRIGHTNESS = str("light -A %d");
const Str CMD_FMT_DECREMENT_BRIGHTNESS = str("light -U %d");
const Str CMD_FMT_SET_BRIGHTNESS = str("light -S %s");
const Str CMD_FMT_NOTIFY_BRIGHTNESS = str("dunstify -r 9999 \"Brightness\" \"%d%%\"");

i32 get_brightness();

void control_brightness(Str* flag_str) {
    i32 brightness = get_brightness();

    if (str_equals_str(flag_str, &str("--higher"))) {
        if (brightness + BRIGHTNESS_DELTA > MAX_BRIGHTNESS) {
            command_runf(&CMD_FMT_SET_BRIGHTNESS, &MAX_BRIGHTNESS_STR);
            command_runf(&CMD_FMT_NOTIFY_BRIGHTNESS, MAX_BRIGHTNESS);
        } else {
            command_runf(&CMD_FMT_INCREMENT_BRIGHTNESS, BRIGHTNESS_DELTA);
            command_runf(&CMD_FMT_NOTIFY_BRIGHTNESS, brightness + BRIGHTNESS_DELTA);
        }
    } else if (str_equals_str(flag_str, &str("--lower"))) {
        if (brightness - BRIGHTNESS_DELTA < (BRIGHTNESS_DELTA - 1)) {
            command_runf(&CMD_FMT_SET_BRIGHTNESS, &MIN_BRIGHTNESS_STR);
            command_run(&CMD_NOTIFY_BRIGHTNESS_MIN);
        } else {
            command_runf(&CMD_FMT_DECREMENT_BRIGHTNESS, BRIGHTNESS_DELTA);
            command_runf(&CMD_FMT_NOTIFY_BRIGHTNESS, brightness - BRIGHTNESS_DELTA);
        }
    } else {
        print(&str("%s\n"), &BRIGHTNESS_HELP_MSG);
    }
}

i32 get_brightness() {
    Process* process = process_start(PROCESS_READ, &CMD_GET_BRIGHTNESS);
    c8 character;
    usize number_index = 0;
    char number[4] = "0\0\0\0";

    while ((character = fgetc(process)) && number_index < MAX_BRIGHTNESS_STR.length && character != '.') {
        if (character >= '0' && character <= '9') {
            number[number_index] = character;
            number_index++;
        } else {
            break;
        }
    }

    process_end(process);
    return strtol(number, null, INT_BASE);
}
