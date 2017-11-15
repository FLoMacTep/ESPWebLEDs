
/*-------------------------------------------------------------------------------------
My realisation of FASTLED animations based on  Gyro Gearloose, Feb 2016
---------------------------------------------------------------------------------------*/


#include "LEDEffects.h" //Part of code with LED initialization and strip effects
#include "WebServer.h"  //Part of code with Wi-Fi connection and WebServer initialization


// MAIN PART VOID SETUP +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup() 
{

  ulReqcount=0;         // setup globals for Webserver
  ulReconncount=0;
  
  // start serial
  Serial.begin(9600);
  delay(1);
  
  // inital connect
  WiFi.mode(WIFI_STA);
  WiFiStart();

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  //set_max_power_in_volts_and_milliamps(5, MILLI_AMPERE);
}
// END VOID SETUP +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// MAIN PART VOID LOOP +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void loop() {
  webserver();

  if (ledMode != 999) {

     switch (ledMode) {
      case  1: all_off(); break;
      case  2: rainbow(); break;
      case  3: rainbowWithGlitter(); break;
      case  4: confetti(); break;
      case  5: sinelon(); break;
      case  6: juggle(); break;
      case  7: bpm(); break;
      case  8: TwinkleRandom(20, 100, false); break;
      case  9: softtwinkles(); break;
      case  10: ColorTemperatureStrip(); break;
      case  11: Cylon(); break;
      case  12: RainbowStripPaletteLB(); break;
      case  13: PurpleandGreenPaletteLB(); break;
      case  14: Fire(); break;
      case  15: PartyColorsPaletteLB(); break;
      case  16: BlackAndWhiteStripedLB(); break;
      case  17: RedWhiteBluePaletteLB(); break;
      case  18: CloudColorsPaletteLB(); break;
      case  19: FirstLight(); break;
      case  20: ColourFade(); break;
      case  21: RingPair(leds, frame); break;
      case  22: DoubleChaser(leds,frame); break;
      case  23: TripleBounce(leds,frame); break;
      case  24: WaveInt(leds,frame,180); break;
      case  25: Wave(leds,frame,180); break;
      case  26: Spark(leds,frame,246,188); break;
      case  27: Spark(leds,frame,245); break;
      case  28: RainbowSpark(leds,frame,240); break;
      case  29: AAnimation(); break;
      case  30: BeatWave(); break;
      case  31: Blur(); break;
      case  32: Dot(); break;
      case  33: Move(); break;
      case  34: Noise(); break;


      }
      }
 show_at_max_brightness_for_power();
 frame += animateSpeed;
delay_at_max_brightness_for_power(1000/FRAMES_PER_SECOND); 
//  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}
 
// END VOID LOOP +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
