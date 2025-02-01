#ifndef SIMPLECONTROLS_CONSTANTS_H
#define SIMPLECONTROLS_CONSTANTS_H

#include "ctk/types/string.h"

// Settings
#define VOL_MAX 153
#define VOL_DELTA 5
#define BRIGHTNESS_MAX 100
#define BRIGHTNESS_MIN 0.01
#define BRIGHTNESS_DELTA 5
static const Str URI_MAX_BRIGHTNESS = str("/sys/class/backlight/intel_backlight/max_brightness");
static const Str URI_BRIGHTNESS = str("/sys/class/backlight/intel_backlight/brightness");

// Constants
static const i32 INT_BASE = 10;
static const Str NOTIFY_KEY = str("9999");
static const Str MAX_VOL_STR = str(STRINGIFY(VOL_MAX));
static const Str MAX_BRIGHTNESS_STR = str(STRINGIFY(BRIGHTNESS_MAX));
static const Str MIN_BRIGHTNESS_STR = str(STRINGIFY(BRIGHTNESS_MIN));

#endif
