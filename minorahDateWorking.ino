#include <RTClib.h>
// #include <Wire.h>
RTC_DS3231 rtc;
// the pin that is connected to SQW
#define CLOCK_INTERRUPT_PIN 4
RTC_DATA_ATTR int bootCount = 1;

#include <Adafruit_AW9523.h>
Adafruit_AW9523 aw;
volatile bool bingo = false;
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
  bootCount = bootCount * -1;

    if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        abort();
    }

     print_wakeup_reason();
//pinMode(GPIO_NUM_4, INPUT_PULLUP);


//    if(rtc.lostPower()) {
//        // this will adjust to the date and time at compilation
//        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//    }
    
    //we don't need the 32K Pin, so disable it
    rtc.disable32K();
    
    // Making it so, that the alarm will trigger an interrupt
    pinMode(GPIO_NUM_4, INPUT_PULLUP);
    gpio_hold_en(GPIO_NUM_4);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low
    //attachInterrupt(digitalPinToInterrupt(GPIO_NUM_4), onAlarm, FALLING);
    
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
    
    // schedule an Alarm for a certain date (day of month), hour, minute, and second
    DateTime alarmTime (2021, 2, 17, 15, 38, 0);
    Serial.print("Current time: ");
     if(!(rtc.setAlarm1(alarmTime, DS3231_A1_Date)) ) {
        Serial.println("Error, alarm wasn't set!");
    }else {
        Serial.println("Alarm will happen in 10 seconds!");  
    }
    
    //printTime();
    //Serial.println();
    if(!bootCount){
    Serial.println("Going to sleep now");
        Serial.flush();
        esp_deep_sleep_start();
        Serial.println("This will never be printed");
  }
    
}
void loop() {
  
    if(bootCount){
        if(rtc.alarmFired(1)) {
            Serial.println("Alarm occured, current time: ");
            //printTime();
            rtc.clearAlarm(1);
            Serial.println("Alarm 1 cleared");
        }
    }
}
void printTime(){
     
    DateTime now = rtc.now();
    char date[] = "DD.MM.YYYY, ";
    Serial.print(now.toString(date));
    char time[] = "hh:mm:ss";
    rtc.now().toString(time);
    Serial.println(time);
}
void onAlarm() {
    bootCount = true;
    bingo = true;
}
void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}
