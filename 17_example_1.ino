#include <Servo.h>

// Arduino pin assignment

#define PIN_POTENTIOMETER 3 // Potentiometer at Pin A3
// Add IR Sensor Definition Here !!!
#define PIN_IRSensor 0
#define PIN_SERVO 10
#define PIN_LED 9

#define _DUTY_MIN 553  // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1476 // servo neutral position (90 degree)
#define _DUTY_MAX 2399 // servo full counter-clockwise position (180 degree)

#define LOOP_INTERVAL 50   // Loop Interval (unit: msec)

//global variables
float dist_prev = 150;
float dist_ema;
float DIST_MIN = 150;
float DIST_MAX = 150;

Servo myservo;
unsigned long last_loop_time;   // unit: msec

void setup()
{
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);
  pinMode(PIN_LED,OUTPUT);
  
  Serial.begin(1000000);
}

void loop()
{
  unsigned long time_curr = millis();
  int a_value, IR_value, duty, dist;

  // wait until next event time
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  // Remove Next line !!!
  a_value = analogRead(PIN_POTENTIOMETER);
  // Read IR Sensor value !!!
  IR_value = analogRead(PIN_IRSensor);
  // Convert IR sensor value into distance !!!
  dist = (6762.0/(IR_value-9)-4.0)*10.0;
  // we need distance range filter here !!!
  if ((dist < 100) || (dist > 250)) {
    dist = dist_prev; 
    digitalWrite(PIN_LED, 1);       // LED OFF
  } else {  
    digitalWrite(PIN_LED, 0);       // LED ON      
  }
  if (dist < DIST_MIN){
    DIST_MIN = dist;
  } else if (dist > DIST_MAX) {
    DIST_MAX = dist;
  }
  dist_prev = dist;
  // we need EMA filter here !!!
  dist_ema = 0.3*(dist)+ (0.7)*(dist_ema);
  // map distance into duty
  duty = ((_DUTY_MAX-_DUTY_MIN)/150)*(dist_ema-100)+_DUTY_MIN;
  myservo.writeMicroseconds(duty);

  // print IR sensor value, distnace, duty !!!
  Serial.print("MIN:"); Serial.print(DIST_MIN);
  Serial.print(",IR:"); Serial.print(a_value);
  Serial.print(",dist:"); Serial.print(dist);
  Serial.print(",ema:"); Serial.print(dist_ema);
  Serial.print(",servo:"); Serial.print(duty);
  Serial.print(",MAX:"); Serial.print(DIST_MAX);
  Serial.println("");
}
