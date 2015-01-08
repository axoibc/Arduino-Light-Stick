# Arduino-Light-Stick
Based off of code from http://scott-bot.com/led-light-painting/

This updates the code to utilitize adafruit's new ST7735R with built in microSD card reader. 
Plus changes to the libraries that have happened since the original project was created. 
The original project used a seperate card reader which is now unnecessary. 
(Wish I'd known that before I bought the other reader though...)

This project requires:
  1     ST7735R (https://www.adafruit.com/products/358)
  1     Arudino Uno (I used a version 2 board which works the same as an R3 board) (https://www.sparkfun.com/products/11021)
  2     LED Strips (2 meters = 64 LEDs) (https://www.adafruit.com/products/306)
  
Hookup is pretty straight forward following these instructions from 
https://learn.adafruit.com/1-8-tft-display/displaying-bitmaps.

LCD PIN (left to right facing LCD - breakout)
  1. GND
  2. Vcc
  3. RESET
  4. D/C
  5. CARD_CS Card Select
  6. TFT_CS Screen Select
  7. MOSI Master Out Slave In
  8. SCK Serial Clock
  9. MISO Master In Slave Out
  10. LITE Background Light Control

  1. 3-5V Vin connects to the Arduino 5V pin (LCD Pin 2)
  2. GND connects to Arduino ground (LCD Pin 1)
  3. SCLK connects to SPI clock (LCD Pin 8 to Arudino PIN 13)
  4. MOSI connects to SPI MOSI. (LCD Pin 7 to Arudino PIN 11) 
  5. CS connects to our SPI Chip Select pin. (LCD Pin 6 to Arudino PIN 10)
  6. RST connects to our TFT reset pin. (LCD Pin 3 to Arudino PIN 9) 
  7. D/C connects to our SPI data/command select pin. (LCD Pin 4 to Arudino PIN 8) 
  8. LITE (LCD Pin 10 to Arducino Vcc)
  8. CARD_CS (LCD Pin 5 to Arduino PIN 3) 
  9. MISO (LCD Pin 9 to Arudino PIN 4 )

