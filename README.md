# EInk Camera
A camera that uses an eInk display, using the Adafruit Grand Central M4.
![Image of the eink camera](Media/Images/FinalBuild.png)

## Summary
This project is an eInk camera. The camera takes a picture of the user when they click a button, saves it to an SD card, then flashes it onto an eInk display. 
More of the design details and process can be found in the writeup I did on [my website](https://davidjihwan.com/TODO)

## Media
Thresholding:
![Gif of the camera's thresholding function](Media/Gifs/threshold.gif)

Dithering (default):
![Gif of the camera's dithering function](Media/Gifs/dither1.gif)

The full interaction (dithering):
![Gif of the full camera interaction](Media/Gifs/dither2.gif)

## Materials
- [Arduino IDE](https://www.arduino.cc/en/software)
- [Adafruit Grand Central M4 Express](http://www.adafruit.com/product/4064#tutorials)
- [Adafruit 1.54" Monochrome eInk / ePaper Display with SRAM](http://www.adafruit.com/product/4196#tutorials)
- [Adafruit EYESPI Breakout Board](http://www.adafruit.com/product/5613#tutorials)
- [EYESPI Cable - 18 Pin 100mm long Flex PCB (FPC) A-B type](https://www.adafruit.com/product/5239#tutorials)
- Micro USB cable
- OV7670 Camera Module
- 2x: 2.2k resistors
- Breadboard
- M-F jumper wires
- M-M jumper wires
- Wires
- Pushbutton

This project also requires soldering.

## Quick Start Guide
Camera Setup:

![Image of the camera in the Grand Central](Media/Images/CameraAttached.png)

EInk Setup: 


Wiring:
1. Solder the pins that came with the ESP32 onto the board.
2. Insert the ESP-32 into the breadboard following the wiring diagram below.
3. Connect the pins following the wiring diagram below. Take note that the jumper wires connecting the joystick to the breadboard should be male to female wires.

Here's the wiring diagram for this project. 
![Image of the Fritzing wiring diagram](Media/Images/EInkFritzing.png)

Here's what your breadboard should look like:
![Image of the breadboard with wiring, no camera](Media/Images/EInkWiring.png)
![Image of the breadboard with wiring](Media/Images/EInkWiring.png)

General Installations:
1. Download the [Arduino IDE](https://www.arduino.cc/en/software) on your computer of choice.
2. Install the "TFT_eSPI" library by Bodmer and the "Adafruit GFX" library by Adafruit in the Arduino IDE using the library manager (Tools/Manage Libraries).
3. Open the file Arduino/libraries/TFT_eSPI/User_Setup_Select.h
4. Comment out the line #include <User_setup.h> and uncomment out the line include <User_Setups/Setup25_TTGO_T_Display.h>
5. Install the relevant [ESP32 Drivers](https://github.com/Xinyuan-LilyGO/TTGO-T-Display) (CH9102).

Project Installation and Setup:
1. Download the Arduino project file to write to serial output [here](Arduino/Cat_Toy_Serial/Cat_Toy_Serial.ino).
2. Open the project file.
3. Change your board: Tools > Boards > esp32 > ESP32 Dev Module
4. Change the port: Tools > Port > your-port (mac users should select “wchusb")
5. Plug the ESP32 into your computer.
6. Press the upload button (at the top left of the window) to upload your code to the ESP32. 
7. That's it! You should see the running Processing sketch, with the ESP32 now taking in inputs from the joystick and button reflected in the sketch. 

Optional Joystick Enclosure:
<a href="url"><img src="Media/Images/enclosure_model.png" width="400">
1. Download the files for the joystick enclosure [TODO: here]
2. 3D print the top and bottom sections of the enclosure using your printer of choice. 
3. Insert the head inserts into the designated locations on the bottom section of the enclosure. [TODO: this] is a good tutorial for heat inserts.
8. Screw the sections together using the 3mm by 7?mm [TODO] screws. Place the cap back onto the joystick.
![Picture of the assembled enclosure](Media/Images/enclosure_assembled.png)

## Credits
-https://www.google.com/url?sa=i&url=https%3A%2F%2Fca.pinterest.com%2Fpin%2Forange-tabby-kitten-cute-cat-transparent-background-png-clipart--364369426114234340%2F&psig=AOvVaw0wjowez13AuZsIqCFCP_U9&ust=1729562625831000&source=images&cd=vfe&opi=89978449&ved=0CBcQjhxqFwoTCJjgp4OxnokDFQAAAAAdAAAAABBW
- https://www.google.com/url?sa=i&url=https%3A%2F%2Fcommons.wikimedia.org%2Fwiki%2FFile%3AExotic_cat_transparent.png&psig=AOvVaw0wjowez13AuZsIqCFCP_U9&ust=1729562625831000&source=images&cd=vfe&opi=89978449&ved=0CBcQjhxqFwoTCJjgp4OxnokDFQAAAAAdAAAAABBk
- https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.deviantart.com%2Fanavrin-ai%2Fart%2FCat-Kitten-Isolated-On-Transparent-Background-13-965728152&psig=AOvVaw0wjowez13AuZsIqCFCP_U9&ust=1729562625831000&source=images&cd=vfe&opi=89978449&ved=0CBcQjhxqFwoTCJjgp4OxnokDFQAAAAAdAAAAABBv
