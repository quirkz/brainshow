#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
    #include <avr/power.h>
#endif

#define STRIP_LEN       144
#define STRIP_CNT       1

#define NEURON_PIN_0    2
#define NEURON_PIN_1    3
#define NEURON_PIN_2    4

#define NEURON_CNT_0    STRIP_LEN
#define NEURON_CNT_1    0
#define NEURON_CNT_2    0

uint32_t lens[] = { NEURON_CNT_0, NEURON_CNT_1, NEURON_CNT_2 };



/* Parameter 1 = number of pixels in strip */
/* Parameter 2 = Arduino pin number (most are valid) */
/* Parameter 3 = pixel type flags, add together as needed: */
/*   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs) */
/*   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers) */
/*   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products) */
/*   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2) */
/*   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products) */
Adafruit_NeoPixel strips[] = {
    Adafruit_NeoPixel( NEURON_CNT_0, NEURON_PIN_0, NEO_GRB + NEO_KHZ800 ) };
/*  Adafruit_NeoPixel(NEURON_CNT_1, NEURON_PIN_1, NEO_GRB + NEO_KHZ800), */
/*  Adafruit_NeoPixel(NEURON_CNT_2, NEURON_PIN_2, NEO_GRB + NEO_KHZ800) }; */


enum TraceState
{
    FINISHED,
    RUNNING
};

#define TRACE_CNT    5

struct Trace
{
    uint32_t len;
    uint32_t seg_len;
    uint32_t dir;
    int32_t move_speed;
    uint32_t cycle_speed;
    uint32_t pause;
    uint32_t color;
    int32_t pos;
    int strip;
}
traces[ TRACE_CNT ];


#define MAX_SPARKLES    20

struct Sparkle
{
    int16_t pos;
    uint32_t color;
    float rads;
    float inc;
}
sparkles[ MAX_SPARKLES ];

const float Pi = 3.14159;

void modulateNeuralInterferance( int strip_idx )
{
    Adafruit_NeoPixel * strip = &strips[ strip_idx ];

    for( int i = 0; i < MAX_SPARKLES; i += 1 )
    {
        if( sparkles[ i ].pos < 0 )
        {
            int err_occured = ( random( 0, 1000 ) / 995 ) > 0;

            if( err_occured )
            {
                sparkles[ i ].pos = random( 0, strip->numPixels() );
                sparkles[ i ].rads = 0;
                sparkles[ i ].inc = ( float ) random( 1, 10 ) / 100.0;
            }
        }
        else
        {
            sparkles[ i ].rads += sparkles[ i ].inc;

            if( sparkles[ i ].rads < Pi )
            {
                uint16_t bright = ( uint16_t ) ( sin( sparkles[ i ].rads ) * 127.0 );

                uint32_t color = strip->Color( bright, bright, bright );
                strip->setPixelColor( sparkles[ i ].pos, color );
            }
            else
            {
                sparkles[ i ].pos = -1;
            }
        }
    }
}



void generateNeuralSignal( int trace_idx,
                           int strip )
{
    struct Trace t;

    memset( &t, 0, sizeof( struct Trace ) );

    t.strip = strip;
    t.seg_len = random( 2, 8 );
    t.len = t.seg_len * 2;
    t.dir = random( 0, 2 );
    t.pos = random( 10, 290 ); /* t.dir > 0 ? 0 : (lens[trace_idx] - t.len - 1); */
    t.move_speed = random( 1, 5 );
    t.cycle_speed = random( 1, 3 );
    t.pause = millis() + random( 50, 1500 );
    uint32_t max_clr = 127 / 2;
    t.color = strips[ 0 ].Color( random( 0, max_clr ), random( 0, max_clr ), random( 0, max_clr ) );

    memcpy( &traces[ trace_idx ], &t, sizeof( struct Trace ) );
}


int moveNeuralSignal( int trace_idx )
{
    struct Trace * t = &traces[ trace_idx ];
    Adafruit_NeoPixel * strip = &strips[ t->strip ];

    uint32_t pos = t->pos;
    uint32_t seg_len = t->seg_len;
    byte wheel_pos = t->color;
    uint32_t i;

    if( ( t->pos >= strip->numPixels() ) || ( t->pos < 0 ) )
    {
        return t->pause > millis();
    }

    /*t->move_speed += random(0,100) / 97; */

    if( t->dir > 0 )
    {
        t->pos += t->move_speed;
    }
    else
    {
        t->pos -= t->move_speed;
    }

    pos = t->pos;

    /* segment start one color wheel to middle */
    byte wheel_step = 0x10;

    for( i = 0; i < seg_len; i += 1, pos += 1, wheel_pos += wheel_step )
    {
        strip->setPixelColor( pos, Wheel( wheel_pos, strip ) );
    }

/*
 * // segment middle is random
 * uint32_t max_clr = 127 / random(1,4), random_clr;
 * random_clr = strip->Color(random(0,max_clr), random(0,max_clr), random(0,max_clr));
 * for (i = 0; i < seg_len; i += 1, pos += 1, max_clr += 1) {
 *  random_clr = strip->Color(random(0,max_clr), random(0,max_clr), random(0,max_clr));
 *  strip->setPixelColor(pos, random_clr);
 * } */

    /* fade out with wheel */
    /*pos += seg_len - 1; */
    for( i = 0; i < seg_len; i += 1, pos += 1, wheel_pos -= wheel_step )
    {
        strip->setPixelColor( pos, Wheel( wheel_pos, strip ) );
    }

    return 1;
}


/* Input a value 0 to 255 to get a color value. */
/* The colours are a transition r - g - b - back to r. */
uint32_t Wheel( byte WheelPos,
                Adafruit_NeoPixel * strip )
{
    WheelPos = 255 - WheelPos;

    if( WheelPos < 85 )
    {
        return strip->Color( 255 - WheelPos * 3, 0, WheelPos * 3 );
    }

    if( WheelPos < 170 )
    {
        WheelPos -= 85;
        return strip->Color( 0, WheelPos * 3, 255 - WheelPos * 3 );
    }

    WheelPos -= 170;
    return strip->Color( WheelPos * 3, 255 - WheelPos * 3, 0 );
}

void setup()
{
    Serial.begin( 9600 );
    randomSeed( analogRead( 0 ) );

    memset( &sparkles[ 0 ], 0, sizeof( sparkles ) );

    for( int j = 0; j < MAX_SPARKLES; j += 1 )
    {
        sparkles[ j ].pos = -1;
    }

    pinMode( NEURON_PIN_0, INPUT_PULLUP );
    pinMode( NEURON_PIN_1, INPUT_PULLUP );
    pinMode( NEURON_PIN_2, INPUT_PULLUP );

    for( int i = 0; i < STRIP_CNT; i += 1 )
    {
        strips[ i ].begin();
        strips[ i ].show(); /* Initialize all pixels to 'off' */
    }

    for( int j = 0; j < TRACE_CNT; j += 1 )
    {
        generateNeuralSignal( j, 0 );
    }
}

void sparkle_chaser_loop() {

    for( int j = 0; j < STRIP_CNT; j += 1 )
    {
        strips[ j ].clear();

        for( int i = 0; i < TRACE_CNT; i += 1 )
        {
            if( !moveNeuralSignal( i ) )
            {
                generateNeuralSignal( i, 0 );
            }
        }

        modulateNeuralInterferance( j );

        strips[ j ].show();
    }

    delay( 20 );

}

const byte max_clr = 127 / 2;
static byte wheel_pos  = 0; // strips[ 0 ].Color( random( 0, max_clr ), random( 0, max_clr ), random( 0, max_clr ) );

void color_blend_full() {

    strips[ 0 ].clear();

    byte wheel_step = 1;

    wheel_pos += wheel_step;
    
    for( uint32_t pos = 0; pos < STRIP_LEN; pos += 1 )
    {
        strips[0].setPixelColor( pos % STRIP_LEN, Wheel( wheel_pos, &strips[0] ) );
    }

    strips[ 0 ].show();

    delay(16);
}

static uint32_t pos_l = 0, pos_r = 0;

void color_crawl_from_midle() {

    uint32_t pos_middle = STRIP_LEN / 2;

    strips[ 0 ].clear();

    byte wheel_step = 1;

    wheel_pos += random( 0, 4 );
    
    uint32_t pos;
    for( pos = pos_middle; pos < pos_r; pos += 1 )
    {
        strips[0].setPixelColor( pos % STRIP_LEN, Wheel( wheel_pos, &strips[0] ) );
    }

    for( pos = pos_middle - 1; pos >= pos_l; pos -= 1 )
    {
        strips[0].setPixelColor( pos % STRIP_LEN, Wheel( wheel_pos, &strips[0] ) );
    }

    pos_r += random( -2, 3 );
    pos_l += random( -3, 2 );

    if ((pos_r > STRIP_LEN) || (pos_r < (STRIP_LEN / 2))
    {
        pos_r = STRIP_LEN / 2;
    }

    if ((pos_l > STRIP_LEN) || (pos_l > (STRIP_LEN / 2 - 1))
    {
        pos_l = STRIP_LEN / 2 - 1;
    }

    strips[ 0 ].show();

    delay(64);
}


static uint32_t ms_prev = 0;
static int loop_mode = 0;
const int mode_count = 2;

void loop()
{
    const uint32_t ms_switch = 16 * 1024;

    uint32_t t_now = millis();

    bool switch_mode = (t_now - ms_prev) > ms_switch;

    if (switch_mode) {
        loop_mode += 1;
        if (loop_mode >= mode_count)
            loop_mode = 0;

        pos_l = STRIP_LEN / 2 - 1;
        pos_r = pos_l + 1;

        ms_prev = t_now;
    }

    switch (loop_mode) {
        case 0:
            sparkle_chaser_loop();
            break;
        case 1:
            color_blend_full();
            break;
        default:
        break;
    }
}
