#include <ESP32Servo.h>  //micro
#include <Wire.h>   //comunicación I2C (protocolo de comuni)
#include <Adafruit_PWMServoDriver.h>  //controla varios servos




//se definen constantes

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 

#define SG90_SERVOMIN  200 // This is the 'minimum' pulse length (1ms) count (out of 4096)90°
#define SG90_SERVOMAX  400 // This is the 'Maximum' pulse length (2ms) count (out of 4096) -90°

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates


// Se pueden crear doce objetos servo en la mayoría de las placas
// crea objetos Servo para controlar motores servo mediante la librería ESP32Servo.h

Servo myservo_1;  
Servo myservo_2;  
Servo myservo_3; 
Servo myservo_4; 


//declaración de variables
int Mvt_Mode_1 = 1;  // Modo de movimiento           
int Mvt_Mode_2 = 1;  // Modo de movimiento         
int first_pass = 1;                   
int servo_pos, servo;  
int Mvt_Mode_1_Label=1;
int Mvt_Mode_2_Label=1;
float time_step, servo_1_pos, servo_2_pos, servo_3_pos, servo_4_pos;

//time_step--> intervalo de tiempo entre pasos de movimiento de los servos.
//servo_post--> Posiciones actuales de los servos 1 a 4


String inputString = "";  // una cadena para almacenar datos entrantes    
String inputAngle = "";    // una cadena para almacenar grados entrantes   
bool stringComplete = false;   // si la cadena está completa


Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);

// VARIABLES GLOBALES PARA LAS ÚLTIMAS POSICIONES DE LOS SERVOS
int lastPosServo0 = 0;  // Última posición conocida del servo 0
int lastPosServo1 = 0;  
int lastPosServo2 = 0;  
int lastPosServo3 = 0;  
int lastPosServo4 = 0;  
int lastPosServo5 = 0;  



//fc que permite mover los servos
//esta fc, se encarga de mover un servo de manera suave desde una p.i hasta una p.f 

void moveServoSmoothly(int servoNum, int currentPos, int targetPos, int stepDelay = 10) {   //la funcion stepdelay en 10 ms hace que se mueva lento
    int step = (targetPos > currentPos) ? 1 : -1;    // Decide si incrementar o decrementar //Si el a es mayor que la posición actual, 
                                                                                            // el servo se moverá hacia adelante.
                                                                                            //Si es menor, se moverá hacia atrás
    for (int pos = currentPos; pos != targetPos; pos += step) {
        pwm1.setPWM(servoNum, 0, pos);
        delay(stepDelay);         // Pequeño retraso para suavizar el movimiento
    }
    pwm1.setPWM(servoNum, 0, targetPos); //se asegura de que llegue al final el ángulo
}

void Movement_1() {
    if (stringComplete) {    // Verifica si hay un comando disponible
        int targetAngle;
        int inputDegrees = inputString.substring(1).toInt();   // Convertir el ángulo ingresado a entero

        if (inputDegrees < 0 || inputDegrees > 180) {     // Si el ángulo está fuera de rango, muestra un mensaje de error y termina la función
            Serial.println("Ángulo fuera de rango");
        } else {
            switch (inputString.charAt(0)) {
                case '0':
                    Serial.println("Servo Elegido: 0");
                    inputAngle = inputString.substring(1);
                    targetAngle = map(inputDegrees, 0, 180, SERVOMIN, SERVOMAX);
                    moveServoSmoothly(0, lastPosServo0, targetAngle);   // Usa lastPosServo para actualizar la posición
                    lastPosServo0 = targetAngle;   // Actualiza la última posición. se agrego porque sino queda esperando....
                    break;
                case '1':
                    Serial.println("Servo Elegido: 1");
                    inputAngle = inputString.substring(1);
                    targetAngle = map(inputDegrees, 0, 180, SERVOMIN, SERVOMAX);
                    moveServoSmoothly(1, lastPosServo1, targetAngle);
                    lastPosServo1 = targetAngle;
                    break;
                case '2':
                    Serial.println("Servo Elegido: 2");
                    inputAngle = inputString.substring(1);
                    targetAngle = map(inputDegrees, 0, 180, SERVOMIN, SERVOMAX);
                    moveServoSmoothly(2, lastPosServo2, targetAngle);
                    lastPosServo2 = targetAngle;
                    break;
                case '3':
                    Serial.println("Servo Elegido: 3");
                    inputAngle = inputString.substring(1);
                    targetAngle = map(inputDegrees, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);
                    moveServoSmoothly(3, lastPosServo3, targetAngle);
                    lastPosServo3 = targetAngle;
                    break;
                case '4':
                    Serial.println("Servo Elegido: 4");
                    inputAngle = inputString.substring(1);
                    targetAngle = map(inputDegrees, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);
                    moveServoSmoothly(4, lastPosServo4, targetAngle);
                    lastPosServo4 = targetAngle;
                    break;
                case '5':
                    Serial.println("Servo Elegido: 5 ");
                    inputAngle = inputString.substring(1);
                    servo_pos = map(inputDegrees, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);   // Asignación de grados a la longitud del pulso
                    pwm1.setPWM(5, 0, servo_pos);        // Actualiza la última posición del servo 5
                    lastPosServo5 = servo_pos;  
                    break;
                default:
                    break; //termine todos los casos
            } 
        } 

        // Limpia el string para el siguiente comando

        inputString = "";
        stringComplete = false;
    }
}

// aca se resetean los servos en su posicion inicial
void reset_all_servos() {
    Serial.println("Reseting Servos..");
    pwm1.setPWM(0, 0, map(80, 0, 180, SERVOMIN, SERVOMAX));
    lastPosServo0 = map(80, 0, 180, SERVOMIN, SERVOMAX); // Actualiza la última posición del servo 0
    delay(500);
    
    pwm1.setPWM(1, 0, map(95, 0, 180, SERVOMIN, SERVOMAX));
    lastPosServo1 = map(95, 0, 180, SERVOMIN, SERVOMAX);  // Actualiza la última posición del servo 1
    delay(500);
    
    pwm1.setPWM(2, 0, map(50, 0, 180, SERVOMIN, SERVOMAX));
    lastPosServo2 = map(50, 0, 180, SERVOMIN, SERVOMAX);  // Actualiza la última posición del servo 2
    delay(500);
    
    pwm1.setPWM(3, 0, map(50, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
    lastPosServo3 = map(50, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);   // Actualiza la última posición del servo 3
    delay(500);
    
    pwm1.setPWM(4, 0, map(60, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
    lastPosServo4 = map(60, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);   // Actualiza la última posición del servo 4
    delay(500);
    
    pwm1.setPWM(5, 0, map(100, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
    lastPosServo5 = map(100, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);   // Actualiza la última posición del servo 5
    delay(500);
    
    pwm1.setPWM(6, 0, map(20, 0, 180, SERVOMIN, SERVOMAX));  //no se usa
    delay(500);
    
    pwm1.setPWM(7, 0, map(30, 0, 180, SERVOMIN, SERVOMAX));   //no se usa
    delay(500);
}

void setup() {
  Serial.begin(9600);  //declaracion del puerto serial 9600 de velocidad
  inputString.reserve(10);   // se reserva 10 lugares
  inputAngle.reserve(10);   
  pwm1.begin();
  pwm1.setPWMFreq(SERVO_FREQ);  //Los servos analógicos funcionan a actualizaciones de aproximadamente 50 Hz

  delay(50);
  reset_all_servos();
  Serial.println("Entrada manual para");
  Serial.println("Prueba y calibración de la placa PWM de 16 canales!");
}

void loop() {   
     Movement_1();  //llama a esta fc
}


//leer datos que llegan al microcontrolador a través del puerto serie y almacenarlos en una cadena (inputString)
void serialEvent() {
    while (Serial.available()) {
        char inChar = (char) Serial.read();  //leer un carácter de los datos entrantes.
        inputString += inChar;

  // Si el carácter entrante es un salto de línea, significa que el mensaje ha terminado, se completo la cadena
  
        if (inChar == '\n') {
            stringComplete = true;
        }
    }
}








/*
#include <ESP32Servo.h>
//#include <SoftwareSerial.h>  // include the SoftwareSerial library so you can use its functions
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Dependiendo de la marca de su servo, el ancho de pulso mínimo y máximo puede variar. 
//Usted querrá que sean lo más pequeños o grandes posibles sin llegar al tope para el rango máximo. 
//¡Tendrá que ajustarlos según sea necesario para que coincidan con los servos que tiene!

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)

//SG90 Servos
#define SG90_SERVOMIN  200 // This is the 'minimum' pulse length (1ms) count (out of 4096)90°
#define SG90_SERVOMAX  400 // This is the 'Maximum' pulse length (2ms) count (out of 4096) -90°


#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// Se pueden crear doce objetos servo en la mayoría de las placas
Servo myservo_1;  // create servo object to control a servo
Servo myservo_2;  // create servo object to control a servo
Servo myservo_3;  // create servo object to control a servo
Servo myservo_4;  // create servo object to control a servo

int Mvt_Mode_1 = 1;           // Modo de movimiento 
int Mvt_Mode_2 = 1;           // Modo de movimiento 
int first_pass = 1;                  // variable para almacenar la posición del servo
int servo_pos, servo;
int Mvt_Mode_1_Label=1;
int Mvt_Mode_2_Label=1;
float time_step, servo_1_pos, servo_2_pos, servo_3_pos, servo_4_pos;

String inputString = "";      // una cadena para almacenar datos entrantes
String inputAngle = "";      // una cadena para almacenar datos entrantes

bool stringComplete = false;  // si la cadena está completa

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);

// VARIABLES GLOBALES PARA LAS ÚLTIMAS POSICIONES DE LOS SERVOS
int lastPosServo0 = 0;  // Última posición conocida del servo 0
int lastPosServo1 = 0;  // Última posición conocida del servo 1
int lastPosServo2 = 0;  // Última posición conocida del servo 2
int lastPosServo3 = 0;  // Última posición conocida del servo 3
int lastPosServo4 = 0;  // Última posición conocida del servo 4
int lastPosServo5 = 0;  // Última posición conocida del servo 5

//Para suavizar los movimientos de los servos y evitar que sean bruscos, puede implementar una 
//transición gradual entre el ángulo actual y el ángulo deseado. Esto se puede hacer 
//dividiendo el movimiento en pasos más pequeños y aplicando un pequeño retraso entre cada paso. 
//De esta forma, el servo se mueve en incrementos menores, lo cual resulta en un movimiento más fluido.

void moveServoSmoothly(int servoNum, int currentPos, int targetPos, int stepDelay = 10) {   //la funcion stepdelay en 10 ms hace que se mueva lento
    int step = (targetPos > currentPos) ? 1 : -1; // Decide si incrementar o decrementar
    for (int pos = currentPos; pos != targetPos; pos += step) {
        pwm1.setPWM(servoNum, 0, pos);
        delay(stepDelay); // Pequeño retraso para suavizar el movimiento
    }
    pwm1.setPWM(servoNum, 0, targetPos); // Asegúrate de que llegue al ángulo final
}

void Movement_1() {
    if (stringComplete) // Verifica si hay un comando disponible
    {
        int targetAngle;
        int inputDegrees = inputString.substring(1).toInt(); // Convertir el ángulo ingresado a entero

        if (inputDegrees < 0 || inputDegrees > 180) {
            // Si el ángulo está fuera de rango, muestra un mensaje de error y termina la función
            Serial.println("Ángulo fuera de rango");
        } else {
            switch (inputString.charAt(0)) // Verifica el servo seleccionado
            {
                case '0':
                    Serial.println("Servo Elegido: 0");
                    Serial.print("--");
                    inputAngle = inputString.substring(1);
                    Serial.print("Angle Chosen: ");
                    Serial.println(inputAngle);
                    targetAngle = map(inputDegrees, 0, 180, SERVOMIN, SERVOMAX);
                    moveServoSmoothly(0, lastPosServo0, targetAngle);  // Usa lastPosServo0 para actualizar la posición
                    lastPosServo0 = targetAngle;  // Actualiza la última posición. se agrego porque sino queda esperando....
                    break;
                case '1':
                    Serial.println("Servo Elegido: 1");
                    Serial.print("--");
                    inputAngle = inputString.substring(1);
                    Serial.print("Angle Chosen: ");
                    Serial.println(inputAngle); 
                    targetAngle = map(inputDegrees, 0, 180, SERVOMIN, SERVOMAX);
                    moveServoSmoothly(1, lastPosServo1, targetAngle);
                    lastPosServo1 = targetAngle;
                    break;
                case '2':
                    Serial.println("Servo Elegido: 2");
                    Serial.print("--");
                    inputAngle = inputString.substring(1);
                    Serial.print("Angle Chosen: ");
                    Serial.println(inputAngle);
                    targetAngle = map(inputDegrees, 0, 180, SERVOMIN, SERVOMAX);
                    moveServoSmoothly(2, lastPosServo2, targetAngle);
                    lastPosServo2 = targetAngle;
                    break;
                case '3':
                    Serial.println("Servo Elegido: 3");
                    Serial.print("--");
                    inputAngle = inputString.substring(1);
                    Serial.print("Angle Chosen: ");
                    Serial.println(inputAngle);
                    targetAngle = map(inputDegrees, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);
                    moveServoSmoothly(3, lastPosServo3, targetAngle);
                    lastPosServo3 = targetAngle;
                    break;
                case '4':
                    Serial.println("Servo Elegido: 4");
                    Serial.print("--");
                    inputAngle = inputString.substring(1);
                    Serial.print("Angle Chosen: ");
                    Serial.println(inputAngle);
                    targetAngle = map(inputDegrees, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);
                    moveServoSmoothly(4, lastPosServo4, targetAngle);
                    lastPosServo4 = targetAngle;
                    break;
                case '5':
                    Serial.println("Servo Elegido: 5 ");
                    Serial.print("--");
                    inputAngle = inputString.substring(1);
                    Serial.print("Angle Chosen: ");
                    Serial.println(inputAngle);  
                    servo_pos = map(inputDegrees, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX); // Asignación de grados a la longitud del pulso
                    pwm1.setPWM(5, 0, servo_pos);
                    lastPosServo5 = servo_pos;  // Actualiza la última posición del servo 5
                    Serial.print("Servo_POS: ");
                    Serial.print(servo_pos);
                    Serial.print("PWM: ");
                    Serial.println(pwm1.getPWM(5, true));
                    delay(100);
                    break;
                default:
                    break;
            } // <- Cierra el switch 
        }

        // Limpia el string para el siguiente comando
        inputString = "";
        stringComplete = false;
    }
}

// aca se resetean los servos en su posicion inicial

void reset_all_servos()
{
    Serial.println("Reseting Servos..");
    pwm1.setPWM(0, 0, map(80, 0, 180, SERVOMIN, SERVOMAX));
    lastPosServo0 = map(80, 0, 180, SERVOMIN, SERVOMAX);  // Actualiza la última posición del servo 0
    delay(500);
    
    pwm1.setPWM(1, 0, map(95, 0, 180, SERVOMIN, SERVOMAX));
    lastPosServo1 = map(95, 0, 180, SERVOMIN, SERVOMAX);  // Actualiza la última posición del servo 1
    delay(500);
    
    pwm1.setPWM(2, 0, map(50, 0, 180, SERVOMIN, SERVOMAX));
    lastPosServo2 = map(50, 0, 180, SERVOMIN, SERVOMAX);  // Actualiza la última posición del servo 2
    delay(500);
    
    pwm1.setPWM(3, 0, map(50, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
    lastPosServo3 = map(50, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);  // Actualiza la última posición del servo 3
    delay(500);
    
    pwm1.setPWM(4, 0, map(60, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
    lastPosServo4 = map(60, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);  // Actualiza la última posición del servo 4
    delay(500);
    
    pwm1.setPWM(5, 0, map(100, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
    lastPosServo5 = map(100, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);  // Actualiza la última posición del servo 5
    delay(500);
    
    pwm1.setPWM(6, 0, map(20, 0, 180, SERVOMIN, SERVOMAX));  //no se usa
    delay(500);
    
    pwm1.setPWM(7, 0, map(30, 0, 180, SERVOMIN, SERVOMAX));  //no se usa
    delay(500);
}
void setup() {
  Serial.begin(9600); //Depuración de errores


  //Serial1.begin(9600);


  
  inputString.reserve(10);
  inputAngle.reserve(10);   
  pwm1.begin();
  pwm1.setPWMFreq(SERVO_FREQ);  //Los servos analógicos funcionan a actualizaciones de aproximadamente 50 Hz

  delay(50);
  reset_all_servos();
  Serial.println("Entrada manual para");
  Serial.println("Prueba y calibración de la placa PWM de 16 canales!");

}


void loop() 
{   
     Movement_1();  
}

void serialEvent()
{
    while (Serial.available())
    {
      //obtener el nuevo byte:

      char inChar = (char) Serial.read();
      //agrégalo a la inputString:
      inputString += inChar;
      // Si el carácter entrante es un salto de línea, establece una bandera para que el bucle principal pueda
      // hacer algo al respecto:

      if (inChar == '\n')
      {
        stringComplete = true;
      }
    }
}

*/
