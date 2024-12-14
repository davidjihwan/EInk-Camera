# EInk Camera
A camera that uses an eInk display, using the Adafruit Grand Central M4.
![Image of the eink camera](Media/Images/ProjectPic.png)

## Summary
This project is an eInk camera. The camera takes a picture of the user when they click a button, saves it to an SD card, then flashes it onto an eInk display. 
More of the design details and process can be found in the writeup I did on [my website](https://davidjihwan.com/TODO)

## Media
Thresholding: <br />
![Gif of the camera's thresholding function](Media/Gifs/threshold.gif)

Dithering (default): <br />
![Gif of the camera's dithering function](Media/Gifs/dither1.gif)

The full interaction (dithering): <br />
![Gif of the full camera interaction](Media/Gifs/fullInteraction.gif)

Closeup: <br />
![Gif of the full camera interaction](Media/Gifs/eInkCloseup.gif)

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

Eink setup:
1. Solder the pins that came with the ESP32 onto the board.
2. Insert the ESP-32 into the breadboard following the wiring diagram below.
3. Connect the pins following the wiring diagram below. Take note that the jumper wires connecting the joystick to the breadboard should be male to female wires.

Here's the wiring diagram for this project. 
![Image of the Fritzing wiring diagram](Media/Images/EInkFritzing.png)

Here's what your wiring should look like:
![Image of the breadboard with wiring, no camera](Media/Images/EInkWiring.png)
![Image of the breadboard with wiring](Media/Images/WiringFull.png)

Project Installation and Setup:
1. Download the Arduino project file to write to serial output [here](Arduino/Eink_Camera/Eink_Camera.ino).
3. Open the project file.
4. Change your board: Tools > Boards > Adafruit SAMD Boards > Adafruit Grand Central M4
5. Change the port: Tools > Port > your-port (mac users should select “wchusb")
6. Plug the Grand Central into your computer.
7. Press the upload button (at the top left of the window) to upload your code to the board. 
8. That's it! After you click the button, the image should be flashed onto the eink display. 

Optional Enclosure:
<a href="url"><img src="Media/Images/enclosure_model.png" width="400">
1. Download the files for the joystick enclosure [TODO: here]
2. 3D print the top and bottom sections of the enclosure using your printer of choice. 
3. Insert the head inserts into the designated locations on the bottom section of the enclosure. [TODO: this] is a good tutorial for heat inserts.
8. Screw the sections together using the 3mm by 7?mm [TODO] screws. Place the cap back onto the joystick.
![Picture of the assembled enclosure](Media/Images/enclosure_assembled.png)
