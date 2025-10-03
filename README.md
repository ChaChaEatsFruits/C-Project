# Prerequisites
## 1. wsl-open or opening the saved image from wsl ubuntu with the windows photo app
### Run all these commands being at the root
`mkdir -p ~/bin` <br>
`echo '[[ -e ~/bin ]] && export PATH=$PATH:~/bin' >> ~/.bashrc`<br>
`curl -o ~/bin/wsl-open https://raw.githubusercontent.com/4U6U57/wsl-open/master/wsl-open.sh`<br>
`source ~/.bashrc`<br>

## 3. gtk-4 installation in ubuntu
`sudo apt-get install libgtk-4-dev
`
## gtk-4 installation in Mac OS
`
brew install gtk4 pkg-config
`
## 4. updated gtk-4 main.c file 
## 5. XLaunch is required to run the GUI alongside the windows
Link for the XLaunch[https://sourceforge.net/projects/vcxsrv/]

## Compile Command: 

gcc main.c helpers.c -o image_editor `pkg-config --cflags --libs gtk4` -lm

## Run command:

 `./image_filter`

 ## To see the ouput image two ways can be followed
 The repo has to be cloned in the Ubuntu 
 1. In windows system :<br>
    Go to <b>File Explorer</b>, click on the address bar and then type `\\wsl$` and hit <b>Enter</b><br>
    Click on the <b>Ubuntu-24.04</b> -> <b>home</b> -> <b><Your username></b> -> <b>Ubuntu-24.04</b> <br>
    Choose the Project Folder and then choose the output image.
 2. In Ubuntu System:<br>
    Install <b>wsl-open</b> using the above procedure and then use the command `wsl-open` filtered-image.png
