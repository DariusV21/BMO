
// Darius Vaitiekus
// BMO New Edition.
// SD, TMRPCM, LCD.
// V1.0

#include "PCD8544.h"          // NOKIA 5110 LCD Library
#include <SdFat.h>
#include <TMRpcm.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include "BMO_Strings.h"

#define SD_ChipSelectPin A0   // using digital pin 0A on arduino UNO 328

TMRpcm tmrpcm;                // create an object for use in this sketch
SdFat SD;
SdFile myFile;

// Define RAM Buffer [bytes]
uint8_t ramBuffer[504];

char str3 ;                                                                                                                                                             
// Define string buffers for PROGMEM
char pBuffer[17];
char mBuffer[17];

// Define blocked variants table (pictureBVT and musicBVT)
byte pbvt[5] = {0, 0, 0, 0, 0};
byte mbvt[5] = {0, 0, 0, 0, 0};
byte picNumBVT = 0;
byte musNumBVT = 0;

// The dimensions of the LCD (in pixels)...
static const byte LCD_WIDTH = 84;
static const byte LCD_HEIGHT = 48;

// LCD pins
unsigned char sclk  = 7;    // clock       (LCD display pin 7)
unsigned char sdin  = 6;    // data-in     (LCD display pin 6)
unsigned char dc    = 5;     // data select (LCD display pin 5)
unsigned char reset = 3;     // reset       (LCD display pin 4)
unsigned char sce   = 4;     // enable      (LCD display pin 3)
PCD8544 lcd(sclk, sdin, dc, reset, sce);

const byte ledLCD = 8;        // Led Pin
int batteryValue = analogRead(10);  // vBat value


// Random Number Generator
byte pRNG = 0;
byte mRNG = 0;

//**********************************************************************************************************
// Insert numbers of files in SD card
int allPics = 51;             // Number of Picture Files
int allMusic = 198;           // Number of Music Files
//**********************************************************************************************************

//////////////////////////////////////////////////// VOID SETUP ////////////////////////////////////////////
void setup() {

  // Begin Serial @ 9600 baudrate:
  Serial.begin(9600);       // set up Serial library at 9600 bps

  // Begin LCD display @ 84x48 resolution:
  lcd.begin(84, 48);        // PCD8544-compatible displays may have a different resolution... Begin (resolution)

  // Check if SDcard is present:
  Serial.println(F("Initializing SD card...")); // see if the card is present and can be initialized:
  while (!SD.begin(SD_ChipSelectPin)) {
      Serial.println(F("Card failed, or not present"));
      lcd.setCursor(1, 2);
      lcd.print(F("SD Card Error!"));                  // SD Error
      delay(3000);
    }
  if (!SD.begin(SD_ChipSelectPin)) {            // if card is not responsive 
    return;                     // don't do anything more if not
  }
  else {
    Serial.println(F("SD Card initialized !"));  // Card check success !
  }

  //Asingns the speaker pin.
  tmrpcm.speakerPin = 9;        //11 on Mega, 9 on Uno, Nano, etc

  // LCD LED variable declaration, uncoment to turn on LCD LED light.
  pinMode(ledLCD, OUTPUT);
  digitalWrite (ledLCD, HIGH);

  bmo(500, 3, 0);      // Startup screen (Delay, picture, music)
  lcd.setCursor(60, 5);                  // Set Cursor(X pixels, Y rows)
  lcd.print(F("v1.0"));                  // Firmware version
  delay(2000);

  randomSeed(analogRead(0)); // Seed the random generator
}


//////////////////////////////////////////////////// VOID LOOP /////////////////////////////////////////////

void loop() {
  // put your main code here, to run repeatedly:

  pRNG = random(1, allPics + 1);
  //  pictureBVT(pRNG);

  delay (10);


/*
  // SORTING ALGORYTHM ********
  if (1 <= pRNG <= 10) {
    mRNG = random(0, 31); // max files +1
  }
  else if (11 <= pRNG <= 20) {
    mRNG = random(31, 61); // max files +1
  }
  else if (21 <= pRNG <= 30) {
    mRNG = random(61, 91); // max files +1
  }
  else if (31 <= pRNG <= 40) {
    mRNG = random(91, 121); // max files +1
  }
  else if (41 <= pRNG <= 45) {
    mRNG = random(121, 195); // max files +1
  }
  else if (51 <= pRNG <= 60) {                       // PORTAL2
    mRNG = random(200, 205); // max files +1
  }

*/


    mRNG = random(0, allMusic + 1);
  //  musicBVT(mRNG);


  bmo(1000, pRNG, mRNG);

  Serial.print(F("pRNG - "));
  Serial.println(pRNG);
  Serial.print(F("mRNG - "));
  Serial.println(mRNG);


//voltageBattery();

}

// ////////////////FUNCTIONS//////////////

// Bmp drawing from SD, call - show_bmp("pic001.bin");
void show_bmp(const char* filename) {                // show_bmp("pic001.bin");
  myFile.open(filename, O_READ);                     // O_READ - Prepare for reading
  myFile.read (ramBuffer, 504);                      // Read opened file to ramBuffer
  myFile.close();                                    // close the file:
  lcd.drawBitmap (ramBuffer, LCD_WIDTH, LCD_HEIGHT); // LCD draw BMP lcd.drawBitmap(bmpName, Width, Height);
}
/*
// EXPERIMENTAL BMO CONTROL FUNCTION --->> bmo(delay, picture, music)    // NEW TRY TO OPTIMIZE
void bmoOPT(int tdelay, int picture, int music) {
  str3 = str1 + str2;
  strcpy_P(pBuffer, (char*)pgm_read_word(str1));
  strcpy_P(mBuffer, (char*)pgm_read_word(&(music_table[music])));
  showtime(0, 0, tdelay, pBuffer, mBuffer);
}
*/
// MAIN BMO CONTROL FUNCTION --->> bmo(delay, picture, music)
void bmo(int tdelay, int picture, int music) {
  strcpy_P(pBuffer, (char*)pgm_read_word(&(picture_table[picture])));
  strcpy_P(mBuffer, (char*)pgm_read_word(&(music_table[music])));
  showtime(0, 0, tdelay, pBuffer, mBuffer);
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

// Blocked variants table (pictureBVT)  (UNUSED)
void pictureBVT(int lpicRNG) {
  if (picNumBVT >= 5) {
    picNumBVT = 0;
  }
  //  pic_restart:
  if (lpicRNG == pbvt[0] || pbvt[1] || pbvt[2] || pbvt[3] || pbvt[4]) {
    pRNG = random(0, allPics + 1);
    //    goto pic_restart;
  }
  else {
    pbvt[picNumBVT] = lpicRNG;
    picNumBVT++;
  }
}

// Blocked variants table (musicBVT)   (UNUSED)
void musicBVT(int lmusBVT) {
  if (musNumBVT >= 5) {
    musNumBVT = 0;
  }
  //  mus_restart:
  if (lmusBVT == mbvt[0] || mbvt[1] || mbvt[2] || mbvt[3] || mbvt[4]) {
    mRNG = random(0, allMusic + 1);
    //    goto mus_restart;
  }
  else {
    mbvt[musNumBVT] = lmusBVT;
    musNumBVT++;
  }
}

void voltageBattery(){
  float voltage = batteryValue * (4.2 / 1023);
  if (voltage <= 3.1){
    bmo(500, 53, 0);      // Startup screen (Delay, picture, music)
    delay(2000);
  }
  
}
