#include <ESP32Servo.h>  //micro
#include <Wire.h>   //comunicación I2C (protocolo de comuni)
#include <Adafruit_PWMServoDriver.h>  //controla varios servos
#include <WiFi.h>
#include <HTTPClient.h> //realizar solicitudes HTTP GET
#include <ArduinoJson.h> //TRABAJA LOS DATOS EN FORMATO JSON




//se definen constantes

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 

#define SG90_SERVOMIN  200 // This is the 'minimum' pulse length (1ms) count (out of 4096)90°
#define SG90_SERVOMAX  400 // This is the 'Maximum' pulse length (2ms) count (out of 4096) -90°

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

void obtenerComando(); //se define un prototipo antes para que conocer que es lo que hay dentro de esa funcion


// Configuración de tu red Wi-Fi

const char* ssid = "laboratorio";
const char* password = "laboratorio"; 

//declaro una instacia de la clase wifi
WiFiClient wifiClient; 

//instanciar la clase http client
HTTPClient httpClient; // Instancia de HTTPClient


// URL base de la API
const String serverName = "http://192.168.1.26:3001/api/Brazo";


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
              //POS OBJ    //POS ACT                                                        //Si es menor, se moverá hacia atrás

/*
 ?es parte del operador ternario, usado para decidir entre dos valores.
stepes una variable que almacena el "paso" o dirección del movimiento.
:separa el valor asignado si la condición es falsa del valor asignado si es verdadera.
 */

//determinar la dirección de movimiento entre dos posiciones:

//Si la posición objetivo es mayor, el valor 1 indica un movimiento hacia adelante o incremento positivo.
//Si la posición objetivo es menor, el valor -1 indica un movimiento hacia atrás o decremento negativo.  
                                                                                         

 // Convierte los valores actuales y objetivos a grados para mostrar en el monitor serie
    int currentPosGrados = map(currentPos, SERVOMIN, SERVOMAX, 0, 180);
    int targetPosGrados = map(targetPos, SERVOMIN, SERVOMAX, 0, 180);



// Imprime un mensaje indicando qué servo se está moviendo y hacia dónde
    Serial.print("Moviendo el servo número: ");
    Serial.print(servoNum);
    Serial.print(" desde la posición: ");
    Serial.print(currentPosGrados); // Mostrar en grados
    Serial.print("° hasta la posición: ");
    Serial.print(targetPosGrados); // Mostrar en grados
    Serial.println("°");

 
    for (int pos = currentPos; pos != targetPos; pos += step) {
        pwm1.setPWM(servoNum, 0, pos);
        delay(stepDelay);         // Pequeño retraso para suavizar el movimiento
    }
    pwm1.setPWM(servoNum, 0, targetPos); //se asegura de que llegue al final el ángulo

    Serial.print("Servo ");
    Serial.print(servoNum);
    Serial.println(" alcanzó la posición objetivo.");
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
                    targetAngle = map(inputDegrees, 0, 180, SERVOMIN, SERVOMAX);   //El uso de map()para convertir los grados de 0 a 180 a los valores de pulsos de los servos es adecuado
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
                    lastPosServo5 = servo_pos;     //pwml para q la usa?
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
  WiFi.begin(ssid, password); //inicilaizar con la red y contraseña ya declarada en un punturo. pq puntero? pq es valor que solo se usa una unica vez y lo vas a buscar ahi. se reserva en ese espacio
  unsigned long startAttemptTime = millis (); //tiempo inicial para el intento de conexion
  const unsigned long connectionTimeout = 20000; //tiempo maximo de espera 20 seg 
  while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime) < connectionTimeout) {
    delay (1000);
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
      Serial.println("ssid no disponible");
      break;
      case WL_CONNECT_FAILED:
      Serial.println("Fallo al conectar. Red no disponible");
      break; 
      case WL_DISCONNECTED:
      Serial.println ("Desconctado");
      break;
      default: 
      Serial.println("Conectado a wifi");
      Serial.println(WiFi.localIP());
      
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED ){
    Serial.println ("Conectado a WiFi");
  } else{
    Serial.println ("error no se pudo conectar a WiFi");
  }
 
  
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
  Movement_1();  // Llama a esta función para manejar movimientos

  // Solicitud GET
  // Llama a la función obtenerComando para obtener el estado o comando actual del brazo robótico.
  obtenerComando(); 

  delay(1000);
}

void obtenerComando() {
  if (WiFi.status() == WL_CONNECTED) { // Verifica conexión Wi-Fi
    HTTPClient http;
    String endpoint = serverName + "/comando";    //serverNamees: contiene la dirección URL

    http.begin(endpoint); // Inicializa la conexión con la URL
    int httpResponseCode = http.GET(); // Realiza la solicitud GET

    if (httpResponseCode > 0) {         //SI LA SOLICITUD > 0-- se obtuvo una respuesta del servidor.
      String respuesta = http.getString(); // Obtiene la respuesta del servidor
      Serial.println("Respuesta: " + respuesta);

      // Procesar JSON de la respuesta
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, respuesta);

      if (!error) {
        // Extraer valores del JSON
        const char* articulacion = doc["articulacion"];
        int grados = doc["grados"];
        bool abrirMano = doc["abrirMano"]; // Campo para abrir/cerrar la mano

/*
 Este bloque de código recibe un comando que especifica la articulación que se debe 
 mover (por nombre) y el ángulo en el que debe moverse. Dependiendo del nombre de 
 la articulación, se mueve el servo correspondiente a un ángulo específico, y se 
 actualiza la última posición conocida del servo para realizar el movimiento de manera suave. 
 Además, en el caso de la mano, el código también maneja la apertura y cierre de la mano. 
 Finalmente, se maneja la comunicación HTTP y se muestra información sobre la ejecución del comando.
 */


        // Lógica para mover servos basada en los datos obtenidos
        if (strcmp(articulacion, "cintura") == 0) {                  //strcmp()devuelve 0 si ambas cadenas son idénticas. COMPARA Y MUEVE EL SERVO DE "CINTURA--0"
          moveServoSmoothly(0, lastPosServo0, map(grados, 0, 180, SERVOMIN, SERVOMAX));
          lastPosServo0 = map(grados, 0, 180, SERVOMIN, SERVOMAX);
        } else if (strcmp(articulacion, "hombro") == 0) {            //COMPARA Y MUEVE EL SERVO DE "HOMBRO--1"
          moveServoSmoothly(1, lastPosServo1, map(grados, 0, 180, SERVOMIN, SERVOMAX));
          lastPosServo1 = map(grados, 0, 180, SERVOMIN, SERVOMAX);
        } else if (strcmp(articulacion, "codo") == 0) {             //COMPARA Y MUEVE EL SERVO DE "CODO--2"
          moveServoSmoothly(2, lastPosServo2, map(grados, 0, 180, SERVOMIN, SERVOMAX));
          lastPosServo2 = map(grados, 0, 180, SERVOMIN, SERVOMAX);
        } else if (strcmp(articulacion, "muneca") == 0) {          //COMPARA Y MUEVE EL SERVO DE "MUNECA--3"
          moveServoSmoothly(3, lastPosServo3, map(grados, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
          lastPosServo3 = map(grados, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);
        } else if (strcmp(articulacion, "mano") == 0) {            //COMPARA Y MUEVE EL SERVO DE "MANO--4"
          moveServoSmoothly(4, lastPosServo4, map(grados, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
          lastPosServo4 = map(grados, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);
        } else if (strcmp(articulacion, "AbrirCerrarMano") == 0) {   //COMPARA SI EL SERVO ES ABRIR
          if (abrirMano) {
            moveServoSmoothly(5, lastPosServo5, SG90_SERVOMIN);
            lastPosServo5 = SG90_SERVOMIN;
            Serial.println("Mano abierta.");
          } else {
            moveServoSmoothly(5, lastPosServo5, SG90_SERVOMAX);
            lastPosServo5 = SG90_SERVOMAX;
            Serial.println("Mano cerrada.");
          }
        }
      

        Serial.println("Comando ejecutado: " + String(articulacion) + " a " + String(grados) + " grados.");
      } else {
        Serial.println("No hay valores en el get de JSON");   //"Error al parsear JSON"
      }
    } else {
      Serial.println("Error en la solicitud GET: " + String(httpResponseCode));
    }

    http.end(); // Finaliza la conexión HTTP
  } else {
    Serial.println("WiFi no conectado");
  }
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

#include <ESP32Servo.h>  //micro
#include <Wire.h>   //comunicación I2C (protocolo de comuni)
#include <Adafruit_PWMServoDriver.h>  //controla varios servos
#include <WiFi.h>
#include <HTTPClient.h> //realizar solicitudes HTTP GET
#include <ArduinoJson.h> //TRABAJA LOS DATOS EN FORMATO JSON




//se definen constantes

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 

#define SG90_SERVOMIN  200 // This is the 'minimum' pulse length (1ms) count (out of 4096)90°
#define SG90_SERVOMAX  400 // This is the 'Maximum' pulse length (2ms) count (out of 4096) -90°

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

void obtenerComando();


// Configuración de tu red Wi-Fi

const char* ssid = "laboratorio";
const char* password = "laboratorio"; 

//declaro una instacia de la clase wifi
WiFiClient wifiClient; 

//instanciar la clase http client
HTTPClient httpClient; // Instancia de HTTPClient


// URL base de la API
const String serverName = "http://172.23.5.203:3001/api/Brazo";


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
              //POS OBJ    //POS ACT                                                        //Si es menor, se moverá hacia atrás

//determinar la dirección de movimiento entre dos posiciones:

//Si la posición objetivo es mayor, el valor 1 indica un movimiento hacia adelante o incremento positivo.
//Si la posición objetivo es menor, el valor -1 indica un movimiento hacia atrás o decremento negativo.  
                                                                                         

 // Convierte los valores actuales y objetivos a grados para mostrar en el monitor serie
    int currentPosGrados = map(currentPos, SERVOMIN, SERVOMAX, 0, 180);
    int targetPosGrados = map(targetPos, SERVOMIN, SERVOMAX, 0, 180);



// Imprime un mensaje indicando qué servo se está moviendo y hacia dónde
    Serial.print("Moviendo el servo número: ");
    Serial.print(servoNum);
    Serial.print(" desde la posición: ");
    Serial.print(currentPosGrados); // Mostrar en grados
    Serial.print("° hasta la posición: ");
    Serial.print(targetPosGrados); // Mostrar en grados
    Serial.println("°");

 
    for (int pos = currentPos; pos != targetPos; pos += step) {
        pwm1.setPWM(servoNum, 0, pos);
        delay(stepDelay);         // Pequeño retraso para suavizar el movimiento
    }
    pwm1.setPWM(servoNum, 0, targetPos); //se asegura de que llegue al final el ángulo

    Serial.print("Servo ");
    Serial.print(servoNum);
    Serial.println(" alcanzó la posición objetivo.");
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
                    targetAngle = map(inputDegrees, 0, 180, SERVOMIN, SERVOMAX);   //El uso de map()para convertir los grados de 0 a 180 a los valores de pulsos de los servos es adecuado
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
                    lastPosServo5 = servo_pos;     //pwml para q la usa?
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
  WiFi.begin(ssid, password); //inicilaizar con la red y contraseña ya declarada en un punturo. pq puntero? pq es valor que solo se usa una unica vez y lo vas a buscar ahi. se reserva en ese espacio
  unsigned long startAttemptTime = millis (); //tiempo inicial para el intento de conexion
  const unsigned long connectionTimeout = 20000; //tiempo maximo de espera 20 seg 
  while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime) < connectionTimeout) {
    delay (1000);
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
      Serial.println("ssid no disponible");
      break;
      case WL_CONNECT_FAILED:
      Serial.println("Fallo al conectar. Red no disponible");
      break; 
      case WL_DISCONNECTED:
      Serial.println ("Desconctado");
      break;
      default: 
      Serial.println("Conectado a wifi");
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED ){
    Serial.println ("Conectado a WiFi");
  } else{
    Serial.println ("error no se pudo conectar a WiFi");
  }
 
  
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
  Movement_1();  // Llama a esta función para manejar movimientos

  // Solicitud GET
  // Llama a la función obtenerComando para obtener el estado o comando actual del brazo robótico.
  obtenerComando(); 

  delay(10000);
}

void obtenerComando() {
  if (WiFi.status() == WL_CONNECTED) { // Verifica conexión Wi-Fi
    HTTPClient http;
    String endpoint = serverName + "/comando";

    http.begin(endpoint); // Inicializa la conexión con la URL
    int httpResponseCode = http.GET(); // Realiza la solicitud GET

    if (httpResponseCode > 0) {
      String respuesta = http.getString(); // Obtiene la respuesta del servidor
      Serial.println("Respuesta: " + respuesta);

      // Procesar JSON de la respuesta
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, respuesta);

      if (!error) {
        // Extraer valores del JSON
        const char* articulacion = doc["articulacion"];
        int grados = doc["grados"];
        bool abrirMano = doc["abrirMano"]; // Campo para abrir/cerrar la mano

        
      
        // Lógica para mover servos basada en los datos obtenidos
        if (strcmp(articulacion, "cintura") == 0) {
          moveServoSmoothly(0, lastPosServo0, map(grados, 0, 180, SERVOMIN, SERVOMAX));
          lastPosServo0 = map(grados, 0, 180, SERVOMIN, SERVOMAX);
        } else if (strcmp(articulacion, "hombro") == 0) {
          moveServoSmoothly(1, lastPosServo1, map(grados, 0, 180, SERVOMIN, SERVOMAX));
          lastPosServo1 = map(grados, 0, 180, SERVOMIN, SERVOMAX);
        } else if (strcmp(articulacion, "codo") == 0) {
          moveServoSmoothly(2, lastPosServo2, map(grados, 0, 180, SERVOMIN, SERVOMAX));
          lastPosServo2 = map(grados, 0, 180, SERVOMIN, SERVOMAX);
        } else if (strcmp(articulacion, "muneca") == 0) {
          moveServoSmoothly(3, lastPosServo3, map(grados, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
          lastPosServo3 = map(grados, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);
        } else if (strcmp(articulacion, "mano") == 0) {
          moveServoSmoothly(4, lastPosServo4, map(grados, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX));
          lastPosServo4 = map(grados, 0, 180, SG90_SERVOMIN, SG90_SERVOMAX);
        } else if (strcmp(articulacion, "AbrirCerrarMano") == 0) {
          if (abrirMano) {
            moveServoSmoothly(5, lastPosServo5, SG90_SERVOMIN);
            lastPosServo5 = SG90_SERVOMIN;
            Serial.println("Mano abierta.");
          } else {
            moveServoSmoothly(5, lastPosServo5, SG90_SERVOMAX);
            lastPosServo5 = SG90_SERVOMAX;
            Serial.println("Mano cerrada.");
          }
        }
       

        Serial.println("Comando ejecutado: " + String(articulacion) + " a " + String(grados) + " grados.");
      } else {
        Serial.println("No hay valores en el get de JSON");   //"Error al parsear JSON"
      }
    } else {
      Serial.println("Error en la solicitud GET: " + String(httpResponseCode));
    }

    http.end(); // Finaliza la conexión HTTP
  } else {
    Serial.println("WiFi no conectado");
  }
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




 
 */













































































/*


#include <ESP32Servo.h>  //micro
#include <Wire.h>   //comunicación I2C (protocolo de comuni)
#include <Adafruit_PWMServoDriver.h>  //controla varios servos
#include <WiFi.h>
#include <HTTPClient.h> // Incluir la librería HTTPClient
#include <ArduinoJson.h>






//se definen constantes

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 

#define SG90_SERVOMIN  200 // This is the 'minimum' pulse length (1ms) count (out of 4096)90°
#define SG90_SERVOMAX  400 // This is the 'Maximum' pulse length (2ms) count (out of 4096) -90°

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates



// Configuración de tu red Wi-Fi

const char* ssid = "BARRIO_EXT";
const char* password = "42105610"; 

//declaro una instacia de la clase wifi
WiFiClient wifiClient; 

//instanciar la clase http client
HTTPClient httpClient; // Instancia de HTTPClient


// URL base de la API
const String serverName = "http://192.168.1.27:5095/api/Brazo";




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

                                                                                           //determinar la dirección de movimiento entre dos posiciones:

                                                                         //Si la posición objetivo es mayor, el valor 1 indica un movimiento hacia adelante o incremento positivo.
                                                                         //Si la posición objetivo es menor, el valor -1 indica un movimiento hacia atrás o decremento negativo.  
                                                                                         



 
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
                    lastPosServo5 = servo_pos;     //pwml para q la usa?
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




//solicitud GET
//Usa la ruta /comando de tu API para obtener el estado o comando actual del brazo robótico.
//Llama a esta función cuando necesites obtener datos de la API.

void obtenerComando() {
  if (WiFi.status() == WL_CONNECTED) { // Verifica conexión Wi-Fi
    HTTPClient http;
    String endpoint = serverName + "/comando";

    http.begin(endpoint); // Inicializa la conexión con la URL
    int httpResponseCode = http.GET(); // Realiza la solicitud GET

    if (httpResponseCode > 0) {
      String respuesta = http.getString(); // Obtiene la respuesta del servidor
      Serial.println("Respuesta: " + respuesta);
    } else {
      Serial.println("Error en la solicitud GET: " + String(httpResponseCode));
    }

    http.end(); // Finaliza la conexión HTTP
  } else {
    Serial.println("WiFi no conectado");
  }
}



//Realizar solicitudes POST
//Para enviar datos desde el ESP32 a tu API (ya sea para mover el brazo o realizar una acción), debes enviar un JSON con el formato correcto.
//a) Mover el brazo (POST a /mover)
















void setup() {
  Serial.begin(9600);  //declaracion del puerto serial 9600 de velocidad
  WiFi.begin(ssid, password); //inicilaizar con la red y contraseña ya declarada en un punturo. pq puntero? pq es valor que solo se usa una unica vez y lo vas a buscar ahi. se reserva en ese espacio
  unsigned long startAttemptTime = millis (); //tiempo inicial para el intento de conexion
  const unsigned long connectionTimeout = 20000; //tiempo maximo de espera 20 seg 
  while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime) < connectionTimeout) {
    delay (1000);
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
      Serial.println("ssid no disponible");
      break;
      case WL_CONNECT_FAILED:
      Serial.println("Fallo al conectar. Red no disponible");
      break; 
      case WL_DISCONNECTED:
      Serial.println ("Desconctado");
      break;
      default: 
      Serial.println("Conectado a wifi");
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED ){
    Serial.println ("Conectado a WiFi");
  } else{
    Serial.println ("error no se pudo conectar a WiFi");
  }
 
  
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


 */
