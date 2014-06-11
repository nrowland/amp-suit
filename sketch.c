#include 

#define N_PIXELS  180  // Number of pixels in strand
#define MIC_PIN   A3  // Microphone is attached to this analog pin
#define LED_PIN    A9  // NeoPixel LED strand is connected to this pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     30  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment

// Holds state of LED in the grid.
struct led {
  bool on;
  byte r;
  byte g;
  byte b;
  int address;
};

// assuming grid looks like this:
// **********
//  ********
//   ******
//    ****

led* grid[5]; // Holds the state of the led 5 grid rows.
led row0[2]; // The bottom row has two leds.
grid[0] = row0;
led row1[4]; // The next row has four leds.
grid[1] = row1;
led row2[6]; // And so on.
grid[2] = row2;
led row3[8];
grid[3] = row3;
led row4[10];
grid[4] = row4;

// Initialize the grid
void setup {
  // loop through the grid and setup the default values for each led.
  int current_addr = 0;
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < sizeof(grid[i]) / sizeof(grid[i][0]); j++) {
      grid[i][j].address = current_addr++;
      // set the default color
      grid[i][j].r = 255;
      grid[i][j].g = 255;
      grid[i][j].b = 255;
      // change default state to off
      grid[i][j].on = false;
    }
  }

  // grid[0][1] will give you the state of the second led of the bottom row
}
void visualizer() {

}
void loop {
  // read sound input. Let's say it ends up being level 3;
  float n   = analogRead(MIC_PIN);                        // Raw reading from mic 
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  n = n/512 * 100;
  int sound_level = round(n/20);

  int i = 0;
  for (; i < sound_level; i++) {
    turn_on_row(i);
  }
  for (; i < 5; i++) {
    turn_off_row(i);
  }

  load_grid_state();
}

// Set the state of a row to on.
void turn_on_row(int row) {
  for (int i = 0; i < sizeof(grid[row]) / sizeof(grid[row][0]), j++) {
    grid[row][i].on = true;
  }
}

// Set the state of a row to off.
void turn_off_row(int row) {
  for (int i = 0; i < sizeof(grid[row]) / sizeof(grid[row][0]), j++) {
    grid[row][i].on = false;
  }
}

// Uploads the grids state to the LEDS
void load_grid_state() {
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < sizeof(grid[i]) / sizeof(grid[i][0]), j++) {
      strip.setPixelColor(grid[i][j].address, grid[i][j].r, grid[i][j].g, grid[i][j].b)
      if (grid[i][j].on) {
        strip.show();
      } else {
        strip.setPixelColor(grid[i][j].address,0,0,0);
        strip.show();
      }
    }
  }
}
