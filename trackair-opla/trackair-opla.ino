// ========================================START Opla Initialization========================================
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_MKRIoTCarrier_Buzzer.h>
#include <Arduino_MKRIoTCarrier_Qtouch.h>
#include <Arduino_MKRIoTCarrier_Relay.h>

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

// ========================================START LIS2MDL Initialization========================================
// #include <Adafruit_LIS2MDL.h>
// #include <Adafruit_Sensor.h>

// /* Assign a unique ID to this sensor at the same time */
// Adafruit_LIS2MDL lis2mdl = Adafruit_LIS2MDL(12345);
// #define LIS2MDL_CLK 13
// #define LIS2MDL_MISO 12
// #define LIS2MDL_MOSI 11
// #define LIS2MDL_CS 10
// ========================================END LIS2MDL Initialization========================================


float temperature = 0;
float humidity = 0;
int toxic_chemical_level = -1;
int tvoc_level = -1;
int eco2_level = -1;
int h2_level = -1;
int ethanol_level = -1;

int moderateNote = 3;
int dangerousNote = 6;
int moderateNoteDuration = 50;
int dangerousNoteDuration = 5;

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

  Serial.begin(115200);
  
  carrier.begin();
  carrier.display.setRotation(2);

  // For SGP30 Sensor  
  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

  // For LIS2MDL Sensor
  // lis2mdl.enableAutoRange(true);  // Enable auto-gain
  // if (!lis2mdl.begin()) {  // I2C mode
  //   Serial.println("Ooops, no LIS2MDL detected ... Check your wiring!");
  //   while (1) delay(10);
  // }
  // lis2mdl.printSensorDetails();  // Display some basic information on this sensor

  // For Initializing LEDs
  carrier.leds.setBrightness(10);
  carrier.leds.fill(whiteColor, 0, 5);
  carrier.leds.show();
}

void loop() {

  temperature = carrier.Env.readTemperature();
  humidity = carrier.Env.readHumidity();

  float air_pressure = carrier.Pressure.readPressure();
  
  int r, g, b, light_intensity; 
  if (carrier.Light.colorAvailable()){
    carrier.Light.readColor(r, g, b, light_intensity); 
  }  
  

  toxic_chemical_level = analogRead(A5);  // [1]
  

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }

  tvoc_level = sgp.TVOC;
  eco2_level = sgp.eCO2;

  carrier.Buttons.update();

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  if (! sgp.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return;
  }

  h2_level = sgp.rawH2;
  ethanol_level = sgp.rawEthanol;
  
  Serial.print(temperature);
  Serial.print(", ");
  Serial.print(humidity);
  Serial.print(", ");
  Serial.print(air_pressure);
  Serial.print(", ");
  Serial.print(light_intensity);
  Serial.print(", ");
  Serial.print(toxic_chemical_level);
  Serial.print(", ");
  Serial.print(tvoc_level);
  Serial.print(", ");
  Serial.print(eco2_level);
  Serial.print(", ");
  Serial.print(h2_level);
  Serial.print(", ");
  Serial.print(ethanol_level);
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
  printTVOCLevel();

  carrier.leds.setPixelColor(1, greenColor);
  carrier.leds.show();
  carrier.leds.clear();
}

void taskOnTouchButton2(){
  printeCO2Level();

  carrier.leds.setPixelColor(2, greenColor);
  carrier.leds.show();
  carrier.leds.clear();
}

void taskOnTouchButton3(){
  printH2Level();

  carrier.leds.setPixelColor(3, greenColor);
  carrier.leds.show();
  carrier.leds.clear();
}

void taskOnTouchButton4(){
  printEthanolLevel();

  carrier.leds.setPixelColor(4, greenColor);
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

void printTVOCLevel() {

  // Threshold, can be changed.
  int toxic_tvoc_clean = 220;
  int toxic_tvoc_danger = 2200; // [3]

  // Set background based on air quality.
  if(tvoc_level < toxic_tvoc_clean){
    carrier.display.fillScreen(ST77XX_GREEN);    
  } else if (tvoc_level > toxic_tvoc_danger){
    carrier.display.fillScreen(ST77XX_RED);
  } else {
    carrier.display.fillScreen(ST77XX_YELLOW);
  }
    
  carrier.display.setTextColor(ST77XX_WHITE); // White text
  carrier.display.setTextSize(3); // Medium sized text

  carrier.display.setCursor(76, 70); //sets position for printing (x and y)
  carrier.display.println("TVOC");
  carrier.display.setCursor(73, 100); //sets position for printing (x and y)
  carrier.display.println("Level");
  
  carrier.display.setCursor(80, 150); //sets position for printing (x and y)
  carrier.display.print(tvoc_level);
  carrier.display.println(" ppb");
  
  if(tvoc_level < toxic_tvoc_clean){
    carrier.display.setCursor(70, 180);
    carrier.display.println("Clean");  
  } else if (tvoc_level > toxic_tvoc_danger){
    carrier.display.setCursor(40, 180);
    carrier.display.println("Dangerous");
  } else {
    carrier.display.setCursor(50, 180);
    carrier.display.println("Moderate");
  }

}

void printeCO2Level() {

  // Threshold, can be changed.
  int toxic_eco2_clean = 1000;
  int toxic_eco2_danger = 4000; // [3]

  // Set background based on air quality.
  if(eco2_level < toxic_eco2_clean){
    carrier.display.fillScreen(ST77XX_GREEN);  
    carrier.Buzzer.sound(moderateNote);
    delay(moderateNoteDuration);
    //delay(moderateNoteDuration);
    carrier.Buzzer.noSound(); 
  } else if (eco2_level > toxic_eco2_danger){
    carrier.display.fillScreen(ST77XX_RED);
    carrier.Buzzer.sound(10);
    delay(dangerousNoteDuration);
    carrier.Buzzer.noSound();
  } else {
    carrier.display.fillScreen(ST77XX_YELLOW);
    carrier.Buzzer.sound(dangerousNote);
    delay(dangerousNoteDuration);
    //delay(moderateNoteDuration);
    carrier.Buzzer.noSound();
  }
    
  carrier.display.setTextColor(ST77XX_WHITE); // White text
  carrier.display.setTextSize(3); // Medium sized text

  carrier.display.setCursor(76, 70); //sets position for printing (x and y)
  carrier.display.println("eCO2");
  carrier.display.setCursor(73, 100); //sets position for printing (x and y)
  carrier.display.println("Level");
  
  carrier.display.setCursor(70, 150); //sets position for printing (x and y)
  carrier.display.print(eco2_level);
  carrier.display.println(" ppm");
  
  if(eco2_level < toxic_eco2_clean){
    carrier.display.setCursor(70, 180);
    carrier.display.println("Clean");  
  } else if (eco2_level > toxic_eco2_danger){
    carrier.display.setCursor(40, 180);
    carrier.display.println("Dangerous");
  } else {
    carrier.display.setCursor(50, 180);
    carrier.display.println("Moderate");
  }

}

void printH2Level() {

  // Threshold, can be changed.
  int toxic_h2_clean = 14000;
  int toxic_h2_danger = 20000; // [3]

  // Set background based on air quality.
  if(h2_level < toxic_h2_clean){
    carrier.display.fillScreen(ST77XX_GREEN);    
  } else if (h2_level > toxic_h2_danger){
    carrier.display.fillScreen(ST77XX_RED);
  } else {
    carrier.display.fillScreen(ST77XX_YELLOW);
  }
    
  carrier.display.setTextColor(ST77XX_WHITE); // White text
  carrier.display.setTextSize(3); // Medium sized text

  carrier.display.setCursor(80, 40); //sets position for printing (x and y)
  carrier.display.println("Raw");
  carrier.display.setCursor(90, 70); //sets position for printing (x and y)
  carrier.display.println("H2");
  carrier.display.setCursor(73, 100); //sets position for printing (x and y)
  carrier.display.println("Level");
  
  carrier.display.setCursor(70, 150); //sets position for printing (x and y)
  carrier.display.println(h2_level);
  
  if(h2_level < toxic_h2_clean){
    carrier.display.setCursor(70, 180);
    carrier.display.println("Clean");  
  } else if (h2_level > toxic_h2_danger){
    carrier.display.setCursor(40, 180);
    carrier.display.println("Dangerous");
  } else {
    carrier.display.setCursor(50, 180);
    carrier.display.println("Moderate");
  }

}

void printEthanolLevel() {

  // Threshold, can be changed.
  int toxic_ethanol_clean = 25000;
  int toxic_ethanol_danger = 40000; // [3]

  // Set background based on air quality.
  if(ethanol_level < toxic_ethanol_clean){
    carrier.display.fillScreen(ST77XX_GREEN);    
  } else if (ethanol_level > toxic_ethanol_danger){
    carrier.display.fillScreen(ST77XX_RED);
  } else {
    carrier.display.fillScreen(ST77XX_YELLOW);
  }
    
  carrier.display.setTextColor(ST77XX_WHITE); // White text
  carrier.display.setTextSize(3); // Medium sized text

  carrier.display.setCursor(80, 40); //sets position for printing (x and y)
  carrier.display.println("Raw");
  carrier.display.setCursor(55, 70); //sets position for printing (x and y)
  carrier.display.println("Ethanol");
  carrier.display.setCursor(73, 100); //sets position for printing (x and y)
  carrier.display.println("Level");
  
  carrier.display.setCursor(70, 150); //sets position for printing (x and y)
  carrier.display.println(ethanol_level);
  
  if(ethanol_level < toxic_ethanol_clean){
    carrier.display.setCursor(70, 180);
    carrier.display.println("Clean");  
  } else if (ethanol_level > toxic_ethanol_danger){
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
// [3]: Safe CO2 and TVOC numbers
//      https://www.ibm.com/docs/en/mwi?topic=shields-air-quality-shield
