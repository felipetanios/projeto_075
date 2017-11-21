// Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>
 
// Define os pinos para o trigger e echo
#define pino_trigger 4
#define pino_echo 5

int control_led = 11;

// Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

int i;
long microsec;
int dist;
int movimento = 0;
boolean estado = LOW;

void setup()
{
  Serial.begin(9600);
  Serial.println("Lendo dados do sensor...");
  pinMode(control_led, OUTPUT);
}
 
void loop() {

  // Le as informacoes do sensor em cm
  microsec = ultrasonic.timing();
  dist = ultrasonic.convert(microsec, Ultrasonic::CM);
  
  // Condicao para tentar evitar bugs de distancias muito grandes
  if (dist < 100) { 

    if (dist < 30) movimento++;
    
    Serial.println (dist);
    
    if (movimento == 5) {
      while (dist < 30) {
        microsec = ultrasonic.timing();
        dist = ultrasonic.convert(microsec, Ultrasonic::CM);
        if (dist > 100) dist = 0;
        Serial.print ("lendo");
        Serial.println (dist);
        delay(100);
      }
      estado = !estado;
      digitalWrite(control_led, estado);
      movimento = 0;
    }
    delay(100);
  }
 
}
