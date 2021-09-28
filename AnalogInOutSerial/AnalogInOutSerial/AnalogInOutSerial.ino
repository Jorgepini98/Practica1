volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}


const int analogInPin = 34;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 32; // Analog output pin that the LED is attached to
const int PWM1_Ch = 0;
const int PWM1_Res = 8;
const int PWM1_Freq = 5000;

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

String mensaje;
char caracter;

void setup() {
  
  Serial.begin(115200);

  

  ledcAttachPin(analogOutPin, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
  
  
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000000, true);
  timerAlarmEnable(timer);
}

void loop() {

  if(Serial.available()>0){
    caracter = (char)Serial.read();
    mensaje += caracter;
    
    Serial.print(mensaje);

    switch (mensaje)

      case xxx:
        sensorValue = analogRead(analogInPin);
        Serial.print("sensor = ");
        Serial.println(sensorValue);
        break;

      case xxx:
        newTime = x*1000000;
        timerAlarmWrite(timer, x, true);
        timerAlarmEnable(timer);
        break;

      case xxx:
        outputValue = 4096/9
        ledcWrite(PWM1_Ch, outputValue*dutyCycle);
    
    }
    
    mensaje = "";
  }

  if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter = 0;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;

    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);


  
  sensorValue = analogRead(analogInPin);
  Serial.print("sensor = ");
  Serial.println(sensorValue);
  
  outputValue = (sensorValue*1000) / 4096;
  outputValue = (outputValue*256) / 1000;

  ledcWrite(PWM1_Ch, outputValue);

  
  Serial.print("\t output = ");
  Serial.println(outputValue);

 
}
}
