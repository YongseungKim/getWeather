#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include "Melopero_APDS9960.h"

Melopero_APDS9960 device;

/* Definitions */
#define WIFI_SSID "VCOMM_LAB"
#define WIFI_PWD "vcomm13579"
#define LED_BUILTIN 2
#define uS_TO_S_FACTOR 1000000 //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 5        //Time ESP32 will go to sleep (in seconds)

//Define touch sensitivity. Greater the value, more the sensitivity.
#define Threshold 40
touch_pad_t touchPin;

RTC_DATA_ATTR int bootCount = 0;

#define ADPS9960_PIN    GPIO_NUM_15

//i2c define
int pin_i2c_sda = GPIO_NUM_21;
int pin_i2c_scl = GPIO_NUM_22;
int apds9960_int = ADPS9960_PIN;

const char *ssid = WIFI_SSID;    // 연결할 SSID
const char *password = WIFI_PWD; // 연결할 SSID의 비밀번호
bool tickerFlag = false;
bool initWiFi();

int isr_flag = 0;
void interruptRoutine();
bool handleGesture();
void initDeepSleep();
void print_wakeup_reason();
void print_wakeup_touchpad();
void print_GPIO_wake_up();

unsigned long t_loop;
unsigned long t_sleep;
unsigned long t_gesture;
int led = 0;

void callback()
{
    //placeholder callback function
}

void init_APDS9960(){
    int8_t status = NO_ERROR;
    // Initialize APDS-9960 (configure I2C and initial values)
    status = device.init(); // Initialize the comunication library
    if (status != NO_ERROR){
        Serial.println("Error during APDS-9960's initialization");
        while(true);
    }
    status = device.reset(); // Reset all interrupt settings and power off the device
    if (status != NO_ERROR){
        Serial.println("Error during APDS-9960 reset.");
        while(true);
    }
    Serial.println(F("APDS-9960 initialization complete"));
    
    // Gesture engine settings
    device.enableGesturesEngine(); // enable the gesture engine
    device.setGestureProxEnterThreshold(25); // Enter the gesture engine only when the proximity value 
    // is greater than this value proximity value ranges between 0 and 255 where 0 is far away and 255 is very near.
    device.setGestureExitThreshold(20); // Exit the gesture engine only when the proximity value is less 
    // than this value.
    device.setGestureExitPersistence(EXIT_AFTER_4_GESTURE_END); // Exit the gesture engine only when 4
    // consecutive gesture end signals are fired (distance is greater than the threshold)

    // device.setLedDrive(LED_DRIVE_100_mA);
    // device.setLedBoost(LED_BOOST_300);
    device.setGestureLedDrive(LED_DRIVE_100_mA);

    device.enableGestureInterrupts(true);
    device.wakeUp(); // wake up the device
}   


void setup()
{
    
    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    initWiFi();

    Serial.println(F("--------------------------------"));
    Serial.println(F("SparkFun APDS-9960 - GestureTest"));
    Serial.println(F("--------------------------------"));
    
    // Initialize interrupt service routine
    // attachInterrupt(0, interruptRoutine, FALLING);

    
    init_APDS9960();

    initDeepSleep();
    // attachInterrupt(digitalPinToInterrupt(apds9960_int), interruptRoutine, FALLING);

    t_loop = millis();
    t_sleep = millis();
}

void loop()
{
    // put your main code here, to run repeatedly:
    if (millis() > (t_loop + 500)){
        t_loop = millis();
        Serial.write('*');
        if (led){
            digitalWrite(LED_BUILTIN, LOW);
        }
        else{
            digitalWrite(LED_BUILTIN, HIGH);
        }
        led = 1 - led;
    }

    if(millis() > (t_gesture+50)){
        t_gesture = millis();
        if(handleGesture()){
            tickerFlag = true;
        }
    }    

    if(millis() > (t_sleep + 30000)){        
        t_sleep = millis();
        if(!tickerFlag){
            //Go to sleep now
            Serial.println("Going to sleep now");
            digitalWrite(LED_BUILTIN, HIGH);
            delay(1000);
            esp_deep_sleep_start();
            Serial.println("This will never be printed");   
        } else {            
            tickerFlag =  false;
        }
    }
}

bool initWiFi()
{
    Serial.println();

    if (!WiFi.begin(ssid, password)){
        Serial.println("ERROR: WiFi.begin");
        return false;
    }

    Serial.println("OK: WiFi.begin");

    while (WiFi.status() != WL_CONNECTED){
        delay(100);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("OK: WiFi connected");

    delay(1000);

    return true;
}


bool handleGesture()
{

    bool ret = false;
    device.updateGestureStatus();

    if (device.gestureFifoHasData){

        // Reads the gesture data for the given amount of time and tries to interpret a gesture. 
        // The device tries to detect a gesture by comparing the gesture data values through time. 
        // The device compares the up data with the down data to detect a gesture on the up-down axis and
        // it compares the left data with the right data to detect a gesture on the left right axis.
        //
        // ADVANCED SETTINGS:
        // device.parseGesture(uint parse_millis, uint8_t tolerance = 12, uint8_t der_tolerance = 6, uint8_t confidence = 6);
        //
        // parse_millis: the time in millisecond to read the gesture data and try to interpret a gesture
        //
        // The tolerance parameter determines how much the two values (on the same axis) have to differ to interpret
        // the current dataset as valid for gesture detection (if the values are nearly the same then its not possible to decide the direction 
        // in which the object is moving).
        //
        // The der_tolerance does the same for the derivative of the two curves (the values on one axis through time):
        // this prevents the device from detecting a gesture if the objects surface is not even...
        //
        // The confidence tells us the minimum amount of "detected gesture samples" needed for an axis to tell that a gesture has been detected on that axis:
        // How its used in the source code: if (detected_up_gesture_samples > detected_down_gesture_samples + confidence) gesture_up_down = GESTURE_UP
        device.parseGesture(300);

        if (device.parsedUpDownGesture != NO_GESTURE || device.parsedLeftRightGesture != NO_GESTURE){
            Serial.print("Gesture : ");
            ret = true;
        }

        if (device.parsedUpDownGesture == UP_GESTURE)
            Serial.print("UP ");
        else if (device.parsedUpDownGesture == DOWN_GESTURE)
            Serial.print("DOWN ");

        if (device.parsedLeftRightGesture == LEFT_GESTURE)
            Serial.print("LEFT ");
        else if (device.parsedLeftRightGesture == RIGHT_GESTURE)
            Serial.print("RIGHT ");

        if (device.parsedUpDownGesture != NO_GESTURE || device.parsedLeftRightGesture != NO_GESTURE)
            Serial.println();
    }
    return ret;
}


void initDeepSleep(){

    //Increment boot number and print it every reboot
    ++bootCount;
    Serial.println("Boot number: " + String(bootCount));
    //Print the wakeup reason for ESP32
    print_wakeup_reason();

    #if 0    
    print_wakeup_touchpad();
    //Setup interrupt on Touch Pad 3 (GPIO15)
    touchAttachInterrupt(T3, callback, Threshold);
    //Configure Touchpad as wakeup source
    esp_sleep_enable_touchpad_wakeup();
    #endif

    #if 0
    //Set timer to 5 seconds    
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                   " Seconds");
    #endif

    #if 0
    //RTC GPIO핀들 (32~39)을 사용할 수 있다.
    //Configure GPIO33 as ext0 wake up source for HIGH logic level
    esp_sleep_enable_ext0_wakeup(ADPS9960_PIN, 0);
    #endif
    
    #if 1
    //Configure GPIO32 & GPIO33 as ext1 wake up source for HIGH logic level
    //Pushbuttons connected to GPIO32 & GPIO33
    #define BUTTON_PIN_BITMASK 0x300000000
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW);
    #endif
}
//Function that prints the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason)
    {
    case 1:
        Serial.println("Wakeup caused by external signal using RTC_IO");
        break;
    case 2:
        Serial.println("Wakeup caused by external signal using RTC_CNTL");
        print_GPIO_wake_up();
        break;
    case 3:
        Serial.println("Wakeup caused by timer");
        break;
    case 4:
        Serial.println("Wakeup caused by touchpad");
        break;
    case 5:
        Serial.println("Wakeup caused by ULP program");
        break;
    default:
        Serial.println("Wakeup was not caused by deep sleep");
        break;
    }
}

//Function that prints the touchpad by which ESP32 has been awaken from sleep
void print_wakeup_touchpad()
{
    // touch_pad_t pin;
    touchPin = esp_sleep_get_touchpad_wakeup_status();
    switch (touchPin)
    {
    case 0:
        Serial.println("Touch detected on GPIO 4");
        break;
    case 1:
        Serial.println("Touch detected on GPIO 0");
        break;
    case 2:
        Serial.println("Touch detected on GPIO 2");
        break;
    case 3:
        Serial.println("Touch detected on GPIO 15");
        break;
    case 4:
        Serial.println("Touch detected on GPIO 13");
        break;
    case 5:
        Serial.println("Touch detected on GPIO 12");
        break;
    case 6:
        Serial.println("Touch detected on GPIO 14");
        break;
    case 7:
        Serial.println("Touch detected on GPIO 27");
        break;
    case 8:
        Serial.println("Touch detected on GPIO 33");
        break;
    case 9:
        Serial.println("Touch detected on GPIO 32");
        break;
    default:
        Serial.println("Wakeup not by touchpad");
        break;
    }
}

/*
Method to print the GPIO that triggered the wakeup
*/
void print_GPIO_wake_up(){
  int GPIO_reason = esp_sleep_get_ext1_wakeup_status();
  Serial.print("GPIO that triggered the wake up: GPIO ");
  Serial.println((log(GPIO_reason))/log(2), 0);
}

