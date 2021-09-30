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

 int newTime = 0;
 int x = 0;
 int dutyCycle = 0;
 int sec = 0;
 int check = 0;

void setup() {
  
  Serial.begin(115200);

  ledcAttachPin(analogOutPin, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
  
  
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000000, true);
  timerAlarmEnable(timer);

 

}

void loop() {

  if(Serial.available()>0){
    mensaje = Serial.readString();
    
    Serial.print(mensaje);
    //Serial.print(mensaje.substring(0,3));

   if (mensaje.substring(0,3) == "ADC"){

        if (mensaje.length() <= 5){

        sensorValue = analogRead(analogInPin);
        Serial.print("sensor = ");
        Serial.println(sensorValue);

        }
        else{

        check = mensaje.charAt(4) - '0';

        if (check > 0 && check < 10){

        char info[mensaje.length()-6] = "";
        //Serial.print("Tamaño mensaje: ");
        //Serial.println(mensaje.length());
        //Serial.print("Tamaño info: ");
        //Serial.println(mensaje.length()-6);
        //Serial.print("Info: ");
        //Serial.println(info);

        //Serial.println("Paso 1");

        //pongo en info los caracteres del tiempo pasado por teclado

        for(int i = 4;i < mensaje.length() - 2;i++){

          //Serial.println("Paso 2");
          //Serial.println(i);

        info[i-4] = mensaje.charAt(i);

        //Serial.println(mensaje.charAt(i));

        }

        //Serial.print("info: ");
        //Serial.println(info);

        //lo transformo a int

        for (int i = 0; i <= mensaje.length() - 7;i++){

          //Serial.println("Paso 3");
        
        sec += (info[i] - '0')*pow(10,(mensaje.length() - 7) - i);

        //Serial.println(info[i] - '0');
        
        }

        Serial.print("Segundos: ");
        Serial.println(sec);

        if (sec == 0){
            timerAlarmDisable(timer);
            
          }else{
              
        newTime = sec*1000000;
        timerAlarmWrite(timer, newTime, true);
        timerAlarmEnable(timer); 

        sec = 0;
             
        }
        }
        }
   }
       
   
   else
     {
      if (mensaje.substring(0,3) == "PWM"){
        
        outputValue = (256*1000)/9;
        
        char info = mensaje.charAt(4);
        dutyCycle = info - '0';

        if(dutyCycle > 0 && dutyCycle <= 9){
       
        ledcWrite(PWM1_Ch, (outputValue*dutyCycle)/1000);
       Serial.print("dutyCycle: ");
       Serial.println(outputValue*dutyCycle/1000);
     }
     else{
      Serial.println("Introduzca un valor entre 0 y 9");
     }
    
    }

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
  Serial.println(sensorValue);
  outputValue = (outputValue*256) / 1000;
  Serial.println(sensorValue);

  ledcWrite(PWM1_Ch, outputValue);

  
  Serial.print("\t output = ");
  Serial.println(outputValue);

 
}
}
