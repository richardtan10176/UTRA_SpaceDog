#include <LiquidCrystal.h>

const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define LCD_CONTRAST_PIN 6
#define LCD_BACKLIGHT_PIN 10


//ultrasonic sensor
#define trigPin 1 //trigger
#define echoPin 0 //echo


//motors
int motor1pin1 = 2;
int motor1pin2 = 3;

int motor2pin1 = 4;
int motor2pin2 = 5;


//photoresistors 
#define pRes1 A0 //left
#define pRes2 A2 //right

//constants
#define turnDelay 30
#define updateDelay 70
#define lightThreshold 50

bool turnLeft = 0;
bool turnRight = 0;


bool targetAquireMode = 1;

void setup() {
  Serial.begin(9600);
  /*
  analogWrite(LCD_BACKLIGHT_PIN, 255);
  analogWrite(LCD_CONTRAST_PIN, 30);
  */

  lcd.begin(16, 2);

  //ULTRASONIC
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //PHOTORESISTORS
  pinMode(pRes1, INPUT);
  pinMode(pRes2, INPUT);



  //MOTORS
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);

  pinMode(6, OUTPUT); 
  pinMode(7, OUTPUT);
}

void loop() {

    long amplitudeLeft;
    long amplitudeRight;

    amplitudeLeft = analogRead(pRes1);
    amplitudeRight = analogRead(pRes2);
    
    
    Serial.print(amplitudeLeft);
    Serial.println(" amplitudeLeft");
    Serial.print(amplitudeRight);
    Serial.println(" amplitudeRight");
    

  if (turnLeft) {
    Serial.println("left");
    analogWrite(6, 0);
    analogWrite(7, 255);
    
  } 
  else if (turnRight) {
    Serial.println("right");
    analogWrite(6, 255);
    analogWrite(7, 0);
  } 
  else {
    Serial.println("fwd");
    analogWrite(6, 255);
    analogWrite(7, 150);
  }
  if (targetAquireMode) {
    turnLeft = 1;
    lcd.setCursor(0, 0);
    lcd.print("Acquiring Target");

    if(amplitudeLeft > lightThreshold && amplitudeRight > lightThreshold){
      lcd.clear();
      lcd.print("Moving To Target");
      lcd.setCursor(0, 1);
      targetAquireMode = 0;
      turnLeft = 0;
    }
    
    delay(updateDelay);
  }
  // begin moving towards target
  else { 
    long duration, distance;
    /*
    lcd.setCursor(0, 1);
    int distancePrint = (int)distance;
    lcd.print(distancePrint);
    */
    ping(); //ping the ultrasonic sensor

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    Serial.print(distance);
    Serial.println(" cm");

    if (distance <= 6 && distance > 2) {
      //stop condition
      Serial.println("Obstacle detected");
      turnLeft = 1;
      turnRight = 0;
    } 
    
    else {
      turnLeft = 0;
      turnRight = 0;
      
      if(abs(amplitudeLeft - amplitudeRight) < 10){
        turnLeft = 0;
        turnRight = 0;
      }
      //tr
      else if(amplitudeLeft > amplitudeRight){
        turnLeft = 1;
        delay(turnDelay);
        turnRight = 0;
      }
      else if(amplitudeRight > amplitudeLeft){
        turnRight = 1;
        delay(turnDelay);
        turnLeft = 0;
      }
    }
    
    delay(updateDelay);
  }
}

void ping(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}
