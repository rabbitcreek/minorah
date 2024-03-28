#include <RTClib.h>
//#include <Wire.h>
RTC_DS3231 rtc;
bool alarmCheck = 0;
RTC_DATA_ATTR int primeIndex = 0;//primeIndex is the array index in holiday that holds the current year
RTC_DATA_ATTR int primeDay = 0;//primeDay is the current day in the eight day holiday series 
 uint32_t holiday[14][4] = {{12,12,2023,1702443600},{3,20,2024,1710997200},{12,14,2025,1765774800},{12,4,2026,1796446800},{12,24,2027,1829710800},{12,12,2028,1860296400},{12,1,2029,1890882000},{12,20,2030,1924059600},{12,9,2031,1954645200},
{11,27,2032,1985230800},{12,16,2033,2018408400},{12,6,2034,2049080400},{12,25,2035,2082258000},{12,13,2036,2112843600}};//array of dates for next 14 years of hanukah
int ranFile[9] = {0,0,0,0,0,0,0,0,0}; //zeros out random array ranFile which is an array that randomises off time for candles
#include <Adafruit_AW9523.h>
Adafruit_AW9523 aw; //starts the aw9523 
int curStrength = 120;
// Maximum amplitude of flickering
int maxAmp = 100;
// Milliseconds per frame

int frameLength = 10;
int fakePrime = 0;
void setup() {
    Serial.begin(9600);

    delay(5000);
     if (! aw.begin(0x58)) {
    Serial.println("AW9523 not found? Check wiring!");
    while (1) delay(10);  // halt forever
  }
 Serial.println("AW9523 found!");
  for(int zip = 0; zip< 10; zip++){
    aw.pinMode(zip, AW9523_LED_MODE);
    aw.analogWrite(zip, 0);
  }
    if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        //Serial.flush();
        abort();
    }
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  //opened this will synch RTC with computer
    //rtc.adjust(DateTime(2024, 3, 19, 19, 0, 0));
    if(rtc.lostPower()) {
        // this will adjust to the date and time at compilation
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

     print_wakeup_reason();
     rtc.disable32K();
    printTime();
    // Making it so, that the alarm will trigger an interrupt
    pinMode(GPIO_NUM_4, INPUT_PULLUP);
    pinMode(GPIO_NUM_2, OUTPUT);//this is the GPIO connected to the transistor that turns on MP3 player
    digitalWrite(GPIO_NUM_2, LOW);
    gpio_hold_en(GPIO_NUM_4);  //GPIO that wakes up ESP from RTC
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low
    //attachInterrupt(digitalPinToInterrupt(GPIO_NUM_4), onAlarm, FALLING);
     if(rtc.alarmFired(1)) {
            Serial.println("Alarm occured, current time: ");
            alarmCheck = 1;
            printTime();
            rtc.clearAlarm(1);
            Serial.println("Alarm 1 cleared");
        }
    
Serial.print("PimeDay = ");
Serial.println(primeDay);
Serial.print("PrimeIndex = ");
Serial.println(primeIndex);
    

if(primeDay  == 0 || primeIndex == 0 ){  //power went out ... didnt wake from sleep need to set what day and what year
onPower(); //function that demos the lights and song 
firstDater();
}
fakePrime = primeDay; 
Serial.print("PimeDay = ");
Serial.println(primeDay);
Serial.print("PrimeIndex = ");
Serial.println(primeIndex);
     
if(primeIndex != 0 && primeDay != 0 )startHoliday();      //found day and year to start the show...   
}

void loop() { //loop function never used
  
  
}
void printTime(){//call this to print the current time on RTC
     
    DateTime now = rtc.now();
    char date[] = "DD.MM.YYYY, ";
    Serial.print(now.toString(date));
    char time[] = "hh:mm:ss";
    rtc.now().toString(time);
    Serial.println(time);
}

void print_wakeup_reason()//the usual function that tells what woke esp32 up
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
DateTime cal = rtc.now();
uint32_t unix_time = rtc.now().unixtime();
unix_time = unix_time + 36000;
for (int i = 0; i < 15; i++){ //goes through holiday index of years looking for this year...last year included so primeIndex will not equal 0..
 if(  holiday[i][2] == cal.year()){ //checks to  see if its this year..
  primeIndex = i;//sets primeIndex to current year data
  break;
 }
 if(primeIndex = 0) Serial.println( "cannot find primeIndex");//if primeIndex is still zero after search something is wrong
}

if(cal.day() == holiday[primeIndex][1] && cal.month() == holiday[primeIndex ][0])primeDay = 1; //checks to see if date match is correct
 else if(((holiday[primeIndex][3] + (86400 * 7)) > unix_time) && unix_time >= holiday[primeIndex][3]) {  //if not perfect date is it withing 7 days of start
primeDay = ((unix_time - holiday[primeIndex][3]) / 86400 ) + 1;
 
}
else {Serial.println(" cannot find primeDay ");// if its not the day and not within 7 days of it  try next month ... date doesnt know month...
seeUNxtYr();
}

  
}

void startHoliday(){
  
  int candleHold = 10000;    // delay in between lighting next candle
  int candleTimer = 10 * 1000 * 60;   //ten minutes that candles will be on
  
  
  aw.analogWrite(9, 255); //this is the red part of the chamas
  aw.analogWrite(0,0);
  thePrayer();  //cue Jack Black
  for ( int x = 1; x <= primeDay; x++){   //this  segment turns candles on
    for (int i = 270; i > 0; i--){                          // 360 degrees of an imaginary circle.
    
    float angle = radians(i);                             // Converts degrees to radians.
     int brightness = (255 / 2) + (255 / 2) * sin(angle);      // Generates points on a sign wave.
    aw.analogWrite(x,brightness);                          // Sends sine wave information to pin 9.
    delay(30);                                            // Delay between each point of sine wave.
  }
  delay(candleHold);  //delay in between candles
  }
  
  double timeTracker = millis();  //this is baseline timing for when candles will be lit

   while ( millis() - timeTracker <= candleTimer){   // delay between candles on and off 
    flicker();
   }
   randomizer(primeDay);//this randomizes the array with a card shuffle
   for ( int x = primeDay; x >= 0; x--){ //primeDay is number of days of the 8 in sequence ...this turns candles off
   for (int i = 0; i < 270; i++){                          // 360 degrees of an imaginary circle.
    
    float angle = radians(i);                             // Converts degrees to radians.
     int brightness = (255 / 2) + (255 / 2) * sin(angle);      // Generates points on a sign wave.
    aw.analogWrite(ranFile[x],brightness);                          // Sends sine wave information to pin 9.
    delay(30);                           
   }
  
}

uint32_t unix_time = rtc.now().unixtime();
unix_time = unix_time + 36000;//grabs unix time from rtc
Serial.print("unix time:");
Serial.println(unix_time);
Serial.print("PimeDay = ");
Serial.println(primeDay);
Serial.print("PrimeIndex = ");
Serial.println(primeIndex);
uint32_t timeLeft = (unix_time - holiday[primeIndex][3])  - ((primeDay -1) * 86400 );  //checks to see how much time has elapsed from 19:00 hours to compensate timing for next light
Serial.print("Time Left = ");
Serial.println(timeLeft);
uint32_t spanner = 86400 - timeLeft;
DateTime celebrate = rtc.now() + TimeSpan(spanner);
primeDay ++; //added another day to festivities
if(primeDay == 9) {
  primeIndex ++;  //going for next year on list
  seeUNxtYr();
} //if over 8 days schedule next year

//DateTime celebrate = rtc.now() + TimeSpan(1,0,0,0); //celebrate is dateTime for one day from now timespan function takes (day,hour,minute,sec)
goToSleepDate(celebrate); //goes to sleep with wakup on correct date/hour/min/seconds match


}


void goToSleepDate(DateTime sleepy){
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
    aw.analogWrite(9, 0);
    char date[] = "DD.MM.YYYY, ";
    Serial.print(sleepy.toString(date));
    char time[] = "hh:mm:ss";
    sleepy.toString(time);
    Serial.println(time);
    Serial.println("Going to sleep now");
        //Serial.flush();
        esp_deep_sleep_start(); //starts deep sleep
        Serial.println("This will never be printed");

}
void seeUNxtYr(){  //looking for next alarm

DateTime now = rtc.now();
aw.analogWrite(9, 0);
int p = primeIndex;
if(p >= 15)while(1); //over ten years ... hopefully I will still be alive
//DateTime alarmTime (2021, 2, 17, 18, 59, 0);
DateTime future (holiday[p][2],holiday[p][0], holiday[p][1],19, 0,0); //sets up alarm for the next set of numbers contained in array holiday...
//if(now.year() +1 != holiday[p][2]) Serial.println("Next year schedule is broken"); //if now.year +1 does not correspond to next year something is wrong...
goToSleepDate(future); //send future to sleep function...
}
void flicker() {//these functions produce flickering...they are all borrowed...thanx
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
        for(int LedPin = 0; LedPin < fakePrime +1; LedPin ++){   //fakePrime is used by several functions to produce the wiggle light numbers
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
void thePrayer(){//cue Jack Black ...thanx for the prayer...everyone else forgets it...this activates transistor for 17 secs of prayer
  digitalWrite(GPIO_NUM_2, HIGH);
  delay(17200);
  digitalWrite(GPIO_NUM_2, LOW);
  delay(5000);
}
void onPower(){
  int candleHold = 500;    // delay in between lighting next candle
  int candleTimer = 0.5 * 1000 * 60;   //ten minutes that candles will be on
   fakePrime = 8;
  
  aw.analogWrite(9, 255);
  aw.analogWrite(0,0);
  thePrayer();
  for ( int x = 0; x <= 8; x++){   //this  segment turns candles on
    for (int i = 270; i > 0; i--){                          // 360 degrees of an imaginary circle.
    
    float angle = radians(i);                             // Converts degrees to radians.
     int brightness = (255 / 2) + (255 / 2) * sin(angle);      // Generates points on a sign wave.
    aw.analogWrite(x,brightness);                          // Sends sine wave information to pin 9.
    delay(10);                                            // Delay between each point of sine wave.
  }
  delay(candleHold);  //delay in between candles
  }
  double timeTracker = millis(); //this is baseline timing for when candles will be lit
   while ( millis() - timeTracker <= candleTimer){   // delay between candles on and off 
    flicker();
   }
   randomizer(8);
   for ( int x = 8; x >= 0; x--){ //primeDay is number of days of the 8 in sequence ...this turns candles off
   for (int i = 0; i < 270; i++){                          // 360 degrees of an imaginary circle.
    
    float angle = radians(i);                             // Converts degrees to radians.
     int brightness = (255 / 2) + (255 / 2) * sin(angle);      // Generates points on a sign wave.
    aw.analogWrite(ranFile[x],brightness);                          // Sends sine wave information to pin 9.
    delay(10);                           
   }
  
}
}
void randomizer( int   howMany){//shuffles array of candles   
  for(int i = 0; i <= howMany; i ++){
  ranFile[i] = i;}
  for(int i = 0; i <= howMany; i ++){
    int n = random(0, howMany);
    int temp = ranFile[n];
    ranFile[n] = ranFile[i];
    ranFile[i] = temp;
  }
}