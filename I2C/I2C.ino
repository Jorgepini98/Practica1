#include <MPU9250_asukiaaa.h>

//defino los pines del I2C para el ESP32
#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

// se define el objeto utilizado para esta libreria
MPU9250_asukiaaa mySensor;
// Defino variables
double aX, aY, aZ, aSqrt;
uint8_t sensorId;

//Como en la primera parte, se va ha utilizar una interrupción. Dado que se quiere ver las medidas del acelerómetro cada 100ms, 
//La interrupción de define cada 100ms, y tomaremos como referencia un contador de la interruciones para las otras funciones, 
// led 200ms(2 interrupciones) y mostrar datos cada 1sec (10 interruciones)

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

  //inicializo
  
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
  
// leo sensor

  if (mySensor.accelUpdate() == 0) {
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();
    aSqrt = mySensor.accelSqrt();    
  } else {
    Serial.println("Cannod read accel values");
  }

// apaga led
  if (totalInterruptCounter - LedCounter >= 2 && LedOn == true){
    ledcWrite(PWM1_Ch, 0);
    Serial.println("Led off");
    LedOn = false;
  }

  //muestra por pantalla la info y enciende led
  if (totalInterruptCounter % 10 == 0){
    Serial.println("accelX: " + String(aX));
    Serial.println("accelY: " + String(aY));
    Serial.println("accelZ: " + String(aZ));
    //Serial.println("accelSqrt: " + String(aSqrt));
    
    ledcWrite(PWM1_Ch, 256);
    LedOn = true;
    LedCounter = totalInterruptCounter;
    Serial.println("Led on");
  }

}
  

  

}
