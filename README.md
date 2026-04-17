# **audio-wave**

Simple C++ audio player to play music

---

## **Key Features**
- You can use it as desktop player to play music
- Supports .wav and .mp3
- Repeat and Shuffle modes
- Reading id3 tags from .mp3

---

## **Usage**

**!This project uses QT, so you need it to build and run the project**

### Setup:
- Download this project via git `git clone https://github.com/VadiksMoniks/audio-wave` or just by copying files.
- Then go to https://github.com/lieff/minimp3 and download `minimp3_ex.h` and `minimp3.h`
- You also need to download **SDL2** library
- After instaling SDL2 change `CMake.txt` on line 15. Enter your path to this lib
- After all you can build the project
- **Now go inside`build` folder and look for `music` folder. Add here music files to listhen to them**

## **How it looks like**
![Screenshot](screen1.png)