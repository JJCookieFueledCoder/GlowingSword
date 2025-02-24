#include <FastLED.h>
#include <Arduino.h>
#include <OneButton.h>



// Which pin on the Arduino is connected to the LEDs?
#define DATA_PIN1 6
#define DATA_PIN2 5


// How many LEDs are attached to the Arduino?
#define NUM_LEDS_PIN6 49
#define NUM_LEDS_PIN5 50

// LED brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 255

#define BUTTON_PIN 2

OneButton btn = OneButton(
  BUTTON_PIN,
  true,
  true
);


CRGB output1[NUM_LEDS_PIN6];
CRGB output2[NUM_LEDS_PIN5];


uint8_t blendAmount = 0;
uint8_t patternCounter = 0;
uint8_t source1Pattern = 0;
uint8_t source2Pattern = 1;
bool useSource1 = false;

bool ignited = false;

int LEDstate = false;

int slow = 5;

uint32_t LED;

uint16_t brightnessScale = 25;
uint16_t indexScale = 50;

uint8_t paletteIndex = 0;
DEFINE_GRADIENT_PALETTE (Rainbows_gp)
  {
    0xFF0000, 0x000000, 0xAB5500, 0x000000,
    0xABAB00, 0x000000, 0x00FF00, 0x000000,
    0x00AB55, 0x000000, 0x0000FF, 0x000000,
    0x5500AB, 0x000000, 0xAB0055, 0x000000
};

DEFINE_GRADIENT_PALETTE (heatmap_gp){
    0x000000, 0x330000, 0x660000, 0x990000, 
    0xCC0000, 0xFF0000, 0xFF3300, 0xFF6600, 
    0xFF9900, 0xFFCC00, 0xFFFF00, 0xFFFF33, 
    0xFFFF66, 0xFFFF99, 0xFFFFCC, 0xFFFFFF
};

DEFINE_GRADIENT_PALETTE (Green_gp){
  0,   0,   189,  163,
  50,  52,  50,   194,
  113, 38,  40,   213,
  166, 23,  130,  65,
  255, 0,   189,  163
};
CRGBPalette16 lavaPalette = CRGBPalette16(
    CRGB::DarkRed,  CRGB::Maroon,     CRGB::DarkRed,  CRGB::Maroon, 
    CRGB::DarkRed,  CRGB::Maroon,     CRGB::DarkRed,  CRGB::DarkRed, 
    CRGB::DarkRed,  CRGB::DarkRed,    CRGB::Red,      CRGB::Orange, 
    CRGB::Yellow,    CRGB::Orange,     CRGB::Red,      CRGB::DarkRed
  );
  CRGBPalette16 ForestPalette = CRGBPalette16(
    CRGB::DarkGreen,  CRGB::DarkGreen,        CRGB::DarkOliveGreen,   CRGB::DarkGreen,
    CRGB::Green,      CRGB::ForestGreen,      CRGB::OliveDrab,        CRGB::Green,
    CRGB::SeaGreen,   CRGB::MediumAquamarine, CRGB::LimeGreen,        CRGB::YellowGreen,
    CRGB::LightGreen, CRGB::LawnGreen,        CRGB::MediumAquamarine, CRGB::ForestGreen
  );
CRGBPalette16 pacifica_palette_1 = 
  { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
    0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };


bool isrunning = false;


CRGBPalette16 heatGP(heatmap_gp);
CRGBPalette16 greenGP(Green_gp);
CRGBPalette16 RainbowGP(Rainbows_gp);

CRGBPalette16 OutputPalette1 = lavaPalette;
CRGBPalette16 OutputPalette2 = lavaPalette;


void setup() {
  FastLED.addLeds<WS2812B, 6,GRB>(output1,NUM_LEDS_PIN6);  // initialize strip (required!)
  FastLED.addLeds<WS2812B, 5,GRB>(output2,NUM_LEDS_PIN5);
  FastLED.setBrightness(BRIGHTNESS);
  fill_solid(output1,NUM_LEDS_PIN6,CRGB::Black);
  fill_solid(output2,NUM_LEDS_PIN5,CRGB::Black);

  Serial.begin(9600);

 btn.attachClick(nextPattern);
 btn.attachLongPressStart(LEDStart);
  ignite(OutputPalette1,output1,output2, 20, NUM_LEDS_PIN6);
}

bool isIgnite = false;
void benstest(){

  if (isIgnite){
    //unignite(output1, 20, NUM_LEDS_PIN6);
    unignite(output1,output2,20, NUM_LEDS_PIN6);
    //ignite(ForestPalette,output1, 20, NUM_LEDS_PIN6);
    //ignite(ForestPalette,output2, 20, NUM_LEDS_PIN5);
  }
  else{
    ignite(OutputPalette1,output1,output2, 20, NUM_LEDS_PIN6);
    //ignite(OutputPalette2,output2, 20, NUM_LEDS_PIN5);
  }
  Serial.println(isIgnite);
  isIgnite = !isIgnite;

}

void loop() {

  btn.tick();

  EVERY_N_MILLISECONDS(2){

    if (ignited == true){
      runPattern(patternCounter, OutputPalette1);
    }
    FastLED.show();
  }
  EVERY_N_MILLISECONDS(30){
    paletteIndex++;
  }



}

void LEDStart(){

  LEDstate = !LEDstate;
  if(LEDstate == true){
    Serial.println("ON");
    ignite(OutputPalette1,output1,output2, 20, NUM_LEDS_PIN6);
  }
  if(LEDstate == false){
    Serial.println("OFF");
    unignite(output1,output2,20, NUM_LEDS_PIN6);
  }


}

void runPattern(uint8_t pattern, CRGBPalette16 CPAL){
  switch (pattern) {
    case 0:
      CPAL = lavaPalette;
      runBasicPattern(lavaPalette, output1, NUM_LEDS_PIN6,65);
      runBasicPattern(lavaPalette, output2, NUM_LEDS_PIN5,65);
      break;
    case 1:
      CPAL = ForestPalette;
      runBasicPattern(ForestPalette, output1, NUM_LEDS_PIN6,255);
      runBasicPattern(ForestPalette, output2, NUM_LEDS_PIN5,255);
    break;
    case 2:
      CPAL = lavaPalette;
      runLavaPattern(output1, NUM_LEDS_PIN6);
      runLavaPattern(output2, NUM_LEDS_PIN5);
      break;
    case 3:
      CPAL = greenGP;
      runBasicPattern(greenGP, output1, NUM_LEDS_PIN6,65);
      runBasicPattern(greenGP, output2, NUM_LEDS_PIN5,65);
      break;
      
    case 4:
      CPAL = Rainbows_gp;
      runBasicPattern(Rainbows_gp, output1, NUM_LEDS_PIN6,65);
      runBasicPattern(Rainbows_gp, output2, NUM_LEDS_PIN5,65);
      break;
    case 5:
      CPAL = Rainbows_gp;
      runDualPattern1();
      break;
  }
}


void runLavaPattern(CRGB *leds, int NUM_LEDS){
    uint8_t brightness = inoise8(brightnessScale, millis()/5);
    uint8_t index = inoise8(indexScale, millis()/10);
  for (int i = 0; i< NUM_LEDS; i++){
    leds[i] = ColorFromPalette(lavaPalette, index+i, brightness+i,LINEARBLEND);
  }
}

void runDualPattern1(){
    uint8_t brightness = inoise8(brightnessScale, millis()/5);
    uint8_t index = inoise8(indexScale, millis()/10);
  for (int i = 0; i< NUM_LEDS_PIN6; i++){
    output1[i] = ColorFromPalette(ForestPalette, (paletteIndex+i), brightness+i,LINEARBLEND);
  }
  for (int i = 0; i< NUM_LEDS_PIN5; i++){
    output2[i] = ColorFromPalette(RainbowGP, (paletteIndex+i), brightness+i,LINEARBLEND);
  }
}

void runBasicPattern(CRGBPalette16 PAL, CRGB *leds, int NUM_LEDS, int brightness){
  for (unsigned int i = 0; i < NUM_LEDS; i++) {
   leds[i] = ColorFromPalette(PAL ,(paletteIndex+i),brightness,LINEARBLEND);
  }
}

void nextPattern(){
  isrunning = false;
  patternCounter = (patternCounter + 1) % 6;
  Serial.println("change");
}

void ignite(CRGBPalette16 color,CRGB *output1, CRGB *output2, unsigned long wait, int NUM_LEDS) {

  for (unsigned int i = 0; i < NUM_LEDS; i++) {
    output1[i] = ColorFromPalette(color ,paletteIndex+i, 255,LINEARBLEND);
    output2[i] = ColorFromPalette(color ,paletteIndex+i, 255,LINEARBLEND);
    delay(wait);
    FastLED.show();
  }
  
  ignited = true;
}

void unignite(CRGB *output1, CRGB *output2,unsigned long wait, int NUM_LEDS){
  for (unsigned int i = NUM_LEDS-1; i >= 0; i--) {
    output1[i] = CRGB::Black;
    output2[i] = CRGB::Black;
    //output[i] = ColorFromPalette(ForestPalette ,paletteIndex+i, 255,LINEARBLEND);
    delay(wait);
    FastLED.show();
  }
  ignited = false;
}

