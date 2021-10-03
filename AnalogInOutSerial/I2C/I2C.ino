#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

MPU9250_asukiaaa mySensor;
double aX, aY, aZ, aSqrt;
uint8_t sensorId;

volatile int interruptCounter;
int totalInterruptCounter;

  hw_timer_t * timer = NULL;
  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

  void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

const int analogOutPin = 32; // Analog output pin that the LED is attached to
const int PWM1_Ch = 0;
const int PWM1_Res = 8;
const int PWM1_Freq = 5000;

long LedCounter = 0;
boolean LedOn = false;

void setup() {
  
  Serial.begin(115200);
  while(!Serial);
  Serial.println("started");

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  mySensor.setWire(&Wire);
#endif

  mySensor.beginAccel();

  ledcAttachPin(analogOutPin, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
  
  
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true);
  timerAlarmEnable(timer);
  
}
void loop() {
  
if (interruptCounter > 0) {

    portENTER_CRITICAL(&timerMux);
    interruptCounter = 0;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;
  
//  if (mySensor.readId(&sensorId) == 0) {
//    Serial.println("sensorId: " + String(sensorId));
//  } else {
//    Serial.println("Cannot read sensorId");
//  }

  if (mySensor.accelUpdate() == 0) {
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();
    aSqrt = mySensor.accelSqrt();    
  } else {
    Serial.println("Cannod read accel values");
  }

  if (totalInterruptCounter - LedCounter >= 2 && LedOn == true){
    ledcWrite(PWM1_Ch, 0);
    Serial.println("Led off");
    LedOn = false;
  }
  
  if (totalInterruptCounter % 10 == 0){
    Serial.println("accelX: " + String(aX));
    Serial.println("accelY: " + String(aY));
    Serial.println("accelZ: " + String(aZ));
    Serial.println("accelSqrt: " + String(aSqrt));
    
    ledcWrite(PWM1_Ch, 256);
    LedOn = true;
    LedCounter = totalInterruptCounter;
    Serial.println("Led on");
  }

}
  

  

}
