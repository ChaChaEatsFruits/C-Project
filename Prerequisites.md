## Prerequisites
1. wsl-open or opening the saved image from wsl ubuntu with the windows photo app
2. gtk-4 installed in ubuntu
3. updated gtk-4 main.c file 
4. XLaunch is required to run the GUI alongside the windows

Compile Command: 

gcc main.c helpers.c -o image_editor `pkg-config --cflags --libs gtk4` -lm

Run command:

 ./image_filter
