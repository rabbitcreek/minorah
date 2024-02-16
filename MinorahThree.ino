
#include <stdint.h>
#include <Adafruit_AW9523.h>

Adafruit_AW9523 aw;

int masterlist[7];
// Current LED Strength
int curStrength = 120;

// Maximum amplitude of flickering
int maxAmp = 100;

// Milliseconds per frame
int frameLength = 10;
int endBlink = 0;
int ii = 0;

 

//uint8_t LedPin = 1;  // 0 thru 15

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(1);  // wait for serial port to open
  Serial.println("Adafruit AW9523 Constant Current LED test!");

  if (! aw.begin(0x58)) {
    Serial.println("AW9523 not found? Check wiring!");
    while (1) delay(10);  // halt forever
  }

  Serial.println("AW9523 found!");
    // Pin configuration
  aw.pinMode(0, AW9523_LED_MODE); // set to constant current drive!
  aw.pinMode(1, AW9523_LED_MODE); 
  aw.pinMode(2, AW9523_LED_MODE); 
  aw.pinMode(3, AW9523_LED_MODE); 
  aw.pinMode(4, AW9523_LED_MODE); 
  aw.pinMode(5, AW9523_LED_MODE); 
  aw.pinMode(6, AW9523_LED_MODE); 
  aw.pinMode(7, AW9523_LED_MODE); 
  aw.pinMode(9, AW9523_LED_MODE);
  aw.pinMode(10, AW9523_LED_MODE); 
  
}


void loop() {
    // Keep the lights flickering
    flicker();
    ii ++;
    if(ii == 15){
      ii = 0;
      //biggerErase();
    }
    endBlink = ii;
    dead(3);
   
}
void flicker() {
    // Amplitude of flickering
    int amp = random(maxAmp)+1;
    
    // Length of flickering in milliseconds
    int length = random(10000/amp)+1000;
    
    // Strength to go toward
    int endStrength = random(255-(amp/4))+(amp/4);
    
    // Flicker function
    flickerSection(length, amp, endStrength);
}

void flickerSection(int length, int amp, int endStrength) {
    // Initilize variables
    int i, max, min;
    double oldStrengthRatio, endStrengthRatio;
    
    // Number of frames to loop through
    int frameNum = length/frameLength;
    
    // Use variable to hold the old LED strength
    int oldStrength = curStrength;
    
    
    // Loop <frameNum> times 
    for(i = 0; i <= frameNum; i += 1){
        // The ratio of the old/end strengths should be proprtional to the ratio of total frames/current frames
        // Use type casting to allow decimals
        oldStrengthRatio = (1-(double)i/(double)frameNum);
        endStrengthRatio = ((double)i/(double)frameNum);
        
        // Fade current LED strength from the old value to the end value
        curStrength = (oldStrength*oldStrengthRatio) + (endStrength*endStrengthRatio);
        
        // LED brightnesses must be in between max and min values
        // Both values are half an amplitude's distance from the average
        max = curStrength+(amp/2);
        min = curStrength-(amp/2);
        
        // Light LEDs to random brightnesses
        for(int LedPin = 0; LedPin < endBlink; LedPin ++){
        setRandom(LedPin, max, min);
    }
        
        
        // Wait until next frame
        delay(frameLength);
    }
}

void setRandom(int led, int max, int min) {
    // Set chosen LED to a random brightness between the maximum and minimum values
    aw.analogWrite(led, random(max - min) + min);
}
void biggerErase(){
  for (int i = 0; i < 15; i ++){
    aw.analogWrite(i,0);
  }
}
void dead( int deadLed){
  for(int i = 0; i < 11; i ++)aw.analogWrite(i, 255);
   for (int i = 0; i < 270; i++){                          // 360 degrees of an imaginary circle.
    
    float angle = radians(i);                             // Converts degrees to radians.
     int brightness = (255 / 2) + (255 / 2) * sin(angle);      // Generates points on a sign wave.
    aw.analogWrite(deadLed,brightness);                          // Sends sine wave information to pin 9.
    delay(30);                                            // Delay between each point of sine wave.
  }
}

