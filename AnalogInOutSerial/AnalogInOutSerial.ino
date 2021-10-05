//La variable encargada de indicar si se ha accedido a la interrupción se define como “volatile”
//con objeto de evitar que sea eliminada por optimizaciones del compilador.
volatile int interruptCounter;
//Cuenta el numero total de interrupciones
int totalInterruptCounter;

//para configurar el timer, se necesita un pointer a una variable de tipo hw_timer_t
hw_timer_t * timer = NULL;

//se declara una variable tipo portMUX_Type que se encargara de la sincronización entre el main loop y la interrupción
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


//Se define la interrupción. 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

// Se definen los pines de entrada (A/D) y salida (PWM)
const int analogInPin = 34;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 32; // Analog output pin that the LED is attached to

// COnfiguraciones del PWM
const int PWM1_Ch = 0;
const int PWM1_Res = 8;
const int PWM1_Freq = 5000;

//Variables utilizadas 
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

// Setup del PWM
  ledcAttachPin(analogOutPin, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
  
 // Setup de la interrupción/timer. Teniendo en cuenta que la frecuancia base utilizada por el controlador esp32 es de 80Mhz,
 //  utilizando un prescaler de 80, conseguiremos una señal de 1 000 000 veces por segundo.
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000000, true);
  timerAlarmEnable(timer);

 

}

void loop() {

// Chequeo el serial
  if(Serial.available()>0){
    // guardo en la variable la información recibida
    mensaje = Serial.readString();    
    Serial.print(mensaje);

// Diferencia en función de la información recibida entre ADC ADC(x) y PWM(x)
   if (mensaje.substring(0,3) == "ADC"){

        if (mensaje.length() <= 5){

// Leo valor del A/D (12bits)
        sensorValue = analogRead(analogInPin);
        Serial.print("sensor = ");
        Serial.println(sensorValue);

        }
        else{

//Se resta '0' al tratarse de ASCI
        check = mensaje.charAt(4) - '0';

// Verifica que el primer haya un numero en la posicion x
        if (check > 0 && check < 10){

// Se restan 6 para calcular el tamaño de x
        char info[mensaje.length()-6] = "";

//pongo en info los caracteres del tiempo pasado por teclado

        for(int i = 4;i < mensaje.length() - 2;i++){

        info[i-4] = mensaje.charAt(i);

        }

        //lo transformo a int

        for (int i = 0; i <= mensaje.length() - 7;i++){
        
        sec += (info[i] - '0')*pow(10,(mensaje.length() - 7) - i);
        
        }

        Serial.print("Segundos: ");
        Serial.println(sec);

        if (sec == 0){
          //desabilito el timer si sec == 0
            timerAlarmDisable(timer);
            
          }else{
        //Multiplico los sec por los 1000000 times/s y fijar el timpo que transcurre por cada iteración
        newTime = sec*1000000;
        timerAlarmWrite(timer, newTime, true);
        timerAlarmEnable(timer); 

        sec = 0;
             
        }
        }
        }
   }
       
   // salida PWM (8 bit) regulado por lo pasado por teclado, entre 1 y 9
   //se multiplica y divide por 1000 para que los decimales no se borren
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
  

// se cumple cuando se entra en la interrupción previamente
  if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter = 0;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;

    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);


  // mide la tensión analógica
  sensorValue = analogRead(analogInPin);
  Serial.print("sensor = ");
  Serial.println(sensorValue);

  //se multiplica y divide por 1000 para que los decimales no se borren
  outputValue = (sensorValue*1000) / 4096;
  Serial.println(sensorValue);
  outputValue = (outputValue*256) / 1000;
  Serial.println(sensorValue);

//defino el nuevo PWM
  ledcWrite(PWM1_Ch, outputValue);

  
  Serial.print("\t output = ");
  Serial.println(outputValue);

 
}
}
