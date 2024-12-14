/*
Author: David Jihwan
TODO: github
TODO: website

Takes a picture of the user when they click a button, saves it to an SD card, then flashes it onto an eInk display. 

HARDWARE REQUIRED:
- Adafruit Grand Central board
- OV7670 camera w/2.2K pullups to SDA+SCL
- Adafruit ThinkInk display
- SD Card

This project draws from example code written by Adafruit: 
- Examples/Adafruit ImageReader Library/ThinkInkDisplays: 
https://github.com/adafruit/Adafruit_ImageReader/blob/master/examples/ThinkInkDisplays/ThinkInkDisplays.ino
- Examples/Adafruit OV7670/selfie: 
https://github.com/adafruit/Adafruit_OV7670/blob/master/examples/selfie/selfie.ino

I also referenced Phillip Burgess' guide "Adafruit OV7670 Camera Library For SAMD51 Processors" 
in order to modify my OV7670 camera to interface with the Grand Central: 
https://learn.adafruit.com/adafruit-ov7670-camera-library-samd51/hardware  

*/

#include <Adafruit_GFX.h>             // Core graphics library
#include "Adafruit_ThinkInk.h"        // ThinkInk library
#include <Wire.h>                     // I2C comm to camera
#include "Adafruit_OV7670.h"          // Camera library
#include <SdFat.h>                    // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>        // SPI / QSPI flash library
#include <Adafruit_ImageReader_EPD.h> // Image-reading functions

// SD CONFIG ----------------------------------------------------------
// #define SD_CS SDCARD_SS_PIN       // Grand Central onboard SD card select
#define SD_CS 5                      // SD card chip select (on ThinkInk Display)
SdFat                    SD;         // SD card filesystem
Adafruit_ImageReader_EPD reader(SD); // Image-reader object, pass in SD filesys

// CAMERA CONFIG -----------------------------------------------------------

// Set up arch and pins structures for Grand Central's SAMD51.
// PCC data pins are not configurable and do not need specifying.
OV7670_arch arch = {.timer = TCC1, .xclk_pdec = false};
OV7670_pins pins = {.enable = PIN_PCC_D8, .reset = PIN_PCC_D9,
                    .xclk = PIN_PCC_XCLK};
#define CAM_I2C  Wire1 // Second I2C bus next to PCC pins
#define CAM_SIZE OV7670_SIZE_DIV2 // QVGA (320x240 pixels)
#define CAM_MODE OV7670_COLOR_YUV // We need to process greyscale data
Adafruit_OV7670 cam(OV7670_ADDR, &pins, &CAM_I2C, &arch);

// EINK DISPLAY CONFIG -----------------------------------------------------------
#define EPD_DC      10
#define EPD_CS      9
#define EPD_BUSY    7    // can set to -1 to not use a pin (will wait a fixed delay)
#define SRAM_CS     6
#define EPD_RESET   -1   // can set to -1 and share with microcontroller Reset!
#define EPD_SPI     &SPI // primary SPI
// 1.54" Monochrome displays with 200x200 pixels and SSD1681 chipset
// ThinkInk_154_Mono_D67 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);
ThinkInk_154_Mono_D67 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

// BUTTON CONFIG -----------------------------------------------------------
#define BUTTON_PIN 2

// SETUP - RUNS ONCE ON STARTUP --------------------------------------------
void setup() {
  pinMode(13, OUTPUT);  // Onboard red LED
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pullup resistor
  Serial.begin(9600);
  Serial.println("Began!");

  // Initialize SD & file system
  if(!SD.begin(SD_CS, SD_SCK_MHZ(10))) { // Breakouts require 10 MHz limit due to longer wires
    Serial.println(F("SD begin() failed"));
    for(;;); // Fatal error, do not continue
  }

  // Once started, the camera continually fills a frame buffer using DMA.
  OV7670_status status = cam.begin(CAM_MODE, CAM_SIZE, 30.0, 320 * 240 * 2);
  if (status != OV7670_STATUS_OK) {
    Serial.println("Camera begin() fail");
    for(;;); // Fatal error, do not continue
  }
  // We don't want DMA into the camera buffer for this project, as
  // the camera should only be active when the button is pressed.
  cam.suspend();
  display.begin();
  display.setRotation(2);
  display.clearBuffer();
}

// MAIN LOOP - RUNS REPEATEDLY UNTIL RESET OR POWER OFF --------------------
void loop() {
  delay(40);  // Check button press every 40ms
  int buttonState = digitalRead(BUTTON_PIN);
  if(!buttonState) {
    digitalWrite(13, HIGH);   // turn the LED on

    // ********** HANDLE CAMERA **********
    Serial.println("Taking picture.");
    char filename[50];
    sprintf(filename, "selfie.bmp");
    // delay(500);      // Small delay for user to get ready
    cam.capture();      // Manual (non-DMA) capture
    bool *correspondence = (bool *)malloc(cam.height() * cam.width() * sizeof(bool));   // Remember to free!!!
    if (correspondence == NULL) {
      Serial.println("Correspondence malloc error");
      for(;;); // Fatal error, do not continue
    }
    dither(cam.getBuffer(), cam.width(), cam.height(), correspondence);
    write_bmp_correspond(filename, correspondence, cam.width(), cam.height(), 
                       display.height(), display.width());
    free(correspondence);

    // ********** HANDLE SCREEN **********
    // Load full-screen BMP file 'selfie.bmp' at position (0,0) (top left).
    // The 'reader' object performs this, with 'epd' as an argument.
    Serial.print(F("Loading bmp to canvas..."));
    ImageReturnCode stat; // Status from image-reading functions
    stat = reader.drawBMP((char *)"selfie.bmp", display, 0, 0);
    reader.printStatus(stat); // How'd we do?
    display.display();  
    display.clearBuffer();
    digitalWrite(13, LOW);  // Turn the LED off
    delay(5000);            // Adafruit recommends a max refresh rate of 180 seconds. If you care
                            // about the longevity of your screen, increase the delay. 
  }
}

// Perform Floyd-Steinberg dithering on the image and fill in the correspondence array with the 
// results (0: black, 1: white). 
// This changes the values in the camera buffer in order to avoid using more RAM. Use with caution.
void dither(uint16_t *addr, uint16_t width, uint16_t height, bool *correspondence){
  // Serial.println("Dithering.");
  if(CAM_MODE != OV7670_COLOR_YUV) {
    Serial.println("Camera mode not YUV (continuing)!");
  }
  // Floyd-Steinberg dithering
  // https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering
  for(int y=0; y<height; y++) {
    uint16_t *row = &addr[y * width];
    // uint8_t *row = &addr[y * width];
    for(int x=0;x <width; x++) { 
      // The low-byte of YUV pixels holds the brightness information.
      // Raw data from camera is big-endian, so the second byte will hold brightness.
      uint16_t be_pixel = row[x];
      uint8_t old_pixel = be_pixel & 0xFF;  // brightness
      // uint8_t brightness = (le_pixel >> 8) & 0xFF;
      int dither_thresh = 128;
      uint8_t new_pixel = (old_pixel >= dither_thresh) ? 255 : 0;
      correspondence[y*width + x] = new_pixel == 0 ? 1 : 0; 
      uint8_t quant_error = old_pixel - new_pixel;
      bool xl = true;
      bool xr = true;
      bool yr = true;
      // Check bounds
      if (y==height-1){
        yr = false;
      }
      if (x==0){
        xl = false;
      }
      if (x==width-1){
        xr = false;
      }
      if (xr){
        uint16_t neighbor_pixel = row[x+1];
        uint8_t diffuse_pixel = neighbor_pixel & 0xFF;
        diffuse_pixel += quant_error * (7.0/16.0);
        uint8_t high_byte = (neighbor_pixel >> 8) & 0xFF;
        row[x+1] = (high_byte << 8) | diffuse_pixel;
      }
      if (yr){
        uint16_t *nrow = &addr[(y+1) * width];
        if (xl){
          uint16_t neighbor_pixel = nrow[x-1];
          uint8_t diffuse_pixel = neighbor_pixel & 0xFF;
          diffuse_pixel += quant_error * (3.0/16.0);
          uint8_t high_byte = (neighbor_pixel >> 8) & 0xFF;
          nrow[x-1] = (high_byte << 8) | diffuse_pixel;
        }
        if (xr){
          uint16_t neighbor_pixel = nrow[x+1];
          uint8_t diffuse_pixel = neighbor_pixel & 0xFF;
          diffuse_pixel += quant_error * (1.0/16.0);
          uint8_t high_byte = (neighbor_pixel >> 8) & 0xFF;
          nrow[x+1] = (high_byte << 8) | diffuse_pixel;
        }
        uint16_t neighbor_pixel = nrow[x];
        uint8_t diffuse_pixel = neighbor_pixel & 0xFF;
        diffuse_pixel += quant_error * (5.0/16.0);
        uint8_t high_byte = (neighbor_pixel >> 8) & 0xFF;
        nrow[x] = (high_byte << 8) | diffuse_pixel;
      }
    }
  }
  Serial.println("Dithering done.");
}

// Threshold the image and fill in the correspondence array with the 
// results (0: black, 1: white). 
void threshold(uint16_t *addr, uint16_t width, uint16_t height, bool *correspondence){
  int thresh = 90;
  if(CAM_MODE != OV7670_COLOR_YUV) {
    Serial.println("Camera mode not YUV!");
    for(;;); // Fatal error, do not continue
  }
  for(int y=0; y<height; y++) {
    uint16_t *row = &addr[y * width];
    for(int x=0;x <width; x++) { 
      // The low-byte of YUV pixels holds the brightness information.
      // Raw data from camera is big-endian, so the second byte will hold brightness.
      uint16_t be_pixel = row[x];
      // uint8_t brightness = (le_pixel >> 8) & 0xFF;
      uint8_t brightness = be_pixel & 0xFF;
      bool res = 0;
      if (brightness >= thresh){
        res = 1;
      } 
      correspondence[y*width + x] = res;
    }
  }
  Serial.println("Thresholding done.");
}

// Write 16-bit value to BMP file in little-endian order
void writeLE16(File *file, uint16_t value) {
#if(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  file->write((char *)&value, sizeof value);
#else
  file->write( value       & 0xFF);
  file->write((value >> 8) & 0xFF);
#endif
}

// Write 32-bit value to BMP file in little-endian order
void writeLE32(File *file, uint32_t value) {
#if(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  file->write((char *)&value, sizeof value);
#else
  file->write( value        & 0xFF);
  file->write((value >>  8) & 0xFF);
  file->write((value >> 16) & 0xFF);
  file->write((value >> 24) & 0xFF);
#endif
}

// Dithered array to 24 bit BMP-writing function.
// This also crops the original array to dis_width, dis_height.
// BMP file format: https://en.wikipedia.org/wiki/BMP_file_format
void write_bmp_correspond(char *filename, bool *correspondence, uint16_t width, uint16_t height,
                        uint16_t dis_width, uint16_t dis_height) {
  SD.remove(filename); // Delete existing file, if any
  File file = SD.open(filename, FILE_WRITE);
  if(file) {
    // BMP header, 14 bytes:
    file.write(0x42);                               // Windows BMP signature
    file.write(0x4D);                               // "
    writeLE32(&file, 14 + 56 + dis_width * dis_height * 3); // File size in bytes
    writeLE32(&file, 0);                            // Creator bytes (ignored)
    writeLE32(&file, 14 + 56);                      // Offset to pixel data
  
    // DIB header, 56 bytes "BITMAPV3INFOHEADER" type (for RGB565):
    writeLE32(&file, 56);                     // Header size in bytes
    writeLE32(&file, dis_width);              // Width in pixels
    writeLE32(&file, dis_height);             // Height in pixels (bottom-to-top)
    writeLE16(&file, 1);                      // Planes = 1
    writeLE16(&file, 24);                     // Bits = 24
    // writeLE32(&file, 3);                   // Compression = bitfields
    writeLE32(&file, 0);                      // Compression = bitfields
    writeLE32(&file, dis_width * dis_height); // Bitmap size (adobe adds 2 here also)
    writeLE32(&file, 2835);                   // Horiz resolution (72dpi)
    writeLE32(&file, 2835);                   // Vert resolution (72dpi)
    writeLE32(&file, 0);                      // Default # colors in palette
    writeLE32(&file, 0);                      // Default # "important" colors
    writeLE32(&file, 0b1111100000000000);     // Red mask
    writeLE32(&file, 0b0000011111100000);     // Green mask
    writeLE32(&file, 0b0000000000011111);     // Blue mask
    writeLE32(&file, 0);                      // Alpha mask

    // Colors vals from https://learn.adafruit.com/preparing-graphics-for-e-ink-displays/command-line
    uint32_t white = 0xf4f4ec;
    uint32_t black = 0x2c242c;

    if (dis_width > width || dis_height > height){
      Serial.println("Cropped image exceed original dimensions!");
      dis_width = width;
      dis_height = height;
    }
    int x_buf = floor((width-dis_width)/2);
    int y_buf = floor((height-dis_height)/2);
    for(int y=0; y<dis_height; y++) {
      for(int x=dis_width-1;x >= 0; x--) { // Flip X
        uint32_t pixel = black;
        if (correspondence[(y+y_buf)*width + (x+x_buf)] == true){
          pixel = white;
        }
        file.write((char *)&pixel, 3);
      }
      // No scanline pad is added. We know the OV7670 library image sizes
      // are always an even width (4 bytes). Something to keep in mind if
      // flexible sizing and/or cropping options are added later.
    }
    file.close();
    Serial.println("Writing dithered BMP done.");
  }
}


// // Cropping is centered.
// void crop_img(uint16_t *addr, uint16_t *cropped_img, uint16_t width, uint16_t height, 
//                   uint16_t crop_width, uint16_t crop_height){
//   if (crop_width > width || crop_height > height){
//     Serial.println("Cropped image exceed original dimensions.");
//     return;
//   } 
//   int width_buf = floor((width-crop_width)/2);
//   int height_buf = floor((height-crop_height)/2);
  
//   for(int y=0; y<crop_height; ++y) {
//     uint16_t *row = &addr[(y + height_buf) * width];
//     for(int x=0;x <crop_width; ++x) { 
//       uint16_t pixel = row[x + width_buf];
//       cropped_img[y * crop_width + x] = pixel;
//     }
//   }
//   Serial.println("Done cropping.");
// }

