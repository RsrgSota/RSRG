#include <Wire.h> // Библиотека, которая идет с датчиком цвета
#include <SparkFun_APDS9960.h> // Библиотека для датчика цвета APDS9960

#define PIN_PHOTO_SENSOR A0 // Пин фоторезистора
#include <Servo.h> // Библиотека для сервопривода
#include <Stepper_28BYJ.h> // Библиотека для шагового двигателя 
#define STEPS 4078 // Объект шагового двигателя

Servo sRotate; // Определение объекта сервопривода
Stepper_28BYJ stepper(STEPS, 8, 9, 10, 11); // Определение объекта шагового двигателя
int grad45 = 4075.7728395 / 8; // Один шаг шагового двигателя
int mGrad45 = -1 * grad45; // Один противополжный шаг шагового двигателя

SparkFun_APDS9960 apds = SparkFun_APDS9960(); // Определение объекта датчика цвета
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;

int startPos = 107; // Стартовая позиция
int leftPos = 180; // Поворот влево, в первый контейнер
int rightPos = 30; // Поворот вправо, во второй контейнер

int valBluetooth;
String *cubeColors = new String[2];

// Функции для цветов
int getBlue(int r, int g, int b){
    if (20 - 40 <= r && r <= 20 + 40) {        // red
      if (39 - 40 <= g && g <= 39 + 40){       // green
        if (65 - 40 <= b && b <= 65 + 40) {    // blue
          return true;
        }
      }
    }
    return false;
}

int getGreen(int r, int g, int b){
    if (64 - 40 <= r && r <= 64 + 40) {        // red
      if (105 - 40 <= g && g <= 105 + 40){     // green
        if (97 - 40 <= b && b <= 97 + 40) {    // blue
          return true;
        }
      }
    }
    return false;
}

int getBlack(int r, int g, int b){
    if (20 - 40 <= r && r <= 20 + 40) {       // red
      if (20 - 40 <= g && g <= 20 + 40){      // green
        if (25 - 40 <= b  && b <= 25 + 40) {  // blue
          return true;
        }
      }
    }
    return false;
}

int getRed(int r, int g, int b){
    if (112 - 40 <= r  && r <= 112 + 40) {      // red
      if (37 - 40 <= g  && g <= 37 + 40){       // green
        if (66 - 40 <= b && b <= 66 + 40) {     // blue
          return true;
        }
      }
    }
    return false;
}

bool stopFlag  = false;

void setup() {
  pinMode(3, OUTPUT); // Светодиод под датчика цвета
  Serial.begin(9600);
  Serial.println();
  if ( apds.init() ) { // Инициализация датчика цвета
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }
  
  delay(500);

  stepper.setSpeed(100); // Скорость шагового двигателя
  sRotate.attach(13); // PIN для сервопривода
  sRotate.write(startPos); // Передвижение на стартовую позицию
}

void loop() {
  if (Serial.available()) { // проверка поступления данных и поданных команд
    valBluetooth = Serial.read();

    Serial.print("ПРОВЕРКА ");
    Serial.println(valBluetooth);
    
    if      ( valBluetooth == 48 )  { cubeColors[0] = "red"; cubeColors[1] = "black"; stopFlag  = false;    }
    else if ( valBluetooth == 49 )  { cubeColors[0] = "red"; cubeColors[1] = "blue"; stopFlag  = false;     }
    else if ( valBluetooth == 50 )  { cubeColors[0] = "red"; cubeColors[1] = "green"; stopFlag  = false;    }
    else if ( valBluetooth == 51 )  { cubeColors[0] = "black"; cubeColors[1] = "blue"; stopFlag  = false;   }
    else if ( valBluetooth == 52 )  { cubeColors[0] = "black"; cubeColors[1] = "green"; stopFlag  = false;  }
    else if ( valBluetooth == 53 )  { cubeColors[0] = "blue"; cubeColors[1] = "green"; stopFlag  = false;   }

    else if ( valBluetooth == 33 )  {
      while (true) {
        valBluetooth = Serial.read();
        if ( valBluetooth == 34 ) break;
        sRotate.write(leftPos);
        delay(1500);
        sRotate.write(rightPos);
        delay(1500);
      }
    }

    else if ( valBluetooth == 35 )  {
      while (true) {
        valBluetooth = Serial.read();
        if ( valBluetooth == 36 ) break;
        stepper.step(grad45);
      }
    }
  }

  digitalWrite(3, HIGH); // Включение светодиода
  delay(500);

  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    Serial.println("Error reading light values");
  } else {
    Serial.print("Ambient: ");
    Serial.print(ambient_light);
    Serial.print(" Red: ");
    Serial.print(red_light);
    Serial.print(" Green: ");
    Serial.print(green_light);
    Serial.print(" Blue: ");
    Serial.println(blue_light);
  }
  int mainFlag = true;
  String nowColor = "NO_COLOR";

    while ((mainFlag) && (cubeColors[0] != "") && (!(stopFlag))) {
    int valPhoto = analogRead(PIN_PHOTO_SENSOR);
    if (valPhoto > 500) {
        valBluetooth = Serial.read();
        if (valBluetooth == 65) {
          Serial.print("БЛЮТУС");
          stopFlag = true;
          break;
        }
      stepper.step(grad45); 
    } else {
      stepper.step(0);
    
      while (nowColor == "NO_COLOR") {
        
        if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
          Serial.println("Error reading light values");
        } else {
          Serial.print("Ambient: ");
          Serial.print(ambient_light);
          Serial.print(" Red: ");
          Serial.print(red_light);
          Serial.print(" Green: ");
          Serial.print(green_light);
          Serial.print(" Blue: ");
          Serial.println(blue_light);
        }

        if (getBlue(red_light, green_light, blue_light)) { nowColor = "blue"; mainFlag = false; }
        delay(500);
        if (getGreen(red_light, green_light, blue_light)) { nowColor = "green"; mainFlag = false; }
        delay(500);
        if (getBlack(red_light, green_light, blue_light)) { nowColor = "black"; mainFlag = false; }
        delay(500);
        if (getRed(red_light, green_light, blue_light))  { nowColor = "red"; mainFlag = false; }
        delay(500);
        Serial.println(nowColor);
      }
      if (nowColor == cubeColors[0] or nowColor == cubeColors[1]) {
        sRotate.write(leftPos);
        delay(2000);
        sRotate.write(startPos);
      } else {
        sRotate.write(rightPos);
        delay(2000);
        sRotate.write(startPos);
      }

      }
  }
}
