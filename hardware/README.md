# CMake Build System for Hardware Logic (C++) - Lego Sorter

## How do I use this? How do I write my C++ code and get it to build an executable file?

Easy. All you need to do is the following steps (in order):

1. Go to the `hardware` folder in this repo, this is where ALL the hardware code lives, do NOT go into the `frontend` folder unless you are Dylan or Lachlan.
2. Place your C++ `.cpp` files (these are actually called compilation units by the way) into the `hardware/src` folder or once you are in hardware, create a NEW folder called `src`. This is where all your C++ files (or compilation units) live.
3. Once you have written some code, I have written scripts to build the rest for you. If you are on Windows, go to PowerShell and run the script: `build_win.ps1` to build the executable. If you are on macOS/Linux - run `build_unix.sh` from a Terminal.
4. Then, the executable (if you are on Windows) will be under the build folder (may even be there for macOS/Linux also).
5. Have fun.