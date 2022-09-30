// Arduino pin assignment
#define PIN_LED 9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

float tmp; // 이전 측정값을 저장해놓기 위한 변수 선언. 이값은 루프가 돌때 초기화 되면 안되므로 바깥에서 전역변수로 미리 선언.  

void setup() {
  
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float distance;
  
  // wait until next sampling time. 
  // millis() returns the number of milliseconds since the program started.
  //    Will overflow after 50 days.
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance
  
  // 값이 갑자기 튈 경우 직전에 측정한 값을 거리로 사용
  if ((distance > _DIST_MAX) || (distance < _DIST_MIN)) {
    distance = tmp;
  }
  
  if (distance < _DIST_MIN) {
    distance = _DIST_MIN;    // Set Lower Value
    analogWrite(PIN_LED, 255);       // LED OFF
  } else if (distance > _DIST_MAX) {
    distance = _DIST_MAX;    // Set Higher Value
    analogWrite(PIN_LED, 255);       // LED OFF
  } else if (distance > 200) {
    analogWrite(PIN_LED, 255*(distance-200)/100); //거리가 200일때 밝기가 최대 
  } else {
    analogWrite(PIN_LED, 255*(distance-200)/100*(-1)); //절대값 사용법을 몰라 거리가 200보다 작을때는 -1을 해주는걸로 코드를짬
  }
  tmp = distance;
  
  // output the distance to the serial port
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.println("");
  
  
  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - round trip distance: 34.6m
  // - expected pulse duration: 0.1 sec, or 100,000us
  // - pulseIn(ECHO, HIGH, timeout) * 0.001 * 0.5 * SND_VEL
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346 * micro * sec * milli * meter
  //                                        ----------------------------
  //                                         micro * sec
  //        = 100 * 173 milli*meter = 17,300 mm = 17.3m
  // pulseIn() returns microseconds.
}
