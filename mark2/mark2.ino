#include <Adafruit_NeoPixel.h>

#define N_PIXELS  160  // Number of pixels in strand
#define LED_PIN    6  // NeoPixel LED strand is connected to this pin
#define MIC_PIN    8
#define buttonPin  4 //button pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     30  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment
#define CHEST_MAX 40
#define CENTER_MAX 60    //strip inside the hood
#define RIGHT_MAX 80    //strip on the right
#define LEFT_MAX 100 //strip on the left

Adafruit_NeoPixel
  strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int mode = 0;
int barPixels = RIGHT_MAX;
int chestPixels = CHEST_MAX - RIGHT_MAX;

void setup() {
	strip.begin();
	pinMode(buttonPin, INPUT);
}


void loop() {
	checkButton();
	if (mode = 0){
		dither(strip.Color(127, 127, 0), 20);
		dither(strip.Color(127, 127, 127), 20);
	} else if (mode = 1) {
		visualizer();
	} else {
	for(int i=0; i<12; i++){
			if(i<=3) {
				colorWipe(strip.Color(127,0,0), 10);
    			  //colorWipe(strip.Color(255,69,0), 20);
				colorWipe(strip.Color(255,215,0), 10);      // red
			} else if(i>3 && i<=6){
				wave(strip.Color(238,0,0), 4, 20);

			} else if(i>6 && i<=9) {
				rainbow(20);
				scanner(100,0,0, 20);
				//dither(strip.Color(0,127,127), 10);       
	  			rainbow(20);
                        //dither(strip.Color(127,0,127), 10);       
                       // dither(strip.Color(127,127,0), 10);
	  			colorWipe(strip.Color(54,54,54), 20);       
			} else if(i>9 && i<=12) {
				wave(strip.Color(127,0,0), 4, 20);        // candy cane
				rainbow(30);
	  			//wave(strip.Color(0,0,100), 2, 20);
  				//colorWipe(strip.Color(127,127,127), 20);
			}

		}
	}

	
}

void visualizer() {
    int n   = analogRead(MIC_PIN);                        // Raw reading from mic 
    n   = abs(n - 512 - DC_OFFSET); // Center on zero
    n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
    int barHeight = n/512 * barPixels;
    int chestHeight = n/512 * chestPixels;
    int j = 0;
    while( j<N_PIXELS) {          //BRING IF FUNCTIONS OUTSIDE OF THIS LOOP?
        if(j >= 0 && j <= RIGHT_MAX) {
            	strip.setPixelColor(j,   0,   0, 0); //blank out color if noise level is lower than i
                for (int x = 0; x < barHeight; x++) {
              		strip.setPixelColor(abs(x),Wheel(map(abs(x),0,strip.numPixels()-1,30,150)));
                  j++;
          }         	
        	j++;
        } else if(j>RIGHT_MAX && j <= CHEST_MAX) {
                strip.setPixelColor(j,   0,   0, 0); //blank out color if noise level is lower than i
        	for (int x = RIGHT_MAX+1; x < chestHeight; x++) {
        		strip.setPixelColor(abs(x),Wheel(map(abs(x),0,strip.numPixels()-1,30,150)));
            j++;
          }
        	j++;
        } else if(j>CHEST_MAX && j <= LEFT_MAX) {
                strip.setPixelColor(LEFT_MAX + CHEST_MAX - j,   0,   0, 0); //blank out color if noise level is lower than i
        	for (int x = LEFT_MAX; x < (LEFT_MAX-barHeight); x--) {
        		strip.setPixelColor(abs(x),Wheel(map(abs(x),0,strip.numPixels()-1,30,150)));
            j++;
        	} 
        	j++;
        } else {
            strip.setPixelColor(j,   0,   0, 127); 
        }
        strip.show();
    }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel(((i * 384 / strip.numPixels()) + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
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

// An "ordered dither" fills every pixel in a sequence that looks
// sparkly and almost random, but actually follows a specific order.
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

// "Larson scanner" = Cylon/KITT bouncing light effect
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  int i, j, pos, dir;

  pos = 0;
  dir = 1;

  for(i=0; i<((strip.numPixels()-1) * 2); i++) {
    // Draw 5 pixels centered on pos.  setPixelColor() will clip
    // any pixels off the ends of the strip, no worries there.
    // we'll make the colors dimmer at the edges for a nice pulse
    // look
    strip.setPixelColor(pos - 2, strip.Color(r/4, g/4, b/4));
    strip.setPixelColor(pos - 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos, strip.Color(r, g, b));
    strip.setPixelColor(pos + 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos + 2, strip.Color(r/4, g/4, b/4));

    strip.show();
    delay(wait);
    // If we wanted to be sneaky we could erase just the tail end
    // pixel, but it's much easier just to erase the whole thing
    // and draw a new one next time.
    for(j=-2; j<= 2; j++) 
        strip.setPixelColor(pos+j, strip.Color(0,0,0));
    // Bounce off ends of strip
    pos += dir;
    if(pos < 0) {
      pos = 1;
      dir = -dir;
    } else if(pos >= strip.numPixels()) {
      pos = strip.numPixels() - 2;
      dir = -dir;
    }
  }
}

// Sine wave effect  
#define PI 3.14159265
void wave(uint32_t c, int cycles, uint8_t wait) {
  float y;
  byte  r, g, b, r2, g2, b2;

  // Need to decompose color into its r, g, b elements
  g = (c >> 16) & 0x7f;
  r = (c >>  8) & 0x7f;
  b =  c        & 0x7f; 

  for(int x=0; x<(strip.numPixels()*5); x++)
  {
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
    strip.show();
    delay(wait);
  }
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g - b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(strip.Color(r,g,b));
}



void checkButton(){

	int buttonState = digitalRead(buttonPin);
  int oldbuttonState;
  if(buttonState != oldbuttonState) {
  	if (mode > 2) {
		  mode = 0;
    }
    if (buttonState = HIGH) {
		  mode++;
    }
  }
  oldbuttonState = buttonState;
}