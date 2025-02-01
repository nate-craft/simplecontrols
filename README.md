# SimpleControls
___
SimpleControls is a minimalist audio and brightness controlling program.
It allows for easy interfacing with PulseAudio, direct control of backlight brightness, and
easy commands for shorcut mapping and simple feedback via system notifications.

Functions include incrementing and decrementing audio/brightness as well as muting input, and muting output.  

SimpleControls relies on PulseAudio as a backend for audio and LibNotify for on an screen display.  

No configuration is provided by design, but values can easily be modified with any editor in `src/settings.h`  

___
## Building

System installation can be used if you want the script installed system-wide.
Local installation will install the executable to `out/simplecontrols`.
  
You can uninstall system installation with `./build.sh --delete`.

```sh
# Debian-Based Dependencies
sudo apt install make cmake libnotify-dev glib-dev dunst light pulseaudio libpulse-dev

# Redhat-Based Depedencies
sudo dnf install make cmake libnotify-devel glib-devel dunst light pulseaudio pulseaudio-libs-devel

# Setup
git clone https://github.com/higgsbi/simplecontrols
cd simplecontrols

# System Installation
./build.sh --release --system

# Local Installation 
./build.sh --release
```

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
