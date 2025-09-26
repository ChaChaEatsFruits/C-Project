# Prerequisites
## 1. wsl-open or opening the saved image from wsl ubuntu with the windows photo app
### Run all these commands being at the root
`mkdir -p ~/bin`
`echo '[[ -e ~/bin ]] && export PATH=$PATH:~/bin' >> ~/.bashrc`
`curl -o ~/bin/wsl-open https://raw.githubusercontent.com/4U6U57/wsl-open/master/wsl-open.sh`
`source ~/.bashrc`

## 3. gtk-4 installed in ubuntu
`sudo apt-get install libgtk-3-dev
`
## 4. updated gtk-4 main.c file 
## 5. XLaunch is required to run the GUI alongside the windows

## Compile Command: 

gcc main.c helpers.c -o image_editor `pkg-config --cflags --libs gtk4` -lm

## Run command:

 `./image_filter`
