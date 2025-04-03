# X4:Foundnations Station Editor

Alert: Processing modules is a mystery for me so that part is not working.

### How to complie

#### Build requirements

* Qt6(Tested: Qt 6.2.0. Environment variables `QT` and `Path` must be setted on windows)
* OpenSSL
* cmake
* gcc/mingw/visual studio
* doxygen(Optional)


#### Compile

* Code documents can be found in `doc/` and executable files can be found in `bin/`.

Note:
Doxygen not tested. 

##### Windows
```bat
md build
cd build

REM These two lines for debug build. 
cmake ..
cmake --build . 

REM or these two lines for release build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

##### Linux

###### How to Install Dependencies

Note:
I don't use Arch so no way to tell this works or not. 

1. Arch Linux
    1. Qt6
        1. `sudo pacman -S qt6` - not sure this works or not. 
    1. OpenSSL
        1. `sudo pacman -S openssl`
    1. cmake
        1. `sudo pacman -S cmake`
    1. gcc
        1. `sudo pacman -S gcc`
1. Ubuntu
    1. Qt6
        1. `sudo apt install -y build-essential libgl1-mesa-dev`
		1. Follow this doc to install qt: [Get and Install Qt with Qt Online Installer](https://doc.qt.io/qt-6/qt-online-installation.html)
    1. OpenSSL
        1. `sudo apt-get install libssl-dev`
    1. cmake
        1. `sudo apt-get install cmake`
        1. `sudo apt-get install make`

###### Build Commands

1. `mkdir build`
1. `cd build`
1. `cmake -DCMAKE_BUILD_TYPE=Release ..`
1. `cmake --build .`
