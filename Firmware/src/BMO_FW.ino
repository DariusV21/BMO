
// Darius Vaitiekus
// BMO New Edition.
// SD, TMRPCM, LCD.
// V1.4a

#include "PCD8544.h"          // NOKIA 5110 LCD Library
#include <SdFat.h>			  // SD card custom library
#include <TMRpcm.h>			  // Sound from atmega library
#include <SPI.h>			  // Comunication protocol between modules
#include <avr/pgmspace.h>	  // Protocol, to save RAM space, by using FLASH


#define SD_ChipSelectPin A0   // using digital pin 0A on arduino UNO 328

TMRpcm tmrpcm;                // create an object for use in this sketch
SdFat SD;
SdFile myFile;

// Define RAM Buffer [bytes]
uint8_t ramBuffer[504];


// Define string buffers for PROGMEM
char pBuffer[17];
char mBuffer[17];


// The dimensions of the LCD (in pixels)...
static const byte LCD_WIDTH = 84;
static const byte LCD_HEIGHT = 48;

// LCD pins
unsigned char sclk  = 7;	// clock       (LCD display pin 7)
unsigned char sdin  = 6;	// data-in     (LCD display pin 6)
unsigned char dc    = 5;	// data select (LCD display pin 5)
unsigned char reset = 3;	// reset       (LCD display pin 4)
unsigned char sce   = 4;	// enable      (LCD display pin 3)
PCD8544 lcd(sclk, sdin, dc, reset, sce);

// Led Pin
const byte ledLCD = 8;

// Battery voltage Pin
int analogPin = A5;
// Variable to store the value of voltage read
int val = 0;
// Full Voltage value
float voltage = 0;

// Current FIRMWARE NUMBER
const char* fw = "v1.4a";

// Random Number Generator for pictures
byte pRNG = 0;
// Random Number Generator for music
byte mRNG = 0;



//******************************************************************************
// Insert numbers of files in SD card
int allPics = 144;			// Number of Picture Files
int allMusic = 304;			// Number of Music Files
//******************************************************************************

// Initializing strings
String pic1, pic2, mus1, mus2, masterPStr, masterMStr;




//////////////////////// VOID SETUP ////////////////////////////////////////////
void setup() {

// Initializing string variables for SD card path
// Full picture path: pictures/filename.bin
pic1 = String(F("pictures/"));
pic2 = String(F(".bin"));
// Full music path: music/filename.wav
mus1 = String(F("music/"));
mus2 = String(F(".wav"));



// Begin LCD display @ 84x48 resolution:
lcd.begin(84, 48);		// Begin (resolution)


	val = analogRead(analogPin);
	voltage = (val*0.00484)*2;
	lcd.clear();
	lcd.setCursor(1,2);
	lcd.print(F("Voltage Check"));
	lcd.setCursor(0,0);
	lcd.print(voltage);
	delay(500);
	lcd.clear();


// see if the card is present and can be initialized:
while (!SD.begin(SD_ChipSelectPin)) {

	// If SD card is not inserted, print system status - SD error, Voltage, Firmware.
	lcd.setCursor(1, 1);
	lcd.print(F("SD Card Error!"));
	lcd.setCursor(1, 3);
	lcd.print(F("Voltage:"));
	lcd.setCursor(50, 3);
	lcd.print(voltage);
	lcd.setCursor(75, 3);
	lcd.print("V");
	lcd.setCursor(1, 4);
	lcd.print(F("Firmware:"));
	lcd.setCursor(57, 4);
	lcd.print(fw);

    delay(3000);
}


//Asigns the speaker pin.
tmrpcm.speakerPin = 9;		//9 on Uno, Nano, etc



// LCD LED variable declaration, uncoment to turn on LCD LED light.
pinMode(ledLCD, OUTPUT);
digitalWrite (ledLCD, HIGH);


  bmo(500, 103, 101);			// Startup screen (delay(ms), picture, music)
  lcd.setCursor(60, 5);			// Set Cursor(X pixels, Y rows)
  lcd.print(fw);				// Print firmware version
  delay(2000);


randomSeed(analogRead(A5));	// Seed the random generator


}


//////////////////////////////// VOID LOOP /////////////////////////////////////

void loop() {
  // put your main code here, to run repeatedly:

  pRNG = random(101, allPics + 1);


  delay (10);


  mRNG = random(101, allMusic + 1);


  bmo(1000, pRNG, mRNG);




}   // Void Loop end

///////////////////////////////////////FUNCTIONS////////////////////////////////


// MAIN BMO CONTROL FUNCTION --->> bmo(delay, picture, music)
void bmo(int tdelay, int picture, int music) {

	//Construct full path for picture files from SD card
	masterPStr = pic1 + picture + pic2;
	masterPStr.toCharArray(pBuffer,17);			//Converting String to char array and write it to buffer

	//Construct full path for music files from SD card
	masterMStr = mus1 + music + mus2;
	masterMStr.toCharArray(mBuffer,17);

	showtime(0, 0, tdelay, pBuffer, mBuffer); 	// Passing delay and buffer info for opening files
}


// Function that positions cursor (x,y), sets picture delay, and sets pic, music.
// showtime( cursor X, cursor Y, delay, picture, music)
void showtime(byte cx, byte cy, int tdelay, const char* picture, char* music) {
  lcd.setCursor(cx, cy);
  show_bmp(picture);
  tmrpcm.play(music);
  while (tmrpcm.isPlaying()) {
    delay(10);
  }
  delay (tdelay);
}


// Bmp drawing from SD, call - show_bmp("pic001.bin");
void show_bmp(const char* filename) {                // show_bmp("pic001.bin");
  myFile.open(filename, O_READ);                     // O_READ - Prepare for reading
  myFile.read (ramBuffer, 504);                      // Read opened file to ramBuffer
  myFile.close();                                    // close the file:
  lcd.drawBitmap (ramBuffer, LCD_WIDTH, LCD_HEIGHT); // LCD draw BMP lcd.drawBitmap(bmpName, Width, Height);
}

