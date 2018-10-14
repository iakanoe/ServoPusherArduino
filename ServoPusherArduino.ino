#include <Servo.h>
#define PIN_SERVO 3
#define PIN_POTA A0
#define PIN_POTB A1
#define PIN_POT_TIME A2
#define PIN_BTN 2

#define MIN_US 500
#define MAX_US 2400
#define MAX_TIME 6000

enum {RUN, BUTTON, PROGRAM_A, PROGRAM_B, PROGRAM_T};

Servo servo;
unsigned int posA;
unsigned int posB;
unsigned long changeTime;

void setup() {
  Serial.begin(9600);
  Serial.println("Encendiendo...");
  
  pinMode(PIN_POTA, INPUT);
  posA = map2(analogRead(PIN_POTA), 0, 1023, 0, 180);
  Serial.print("Posicion A obtenida: "); Serial.print(posA); Serial.println("°.");
  
  pinMode(PIN_POTB, INPUT);
  posB = map2(analogRead(PIN_POTB), 0, 1023, 0, 180);
  Serial.print("Posicion B obtenida: "); Serial.print(posB); Serial.println("°.");
  
  pinMode(PIN_POT_TIME, INPUT);
  changeTime = map2(analogRead(PIN_POT_TIME), 0, 1023, 0, MAX_TIME);
  Serial.print("Tiempo de cambio obtenido: "); Serial.print(((double)changeTime) / 1000, 2); Serial.println(" segundos.");

  servo.attach(PIN_SERVO, MIN_US, MAX_US);
  servo.write(posA);
  Serial.println("Servo configurado.");

  pinMode(PIN_BTN, INPUT_PULLUP);
  Serial.println("Inicializacion Completa.");
}

void loop() {
  static unsigned int estado = RUN;
  static unsigned long next = 0;
  static boolean servoStatus = 0;
  static boolean newStatus = 1;
  static unsigned int reading;
  switch(estado){
    case RUN:
      if(!digitalRead(PIN_BTN)){
        estado = BUTTON;
        newStatus = 1;
        break;
      }
      
      if(millis() >= next){
        next = millis() + changeTime;
        servo.write(servoStatus ? posA : posB);
        servoStatus = !servoStatus;
      }
      
      if(newStatus){
        newStatus = 0;
        Serial.println("Corriendo. Presione el boton para entrar en modo de programacion.");
      }
      break;

    case BUTTON:
      if(digitalRead(PIN_BTN)){
        estado = PROGRAM_A;
        newStatus = 1;
        break;
      }
      
      if(millis() >= next){
        next = millis() + changeTime;
        servo.write(servoStatus ? posA : posB);
        servoStatus = !servoStatus;
      }

      if(newStatus){
        newStatus = 0;
        Serial.println("Entrando en modo de programacion...");
      }
      break;

    case PROGRAM_A:
      if(newStatus){
        newStatus = 0;
        Serial.println("Modo programacion activado.");
        Serial.print("Programando posicion A. Posicion actual: ");
        Serial.print(posA);
        Serial.println("°.");
      }
      
      reading = map(analogRead(PIN_POTA), 0, 1023, 0, 180);
      servo.write(reading);

      if(!digitalRead(PIN_BTN)){
        posA = reading;
        Serial.print("Posicion seleccionada: ");
        Serial.print(posA);
        Serial.print("°. Guardando...");
        while(!digitalRead(PIN_BTN));
        Serial.println("Guardado");
        estado = PROGRAM_B;
        newStatus = 1;
      }
      break;

    case PROGRAM_B:
      if(newStatus){
        newStatus = 0;
        Serial.print("Programando posicion B. Posicion actual: ");
        Serial.print(posB);
        Serial.println("°.");
      }
      
      reading = map(analogRead(PIN_POTB), 0, 1023, 0, 180);
      servo.write(reading);

      if(!digitalRead(PIN_BTN)){
        posB = reading;
        Serial.print("Posicion seleccionada: ");
        Serial.print(posB);
        Serial.print("°. Guardando...");
        while(!digitalRead(PIN_BTN));
        Serial.println("Guardado");
        estado = PROGRAM_T;
        newStatus = 1;
      }
      break;

    case PROGRAM_T:
      if(newStatus){
        newStatus = 0;
        Serial.print("Programando tiempo de cambio. Tiempo actual: ");
        Serial.print((double)changeTime / 6000, 2);
        Serial.println(" segundos.");
      }
      
      if(!digitalRead(PIN_BTN)){
        changeTime = map(analogRead(PIN_POT_TIME), 0, 1023, 0, MAX_TIME);
        Serial.print("Tiempo seleccionado: ");
        Serial.print((double)changeTime / 6000, 2);
        Serial.print(" segundos. Guardando...");
        while(!digitalRead(PIN_BTN));
        Serial.println("Guardado");
        estado = RUN;
        newStatus = 1;
      }
      break;
  }
}

inline int map2(int value, int minIn, int maxIn, int minOut, int maxOut){
    return (((((value - minIn) * (maxOut - minOut) * 2) / (maxIn - minIn) + 1) / 2) + minOut);
}
