/***
 * LIBRARY
 ***/
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"

/***
 * DEFINITIONS
 ***/

#define SYNC_INTERVAL 300000 // interval between logging (in ms)
#define ECHO_TO_SERIAL   0 // echo data to serial port
#define WAIT_TO_START    0 
#define DHTPIN 2
#define DHTTYPE DHT22   // DHT 22  (AM2302)

/***
 * VARIABLE SET UP
 ***/

uint32_t syncTime = 0; // time of last sync()
RTC_DS1307 RTC; // define the Real Time Clock object
DHT dht(DHTPIN, DHTTYPE);

const int IR_One = 4;
const int IR_Two = 5;
const int IR_Three = 6;
const int IR_Four = 7;
const int chipSelect = 10;

File logfile;    // the logging file

unsigned long IR_one_count = 0;
char IR_one_state_current = 0;
char IR_one_state_last = 0;

unsigned long IR_two_count = 0;
char IR_two_state_current = 0;
char IR_two_state_last = 0;

unsigned long IR_three_count = 0;
char IR_three_state_current = 0;
char IR_three_state_last = 0;

unsigned long IR_four_count = 0;
char IR_four_state_current = 0;
char IR_four_state_last = 0;


/***
 * IR SENSOR FUNCTIONS
 ***/

void IR_one_counter(void){
  IR_one_state_current = digitalRead(IR_One);
  if (IR_one_state_current != IR_one_state_last){
    if (IR_one_state_current == HIGH){
      IR_one_count++;
#if ECHO_TO_SERIAL 
      Serial.print("First IR Detected | ");
      Serial.print("Number of Detection: ");
      Serial.println(IR_one_count, DEC);
#endif
    }
  }
  IR_one_state_last = IR_one_state_current;
}

void IR_two_counter(void){
  IR_two_state_current = digitalRead(IR_Two);
  if(IR_two_state_current != IR_two_state_last){
    if (IR_two_state_current == HIGH){
      IR_two_count++;
#if ECHO_TO_SERIAL 
      Serial.print("Second IR Detected | ");
      Serial.print("Number of Detection: ");
      Serial.println(IR_two_count, DEC);
#endif
    }
  }
  IR_two_state_last = IR_two_state_current;
}

void IR_three_counter(void){
  IR_three_state_current = digitalRead(IR_Three);
  if(IR_three_state_current != IR_three_state_last){
    if (IR_three_state_current == HIGH){
      IR_three_count++;
#if ECHO_TO_SERIAL 
      Serial.print("Third IR Detected | ");
      Serial.print("Number of Detection: ");
      Serial.println(IR_three_count, DEC);
#endif
    }
  }
  IR_three_state_last = IR_three_state_current;
}

void IR_four_counter(void){
  IR_four_state_current = digitalRead(IR_Four);
  if(IR_four_state_current != IR_four_state_last){
    if (IR_four_state_current == HIGH){
      IR_four_count++;
#if ECHO_TO_SERIAL 
      Serial.print("Fourth IR Detected | ");
      Serial.print("Number of Detection: ");
      Serial.println(IR_four_count, DEC);
#endif
    }
  }
  IR_four_state_last = IR_four_state_current;
}


/***
 * SD CARD ERROR FUNCTION
 ***/


void error(char *str)
{
#if ECHO_TO_SERIAL 
  Serial.print("error: ");
  Serial.println(str);
#endif
  while(1);
}


/***
 * SETUP
 ***/
void setup(void)
{
  delay(1000);
  Serial.begin(9600);
#if ECHO_TO_SERIAL 
  Serial.println(); 
  Serial.print("Initializing SD card...");
#endif
  pinMode(10, OUTPUT);


  // initialize IR sensor pins
  pinMode(IR_One, INPUT);
  pinMode(IR_Two, INPUT);
  pinMode(IR_Three, INPUT);
  pinMode(IR_Four, INPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
#if ECHO_TO_SERIAL 
  Serial.println("card initialized.");
#endif

  char filename[] = "ANTCT000.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      logfile = SD.open(filename, FILE_WRITE); 
      break;
    }
  }

  if (! logfile) {
    error("couldnt create file");
  }
#if ECHO_TO_SERIAL 
  Serial.print("Logging to: ");
  Serial.println(filename);
#endif

  dht.begin(); // Humid & Temperature Sensor Begin
  Wire.begin();  
  if (!RTC.begin()) {
    logfile.println("RTC failed");

#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL
  }

  logfile.println("Date&Time,IR1,IR2,IR3,IR4,Humidity,Temperature");    
#if ECHO_TO_SERIAL 
  Serial.println("Date&Time,IR1,IR2,IR3,IR4,Humidity,Temperature");
#endif

  IR_one_count = 0;
  IR_two_count = 0;
  IR_three_count = 0;
  IR_four_count = 0;
  pinMode(13, OUTPUT);
}

/***
 * MAIN LOOP
 ***/


void loop(void)
{
  DateTime now;

  IR_one_counter();
  IR_two_counter();
  IR_three_counter();
  IR_four_counter();

  if ((millis() - syncTime) > SYNC_INTERVAL) {
    // fetch the time
    now = RTC.now();
    // log time
    logfile.print('"');
    logfile.print(now.year(), DEC);
    logfile.print("/");
    logfile.print(now.month(), DEC);
    logfile.print("/");
    logfile.print(now.day(), DEC);
    logfile.print(" ");
    logfile.print(now.hour(), DEC);
    logfile.print(":");
    logfile.print(now.minute(), DEC);
    logfile.print(":");
    logfile.print(now.second(), DEC);
    logfile.print('"');

#if ECHO_TO_SERIAL
    Serial.print('"');
    Serial.print(now.year(), DEC);
    Serial.print("/");
    Serial.print(now.month(), DEC);
    Serial.print("/");
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(":");
    Serial.print(now.second(), DEC);
    Serial.print('"');
#endif //ECHO_TO_SERIAL

    logfile.print(", ");    
    logfile.print(IR_one_count);
    logfile.print(", ");    
    logfile.print(IR_two_count);
    logfile.print(", ");    
    logfile.print(IR_three_count);
    logfile.print(", ");    
    logfile.print(IR_four_count);

#if ECHO_TO_SERIAL
    Serial.print(", ");   
    Serial.print(IR_one_count);
    Serial.print(", ");   
    Serial.print(IR_two_count);
    Serial.print(", ");   
    Serial.print(IR_three_count);
    Serial.print(", ");   
    Serial.print(IR_four_count);
#endif //ECHO_TO_SERIAL

    IR_one_count = 0;
    IR_two_count = 0;
    IR_three_count = 0;
    IR_four_count = 0; 

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    logfile.print(", ");
    logfile.print(humidity);
    logfile.print("%");
    logfile.print(", ");
    logfile.print(temperature);
    logfile.print("*C");

#if ECHO_TO_SERIAL  
    Serial.print(", ");
    Serial.print(humidity);
    Serial.print("%");
    Serial.print(", ");
    Serial.print(temperature);
    Serial.print("*C");
#endif //ECHO_TO_SERIAL

    logfile.println();
#if ECHO_TO_SERIAL
    Serial.println();
#endif // ECHO_TO_SERIAL

    syncTime = millis();
    logfile.flush();
  }
}



