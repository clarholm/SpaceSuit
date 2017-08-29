
/* 
Custom Space suit with APA106 and WS2812B leds.

By Jens Clarholm @jenslabs, jenslabs.com
 */
 
#include <Bounce2.h>
#include "FastLED.h"

#define NEXTSTATEBUTTON 12
#define TOGGLEBUTTON 11
#define speedPot A1
#define brightnessPot A2

#define LED_TYPE APA106
#define COLOR_ORDER RGB
#define DATA_PIN 3
#define NUM_LEDS 26
#define BRIGHTNESS 60
#define BUTTON_DEBOUNCE_PERIOD 20 //ms
#define MAX_INT_VALUE 65536

//Define and initiate global variables
int currentState = 0;
int nrOfStates = 9;
boolean helmetLightsActive = true;
boolean debug1 = true;

int currentSpeedPotValue = 0;
int currentBrightnessPotValue = 0;



//CRGB CRGBValueReadFromPots;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
// Create debounce objects for state buttons
Bounce nextStateButton = Bounce(); 
Bounce toggleButton = Bounce(); 

//Define LED array
CRGB leds[NUM_LEDS];
uint16_t frame = 0;      //I think I might be able to move this variable to the void loop() scope and save some CPU
uint16_t animateSpeed = 100;            //Number of frames to increment per loop
uint8_t  animation = 10;    //Active animation
uint8_t brightness = 50;    //Global brightness percentage
uint8_t  gHueDelta = 18;

void setup() {
  Serial.begin(9600);
  Serial.println("Start serial");
   
 delay(500);
  
  //Create Led object
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  //Define state change buttons with debounce.
  // Setup the first button with an internal pull-up :
  pinMode(NEXTSTATEBUTTON,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  nextStateButton.attach(NEXTSTATEBUTTON);
  nextStateButton.interval(BUTTON_DEBOUNCE_PERIOD);
  
   // Setup the second button with an internal pull-up :
  pinMode(TOGGLEBUTTON,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  toggleButton.attach(TOGGLEBUTTON);
  toggleButton.interval(BUTTON_DEBOUNCE_PERIOD); // interval in ms

}

void loop() {
  // Turn on the LED if either button is pressed :
  if (nextStateButton.update()){
  if ( nextStateButton.read() == LOW ) {
        Serial.println("This is a printout indicating that the next state button has been pressed.");
    updateState(0);
  } 
  }
 if (toggleButton.update()){
  if ( toggleButton.read() == LOW ) {
    Serial.println("This is a printout indicating that the toggle button has been pressed.");
    updateState(1);
  }
 }
  readPots();
  animateSpeed = currentSpeedPotValue;
  brightness = currentBrightnessPotValue;
  //FastLED.setBrightness(currentBrightnessPotValue);

switch(currentState)
{
case 0:
    RingPair(leds, frame); 
    break;
  case 1:
    DoubleChaser(leds,frame);
    break;
  case 2:
    TripleBounce(leds,frame);
    break;
  case 3:
    WaveInt(leds,frame,gHue);
    break;
  case 4:  
    Wave(leds,frame,gHue);
    break;
  case 5:  //Blue spark (Slow)
    Spark(leds,frame,255-currentSpeedPotValue,gHue);   //Overloaded version of "Spark" with Hue value, 255 for fade is the slowest fade possible. 256 is on/off
    delay(2);       //Slow things down a bit more for Slow Spark
    break;
  case 6: //Blue spark (fast)
    Spark(leds,frame,246,gHue);   //Overloaded version of "Spark" with Hue value, 246 fade is faster which makes for a sharper dropoff
    break;
  case 7:  //White spark (Slow)
    Spark(leds,frame,255);     //"Spark" function without hue make a white spark, 255 for fade is the slowest fade possible.
    delay(2);       //Slow things down a bit more for Slow Spark
    break;
  case 8: //White spark (fast)      //"Spark" function without hue make a white spark, 246 fade is faster which makes for a sharper dropoff
    Spark(leds,frame,245);
    break;
  case 9:
    RainbowSpark(leds,frame,240);    //240 for dropoff is a pretty sharp fade, good for this animation
    break;
    
default:
    delay(100); 
}   
 FastLED.setBrightness(brightness);
 FastLED.show();         //All animations are applied!..send the results to the leds(s)
 frame += animateSpeed;  
}

void generateNewHew(){
  gHue += gHueDelta; // compute new hue
  gHue = gHue % 360; // bring hue back in range
  Serial.print("New gHue: ");
  Serial.println(gHue);
  }

void bpm()
{
  // colored ledses pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void sinelon(int diodeNumber)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
      if (diodeNumber < 6){
    for(int currentLed = 0; currentLed < NUM_LEDS; currentLed = currentLed + 1) {
    if (currentLed  > diodeNumber){
    leds[currentLed] = CRGB::Black;;
    }   

    }
    }
     if (diodeNumber == 7){
      setAllLedsToColor(CRGB::Black);
      }
    FastLED.setBrightness(BRIGHTNESS);
   FastLED.show(); 
}

void confetti(int diodeNumber) 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
      if (diodeNumber < 6){
    for(int currentLed = 0; currentLed < NUM_LEDS; currentLed = currentLed + 1) {
    if (currentLed  > diodeNumber){
    leds[currentLed] = CRGB::Black;;
    }   

    }
    }
    if (diodeNumber == 7){
      setAllLedsToColor(CRGB::Black);
      }
    FastLED.setBrightness(BRIGHTNESS);
   FastLED.show(); 
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}


void readPots(){
int oldCurrentSpeedPotValue = currentSpeedPotValue;
int oldCurrentBrightnessPotValue = currentBrightnessPotValue;

int tempSpeedPot = analogRead(speedPot);
int tempCurrentSpeedPotValue = map(tempSpeedPot, 0, 1023, 255, 1);
if(tempCurrentSpeedPotValue > currentSpeedPotValue+1 || tempCurrentSpeedPotValue < currentSpeedPotValue-1){
  currentSpeedPotValue = tempCurrentSpeedPotValue;
  }
if (currentSpeedPotValue != oldCurrentSpeedPotValue){
      Serial.print("New Speed Pot value: ");
      Serial.print(currentSpeedPotValue);
      Serial.println(".");
      }


      
int tempBrightnessPot = analogRead(brightnessPot);
int tempCurrentBrightnessPotValue = map(tempBrightnessPot, 0, 1023, 255, 1);
if(tempCurrentBrightnessPotValue > currentBrightnessPotValue+1 || tempCurrentBrightnessPotValue < currentBrightnessPotValue-1)
{
  currentBrightnessPotValue = tempCurrentBrightnessPotValue;
  }
if (currentBrightnessPotValue != oldCurrentBrightnessPotValue){
      Serial.print("New Brightness Pot value: ");
      Serial.print(currentBrightnessPotValue);
      Serial.println(".");
      }

}

void updateState(int buttonPressed){
  int newState;
  if (buttonPressed==0){ //nextstate button
    newState=currentState+1;
    if (newState > nrOfStates){
      newState = 0;
      }
    if (debug1 == true){
      Serial.print("State change detected, new state is: ");
      Serial.print(newState);     
      Serial.print(". Previous state was: ");
      Serial.print(currentState);
      Serial.println(".");
      }
      currentState = newState;
    }
   else if (buttonPressed==1){  //toggleButton
    generateNewHew();
    helmetLightsActive = !helmetLightsActive;
          Serial.print("Helmet light is Active: ");
      Serial.println(helmetLightsActive);     
    }
    else Serial.println("Unsupported State change command");
    

  
  }

 

void displayCurrentStateByShowingColorsAndFlashing(CRGB color){
  setAllLedsToColor(color);
  delay(300);
  /*
  int flashNrOfTimes = currentState;
  while(flashNrOfTimes>=0){
  setAllLedsToColor(color);
  delay(500);
  setAllLedsToColor(CRGB::Black);
  delay(500);
  flashNrOfTimes = flashNrOfTimes-1;
  }
  */
}
 void setAllLedsToColor(CRGB color){
    for(int currentLed = 0; currentLed < NUM_LEDS; currentLed = currentLed + 1) {
    leds[currentLed] = color;
    }
      FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();
    }


    //#######################################################################################################
//##                                      AGGREGATE ANIMATIONS                                         ##
//#######################################################################################################
void TripleBounce(CRGB leds[], uint16_t frame)    //3 chaser animations offset by 120 degrees each
{
  FastLED.clear();    //Clear previous buffer
  Bounce(leds,frame,0);
  Bounce(leds,frame+(MAX_INT_VALUE/3),100);
  Bounce(leds,frame+(MAX_INT_VALUE/3)*2,150);
}

void DoubleChaser(CRGB leds[], uint16_t frame)   //2 chaser animations offset 180 degrees
{
  FastLED.clear();    //Clear previous buffer
  frame = frame * 2;
  Ring(leds, frame, 0);
  Ring(leds, frame + (MAX_INT_VALUE / 2), 150);
}

void RingPair(CRGB leds[], uint16_t frame)     //2 rings animations at inverse phases
{
  FastLED.clear();    //Clear previous buffer
  Ring(leds, frame, 30);
  Ring(leds, MAX_INT_VALUE - frame, 150);
}


void RainbowSpark(CRGB targetleds[], uint16_t animationFrame,uint8_t fade){    //Color spark where hue is function of frame
  Spark(targetleds,animationFrame,fade,animationFrame/255);
  delay(20);
}




//#######################################################################################################
//##                                PRIMATIVE ANIMATION FUNCTIONS                                      ##
//#######################################################################################################



//*********************     Bounce      ***************************
// Linear "Larson scanner" (or knight rider effect) with anti-aliasing
// Color is determined by "hue"
//*****************************************************************
void Bounce(CRGB targetleds[], uint16_t animationFrame, uint8_t hue)
{
  uint16_t pos16;
  if (animationFrame < (MAX_INT_VALUE / 2))
  {
    pos16 = animationFrame * 2;
  
  }else{
    pos16 = MAX_INT_VALUE - ((animationFrame - (MAX_INT_VALUE/2))*2);
  }

  int position = map(pos16, 0, MAX_INT_VALUE, 0, ((NUM_LEDS) * 16));
  drawFractionalBar(targetleds, position, 3, hue,0);
}




//************************          Ring           ******************************
// Anti-aliased cyclical chaser, 3 pixels wide
// Color is determined by "hue"
//*****************************************************
void Ring(CRGB targetleds[], uint16_t animationFrame, uint8_t hue)
{
  uint8_t ledsLength = sizeof(leds)/sizeof(CRGB);
  int pos16 = map(animationFrame, 0, MAX_INT_VALUE, 0, ((ledsLength) * 16));
  drawFractionalBar(targetleds, pos16, 3, hue,1);
}

//***************************   Wave [Float Math]  *******************************
// Squeezed sine wave  
// Uses slow, Arduino sin() function
// Squeezing achieved by using an exponential (^8) sin value
// Color is determined by "hue"
//***********************************************************************************
void Wave(CRGB targetleds[], uint16_t animationFrame, uint8_t hue){
  FastLED.clear();    //Clear previous buffer
  float deg; 
  float value; 
  uint8_t ledsLength = sizeof(leds)/sizeof(CRGB);
  for(uint8_t i=0;i<ledsLength;i++)
  {
    deg=float(animationFrame+((MAX_INT_VALUE/ledsLength)*i))/(float(MAX_INT_VALUE)) * 360.0;
    value = pow(sin(radians(deg)),8);    //Squeeeeeeze

    if(value>=0){   //Chop sine wave (no negative values)
      targetleds[i] += CHSV(hue,255,value*256);
    }
  } 
}


//***************************   Wave [Integer Math]  *******************************
// unadulterated sine wave.  
// Uses FastLED sin16() and no float math for efficiency. 
// Since im stuck with integer values, exponential wave-forming is not possible (unless i'm wrong???)
// Color is determined by "hue"
//***********************************************************************************
void WaveInt(CRGB targetleds[], uint16_t animationFrame, uint8_t hue){
  FastLED.clear();
  uint8_t ledsLength = sizeof(leds)/sizeof(CRGB);
  uint8_t value;
  for(uint8_t i=0;i<ledsLength;i++)
  {
    value=(sin16(animationFrame+((MAX_INT_VALUE/ledsLength)*i)) + (MAX_INT_VALUE/2))/256;   
    if(value>=0){   
      targetleds[i] += CHSV(hue,255,value);
    }
  } 
}


//********************************   Color Spark  ***********************************
// Color of the sparks is determined by "hue"
// Frequency of sparks is determined by global var "animateSpeed"
// "animateSpeed" var contrained from 1 - 255 (0.4% - 100%)
// "fade" parameter specifies dropoff (next frame brightness = current frame brightness * (x/256)
// fade = 256 means no dropoff, pixels are on or off
// NOTE: this animation doesnt clear the previous buffer because the fade/dropoff is a function of the previous LED state
//***********************************************************************************
void Spark(CRGB targetleds[], uint16_t animationFrame,uint8_t fade, uint8_t hue){

  uint8_t ledsLength = sizeof(leds)/sizeof(CRGB);
  uint16_t rand = random16();

    for(int i=0;i<ledsLength;i++)
    {   
      targetleds[i].nscale8(fade);
    }


  if(rand < (MAX_INT_VALUE / (256 - (constrain(animateSpeed,1,256)))))  ;
  {
    targetleds[rand % ledsLength].setHSV(hue,255,255);
  }
}


//******************************       Spark       **********************************
// Same as color spark but no hue value, // in HSV white is any hue with 0 saturation
// Frequency of sparks is a percentage mapped to global var "animateSpeed"
// "animateSpeed" var contrained from 1 - 255 (0.4% - 100%)
// "fade" parameter specifies dropoff (next frame brightness = current frame brightness * (x/256)
// fade = 256 means no dropoff, pixels are on or off
// NOTE: this animation doesnt clear the previous buffer because the fade/dropoff is a function of the previous LED state
//***********************************************************************************
void Spark(CRGB targetleds[], uint16_t animationFrame,uint8_t fade){
  uint8_t ledsLength = sizeof(leds)/sizeof(CRGB);
  uint16_t rand = random16();
  
  for(int i=0;i<ledsLength;i++)
    {   
      targetleds[i].nscale8(fade);
    }
  

  if(rand < (MAX_INT_VALUE / (256 - (constrain(animateSpeed,1,255)))))
  {
    targetleds[rand % ledsLength].setHSV(0,0,255);
  }

}



//Anti-aliasing code care of Mark Kriegsman Google+: https://plus.google.com/112916219338292742137/posts/2VYNQgD38Pw
void drawFractionalBar(CRGB targetleds[], int pos16, int width, uint8_t hue, bool wrap)
{
  uint8_t ledsLength = sizeof(leds)/sizeof(CRGB);
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

    targetleds[i] += CHSV(hue, 255, bright );
    i++;
    if (i == ledsLength)
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
