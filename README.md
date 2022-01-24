# Practica1
Esta práctica se basa en familiarizarse con el entorno de Arduino para el desarrollo del microcontrolador ESP32 y manejar periféricos típicos de cualquier microcontrolador tales como ADC, PWM, Timer…
1.	Se ha generado una interrupción timer cada 10 seg que activa en el loop principal la lectura del ADC y es mostrado por pantalla.
En función de la tensión del ADC se ha generado una señal PWM a 5 kHz.
Mediante comanda UART se ha definido un protocolo para activar diversas funcionalidades.

2.	Comunicación I2C con un sensor inercial, que mediante una interrupción timer de 100ms muestrea la aceleración y, envía los datos vía UART cada 1 seg contando el numero de interrupciones de 100ms (InterruptCounter % 10 == 0).
