/* Example implementation of an alarm using DS3231
 *
 * VCC and GND of RTC should be connected to some power source
 * SDA, SCL of RTC should be connected to SDA, SCL of arduino
 * SQW should be connected to CLOCK_INTERRUPT_PIN
 * CLOCK_INTERRUPT_PIN needs to work with interrupts
 */
#include <RTClib.h>
// #include <Wire.h>
RTC_DS3231 rtc;
// the pin that is connected to SQW
#define CLOCK_INTERRUPT_PIN 4
#include <Adafruit_AW9523.h>
Adafruit_AW9523 aw;
void setup() {
    Serial.begin(9600);
    if (! aw.begin(0x58)) {
    Serial.println("AW9523 not found? Check wiring!");
    while (1) delay(10);  // halt forever
  }
  Serial.println("AW9523 found!");
  for(int zip = 0; zip<11; zip++){
    aw.pinMode(zip, AW9523_LED_MODE);
    aw.analogWrite(zip, 0);
  }
    // initializing the rtc
    if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        abort();
    }
    
//    if(rtc.lostPower()) {
//        // this will adjust to the date and time at compilation
//        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//    }
    
    //we don't need the 32K Pin, so disable it
    rtc.disable32K();
    
    // Making it so, that the alarm will trigger an interrupt
    pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, FALLING);
    
    // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
    // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    
    // stop oscillating signals at SQW Pin
    // otherwise setAlarm1 will fail
    rtc.writeSqwPinMode(DS3231_OFF);
    
    // turn off alarm 2 (in case it isn't off already)
    // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
    rtc.disableAlarm(2);
    
    // schedule an alarm 10 seconds in the future
    if(!rtc.setAlarm1(
            rtc.now() + TimeSpan(10),
            DS3231_A1_Second // this mode triggers the alarm when the seconds match. See Doxygen for other options
    )) {
        Serial.println("Error, alarm wasn't set!");
    }else {
        Serial.println("Alarm will happen in 10 seconds!");  
    }
}
void loop() {
    // print current time
    char date[10] = "hh:mm:ss";
    rtc.now().toString(date);
    Serial.println(date);
    // resetting SQW and alarm 1 flag
    // using setAlarm1, the next alarm could now be configurated
    if(rtc.alarmFired(1)) {
        rtc.clearAlarm(1);
        Serial.println("Alarm cleared");
    }
    
    delay(2000);
}
void onAlarm() {
    Serial.println("Alarm occured!");
}