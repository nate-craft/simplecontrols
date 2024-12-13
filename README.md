# SimpleControls
___

SimpleControls is a minimalist audio and brightness controlling program.
It allows for easy interfacing with libraries like PulseAudio, Light, and LibNotify for
easy commands for shorcut mapping and simple feedback via notifications.

Functions include incrementing and decrementing audio/brightness as well as muting input, and muting output.  

SimpleControls relies on PulseAudio and Light as backends and LibNotify for on an screen display.  

No configuration is provided by design, but values can easily be changed in `src/settings.h`  

___
## Building
  
To build to a local directory (great for non-root users), use `./build.sh --release`.  
The executable file will be `out/simplecontrols`.  
  
To build and install system wide (simpler, requires root user), use `./build.sh --release --system`  
The executable can then be accessed simply with `simplecontrols`  

Uninstall system wide install with `./build.sh --delete`

___
## Dependencies

### Runtime

- [PulseAudio](https://www.freedesktop.org/wiki/Software/PulseAudio/)
- [LibNotify](https://github.com/GNOME/libnotify)
- [Light](https://github.com/klaxalk/light/tree/master)
- Any notification daemon  

### Building

- [CTK (included in build script)](https://github.com/higgsbi/ctk)
- [CMake (> 3.0)](https://cmake.org/)

___
## Commands

<pre>
simplecontrols audio

--mute       toggles output mute status  
--mute-mic   toggles input mute status  
--higher     increments current output volume by 5  
--lower      decrements current output volume by 5  
  
simplecontrols brightness  
  
--higher     increments the brightness by 5%  
--lower      decrements the brightness by 5%  
</pre>


## Sample Sway/i3 Configuration

<pre>
# Replace YOUR_PATH with your preferred location, or delete if system install was used
bindsym XF86AudioRaiseVolume exec "YOUR_PATH/simplecontrols audio --higher"
bindsym XF86AudioLowerVolume exec "YOUR_PATH/simplecontrols audio --lower"
bindsym XF86AudioMute exec "YOUR_PATH/simplecontrols audio --mute"
bindsym XF86AudioMicMute exec "YOUR_PATH/simplecontrols audio --mute-mic"
bindsym XF86MonBrightnessUp exec "YOUR_PATH/simplecontrols brightness --higher"
bindsym XF86MonBrightnessDown exec "YOUR_PATH/simplecontrols brightness --lower"
</pre>
