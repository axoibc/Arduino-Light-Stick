#include "LPD8806.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

File bmpFile;
int bmpWidth, bmpHeight;
uint8_t bmpDepth, bmpImageoffset;

const int analogInPin = A0;
int sensorValue = 0;
int outputValue = 0;
int BatteryLevel;

#define BUFFSIZE 16
char buffer[] = "0.txt";
char ascii[32];
char widthchar[32];
char timechar[32];
char currentchar[32];
char batterychar[32];
char numfiles[32];


#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#define TFT_CS  10  // Chip select line for TFT display
#define TFT_RST  9  // Reset line for TFT (or see below...)
#define TFT_DC   8  // Data/command line for TFT

#define SD_CS    4  // Chip select line for SD card

#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!

int dataPin = 3;
int clockPin = 2;

char Filename[12];

//Button1 stuff:
#define BUTTON1 6 // pin of button 1
#define BUTTON2 7 // pin of button 2
int buttonState;             // the current reading from the input pin
int ledState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 300;    // the debounce time; increase if the output flickers
int buttonState2;             // the current reading from the input pin
int ledState2;             // the current reading from the input pin
int lastButtonState2 = LOW;   // the previous reading from the input pin
long lastDebounceTime2 = 0;  // the last time the output pin was toggled
long debounceDelay2 = 300;    // the debounce time; increase if the output flickers


// The LED strips are 32 LEDs per meter but you can extend/cut the strip

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

LPD8806 strip = LPD8806(64, dataPin, clockPin);


File root;

int NumberofFiles;
int CurrentFile = 0;
boolean startup;

int displaytime_int;
int maxColors;
int maxColorscount;
int maxCurrent;

int colorred;
int colorgreen;
int colorblue;
uint32_t writtencolor;

void setup() {
  Serial.begin(9600);
  
  //Serial.println("LightStick Booting...");

  strip.begin();
  strip.show();
  delay(200);
  Serial.println( strip.numPixels() );
  
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(127,127,127));  // turn all pixels off
  }
  strip.show();
  
  
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(BLACK);
  tft.setTextColor(ST7735_WHITE);  
  tft.println("LightStick Booting...");

  tft.print("Strip # pixels.. ");
  tft.println( strip.numPixels() );
  

  Serial.print("Initializing SD card...");
  tft.print("Init SD card...");
  if (!SD.begin(SD_CS)) {
    tft.println("failed!");
    Serial.print("SD card FAILED!!");
    return;
  }
  tft.println("OK!");
  Serial.print("SD card OK!");

  tft.println("SD Initialized!");
  tft.println("Mainframe Powerup...");
  tft.drawLine(4, 54, 4, 66, WHITE);

  CurrentFile = 0;
  startup = 1;



  tft.drawLine(tft.width()-4, 54, tft.width()-4, 66, WHITE);
  tft.drawLine(4, 54, tft.width()-4, 54, WHITE);
  tft.drawLine(4, 66, tft.width()-4, 66, WHITE);
  for (uint16_t x=5; x < tft.width()-4; x+=1) {
    tft.drawLine(x, 55, x, 65, RED);
    delay(10);
  }
  
 

  NumberofFiles = getNumOfFiles();
}

int getNumOfFiles()
{
  char FilenamegetNum[12];
  char FilenamegetNumG[12];
  for (int i=0; i<200; i++) {
    sprintf(FilenamegetNum,"%i.BMP",i);
    sprintf(FilenamegetNumG,"%iG.BMP",i);
    if ((!SD.exists(FilenamegetNum)) || (!SD.exists(FilenamegetNumG))) {
      return i;

    }
  }

}

void display_status() {
  tft.fillScreen(BLACK);
  tft.println("Status:");
  sprintf(batterychar,"Battery: %imV",BatteryLevel);
  sprintf(numfiles,"Total Files: %i",NumberofFiles);
  sprintf(numfiles,"Current File: %s",Filename);

  tft.println(batterychar);
  tft.println(numfiles);
}

// function prototypes, do not remove these!
void colorChase(uint32_t c, uint8_t wait);

void loop() {

  int reading = readButton1();
  int reading2 = readButton2();

  sensorValue = analogRead(analogInPin); 
  //outputValue = map(constrain(sensorValue, 653, 784), 653, 784, 0, 63);
  BatteryLevel = 15.3*sensorValue;

  if (reading == 1){
   
    
    sprintf(Filename,"%i.BMP",CurrentFile);

    if (startup == 1) 
    {
      //Serial.println("Startup = 1");
      display_status();
    } 
    else if (not SD.exists(Filename))
    {
      CurrentFile = 0;
      display_status();
      //Serial.println("Setting currentfile to 0");
    } 
    else 
    {
      tft.fillScreen(BLACK);

      bmpFile = SD.open(Filename);
      tft.setCursor(0, 0);

      if (! bmpReadHeader(bmpFile)) { 
        sprintf(ascii,"File Name: %i.BMP",CurrentFile);
        tft.println(ascii);
        tft.println("BMP BAD! OH NO!");
        //Serial.println("Bad bmp header!");
      } 
      else 
      {
        bmpdraw(bmpFile, 0, 90, 1);
        sprintf(ascii,"File Name: %i.BMP",CurrentFile);
        tft.println(ascii);
        sprintf(widthchar,"BMP Width: %i",bmpHeight);
        tft.println(widthchar);
        displaytime_int = (int)(36*bmpHeight);
        sprintf(timechar,"Display Time: %ims",displaytime_int);
        tft.println(timechar);
        maxCurrent = (int)maxColors/(34);
        sprintf(currentchar,"Max Current: %imA",maxCurrent);
        tft.println(currentchar);
        sprintf(batterychar,"Battery: %imV",BatteryLevel);
        tft.println(batterychar);
        tft.println("Preview:");
      }
      bmpFile.close(); 
      CurrentFile = CurrentFile + 1;
    }
    startup = 0;
    //Serial.println(CurrentFile);
    delay(100);
  } 

  if (reading2 == 1){
    tft.setCursor(0, 0);
    sprintf(Filename,"%iG.BMP",CurrentFile-1);
    display_status();
    
    //Serial.print("Current File: ");
    //Serial.println(Filename);
    if (CurrentFile == 999) {
      display_status();
    } 
    else {
      colorChase(strip.Color(0,127,127), 10);
      delay(2000);
      bmpFile = SD.open(Filename);
      bmpdraw(bmpFile, 0, 0, 0);
      //makePicture(bmpFile,0,0);
      bmpFile.close(); 
      delay(2000);
    }      
  }

  // Send a simple pixel chase in...
}

int readButton1() {

  int reading = digitalRead(BUTTON1);
  int buttonFallEdge = 0;

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == 0 && lastButtonState == 1) {
      // reset the debouncing timer
      lastDebounceTime = millis();
      buttonFallEdge = 1;
    }
  }

  lastButtonState = reading;

  return buttonFallEdge;

}

int readButton2() {

  int reading2 = digitalRead(BUTTON2);
  int buttonFallEdge2 = 0;

  if ((millis() - lastDebounceTime2) > debounceDelay2) {
    if (reading2 == 0 && lastButtonState2 == 1) {
      // reset the debouncing timer
      lastDebounceTime2 = millis();
      buttonFallEdge2 = 1;
    }
  }

  lastButtonState2 = reading2;

  return buttonFallEdge2;

}

// Chase a dot down the strip
// good for testing purposes
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  }

  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c); // set one pixel
    strip.show();              // refresh strip display
    delay(wait);               // hold image for a moment
    strip.setPixelColor(i, 0); // erase pixel (but don't refresh yet)
  }
  strip.show(); // for last erased pixel
}

#define BUFFPIXEL 20

void bmpdraw(File f, int x, int y, int TFTchoose) {

  f.seek(bmpImageoffset);

  uint32_t time = millis();
  uint16_t p; 
  uint8_t g, b;
  int i, j;
  maxColors = 0;

  uint8_t sdbuffer[3 * BUFFPIXEL];  // 3 * pixels to buffer
  uint8_t buffidx = 3*BUFFPIXEL;

  //Serial.print("rotation = "); Serial.println(tft.getRotation(), DEC);

  //set up the 'display window'
  //Serial.print("bmp width: ");
  //Serial.println(bmpWidth);
  //Serial.print("bmp height: ");
  //Serial.println(bmpHeight);
  if (TFTchoose == 1){
    tft.setAddrWindow(x, y, x+bmpWidth-1, y+bmpHeight-1);
    uint8_t rotback = tft.getRotation();
  } 
  else {
    x = 0;
    y = 0;
  }
  //tft.setRotation();

  for (i=x; i< bmpHeight+x; i++) {
    // bitmaps are stored with the BOTTOM line first so we have to move 'up'

    maxColorscount = 0;
    for (j=y; j<bmpWidth+y; j++) {
      // read more pixels
      if (buffidx >= 3*BUFFPIXEL) {
        f.read(sdbuffer, 3*BUFFPIXEL);
        buffidx = 0;
      }

      // convert pixel from 888 to 565
      b = sdbuffer[buffidx++];     // blue
      g = sdbuffer[buffidx++];     // green
      p = sdbuffer[buffidx++];     // red
      maxColorscount = maxColorscount + b + g + p;
      colorred = (int)p/2;
      colorgreen = (int)g/2;
      colorblue = (int)b/2;

      //      Serial.print(". B: ");
      //      Serial.print(colorblue,DEC);
      //      Serial.print(", G: ");
      //      Serial.print(colorgreen,DEC);
      //      Serial.print(", P: ");
      //      Serial.println(colorred,DEC);


      p >>= 3;
      p <<= 6;

      g >>= 2;
      p |= g;
      p <<= 5;

      b >>= 3;
      p |= b;
      //Serial.print(p, HEX);
      // write out the 16 bits of color
      if (TFTchoose == 1){
        tft.drawPixel(i, j, p);
      } 
      else {
        //Serial.print(p, HEX);
        strip.setPixelColor(j, strip.Color(colorred,colorgreen,colorblue));
      };
    }

    if (TFTchoose == 1){
      if (maxColorscount >= maxColors) {
        maxColors = maxColorscount;
      }
    } 
    else {
      strip.show();
      delay(12);
    }
  }
  //Serial.print(millis() - time, DEC);
  //Serial.println(" ms");
  if (TFTchoose == 0){
    for (int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);  // turn all pixels off
    } 
    strip.show();
    delay(10);
  }
}

#define BUFFPIXEL 20

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  tft.println();
  tft.println("Loading...");
  tft.print("'");
  tft.print(filename);
  tft.println("'");


  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    tft.println("File not found");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    tft.println("File size: "); 
    tft.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    tft.print("Image Offset: "); 
    tft.println(bmpImageoffset, DEC);
    // Read DIB header
    tft.print("Header size: "); 
    tft.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      tft.print("Bit Depth: "); 
      tft.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        tft.print("Image size: ");
        tft.print(bmpWidth);
        tft.print('x');
        tft.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
          pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        tft.print("Loaded in ");
        tft.print(millis() - startTime);
        tft.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) tft.println("BMP format not recognized.");
}

boolean bmpReadHeader(File f) {
  // read header
  uint32_t tmp;

  if (read16(f) != 0x4D42) {
    // magic bytes missing
    return false;
  }
  //Serial.println("passed 1");
  // read file size
  tmp = read32(f);  
  //Serial.print("size 0x"); Serial.println(tmp, HEX);

  // read and ignore creator bytes
  read32(f);

  bmpImageoffset = read32(f);  
  //Serial.print("offset "); Serial.println(bmpImageoffset, DEC);

  // read DIB header
  tmp = read32(f);
  //Serial.print("header size "); Serial.println(tmp, DEC);
  bmpWidth = read32(f);
  bmpHeight = read32(f);


  if (read16(f) != 1)
    return false;

  //Serial.println("passed 2");

  bmpDepth = read16(f);
  //Serial.print("bitdepth "); Serial.println(bmpDepth, DEC);

  if (read32(f) != 0) {
    // compression not supported!
    return false;
  }

  //Serial.println("passed 3");
  //Serial.print("compression "); Serial.println(tmp, DEC);

  return true;
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


