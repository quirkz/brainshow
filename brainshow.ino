#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define STRIP_LEN 300
#define STRIP_CNT 1

#define NEURON_PIN_0 2
#define NEURON_PIN_1 3
#define NEURON_PIN_2 4

#define NEURON_CNT_0 STRIP_LEN
#define NEURON_CNT_1 0
#define NEURON_CNT_2 0

uint32_t lens[] = { NEURON_CNT_0, NEURON_CNT_1, NEURON_CNT_2 };



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
  RUNNING
};

struct Trace {
  uint32_t len;
  uint32_t seg_len;
  uint32_t dir;
  int32_t move_speed;
  uint32_t cycle_speed;
  uint32_t pause_time;
  uint32_t color;
  int32_t pos;
} traces[STRIP_CNT];

void generateNeuralSignal(int trace_idx) {
  struct Trace t;
  
  memset( &t, 0, sizeof(struct Trace) );

  t.seg_len = random( 2, 7 );
  t.len = t.seg_len * 4;
  t.dir = random( 0, 1 ) ? -1 : 1;
  t.move_speed = random(1, 3);
  t.cycle_speed = random(1, 3);
  t.pause_time = random( 1, 5);
  uint32_t max_clr = 127 / 2;
  t.color = strips[0].Color(random(0,max_clr), random(0,max_clr), random(0,max_clr));
  
  memcpy( &traces[trace_idx], &t, sizeof(struct Trace) );
}

int moveNeuralSignal(int trace_idx) {
  strips[trace_idx].clear();
  
  struct Trace *t = &traces[trace_idx];  
  uint32_t pos = t->pos;
  uint32_t seg_len = t->seg_len;
  uint32_t clr = t->color;
  uint32_t i;
    
  t->move_speed += random(0,100) / 95;
  t->move_speed = min(4, t->move_speed);
  
  t->pos += t->move_speed;
  pos = t->pos;
  
  if (pos < 0)
    pos = 0;

  // segment start one color wheel to middle
  for (i = 0; i < seg_len; i += 1, pos += 1) {
    strips[trace_idx].setPixelColor(pos, clr);
    clr = Wheel( clr, strips );
  }

  // segment middle is random 
  uint32_t max_clr = 127 / random(1,4), random_clr;
  random_clr = strips[i].Color(random(0,max_clr), random(0,max_clr), random(0,max_clr));
  for (i = 0; i < 2*seg_len; i += 1, pos += 1, max_clr += 1) {
    random_clr = strips[i].Color(random(0,max_clr), random(0,max_clr), random(0,max_clr));
    strips[trace_idx].setPixelColor(pos, random_clr);
  }
  
  // fade out with wheel
  clr = t->color;
  pos += seg_len - 1;
  for (i = 0; i < seg_len; i += 1, pos -= 1) {
    strips[trace_idx].setPixelColor(pos, clr);
    clr = Wheel( clr, strips );
  }

  return pos < lens[trace_idx];
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
  Serial.begin(9600);  
  randomSeed(analogRead(0));
  
  pinMode(NEURON_PIN_0, INPUT_PULLUP);
  pinMode(NEURON_PIN_1, INPUT_PULLUP);
  pinMode(NEURON_PIN_2, INPUT_PULLUP);
  
  for (int i = 0; i < STRIP_CNT; i += 1) {
    strips[i].begin();    
    strips[i].show(); // Initialize all pixels to 'off'
  }
 
  for (int i = 0; i < STRIP_CNT; i += 1) 
      generateNeuralSignal(i);
}

void loop() {
//  stripTest();
  
  Serial.print('loop start  ');
  for (int i = 0; i < STRIP_CNT; i += 1) {
    if (!moveNeuralSignal(i))
      generateNeuralSignal(i);
    strips[i].show();
  }
  Serial.println('.. end');
}

