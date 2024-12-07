#include <LiquidCrystal_I2C.h>
#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif


#include <Wire.h>
#include <math.h>

//Speed sensor code
const byte PulsesPerRevolution = 1;
const unsigned long ZeroTimeout = 100000;
const byte numReadings = 2;
const float wheeleRadius = 0.0001; //in kilometers

volatile unsigned long LastTimeWeMeasured;
volatile unsigned long PeriodBetweenPulses = ZeroTimeout + 1000;
volatile unsigned long PeriodAverage = ZeroTimeout + 1000;
unsigned long FrequencyRaw;
unsigned long FrequencyReal;
unsigned long RPM;
unsigned int PulseCounter = 1;
unsigned long PeriodSum;

unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;
unsigned long CurrentMicros = micros();
unsigned int AmountOfReadings = 1;
unsigned int ZeroDebouncingExtra;
unsigned long readings[numReadings];
unsigned long readIndex;  
unsigned long total; 
unsigned long average;

//lcddisplay code
uint8_t uno[4][8] = {{0x1, 0x3, 0x7, 0x1, 0x1, 0x1, 0x1, 0x1},{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},{0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0xf, 0x1f},{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1e, 0x1f}};
uint8_t dos[4][8] = {{0x7, 0xf, 0x1c, 0x18, 0x0, 0x0, 0x0, 0x0},{0x1c, 0x1e, 0x7, 0x3, 0x3, 0x3, 0x7, 0xe},{0x0, 0x0, 0x1, 0x3, 0x7, 0xe, 0x1f, 0x1f},{0x1c, 0x18, 0x10, 0x0, 0x0, 0x0, 0x1f, 0x1f}};
uint8_t tres[4][8] = {{0x7, 0xf, 0x1c, 0x18, 0x0, 0x0, 0x0, 0x1},{0x1c, 0x1e, 0x7, 0x3, 0x3, 0x7, 0xe, 0x1c},{0x1, 0x0, 0x0, 0x0, 0x18, 0x1c, 0xf, 0x7},{0x1c, 0xe, 0x7, 0x3, 0x3, 0x7, 0x1e, 0x1c}};
uint8_t cuatro[4][8] = {{0x1, 0x3, 0x3, 0x6, 0x6, 0xc, 0xc, 0x18},{0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc},{0x18, 0x1f, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0},{0xc, 0x1f, 0x1f, 0xc, 0xc, 0xc, 0xc, 0xc}};
uint8_t cinco[4][8] = {{0x7, 0xf, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1c},{0x1e, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},{0xf, 0x3, 0x0, 0x0, 0x0, 0x0, 0x1f, 0x1f},{0x1e, 0x1f, 0x3, 0x3, 0x3, 0x3, 0x1f, 0x1e}};
uint8_t seis[4][8] = {{0x0, 0x0, 0x1, 0x3, 0x7, 0x6, 0xe, 0xc},{0x1c, 0x18, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0},{0x1f, 0x1c, 0x18, 0x18, 0x18, 0x1c, 0xf, 0x7},{0x1e, 0x7, 0x3, 0x3, 0x3, 0x7, 0x1e, 0x1c}};
uint8_t siete[4][8] = {{0x1f, 0x1f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},{0x1f, 0x1f, 0x3, 0x7, 0x6, 0xc, 0xc, 0x18},{0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1},{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10}};
uint8_t ocho[4][8] = {{0xf, 0x1f, 0x1c, 0x18, 0x18, 0x18, 0x1c, 0xf},{0x1e, 0x1f, 0x7, 0x3, 0x3, 0x3, 0x7, 0x1e},{0xf, 0x1c, 0x18, 0x18, 0x18, 0x1c, 0x1f, 0xf},{0x1e, 0x7, 0x3, 0x3, 0x3, 0x7, 0x1f, 0x1e}};
uint8_t nueve[4][8] = {{0x7, 0xf, 0x1c, 0x18, 0x18, 0x18, 0x1c, 0xf},{0x1c, 0x1e, 0x7, 0x3, 0x3, 0x3, 0x7, 0x1f},{0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x3, 0x7},{0x6, 0xe, 0xc, 0x1c, 0x18, 0x10, 0x0, 0x0}};
uint8_t cero[4][8] = {{0x7, 0xf, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x18},{0x1c, 0x1e, 0x7, 0xb, 0xb, 0x13, 0x13, 0x3},{0x18, 0x19, 0x19, 0x1a, 0x1a, 0x1c, 0xf, 0x7},{0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x1e, 0x1c}};

int positionOrder[4][2] = {{0,0},{1,0},{0,1},{1,1}};

LiquidCrystal_I2C lcd(0x27, 16, 2);
void printNumber(int Cp, bool index){
  for (int i = 0; i < 4; i++){
    lcd.setCursor(Cp + positionOrder[i][0], positionOrder[i][1]);
    lcd.printByte(4*index+i);
  }
}

void changeNumber(int num, bool index){
  switch (num){
    case 0:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,cero[i]);
      }
      break;
    case 1:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,uno[i]);
      }
      break;
    case 2:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,dos[i]);
      }
      break;
    case 3:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,tres[i]);
      }
      break;
    case 4:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,cuatro[i]);
      }
      break;
    case 5:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,cinco[i]);
      }
      break;
    case 6:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,seis[i]);
      }
      break;
    case 7:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,siete[i]);
      }
      break;
    case 8:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,ocho[i]);
      }
      break;
    case 9:
      for (int i = 0; i < 4; i++){
        lcd.createChar(4*index+i,nueve[i]);
      }
      break;
    default:
      break;
  }
}

//input is float with 1 to 2 whole digits
//output is array with 4 numbers
void splitter(float input, int *output){
  int putin = (int)(input * 100);
  output[3] = (putin % 10);
  putin= putin / 10;
  output[2] = (putin % 10);
  putin= putin / 10;
  output[1] = (putin % 10);
  putin= putin / 10;
  output[0] = (putin % 10);
  putin= putin / 10;
}

void fourdisplay(float speed){
  int depositArray[4];
  splitter(speed, depositArray);
  lcd.setCursor(6,0); //speed subdigets location
  lcd.print(".");
  lcd.print(depositArray[2]);
  lcd.print(depositArray[3]);
  changeNumber(depositArray[0],0);
  changeNumber(depositArray[1],1);
}

void Speed_code(){
  LastTimeCycleMeasure = LastTimeWeMeasured;
  CurrentMicros = micros();
  if (CurrentMicros < LastTimeCycleMeasure) {
    LastTimeCycleMeasure = CurrentMicros;
  }
  FrequencyRaw = 10000000000 / PeriodAverage;
  if (PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra) {
    FrequencyRaw = 0;  // Set frequency as 0.
    ZeroDebouncingExtra = 2000;
  } else {
    ZeroDebouncingExtra = 0;
  }
  FrequencyReal = FrequencyRaw / 10000;

  RPM = FrequencyRaw / PulsesPerRevolution * 60;
  RPM = RPM / 10000;
  total = total - readings[readIndex];
  readings[readIndex] = RPM;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;

  float speed = wheeleRadius * RPM * 120 * 3.14159265;
  fourdisplay(speed);

  // Serial.print("Period: ");
  // Serial.print(PeriodBetweenPulses);
  // Serial.print("\tReadings: ");
  // Serial.print(AmountOfReadings);
  // Serial.print("\tFrequency: ");
  // Serial.print(FrequencyReal);
  // Serial.print("\tRPM: ");
  // Serial.print(RPM);
  // Serial.print("\tTachometer: ");
  // Serial.println(average);

  Serial.print("S-RPM:");
  Serial.println(RPM);
}

//Temperature sensor code:
#include <OneWire.h>
#include "DallasTemperature.h"



// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 6
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found
const int WarningTemperature1 = 22;
const int WarningTemperature2 = 24;
int tempexceeded[4] = {0,0,0,0};
float temperatures[4] = {0,0,0,0};
bool blink = 0;
bool on = 0;

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

int checktemp(){
  for(int i=0; i < sizeof(tempexceeded) / sizeof(tempexceeded[0]); i++){
    if(tempexceeded[i] == 1){
      return 1;
    }else if(tempexceeded[i] == 2){
      return 2;
    }
  }
  return 0;
}

void printHeat(float value){
    lcd.setCursor(11, 0); //temperature warning location
    lcd.print(value);
    lcd.setCursor(11, 1);
    lcd.print("heat!");
    on = 1;
}

void unprintHeat(){
      lcd.setCursor(11, 0); //temperature warning location
      lcd.print(".   .");
      lcd.setCursor(11, 1);
      lcd.print(".   .");
      on = 0;
}
/*
void updateState(){
  if(checktemp() == 1){
    printHeat();
  }else if(checktemp() == 2 ){
    if(blink){
      unprintHeat();
      blink = 0;
    }else{
      printHeat();
      blink = 1;
    }
  }else{
    unprintHeat(); 
  }
}
*/

float findMax(float *yarra){
  float max = 0;
  for(int i=0; i<4; i++){
    if(yarra[i]>max){
      max = yarra[i];
    }
  }
  return max;
}

void Temp_code(){
  sensors.requestTemperatures(); // Send the command to get temperatures
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
		
      // Output the device ID
      Serial.print(i,DEC);

      // Print the data
      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print(":");
      Serial.print(tempC);
      Serial.println();
      temperatures[i] = tempC;
      /*if(tempC > WarningTemperature1){
        if(tempC > WarningTemperature2){
          if(tempexceeded[i] != 2){
            tempexceeded[i] = 2;
          }
          updateState();
        }else if(tempexceeded[i] != 1){
          tempexceeded[i] = 1;
          updateState();
        }
      }else if(tempexceeded[i] == 1){
        tempexceeded[i] = 0;
        updateState();
      }
*/
    } 	
  }
  Serial.println(findMax(temperatures));
  float temp = findMax(temperatures);
  if(temp > WarningTemperature1){
    if(temp > WarningTemperature2){
      if(on){
        unprintHeat();
      }else{
        printHeat(temp);
      }
    }else{
      printHeat(temp);
    }
  }else{
    if(on){
      unprintHeat();
    }
  }
}
void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);
  delay(1000);
  Serial.println("enter start");

  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.home();
  printNumber(1,0); //speed letters location (location, index)
  printNumber(3,1);
  lcd.setCursor(6,1);//speed units location
  lcd.print("km/h"); 




  while (Serial.available() == 0) {}     //wait for data available
  //Temperature sensor code:
  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      // printAddress(tempDeviceAddress);
      for (uint8_t i = 0; i < 8; i++) {
        if (tempDeviceAddress[i] < 16) Serial.print("0");
      Serial.print(tempDeviceAddress[i], HEX);
      }

      Serial.println();
		} else {
		  Serial.print("Found ghost device at ");
		  Serial.print(i, DEC);
		  Serial.print(" but could not detect address. Check power and cabling");
		}
  }
}

void loop() {
  if(Serial.available() > 0) {
    String str = Serial.readString();
  }


    //temperature sensor code:
      Temp_code();
        // delay(1500);
        
    //Speed sensor code:
      Speed_code();
}


void Pulse_Event() {
  PeriodBetweenPulses = micros() - LastTimeWeMeasured;
  LastTimeWeMeasured = micros();
  if (PulseCounter >= AmountOfReadings)  {
    PeriodAverage = PeriodSum / AmountOfReadings;
    PulseCounter = 1;
    PeriodSum = PeriodBetweenPulses;

    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);
    AmountOfReadings = RemapedAmountOfReadings;
  } else {
    PulseCounter++;
    PeriodSum = PeriodSum + PeriodBetweenPulses;
  }
}
