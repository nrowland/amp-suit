#include <FastSPI_LED.h> //FastSPI library
#include <FHT.h> // include the library

#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht

#define PixelCount 240
#define MIC_PIN   A9  // Microphone is attached to this analog pin
#define LED_PIN    6  // NeoPixel LED strand is connected to this pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define noiselvl     10  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment
#define TOP       (N_PIXELS + 2) // Allow dot to go slightly off scale
#define PEAK_FALL 40  // Rate of peak falling dot



struct CRGB { unsigned char r; unsigned char g; unsigned char b; }; //sets the bit order of the LED values, so if strip is grb you just order these differently
struct CRGB *leds;  //defines pointer to CRGB as *leds

void setup() {
	memset(leds, 0, PixelCount * 3)
	FastSPI_LED.setLeds(PixelCount); 
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2801);   //other drivers supported: TM1809, LPD6803, HL1606, 595, WS2801
  //FastSPI_LED.setPin(fSPI_PIN); //pin only used for TM1809
  FastSPI_LED.setDataRate(3); //if random flashing occurs (ws2801s i'm looking at you) set to 1-7 to slow data rate
  FastSPI_LED.init();
  FastSPI_LED.start();
  
  leds = (struct CRGB*)FastSPI_LED.getRGBData(); //magical array of LED, idk what this line does but it's required
}

void loop() {

  int      n;
  int	maximum;
  int minimum;
  int i
  n   = analogRead(MIC_PIN);                        // Raw reading from mic 
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  //lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)
  fht_input[i] = n
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
 	fht_mag_log(); // take the output of the fht
	while (i < FHT_N/2)
		if(maximum < fht_log_out[i])
			maximum = fht_log_out[i]
		if(minimum > fht_log_out[i])
			minimum = fht_log_out[i]
	maximum = max(fht_log_out)
  	
	switch (mode) {
    case 0:
        EQcolorOrgan();
        delay(20);
        break;

    case 1:
        EQcenterBar();
        delay(15);
        break;        
        
    case 2:
        SlowWheel(100 + (irscale * 10)); //SLOW wheel						//have to change IR scale
        break;

    case 3: 
        FullFollow();
        delay(5 + irscale);
        break;        
        
    case 4:
        EQsingleColorStep();
        delay(5 + irscale);
        break; 

    case 5:
        EQvolbar();
        delay(20);
        break;   
        
    case 6:  
       Strobe(30 + irscale); //30 is probably a functional minimum before it becomes just a dimmed light bulb, also I hate this mode
       break;
    
    case 7:
       rainbow(); 
       delay(25);
       break;
       
    case 8:
        rainbowCycle(2);
        delay(5 + irscale);
        break;
        
    case 9:
        rainbowCycleEQ(6);
        delay(5 + irscale);
        break;
        //more modes can go in here, but change power function and mode change function accordingly.
        
    default:
      //"off".. more or less.
      break;
  }
}
void SlowWheel(int cchanged){ 
  //SlowWheel uses a timer instead of a delay, fades can be imperceptibly slow
  //cchanged is a color fill delay int, should be a high number otherwise why not just use wheel function.
  if(LoopStart + cchanged < millis()){        //Set the number of milliseconds to wait for a color change here
    Wheel(LoopCnt);
    colorFill(ledRed, ledGreen, ledBlue);  
//          Serial.print("CNT: "); // Debug -----------------------
//          Serial.println(LoopCnt); // Debug -----------------------

    LoopCnt++;                           // Increment the wheel counter
    if (LoopCnt >= 768){LoopCnt=0;}       // Counter at the end of wheel, move it back to 0
    LoopStart = millis();                // Rest the timestamp
  }
}

void EQcolorOrgan(){ 
  //reads eq, punches value into the whole strip at once
  readEQ();
  colorFill(ledRed, ledGreen, ledBlue);
}

void FullFollow(){ 
 //steps from one end of the array to the other updating EQ values pixel by pixel
 readEQ();
 
 if(LoopCnt <= PixelCountZB) { 
    //Add new color to array at pointer position
    leds[PixelCountZB - LoopCnt].r = ledRed; //not really sure why it's from this side
    leds[PixelCountZB - LoopCnt].g = ledGreen;
    leds[PixelCountZB - LoopCnt].b = ledBlue;      
    LoopCnt++;  // Increment the array position
    FastSPI_LED.show();
  }else{LoopCnt = 0;}
}   

void EQsingleColorStep(){//mod to include color RGB scales?
 // this takes the three eq values and just shoves them into a single color's values three at a time. 
 // kinda lame, ill write it anyway. flag it for a remote control option to change colors or something. 
  readEQ();
  
  if (LoopCnt <= PixelCountZB ) {
    
    leds[LoopCnt].r = 0;
    leds[LoopCnt].g = ledRed;
    leds[LoopCnt].b = 0;
    LoopCnt++;
    
    leds[LoopCnt].r = 0;
    leds[LoopCnt].g = ledGreen;
    leds[LoopCnt].b = 0;
    LoopCnt++;
    
    leds[LoopCnt].r = 0;
    leds[LoopCnt].g = ledBlue;
    leds[LoopCnt].b = 0; 
    LoopCnt++;  
    
  } else {LoopCnt=0;}
  
  FastSPI_LED.show();
}

void EQvolbar() {
//EQ volume bar, each color has its own bar from the strip's center
//I want the color to fade out towards the end, to keep center from just being white
//something to do later.

  readEQ();
  
  memset(leds, 0, PixelCount * 3); //quickly wipe the array without touching variables
    
  int rngR = map(ledRed, 0, 256, 0, halfStripZB); //map the average to strip length, a bit sloppy, but i'm not smart enough to calculate once
  int rngG = map(ledGreen, 0, 256, 0, halfStripZB);
  int rngB = map(ledBlue, 0, 256, 0, halfStripZB);
   
  for(int i=0; i <= rngR; i++){
      leds[halfStripZB-i].r = ledRed;
      leds[halfStripZB+i].r = ledRed; 
  }
  
  for(int i=0; i <= rngG; i++){
      leds[halfStripZB-i].g = ledGreen; 
      leds[halfStripZB+i].g = ledGreen;
  }
  
  for(int i=0; i <= rngB; i++){     
      leds[halfStripZB-i].b = ledBlue;
      leds[halfStripZB+i].b = ledBlue;
  
  }
  FastSPI_LED.show();
}


void rainbow() {
  // rainbow accepts a color change delay value and fades the wheel color by color pixel by pixel
  // I pulled out the loop so that the IR and button checks were performed every cycle
  // also uses LoopCnt global variable. the wheel equation scales the entire strip to a 0-255 value based
  // on the position of the pixel and the position of the loop count (which offsets the wheel's start position)
     
  if(LoopCnt <= 767) {
    for (int i=0; i < PixelCountZB; i++) {
      Wheel( (i + LoopCnt) % 767 );
      leds[i].r = ledRed;
      leds[i].g = ledGreen;
      leds[i].b = ledBlue;
    }  
    LoopCnt++;
    FastSPI_LED.show();
  }else{LoopCnt=0;}
}


void rainbowCycle(long stripFades) { //stripfades should be an int, long keeps big number errors at bay
//The confusingly named rainbowcycle attempts to evenly split the rainbow fade values along the LED strip
//stripFades stripfades is the number of fades across the strip, I don't fully understand the equation here
//and I had to make it used floating point calculation, so it runs slow. Adapted from adafruit's library.

  if(LoopCnt <= 767) {
    for (int i=0; i < PixelCountZB; i++) {
      Wheel( ((i * (256 * stripFades) / PixelCount) + LoopCnt) % 767); //loopcnt handles the offset, while i splits 255 evenly among the pixel count
      leds[i].r = ledRed;
      leds[i].g = ledGreen;
      leds[i].b = ledBlue;
    }  
    LoopCnt++; 
    FastSPI_LED.show();
  }else{LoopCnt=0;}
}

void rainbowCycleEQ(long stripFades) {
// Lowers the brightness values of rainbow cycle and uses the EQ color values to make up the difference.
// short strips of colors travel down the pixels, and get larger as volume increases fading into 
// each other. it may look better if the effect is massively slowed or augmented in some way, color 
// interaction is hard to see. 
  int red, green, blue;
  
  if(LoopCnt <= 767) {
    readEQ(); //one eq value per loop, keeps things from gettng wacky.
    
    red = ledRed - 200;  //instead of halving and then adding im taking these into the negatives.
    green = ledGreen - 200;
    blue = ledBlue - 200; 
    
    for (int i=0; i < PixelCountZB; i++) {
      Wheel( ((i * (256 * stripFades) / PixelCount) + LoopCnt) % 767 ); //the fade number is float otherwise int is overloaded... probably very slow
      leds[i].r = constrain(ledRed + red, 0, 255); //this is a dumb way to boost bass, I just need a better input
      leds[i].g = constrain(ledGreen + green, 0, 255);
      leds[i].b = constrain(ledBlue + blue, 0, 255);
    } 

    LoopCnt++; 
    FastSPI_LED.show();
  }else{LoopCnt=0;} 
}
void readEQ() {

    spectrumValue[q] = (fht_log_out, FHT_N/2) //add FFT output read here? or remove spectrum value from the for loop and set it to FFT;
//  Serial.print(q);
//  Serial.print(" ");
//  Serial.println(spectrumValue[q]);
	
  }
	
  //Set the LED Colors
  //this takes the analog frequency values and averages some bands
  //the values are then set zero if not above a noise level, then mapped 
  //from their 0-1024 value to 1-255
  
  ledBlue = max(spectrumValue[6], spectrumValue[5]);
  if (ledBlue <= noiseLvl){
    ledBlue=0;
  }else{
    ledBlue = map(ledBlue, noiseLvl, 1024, 1, 255);
  }

  ledGreen = spectrumValue[4]; //4 is crazy, its like 3x higher than everything else... probably my mic. 
  if (ledGreen <= noiseLvl){
    ledGreen=0;
  }else{
    ledGreen = map(ledGreen, noiseLvl, 1024, 1, 255);
  }
  ledRed = max(spectrumValue[3], max(spectrumValue[2], max(spectrumValue[1], spectrumValue[0]))); //kind of out of control with the max functions, but red isnt showing enough!
  if (ledRed <= noiseLvl){
    ledRed=0;
  }else{
    ledRed = map(ledRed, noiseLvl, 1024, 1, 255);
  }
}

// ----------------------- Color Support Functions -----------------------

void colorWipe(int red, int green, int blue) {
// Used to open up loops, RGB values write to pixel set by loop count, strip is then displayed
// Each call of the function advances pixel count.
  if(LoopCnt <= PixelCountZB) {
    leds[LoopCnt].r = red;
    leds[LoopCnt].g = green;
    leds[LoopCnt].b = blue;
    LoopCnt++;                           // Increment the counter
    FastSPI_LED.show();   // write all the pixels out
  }else{LoopCnt=0;}
}

void colorWipeBack(int red, int green, int blue, uint8_t wait) {
 // Used to open up loops, RGB values write to pixel set by loop count, strip is then displayed
 // Each call of the function LOWERS pixel count.
  if(LoopCnt <= PixelCountZB) {
    leds[PixelCountZB - LoopCnt].r = red;
    leds[PixelCountZB - LoopCnt].g = green;
    leds[PixelCountZB - LoopCnt].b = blue;
    LoopCnt++;                           // Increment the counter
    FastSPI_LED.show();   // write all the pixels out
  }else{LoopCnt=0;}
}


void colorFill(int red, int green, int blue) {
 //makes a filled array of the chosen color all at once (resets pixel count), then writes it. 
  for (int i=0; i < PixelCount; i++) { //is there a more direct way to pass these? this works.
    leds[i].r = red;
    leds[i].g = green;
    leds[i].b = blue;
  }
  FastSPI_LED.show();
}

void Wheel(int WheelPos){
//wheel accepts 0-767 (0 biased I guess) position int, it sets the global ledRed, ledGreen and ledBlue
//values based on this position value. each color gets 256 positions. wheelpos binary number is
//split to thirds by the bit shift.
  switch(WheelPos >> 8) 
  {
    case 0:
      ledRed = 255 - WheelPos % 256;   //Red down
      ledGreen = WheelPos % 256;      // Green up
      ledBlue = 0;                  //blue off
      break; 
    case 1:
      ledGreen = 255 - WheelPos % 256;  //green down
      ledBlue = WheelPos % 256;      //blue up
      ledRed = 0;                  //red off
      break; 
    case 2:
      ledBlue = 255 - WheelPos % 256;  //blue down 
      ledRed = WheelPos % 256;      //red up
      ledGreen = 0;                  //green off
      break; 
  }
}
void ChangeMode(max, min) {
//incrament the mode and recycle if at full
//I can't wait to get the xbox remote running and delete this function
  if (ModeUP){
    digitalWrite(statusPin, LOW);
    mode++;
    if (mode > 9) {digitalWrite(statusPin, LOW); mode = 0;}
  }else{
    digitalWrite(statusPin, LOW);
    mode--;
    if (mode < 0) {digitalWrite(statusPin, LOW); mode = 9;} 
  }
  
  // Clear the strip
  memset(leds, 0, PixelCount * 3);
  FastSPI_LED.show();
  
  LoopStart = 0; //reset time
  delay(20);   //dont like it, but otherwise the LED might not be visible, and modes already change too quickly
  digitalWrite(statusPin, HIGH);
}