// ========================================START Opla Initialization========================================

#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_MKRIoTCarrier_Buzzer.h>
#include <Arduino_MKRIoTCarrier_Qtouch.h>
#include <Arduino_MKRIoTCarrier_Relay.h>

#include <Arduino_MKRIoTCarrier.h>

#include <SoftwareSerial.h>

MKRIoTCarrier carrier;

// ========================================END Opla Initialization========================================

// ========================================START SGP Initialization========================================
#include <Wire.h>
#include <Adafruit_SGP30.h>

Adafruit_SGP30 sgp;

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}
// ========================================END SGP Initialization========================================


float temperature = 0;
float humidity = 0;
int toxic_chemical_level = -1;
int tvoc_level = -1;
int eco2_level = -1;
bool showMainScreen = true;

bool buttonPressed0 = false;
bool buttonPressed1 = false;
bool buttonPressed2 = false;
bool buttonPressed3 = false;
bool buttonPressed4 = false;

uint32_t greenColor = carrier.leds.Color(255,0,0);
uint32_t redColor = carrier.leds.Color(0,255,0);
uint32_t blueColor = carrier.leds.Color(0,0,255);
uint32_t whiteColor = carrier.leds.Color(255,255,255);
uint32_t blackColor = carrier.leds.Color(0,0,0);

void setup() {
  bool CARRIER_CASE = true;

  Serial.begin(9600);

  carrier.begin();
  carrier.display.setRotation(2);

  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

  carrier.leds.setBrightness(10);
  carrier.leds.fill(whiteColor, 0, 5);
  carrier.leds.show();
}

void loop() {

  temperature = carrier.Env.readTemperature();
  humidity = carrier.Env.readHumidity();
  toxic_chemical_level = analogRead(A5);  // [1]
  tvoc_level = sgp.TVOC;
  eco2_level = sgp.eCO2;

  carrier.Buttons.update();

  Serial.print(temperature);
  Serial.print(", ");
  Serial.print(humidity);
  Serial.print(", ");
  Serial.print(toxic_chemical_level);
  Serial.print(", ");
  Serial.print(tvoc_level);
  Serial.print(", ");
  Serial.print(eco2_level);
  Serial.println();


  if (buttonPressed0 == false && buttonPressed1 == false && buttonPressed2 == false && buttonPressed3 == false && buttonPressed4 == false){
    carrier.leds.fill(whiteColor, 0, 5);
    carrier.leds.show();
  }
  
  manipulateButtons();

  if (buttonPressed0 == true){
    taskOnTouchButton0();
  }

  else if (buttonPressed1 == true){
    taskOnTouchButton1();
  }

  else if (buttonPressed2 == true){
    taskOnTouchButton2();
  }

  else if (buttonPressed3 == true){
    taskOnTouchButton3();
  }

  else if (buttonPressed4 == true){
    taskOnTouchButton4();
  }

  else{
    
    printMainScreen();
  }
}

// Wrapper Functions

void manipulateButtons(){
  //function to print out values
  if (carrier.Button4.onTouchDown()) {
      if (buttonPressed4 == false){
        buttonPressed0 = false;
        buttonPressed1 = false;
        buttonPressed2 = false;
        buttonPressed3 = false;
        buttonPressed4 = true;
      } else {
        buttonPressed4 = false;
      }
  }

  if (carrier.Button3.onTouchDown()) {
    if (buttonPressed3 == false){
        buttonPressed0 = false;
        buttonPressed1 = false;
        buttonPressed2 = false;
        buttonPressed3 = true;
        buttonPressed4 = false;
      } else {
        buttonPressed3 = false;
      }
  }
  
  if (carrier.Button2.onTouchDown()) {
    if (buttonPressed2 == false){
        buttonPressed0 = false;
        buttonPressed1 = false;
        buttonPressed2 = true;
        buttonPressed3 = false;
        buttonPressed4 = false;
      } else {
        buttonPressed2 = false;
      }
  }
  
  if (carrier.Button1.onTouchDown()) {
    if (buttonPressed1 == false){
        buttonPressed0 = false;
        buttonPressed1 = true;
        buttonPressed2 = false;
        buttonPressed3 = false;
        buttonPressed4 = false;
      } else {
        buttonPressed1 = false;
      }
  }

  if (carrier.Button0.onTouchDown()) {
    if (buttonPressed0 == false){
        buttonPressed0 = true;
        buttonPressed1 = false;
        buttonPressed2 = false;
        buttonPressed3 = false;
        buttonPressed4 = false;
      } else {
        buttonPressed0 = false;
      }
  }
}

void taskOnTouchButton0(){
  printToxicChemicalLevel();
  
  carrier.leds.setPixelColor(0, greenColor);
  carrier.leds.show();
  carrier.leds.clear();
}

void taskOnTouchButton1(){
  carrier.display.fillScreen(ST77XX_BLUE); //blue background

  printHumidity();

  carrier.leds.setPixelColor(1, redColor);
  carrier.leds.show();
  carrier.leds.clear();
}

void taskOnTouchButton2(){
  carrier.display.fillScreen(ST77XX_RED); //red background

  printTemperature();

  carrier.leds.setPixelColor(2, redColor);
  carrier.leds.show();
  carrier.leds.clear();
}

void taskOnTouchButton3(){
  carrier.display.fillScreen(ST77XX_BLUE); //blue background
  
  printHumidity();

  carrier.leds.setPixelColor(3, redColor);
  carrier.leds.show();
  carrier.leds.clear();
}

void taskOnTouchButton4(){
  carrier.display.fillScreen(ST77XX_RED); //red background

  printTemperature();

  carrier.leds.setPixelColor(4, redColor);
  carrier.leds.show();
  carrier.leds.clear();
}

// Main Screen

void printMainScreen() {
  
  carrier.display.fillScreen(blackColor); //black background
  carrier.display.setTextColor(ST77XX_WHITE); //white text
  carrier.display.setTextSize(4); //medium sized text

  carrier.display.setCursor(30, 110); //sets position for printing (x and y)
  carrier.display.print("TrackAir");

}

// Actual Functions

void printToxicChemicalLevel() {

  // Threshold, can be changed.
  int toxic_chemical_clean = 60;
  int toxic_chemical_danger = 100;

  // Set background based on air quality.
  if(toxic_chemical_level < toxic_chemical_clean){
    carrier.display.fillScreen(ST77XX_GREEN);    
  } else if (toxic_chemical_level > toxic_chemical_danger){
    carrier.display.fillScreen(ST77XX_RED);
  } else {
    carrier.display.fillScreen(ST77XX_YELLOW);
  }
    
  carrier.display.setTextColor(ST77XX_WHITE); // White text
  carrier.display.setTextSize(3); // Medium sized text

  carrier.display.setCursor(73, 40); //sets position for printing (x and y)
  carrier.display.println("Toxic");
  carrier.display.setCursor(45, 70); //sets position for printing (x and y)
  carrier.display.println("Chemical");
  carrier.display.setCursor(70, 100); //sets position for printing (x and y)
  carrier.display.println("Level");
  
  carrier.display.setCursor(100, 150); //sets position for printing (x and y)
  carrier.display.println(toxic_chemical_level);
  
  if(toxic_chemical_level < toxic_chemical_clean){
    carrier.display.setCursor(70, 180);
    carrier.display.println("Clean");  
  } else if (toxic_chemical_level > toxic_chemical_danger){
    carrier.display.setCursor(40, 180);
    carrier.display.println("Dangerous");
  } else {
    carrier.display.setCursor(50, 180);
    carrier.display.println("Moderate");
  }

}

void printTemperature() {
  carrier.display.fillScreen(ST77XX_RED); //red background
  carrier.display.setTextColor(ST77XX_WHITE); //white text
  carrier.display.setTextSize(2); //medium sized text

  carrier.display.setCursor(30, 110); //sets position for printing (x and y)
  carrier.display.print("Temp: ");
  carrier.display.print(temperature);
  carrier.display.println(" C");

}

void printHumidity() {
  carrier.display.fillScreen(ST77XX_BLUE); //red background
  carrier.display.setTextColor(ST77XX_WHITE); //white text
  carrier.display.setTextSize(2); //medium sized text

  carrier.display.setCursor(30, 110); //sets new position for printing (x and y)
  carrier.display.print("Humi: ");
  carrier.display.print(humidity);
  carrier.display.println(" %");
}


// References
// [1]: Read directly from the Air Quality 1.3 sensor
//      https://www.hackster.io/omarecd/monitoring-of-indoor-environmental-conditions-153e32
// [2]: Adafruit SGP30 TVOC/eCO2 Gas Sensor Test
//      https://learn.adafruit.com/adafruit-sgp30-gas-tvoc-eco2-mox-sensor/arduino-code