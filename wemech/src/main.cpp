#include <Arduino.h>
#include <IRremote.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
//#include <AccelStepper.h>

#define stepPin 9
#define dirPin 8
#define enablePin 7

#define motorInterfaceType 1
#define IR_PIN 13

double turn = 0;

int maxTurns = 30;

// IRrecv irrecv(IR_PIN);
// decode_results results;

SoftwareSerial bluetooth(10, 11); // RX, TX

// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void writeIntIntoEEPROM(int address, int number){
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address){
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}


void setup() {

  bluetooth.begin(9600);
  Serial.begin(9600);

  turn = readIntFromEEPROM(0);
  //   lcd.clear();
  // lcd.print(turn);
  
  // lcd.begin(16, 2);

  // irrecv.enableIRIn();

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

}

boolean justStarted = true;

int i = 0;

boolean dir;




void runMotor(int turns, boolean direction) {

  digitalWrite(enablePin, LOW);

  if (direction == true) {
    digitalWrite(dirPin, HIGH);
  } else {
    digitalWrite(dirPin, LOW);
  }

  for(int i = 0; i < turns; i++){

    if(direction == true && turn >= maxTurns){
      break;
    } else if(direction == false && turn <= 0){
      break;
    }


    for(int i = 0; i < 1600; i++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(50);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(50);
    }

    if(direction == true) {
      turn = turn + 1;
      if(turn >= maxTurns){
        turn = maxTurns;
        break;
      }
    } else {
      turn = turn - 1;
      if(turn <= 0){
        turn = 0;
        break;
      }
    }
  }
  // lcd.setCursor(1, 1);
  // lcd.clear();
  // lcd.print(turn);
  digitalWrite(enablePin, HIGH);
bluetooth.println(String(digitalRead(7)) + ";");
  writeIntIntoEEPROM(0, turn);
}

void runMotor_Decimal(int turns, boolean direction) {

  if (direction == true) {
    digitalWrite(dirPin, HIGH);
  } else {
    digitalWrite(dirPin, LOW);
  }

  for(int i = 0; i < turns; i++){

    if(direction == true && turn >= maxTurns){
      break;
    } else if(direction == false && turn <= 1){
      break;
    }


    for(int i = 0; i < 160; i++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(50);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(50);
    }

    if(direction == true) {
      turn = turn + 0.1;
      if(turn >= maxTurns){
        turn = maxTurns;
        break;
      }
    } else {
      turn = turn - 0.1;
      if(turn <= 1){
        turn = 1;
        break;
      }
    }
  }
  // lcd.setCursor(1, 1);
  // lcd.clear();
  // lcd.print(turn);

  writeIntIntoEEPROM(0, turn);
} 

void setMotorPosition(double value) {
  if(value > turn) {
    runMotor((value - turn), true);
  } else if(value < turn) {
    runMotor(abs((turn - value)), false);
  }
} 


/*

  //Tasti del telecomando arduino:

  Power: 0xFFA25D
  Vol+: 0x629D
  Vol-: 0xFFA857
  Play/Pause: 0x2FD
  Prev: 0x22DD
  Next: 0xC23D

  1: 0x30CF
  2: 0x18E7
  3: 0x7A85
  4: 0x10EF
  5: 0x38C7
  6: 0x5AA5
  7: 0x42BD
  8: 0x4AB5
  9: 0x52AD



*/



// void loop() {

//   if (irrecv.decode(&results)){
//   int value = results.value;
//     // Serial.println(value, HEX);
//   //  lcd.print(value, HEX);

//     switch(value){
//       case 0x22DD: // LEFT
//         dir = true;
//         lcd.clear();
//         lcd.print("Sinistra");
//         break;
//       case 0xC23D: // RIGHT
//         dir = false;
//         lcd.clear();
//         lcd.print("Destra");
//         break;

//       case 0x30CF: // 1
//         runMotor(1, dir);
//         break;
//       case 0x18E7: // 2
//         runMotor(2, dir);
//         break;
//       case 0x7A85: // 3
//         runMotor(3, dir);
//         break;
//       case 0x10EF: // 4
//         runMotor(4, dir);
//         break;
//       case 0x38C7: // 5
//         runMotor(5, dir);
//         break;
//       case 0x5AA5: // 6
//         runMotor(6, dir);
//         break;
//       case 0x42BD: // 7
//         runMotor(7, dir);
//         break;
//       case 0x4AB5: // 8
//         runMotor(8, dir);
//         break;
//       case 0x52AD: // 9
//         runMotor(9, dir);
//         break;
//     }
//     irrecv.resume();
//   //  delay(1000);
//   }

// }

String inc = "";
boolean ena = true;

void loop(){

  if(justStarted == true){
    digitalWrite(enablePin, HIGH);
    justStarted = false;
  }



  if(bluetooth.available() > 0){
    inc = bluetooth.readString();

    if(inc.startsWith("pos")){
      setMotorPosition(inc.substring(3).toDouble());
    }
    if(inc.startsWith("get")){
      Serial.println(turn);
    }

    if(inc.startsWith("demo")){
      int actualPos = turn;
      setMotorPosition(1);
      delay(1000);
      setMotorPosition(29);
      delay(1000);
      setMotorPosition(actualPos);
    }

    if(inc.startsWith("set")){
      turn = inc.substring(3).toInt();
      writeIntIntoEEPROM(0, turn);
    }
    boolean direction = inc.startsWith("-") ? false : true;
    boolean hasDecimal = inc.indexOf(".") != -1 ? true : false;

    if(!direction) inc.replace("-", "");

    if(!hasDecimal){
      runMotor(inc.toInt(), direction);
      bluetooth.print(String(turn));
      bluetooth.print(";");
    } else {
      double arr[2];
      arr[0] = inc.substring(0, inc.indexOf(".")).toInt();
      arr[1] = inc.substring(inc.indexOf(".") + 1).toInt();
      runMotor(arr[0], direction);
      runMotor_Decimal(arr[1], direction);
      bluetooth.print(String(arr[0]) + " " + String(arr[1]));
      bluetooth.print(";");
    }

    Serial.println(inc);


  }
}
