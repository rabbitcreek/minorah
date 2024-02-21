#include <RTClib.h>
// #include <Wire.h>
RTC_DS3231 rtc;
// the pin that is connected to SQW
#define CLOCK_INTERRUPT_PIN 4
RTC_DATA_ATTR int bootCount = 1;
RTC_DATA_ATTR int primeIndex = 0;
RTC_DATA_ATTR int primeDay = 0;
const int holiday[11][3] = {{12,12,2023},{12,25,2024},{12,14,2025},{12,4,2026},{12,24,2027},{12,12,2028},{12,1,2029},{12,20,2030},{12,9,2031},
{11,27,2032},{12,16,2033}};
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
  

    if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        abort();
    }

     print_wakeup_reason();
     if(rtc.alarmFired(1)) {
            Serial.println("Alarm occured, current time: ");
            printTime();
            rtc.clearAlarm(1);
            Serial.println("Alarm 1 cleared");
        }
//pinMode(GPIO_NUM_4, INPUT_PULLUP);
if(primeIndex == 0)firstDater();
if(primeIndex != 0 && primeDay != 0 )startHoliday();

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
  
}
void loop() {
  
  
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
void firstDater(){ //this function finds the index in holiday for matching year if primeIndex is 0...like the first time
DateTime current = rtc.now();
for (int i = 0; i < 11; i++){ //goes through holiday index of years looking for this year...last year included so primeIndex will not equal 0..
 if( holiday[i,2] == current.year(){ //checks to  see if its this year..
  primeIndex = i;//sets primeIndex to current year data
  break;
 }
 if(primeIndex = 0) Serial.println( "cannot find primeIndex");//if primeIndex is still zero after search something is wrong
}
if(current.day() == holiday[primeIndex, 1] && current.month() == holiday[primeIndex , 0])primeDay = 1;
else Serial.println(" cannot find primeDay ")
}

  

}
void startHoliday(){
  int candleHold = 5000;    // delay in between lighting next candle
  int candleTimer = 10 * 1000 * 60;   //ten minutes that candles will be on
  double timeTracker = millis();  //this is baseline timing for when candles will be lit
  for ( int x = 0; x <= primeDay; x++){   //this  segment turns candles on
    for (int i = 270; i > 0; i--){                          // 360 degrees of an imaginary circle.
    
    float angle = radians(i);                             // Converts degrees to radians.
     int brightness = (255 / 2) + (255 / 2) * sin(angle);      // Generates points on a sign wave.
    aw.analogWrite(x,brightness);                          // Sends sine wave information to pin 9.
    delay(30);                                            // Delay between each point of sine wave.
  }
  delay(candleHold);  //delay in between candles
  }
   while ( millis() - timeTracker <= candleTimer){   // delay between candles on and off 
    int xo = xo + 1; 
   }
   for ( int x = primeDay; x >= 0; x--){ //primeDay is number of days of the 8 in sequence ...this turns candles off
   for (int i = 0; i < 270; i++){                          // 360 degrees of an imaginary circle.
    
    float angle = radians(i);                             // Converts degrees to radians.
     int brightness = (255 / 2) + (255 / 2) * sin(angle);      // Generates points on a sign wave.
    aw.analogWrite(x,brightness);                          // Sends sine wave information to pin 9.
    delay(30);                           
   }
  
}
primeDay ++; //added another day to festivities
if(primeDay == 9) seeUNxtYr(); //if over 8 days schedule next year
DateTime celebrate = rtc.now() + TimeSpan(1,0,0,0); //celebrate is dateTime for one day from now timespan function takes (day,hour,minute,sec)
goToSleep(celebrate); //goes to sleep with wakup on correct date/hour/min/seconds match


}
void goToSleep(DateTime sleepy){
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
// stop oscillating signals at SQW Pin
    // otherwise setAlarm1 will fail
    rtc.writeSqwPinMode(DS3231_OFF);
    
    // turn off alarm 2 (in case it isn't off already)
    // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
    rtc.disableAlarm(2);
     // schedule an Alarm for a certain date (day of month), hour, minute, and second
    //DateTime alarmTime (2021, 2, 17, 18, 59, 0);
    Serial.print("Current time: ");
     if(!(rtc.setAlarm1(sleepy, DS3231_A1_Date)) ) { //DS3231 Date alarm rings when date, hour, minute, sec match
        Serial.println("Error, alarm wasn't set!");
    }else {
        Serial.println("Alarm will happen in 10 seconds!");  
    }
    Serial.println("Going to sleep now");
        Serial.flush();
        esp_deep_sleep_start(); //starts deep sleep
        Serial.println("This will never be printed");

}
void seeUNxtYr(){
primeIndex ++;  //going for next year on list
int p = primeIndex;
if(p >= 10)break; //over ten years ... hopefully I will still be alive
//DateTime alarmTime (2021, 2, 17, 18, 59, 0);
DateTime future (holiday[p,2],holiday[p, 0], holiday[p, 1],19, 0,0); //sets up alarm for the next set of numbers contained in array holiday...
if(rtc.year() +1 != holiday[p,2]) Serial.println("Next year schedule is broken"); //if now.year +1 does not correspond to next year something is wrong...
goToSleep(future); //send future to sleep function...
}