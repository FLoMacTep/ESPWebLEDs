// FastLED setup -----FastLED has to be declared BEFORE the Webserver-----
//#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

FASTLED_USING_NAMESPACE
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN      5    // Usually I use this pin (WittyCloud-GPIO14)
#define LED_TYPE      WS2812B
#define COLOR_ORDER   GRB
#define NUM_LEDS      64  // Number of LEDs 
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
#define DENSITY             80 //80 before
#define BRIGHTNESS          64
#define FRAMES_PER_SECOND   55 //80 before
#define SPARKING 120 // 4 Fire
#define COOLING  55 // 4 Fire
#define TEMPERATURE_1 Tungsten100W // 4 Color Temperature
#define TEMPERATURE_2 OvercastSky // 4 Color Temperature
#define DISPLAYTIME 20 // 4 Color Temperature
#define BLACKTIME   3 // 4 Color Temperature
#define MAX_INT_VALUE 65536 //For private effects
bool gReverseDirection = false;
int ledMode = 1;            //2 this is the starting animation

// Variables 4 LED animations ###############################################################################

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t startIndex = 0; // rotating number used by many of the patterns for palettes
uint16_t frame = 0;
uint16_t animateSpeed = 270;            //Number of frames to increment per loop!!!!!!! 80 previous
uint8_t  animation = 6;    //Active animation!!!!! 6 previous
uint8_t brightness = 50;    //Global brightness percentage
char CPalette;
const CRGB lightcolor(8,7,1);
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
uint8_t bpm_var = 30; //4 Dot effect
uint8_t fadeval = 224; //4 Dot effect
int thisdelay = 10; //4 Move effect
uint8_t maxChanges = 24;  //4 Noise effect
//currentBlending = LINEARBLEND;

//Variables 4 LED animations ###############################################################################

//MODES ON OFF ###############################################################################
void all_off() {
  fill_solid(leds, NUM_LEDS, CRGB::Black); 
  show_at_max_brightness_for_power();
 // delay_at_max_brightness_for_power(1000/FRAMES_PER_SECOND);   
  //FastLED.show();
  //FastLED.delay(1000/FRAMES_PER_SECOND); 
}
//MODES ON OFF ###############################################################################


//#######################################################################################################
//##                                PRIMATIVE ANIMATION FUNCTIONS                                      ##
//#######################################################################################################

//For TwinkleRandom effect
void setPixel(int Pixel, byte red, byte green, byte blue)
{
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;

}
//For TwinkleRandom effect
void setAll(byte red, byte green, byte blue)
{
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  FastLED.show();
}

// This function fills the palette with totally random colors.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function fills the palette with totally random colors.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}

// This function fills the palette with totally random colors.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Red,
    CRGB::Black,
    
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Red,
    CRGB::Black,
    
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Red,
    CRGB::Red,
    CRGB::Black,
    CRGB::Black
};



//************************          For Palletes effect          ******************************
// Len pisat
void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

//***********************************************************************************


//************************          For ColorWipe effect          ******************************
//For ColorWipe effect
void colorWipe(byte red, byte green, byte blue, int SpeedDelay)
{
  for(uint16_t i=0; i<NUM_LEDS; i++) {
      setPixel(i, red, green, blue);
      FastLED.show();
      delay(SpeedDelay);
  }
}
//***********************************************************************************

//************************          CylonEffect           ******************************
//For Cylon effect
void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

//***********************************************************************************

//************************          drawFractionalBar           ******************************
//Anti-aliasing code care of Mark Kriegsman Google+: https://plus.google.com/112916219338292742137/posts/2VYNQgD38Pw
void drawFractionalBar(CRGB targetStrip[], int pos16, int width, uint8_t hue, bool wrap)
{
  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  uint8_t i = pos16 / 16; // convert from pos to raw pixel number

  uint8_t frac = pos16 & 0x0F; // extract the 'factional' part of the position
  uint8_t firstpixelbrightness = 255 - (frac * 16);
  
  uint8_t lastpixelbrightness = 255 - firstpixelbrightness;

  uint8_t bright;
  for (int n = 0; n <= width; n++) {
    if (n == 0) {
      // first pixel in the bar
      bright = firstpixelbrightness;
    }
    else if (n == width) {
      // last pixel in the bar
      bright = lastpixelbrightness;
    }
    else {
      // middle pixels
      bright = 255;
    }

    targetStrip[i] += CHSV(hue, 255, bright );
    i++;
    if (i == stripLength)
    {
      if (wrap == 1) {
        i = 0; // wrap around
      }
      else{
        return;
      }
    }
  }
}
//***********************************************************************************

//*********************     Bounce      ***************************
// Linear "Larson scanner" (or knight rider effect) with anti-aliasing
// Color is determined by "hue"
//*****************************************************************
void Bounce(CRGB targetStrip[], uint16_t animationFrame, uint8_t hue)
{
  uint16_t pos16;
  if (animationFrame < (MAX_INT_VALUE / 2))
  {
    pos16 = animationFrame * 2;
  
  }else{
    pos16 = MAX_INT_VALUE - ((animationFrame - (MAX_INT_VALUE/2))*2);
  }

  int position = map(pos16, 0, MAX_INT_VALUE, 0, ((NUM_LEDS) * 16));
  drawFractionalBar(targetStrip, position, 3, hue,0);
}

//***********************************************************************************

//************************          Ring           ******************************
// Anti-aliased cyclical chaser, 3 pixels wide
// Color is determined by "hue"
//*****************************************************
void Ring(CRGB targetStrip[], uint16_t animationFrame, uint8_t hue)
{
  uint8_t stripLength = NUM_LEDS; ///!!!! sizeof(leds)/sizeof(CRGB);
  int pos16 = map(animationFrame, 0, MAX_INT_VALUE, 0, ((stripLength) * 16));
  drawFractionalBar(targetStrip, pos16, 3, hue,1);
}
//***********************************************************************************

//***************************   Wave [Float Math]  *******************************
// Squeezed sine wave  
// Uses slow, Arduino sin() function
// Squeezing achieved by using an exponential (^8) sin value
// Color is determined by "hue"

void Wave(CRGB targetStrip[], uint16_t animationFrame, uint8_t hue){
  FastLED.clear();    //Clear previous buffer
  float deg; 
  float value; 
  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  for(uint8_t i=0;i<stripLength;i++)
  {
    deg=float(animationFrame+((MAX_INT_VALUE/stripLength)*i))/(float(MAX_INT_VALUE)) * 360.0;
    value = pow(sin(radians(deg)),8);    //Squeeeeeeze

    if(value>=0){   //Chop sine wave (no negative values)
      targetStrip[i] += CHSV(hue,255,value*256);
    }
  } 
}
//***********************************************************************************

//***************************   Wave [Integer Math]  *******************************
// unadulterated sine wave.  
// Uses FastLED sin16() and no float math for efficiency. 
// Since im stuck with integer values, exponential wave-forming is not possible (unless i'm wrong???)
// Color is determined by "hue"

void WaveInt(CRGB targetStrip[], uint16_t animationFrame, uint8_t hue){
  FastLED.clear();
  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  uint8_t value;
  for(uint8_t i=0;i<stripLength;i++)
  {
    value=(sin16(animationFrame+((MAX_INT_VALUE/stripLength)*i)) + (MAX_INT_VALUE/2))/256;   
    if(value>=0){   
      targetStrip[i] += CHSV(hue,255,value);
    }
  } 
}
//***********************************************************************************

//********************************   Color Spark  ***********************************
// Color of the sparks is determined by "hue"
// Frequency of sparks is determined by global var "animateSpeed"
// "animateSpeed" var contrained from 1 - 255 (0.4% - 100%)
// "fade" parameter specifies dropoff (next frame brightness = current frame brightness * (x/256)
// fade = 256 means no dropoff, pixels are on or off
// NOTE: this animation doesnt clear the previous buffer because the fade/dropoff is a function of the previous LED state

void Spark(CRGB targetStrip[], uint16_t animationFrame,uint8_t fade, uint8_t hue){

  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  uint16_t rand = random16();

    for(int i=0;i<stripLength;i++)
    {   
      targetStrip[i].nscale8(fade);
    }


  if(rand < (MAX_INT_VALUE / (256 - (constrain(animateSpeed,1,256)))))  ;
  {
    targetStrip[rand % stripLength].setHSV(hue,255,255);
  }
}
//***********************************************************************************

//******************************       Spark       **********************************
// Same as color spark but no hue value, // in HSV white is any hue with 0 saturation
// Frequency of sparks is a percentage mapped to global var "animateSpeed"
// "animateSpeed" var contrained from 1 - 255 (0.4% - 100%)
// "fade" parameter specifies dropoff (next frame brightness = current frame brightness * (x/256)
// fade = 256 means no dropoff, pixels are on or off
// NOTE: this animation doesnt clear the previous buffer because the fade/dropoff is a function of the previous LED state

void Spark(CRGB targetStrip[], uint16_t animationFrame,uint8_t fade){
  uint8_t stripLength = sizeof(leds)/sizeof(CRGB);
  uint16_t rand = random16();
  
  for(int i=0;i<stripLength;i++)
    {   
      targetStrip[i].nscale8(fade);
    }
  

  if(rand < (MAX_INT_VALUE / (256 - (constrain(animateSpeed,1,255)))))
  {
    targetStrip[rand % stripLength].setHSV(0,0,255);
  }

}
//***********************************************************************************

//******************************    aanimation    ***********************************
void animationA() {                                             // running red stripe.

  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t red = (millis() / 10) + (i * 12);                    // speed, length
    if (red > 128) red = 0;
    leds2[i] = CRGB(red, 0, 0);
  }
} // animationA()



void animationB() {                                               // running green stripe in opposite direction.
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t green = (millis() / 5) - (i * 12);                    // speed, length
    if (green > 128) green = 0;
    leds3[i] = CRGB(0, green, 0);
  }
} // animationB()
//***********************************************************************************

//******************************    ColourFade    ***********************************
//For ColourFade effect
int fadeAmount = 5;  // Set the amount to fade I usually do 5, 10, 15, 20, 25 etc even up to 255.
int brightnesss = 0;
void FadeInOut()
{
  for(int i = 0; i < NUM_LEDS; i++ )
   {
   leds[i].setRGB(255,0,0);  // setRGB functions works by setting
                             // (RED value 0-255, GREEN value 0-255, BLUE value 0-255)
                             // RED = setRGB(255,0,0)
                             // GREEN = setRGB(0,255,0)
   leds[i].fadeLightBy(brightness);
  }
  FastLED.show();
}
//***********************************************************************************

//******************************    BeatWave    ***********************************
void beatwave() {
  
  uint8_t wave1 = beatsin8(9, 0, 255);                        // That's the same as beatsin8(9);
  uint8_t wave2 = beatsin8(8, 0, 255);
  uint8_t wave3 = beatsin8(7, 0, 255);
  uint8_t wave4 = beatsin8(6, 0, 255);

  for (int i=0; i<NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, i+wave1+wave2+wave3+wave4, 255, currentBlending); 
  }
  
} // beatwave()

//***********************************************************************************

//******************************    DotBeat    ***********************************
void dot_beat() {

  uint8_t inner = beatsin8(bpm_var, NUM_LEDS/4, NUM_LEDS/4*3);    // Move 1/4 to 3/4
  uint8_t outer = beatsin8(bpm_var, 0, NUM_LEDS-1);               // Move entire length
  uint8_t middle = beatsin8(bpm_var, NUM_LEDS/3, NUM_LEDS/3*2);   // Move 1/3 to 2/3

  leds[middle] = CRGB::Purple;
  leds[inner] = CRGB::Blue;
  leds[outer] = CRGB::Aqua;

  nscale8(leds,NUM_LEDS,fadeval);                             // Fade the entire array. Or for just a few LED's, use  nscale8(&leds[2], 5, fadeval);

} // dot_beat()

//***********************************************************************************

//******************************    Ease    ***********************************
void ease() {

  static uint8_t easeOutVal = 0;
  static uint8_t easeInVal  = 0;
  static uint8_t lerpVal    = 0;

  easeOutVal = ease8InOutQuad(easeInVal);                     // Start with easeInVal at 0 and then go to 255 for the full easing.
  easeInVal++;

  lerpVal = lerp8by8(0, NUM_LEDS, easeOutVal);                // Map it to the number of LED's you have.

  leds[lerpVal] = CRGB::Red;
  fadeToBlackBy(leds, NUM_LEDS, 16);                          // 8 bit, 1 = slow fade, 255 = fast fade
  
} // ease()

//***********************************************************************************

//******************************    noise16_3    ***********************************
void noise16_3() {                                            // no x/y shifting but scrolling along 

  uint8_t scale = 1000;                                       // the "zoom factor" for the noise

  for (uint16_t i = 0; i < NUM_LEDS; i++) {

    uint16_t shift_x = 4223;                                  // no movement along x and y
    uint16_t shift_y = 1234;

    uint32_t real_x = (i + shift_x) * scale;                  // calculate the coordinates within the noise field
    uint32_t real_y = (i + shift_y) * scale;                  // based on the precalculated positions
    uint32_t real_z = millis()*2;                             // increment z linear

    uint8_t noise = inoise16(real_x, real_y, real_z) >> 7;    // get the noise data and scale it down

    uint8_t index = sin8(noise*3);                            // map led color based on noise data
    uint8_t bri   = noise;

    leds[i] = ColorFromPalette(currentPalette, index, bri, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }
  
} // noise16_3()
//***********************************************************************************

//#######################################################################################################
//##                                PRIMATIVE ANIMATION FUNCTIONS                                      ##
//#######################################################################################################


//#########################################SPECIALFUNCTIONS#########################################

//#########################################RAINBOW#########################################
void rainbow() 
{
  // FastLED's built-in rainbow generator
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  show_at_max_brightness_for_power();
 // delay_at_max_brightness_for_power(1000/FRAMES_PER_SECOND); 
//  FastLED.show();  
//  FastLED.delay(1000/FRAMES_PER_SECOND); 
}
//#########################################RAINBOW#########################################

//#########################################GLITTER#########################################
void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
//#########################################GLITTER#########################################

//#########################################RAINBOWWITHGLITTER#########################################
void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}
//#########################################RAINBOWWITHGLITTER#########################################

//#########################################CONFETTI#########################################
void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  show_at_max_brightness_for_power();
  // delay_at_max_brightness_for_power(1000/FRAMES_PER_SECOND); 
//  FastLED.show();  
//  FastLED.delay(1000/FRAMES_PER_SECOND); 
}
//#########################################CONFETTI#########################################

//#########################################SINELON#########################################
void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
  show_at_max_brightness_for_power();
 // delay_at_max_brightness_for_power(1000/FRAMES_PER_SECOND); 
//  FastLED.show();  
//  FastLED.delay(1000/FRAMES_PER_SECOND); 
}
//#########################################SINELON#########################################

//#########################################BPM#########################################

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  show_at_max_brightness_for_power();
 // delay_at_max_brightness_for_power(1000/FRAMES_PER_SECOND); 
//  FastLED.show();  
//  FastLED.delay(1000/FRAMES_PER_SECOND); 
}
//#########################################BPM#########################################

//#########################################JUGGLE#########################################
void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  show_at_max_brightness_for_power();
  // delay_at_max_brightness_for_power(1000/FRAMES_PER_SECOND); 
//  FastLED.show();  
//  FastLED.delay(1000/FRAMES_PER_SECOND); 
}
//#########################################JUGGLE#########################################

//#########################################TwinkleRandom#########################################
void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
     FastLED.show();
     delay(SpeedDelay);
     if(OnlyOne) { 
       setAll(0,0,0); 
     }
   }
  
  delay(SpeedDelay);
}
//#########################################TwinkleRandom#########################################

//#########################################Softtwinkles#########################################
void softtwinkles() {
  for( int i = 0; i < NUM_LEDS; i++) {
    if( !leds[i]) continue; // skip black pixels
    if( leds[i].r & 1) { // is red odd?
      leds[i] -= lightcolor; // darken if red is odd
    } else {
      leds[i] += lightcolor; // brighten if red is even
    }
  }
  
  // Randomly choose a pixel, and if it's black, 'bump' it up a little.
  // Since it will now have an EVEN red component, it will start getting
  // brighter over time.
  if( random8() < DENSITY) {
    int j = random16(NUM_LEDS);
    if( !leds[j] ) leds[j] = lightcolor;
  }
}
//#########################################Softtwinkles#########################################

//#########################################RainbowStripPaletteLB#########################################
void RainbowStripPaletteLB()
{
  startIndex = 0;  
  for( int i = 0; i < NUM_LEDS; i++) 
    {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
      startIndex = startIndex + 1; /* motion speed */
      FillLEDsFromPaletteColors(startIndex);
      FastLED.delay(10);
    }
}
//#########################################RainbowStripPaletteLB#########################################

//#########################################PurpleandGreenPaletteLB#########################################
void PurpleandGreenPaletteLB()
{
  startIndex = 0;  
  for( int i = 0; i < NUM_LEDS; i++) 
    {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
      startIndex = startIndex + 1; /* motion speed */
      FillLEDsFromPaletteColors(startIndex);
      FastLED.delay(10);
    }
}    
//#########################################PurpleandGreenPaletteLB#########################################

//#########################################PartyColorsPaletteLB#########################################
void PartyColorsPaletteLB()
{
  startIndex = 0;  
  for( int i = 0; i < NUM_LEDS; i++) 
    {  
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
      startIndex = startIndex + 1; /* motion speed */
      FillLEDsFromPaletteColors(startIndex);
      FastLED.delay(10);
  }
}
//#########################################PartyColorsPaletteLB#########################################

//#########################################BlackAndWhiteStripedLB#########################################
void BlackAndWhiteStripedLB()
{
  startIndex = 0;  
  for( int i = 0; i < NUM_LEDS; i++) 
    {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
      startIndex = startIndex + 1; /* motion speed */
      FillLEDsFromPaletteColors(startIndex);
      FastLED.delay(10);
    }
}    
//#########################################BlackAndWhiteStripedLB#########################################

//#########################################RedWhiteBluePaletteLB#########################################
void RedWhiteBluePaletteLB()
{
  startIndex = 0;  
  for( int i = 0; i < NUM_LEDS; i++) 
    {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
      startIndex = startIndex + 1; /* motion speed */
      FillLEDsFromPaletteColors(startIndex);
      FastLED.delay(10);
    }
}    
//#########################################RedWhiteBluePaletteLB#########################################

//#########################################CloudColorsPaletteLB#########################################
void CloudColorsPaletteLB()
{
  startIndex = 0;  
  for( int i = 0; i < NUM_LEDS; i++) 
    {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
      startIndex = startIndex + 1; /* motion speed */
      FillLEDsFromPaletteColors(startIndex);
      FastLED.delay(10);
    }
}    
//#########################################CloudColorsPaletteLB#########################################

//#########################################ColorTemperature#########################################
void ColorTemperatureStrip()
{
  // draw a generic, no-name rainbow
  static uint8_t starthue = 0;
  fill_rainbow( leds + 5, NUM_LEDS - 5, --starthue, 20);

  // Choose which 'color temperature' profile to enable.
  uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
  if( secs < DISPLAYTIME) {
    FastLED.setTemperature( TEMPERATURE_1 ); // first temperature
    leds[0] = TEMPERATURE_1; // show indicator pixel
  } else {
    FastLED.setTemperature( TEMPERATURE_2 ); // second temperature
    leds[0] = TEMPERATURE_2; // show indicator pixel
  }

  // Black out the LEDs for a few secnds between color changes
  // to let the eyes and brains adjust
  if( (secs % DISPLAYTIME) < BLACKTIME) {
    memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
  }
  
  //FastLED.show();
  FastLED.delay(8);
}
//#########################################ColorTemperature#########################################

//#########################################Cylon#########################################
void Cylon() 
{ 
  static uint8_t hue = 0;
  Serial.print("x");
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(30);
  }
  Serial.print("x");

  // Now go in the other direction.  
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(30);
  }
}
//#########################################Cylon#########################################

//#########################################Fire#########################################
void Fire()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
 
}
//#########################################Fire#########################################

//#########################################FirstLight#########################################
void FirstLight() 
{
   // Move a single white led 
   for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      // Turn our current led on to white, then show the leds
      leds[whiteLed] = CRGB::White;

      // Show the leds (only one of which is set to white, from above)
      FastLED.show();

      // Wait a little bit
      delay(100);

      // Turn our current led back to black for the next loop around
      leds[whiteLed] = CRGB::Black;
   }
}
//#########################################FirstLight#########################################


//#########################################ColorWipeLED#########################################
void ColorWipeLED()
{
    for(uint16_t i=0; i<NUM_LEDS; i++) 
  {
      setPixel(i, 0x00, 0xff, 0x00);
      FastLED.show();
      //delay(50);
  }
}
//#########################################ColorWipeLED#########################################

//#########################################ColourFade#########################################
void ColourFade()
{
  for( int i = 0; i < NUM_LEDS; i++) 
  {
  FadeInOut();
  brightness = brightness + fadeAmount;  // reverse the direction of the fading at the ends of the fade:
    if(brightness == 0 || brightness == 255)
  {
    fadeAmount = -fadeAmount ;
  }   
  delay(75);  // This delay sets speed of the fade. I usually do from 5-75 but you can always go higher.
  }
}

//#########################################ColourFade#########################################  

//#########################################TripleBounce######################################### 
void TripleBounce(CRGB leds[], uint16_t frame)   //3 chaser animations offset by 120 degrees each
{
  FastLED.clear();    //Clear previous buffer
  Bounce(leds,frame,0);
  Bounce(leds,frame+(MAX_INT_VALUE/3),100);
  Bounce(leds,frame+(MAX_INT_VALUE/3)*2,150);
}
//#########################################TripleBounce######################################### 

//#########################################DoubleChaser######################################### 
void DoubleChaser(CRGB leds[], uint16_t frame)   //2 chaser animations offset 180 degrees
{
  FastLED.clear();    //Clear previous buffer
  frame = frame * 2;
  Ring(leds, frame, 0);
  Ring(leds, frame + (MAX_INT_VALUE / 2), 150);
}
//#########################################DoubleChaser######################################### .

//#########################################RingPair######################################### 
void RingPair(CRGB leds[], uint16_t frame)     //2 rings animations at inverse phases
{
  FastLED.clear();    //Clear previous buffer
  Ring(leds, frame, 30);
  Ring(leds, MAX_INT_VALUE - frame, 150);
}
//#########################################RingPair######################################### 

//#########################################RainbowSpark######################################### 
void RainbowSpark(CRGB targetStrip[], uint16_t animationFrame,uint8_t fade){    //Color spark where hue is function of frame
  Spark(targetStrip,animationFrame,fade,animationFrame/255);
  delay(20);
}
//#########################################RainbowSpark######################################### 

//#########################################AAnimation######################################### 
void AAnimation() 
  {

  animationA();                                               // render the first animation into leds2   
  animationB();                                               // render the second animation into leds3

  uint8_t ratio = beatsin8(2);                                // Alternate between 0 and 255 every minute
  
  for (int i = 0; i < NUM_LEDS; i++) {                        // mix the 2 arrays together
    leds[i] = blend( leds2[i], leds3[i], ratio );
  }

  FastLED.show();
  }
//#########################################AAnimation######################################### 

//#########################################BeatWave######################################### 
void BeatWave() {

  beatwave();

  EVERY_N_MILLISECONDS(100) {
    uint8_t maxChanges = 24; 
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
  targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }

  FastLED.show();
    
} // BeatWave()  
//#########################################BeatWave######################################### 

//#########################################Blur########################################
void Blur() 
{

  uint8_t blurAmount = dim8_raw( beatsin8(3,64, 192) );       // A sinewave at 3 Hz with values ranging from 64 to 192.
  blur1d( leds, NUM_LEDS, blurAmount);                        // Apply some blurring to whatever's already on the strip, which will eventually go black.
  
  uint8_t  i = beatsin8(  9, 0, NUM_LEDS);
  uint8_t  j = beatsin8( 7, 0, NUM_LEDS);
  uint8_t  k = beatsin8(  5, 0, NUM_LEDS);
  
  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();  
  leds[(i+j)/2] = CHSV( ms / 29, 200, 255);
  leds[(j+k)/2] = CHSV( ms / 41, 200, 255);
  leds[(k+i)/2] = CHSV( ms / 73, 200, 255);
  leds[(k+i+j)/3] = CHSV( ms / 53, 200, 255);
  
  FastLED.show();
  
} // Blur()
//#########################################Blur########################################

//#########################################Dot########################################
void Dot() {

  dot_beat();
  FastLED.show();

} // Dot()
//#########################################Dot########################################

//#########################################Move########################################
void Move()
  {
  
  EVERY_N_MILLISECONDS(thisdelay) {                           // FastLED based non-blocking delay to update/display the sequence.
    ease();
  }
  
  FastLED.show();
  
} // Move()
//#########################################Move########################################

//#########################################Noise########################################
void Noise()
{

  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
  }

  EVERY_N_SECONDS(5) {             // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }

  noise16_3();
  
  FastLED.show();
  
} // Noise()
//#########################################Noise########################################
