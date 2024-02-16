#include <Adafruit_AW9523.h>

Adafruit_AW9523 aw;

//uint8_t LedPin = 1;  // 0 thru 15

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(1);  // wait for serial port to open
  pinMode(D9, OUTPUT);
  Serial.println("Adafruit AW9523 Constant Current LED test!");

  if (! aw.begin(0x58)) {
    Serial.println("AW9523 not found? Check wiring!");
    while (1) delay(10);  // halt forever
  }

  Serial.println("AW9523 found!");
  //aw.pinMode(15, OUTPUT);
  aw.pinMode(0, AW9523_LED_MODE); // set to constant current drive!
  aw.pinMode(1, AW9523_LED_MODE); 
  aw.pinMode(2, AW9523_LED_MODE); 
  aw.pinMode(3, AW9523_LED_MODE); 
  aw.pinMode(4, AW9523_LED_MODE); 
  aw.pinMode(5, AW9523_LED_MODE); 
  aw.pinMode(6, AW9523_LED_MODE); 
  aw.pinMode(7, AW9523_LED_MODE); 
  aw.pinMode(8, AW9523_LED_MODE); 
  aw.pinMode(9, AW9523_LED_MODE); 
  aw.pinMode(10, AW9523_LED_MODE); 
  aw.pinMode(13, AW9523_LED_MODE); 
  //aw.digitalWrite(15, HIGH);
}


uint8_t x = 0;

void loop() {
 digitalWrite(D9, HIGH);
  // Loop from 0 to 255 and then wrap around to 0 again
  //int x = random(0,255);
    for(int LedPin = 0;LedPin <14; LedPin  ++){
  aw.analogWrite(LedPin, random(155,255)); 
    }
    delay(10);
}

