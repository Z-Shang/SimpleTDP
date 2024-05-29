# SimpleTDP
A simple and Linux native TDP control tool for mobile Ryzen devices.

Based on: [FlyGoat/RyzenAdj](https://github.com/FlyGoat/RyzenAdj)

## Background
So I got my shiny Ryzen powered handheld, installed a Linux distro on it because why not.

Then I realised that hey I cannot set my TDPs in the "desktop mode" because there's only this [Steam plugin](https://github.com/aarron-lee/SimpleDeckyTDP) 
and my knowledge of Vanilla JS wouldn't help me to get this plugin run as a standalone software.

Time to reinvent some wheels I guess, and the only GUI thing I could do is in C++, so here it is.

## Build
### Prerequisite
You will need libpci, SDL2 and OpenGL3 to build this program. Please refer to your Linux distro's package manager for installing the required libraries.

### Build
```bash
git clone --recursive https://github.com/Z-Shang/SimpleTDP
cd SimpleTDP
make
```

## Run
You would need to run it with root previliges for accessing hardware info.

## Note
I've only tested this on my GPD Win Mini 2024 (with AMD Ryzen 7 8840U) with Bazzite OS installed, if you run into any issue I won't guarantee that I can help.

## LICENSE
GPLv3+
