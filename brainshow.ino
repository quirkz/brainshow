#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEURON_PIN_0 2
#define NEURON_PIN_1 3
#define NEURON_PIN_2 4

#define NEURON_CNT_0 300
#define NEURON_CNT_1 0
#define NEURON_CNT_2 0

#define STRIP_LEN 300

#define STRIP_CNT 1


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strips[] =  { 
  Adafruit_NeoPixel(NEURON_CNT_0, NEURON_PIN_0, NEO_GRB + NEO_KHZ800) };
//  Adafruit_NeoPixel(NEURON_CNT_1, NEURON_PIN_1, NEO_GRB + NEO_KHZ800),
//  Adafruit_NeoPixel(NEURON_CNT_2, NEURON_PIN_2, NEO_GRB + NEO_KHZ800) };


enum TraceState {
  FINISHED,
  RUNNING,
  PAUSED,
};

struct Trace {
  int len;
  int dir;
  int move_speed;
  int cycle_speed;
  int pause_time;
  int color;
  int start;
  int state;
  int counter;
} traces[STRIP_CNT];

void fireNeuron() {

}

void generateNeuralSignal(int trace_idx) {
  struct Trace t;

  t.len = random( 10, 30 );
  t.dir = random( 0, 1 ) ? -1 : 1;
  t.move_speed = random(1, 3);
  t.cycle_speed = random(1, 3);
  t.pause_time = random( 1, 5);
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, Adafruit_NeoPixel *strip) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void stripTest() {
  static int32_t pixel = -1;
  
  pixel += 1;
  
  if (pixel >= STRIP_LEN) {
    pixel = 0; 
  }
  
  int32_t mru = pixel - 1;
  
  if (mru < 0)
    mru = STRIP_LEN - 1;
  
  for (int i = 0; i < STRIP_CNT; i += 1) {
    uint32_t off = strips[i].Color(0, 0, 0);
    
    strips[i].setPixelColor(mru, off);
    uint32_t random_clr = strips[i].Color(random(0,127), random(0,127), random(0,127));
    strips[i].setPixelColor(pixel, random_clr);
    random_clr = strips[i].Color(random(0,127), random(0,127), random(0,127));
    strips[i].setPixelColor(pixel + 1, random_clr);
    random_clr = strips[i].Color(random(0,127), random(0,127), random(0,127));
    strips[i].setPixelColor(pixel + 2, random_clr);
    strips[i].show(); // Initialize all pixels to 'off'
  }
}


void setup() {
//  Serial.begin(9600);  
  randomSeed(analogRead(0));
  
  pinMode(NEURON_PIN_0, INPUT_PULLUP);
  pinMode(NEURON_PIN_1, INPUT_PULLUP);
  pinMode(NEURON_PIN_2, INPUT_PULLUP);
  
  for (int i = 0; i < STRIP_CNT; i += 1) {
    strips[i].begin();    
    strips[i].show(); // Initialize all pixels to 'off'
  }
}

void loop() {

  stripTest();
  
  for (int i = 0; i < STRIP_CNT; i += 1) {
    if (traces[i].state = FINISHED) {
    
    } else if (traces[i].state == RUNNING) {
          
    } else if (traces[i].state == PAUSED) {

    } 		
  }

}

