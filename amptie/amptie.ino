/*
LED VU meter for Arduino and Adafruit NeoPixel LEDs.

Hardware requirements:
 - Most Arduino or Arduino-compatible boards (ATmega 328P or better).
 - Adafruit Electret Microphone Amplifier (ID: 1063)
 - Adafruit Flora RGB Smart Pixels (ID: 1260)
   OR
 - Adafruit NeoPixel Digital LED strip (ID: 1138)
 - Optional: battery for portable use (else power through USB or adapter)
Software requirements:
 - Adafruit NeoPixel library

Connections:
 - 3.3V to mic amp +
 - GND to mic amp -
 - Analog pin to microphone output (configurable below)
 - Digital pin to LED data input (configurable below)
 See notes in setup() regarding 5V vs. 3.3V boards - there may be an
 extra connection to make and one line of code to enable or disable.

Written by Adafruit Industries.  Distributed under the BSD license.
This paragraph must be included in any redistribution.
*/

#include <Adafruit_NeoPixel.h>

#define N_PIXELS  180  // Number of pixels in strand
#define MIC_PIN   A3  // Microphone is attached to this analog pin
#define LED_PIN    A9  // NeoPixel LED strand is connected to this pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     30  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment
#define TOP       (N_PIXELS + 2) // Allow dot to go slightly off scale
#define PEAK_FALL 40  // Rate of peak falling dot

byte
  peak      = 0,      // Used for falling dot
  dotCount  = 0,      // Frame counter for delaying dot-falling speed
  volCount  = 0;      // Frame counter for storing past volume data
int
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 10,      // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;
Adafruit_NeoPixel
  strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  // This is only needed on 5V Arduinos (Uno, Leonardo, etc.).
  // Connect 3.3V to mic AND TO AREF ON ARDUINO and enable this
  // line.  Audio samples are 'cleaner' at 3.3V.
  // COMMENT OUT THIS LINE FOR 3.3V ARDUINOS (FLORA, ETC.):
//  analogReference(EXTERNAL);

  memset(vol, 0, sizeof(vol));
  strip.begin();
}

void loop() {
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;
  int CHEST_MAX = 40
  int CENTER_MAX = 0 + LEFT_MAX;		//strip inside the hood
  int RIGHT_MAX = 0 + CHEST_MAX;		//strip on the right
  int LEFT_MAX = 0 + CENTER_MAX;	//strip on the left
;		//set of chest strips
 

  n   = analogRead(MIC_PIN);                        // Raw reading from mic 
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top


  // Color pixels based on rainbow gradient
  int j = 0;
  int x = 0;					//counters the loop
  int checkCounter = 0;				//end position for pixel update
  if (checkCounter % 15 = 0) {
      dither(strip.Color(0,127,127), 20);       // cyan, slow
      dither(strip.Color(0,0,0), 15);           // black, fast
      dither(strip.Color(127,0,127), 20);       // magenta, slow
      dither(strip.Color(0,0,0), 15);           // black, fast
      dither(strip.Color(127,127,0), 20);       // yellow, slow
      dither(strip.Color(0,0,0), 15);
  } else if (checkCounter % 10 = 0) {
          rainbow(0, 0, CHEST_MAX);
          rainbow(0,CHEST_MAX+1, RIGHT_MAX);
          rainbow(0, RIGHT_MAX+1, CENTER_MAX);
          rainbow(0, CENTER_MAX+1, LEFT_MAX)
  } else{
      while(j<N_PIXELS) {          //BRING IF FUNCTIONS OUTSIDE OF THIS LOOP?
          int start_point = 0;      //start position for pixel update
          int end_point = start_point + 2*j + 2;    //set end of bucket

        if(j >= height && j <= CHEST_MAX) {
   		     strip.setPixelColor(i,   0,   0, 0); //blank out color if noise level is lower than i

        } else if(j>CHEST_MAX && j <= RIGHT_MAX) {
   		    //DO SOMETHING CRAZY
            visualizer(end_point+1, RIGHT_MAX);
        } else if(j>RIGHT_MAX && j<= CENTER_MAX) {
            //do something less crazy
            wave(strip.Color(0,0,100), 2, 40);
        } else if(j>CENTER_MAX) {
            //last strip to update
            visualizer(N_PIXELS * -1, CENTER_MAX * -1)
        } else {
            visualizer(start_point, end_point);
        }
      }
      j++;
      strip.show(); // Update strip
    }
  	checkCounter++;
    
  }
  

 

  /* Draw peak dot  
  if(peak > 0 && peak <= N_PIXELS-1) strip.setPixelColor(peak,Wheel(map(peak,0,strip.numPixels()-1,30,150)));
  
   

// Every few frames, make the peak pixel drop by 1:

    if(++dotCount >= PEAK_FALL) { //fall rate 
      
      if(peak > 0) peak--;
      dotCount = 0;
    }
*/

  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Color Functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void wave(uint32_t c, int cycles, uint8_t wait) {
  float y;
  byte  r, g, b, r2, g2, b2;

  // Need to decompose color into its r, g, b elements
  g = (c >> 16) & 0x7f;
  r = (c >>  8) & 0x7f;
  b =  c        & 0x7f; 

      for(int i=0; i<strip.numPixels(); i++) {
      y = sin(PI * (float)cycles * (float)(x + i) / (float)strip.numPixels());
      if(y >= 0.0) {
        // Peaks of sine wave are white
        y  = 1.0 - y; // Translate Y to 0.0 (top) to 1.0 (center)
        r2 = 127 - (byte)((float)(127 - r) * y);
        g2 = 127 - (byte)((float)(127 - g) * y);
        b2 = 127 - (byte)((float)(127 - b) * y);
      } else {
        // Troughs of sine wave are black
        y += 1.0; // Translate Y to 0.0 (bottom) to 1.0 (center)
        r2 = (byte)((float)r * y);
        g2 = (byte)((float)g * y);
        b2 = (byte)((float)b * y);
      }
      strip.setPixelColor(i, r2, g2, b2);
    }
    delay(wait);
}

void dither(uint32_t c, uint8_t wait) {

  // Determine highest bit needed to represent pixel index
  int hiBit = 0;
  int n = strip.numPixels() - 1;
  for(int bit=1; bit < 0x8000; bit <<= 1) {
    if(n & bit) hiBit = bit;
  }

  int bit, reverse;
  for(int i=0; i<(hiBit << 1); i++) {
    // Reverse the bits in i to create ordered dither:
    reverse = 0;
    for(bit=1; bit <= hiBit; bit <<= 1) {
      reverse <<= 1;
      if(i & bit) reverse |= 1;
    }
    strip.setPixelColor(reverse, c);
    strip.show();
    delay(wait);
  }
  delay(250); // Hold image for 1/4 sec
}

/*void rainbowCycle(uint8_t wait, strip_section) {
  uint16_t i, j, location;

//  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      location = i + strip_section
      strip.setPixelColor(location, Wheel(((location * 384 / strip.numPixels()) + j) % 384));
    }
//    strip.show();   // write all the pixels out
    delay(wait);
  }

void visualizer(first, last) {


    for (int x = first; x<= last; x++) {     //loop through desired pixels
          strip.setPixelColor(abs(x),Wheel(map(abs(x),0,strip.numPixels()-1,30,150)));

    }
} */
void rainbow(uint8_t wait, section_start, section_end {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=section_start; i<section_end; i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
//    strip.show();
    delay(wait);
  }
}


/*~~~~~~~~~~~~~~~Color Support~~~~~~~~~~*/
// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
