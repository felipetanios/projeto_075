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
  if (dist < 100) { // *distancia da parede deve ser menor que essa*

    // Distancia máxima que a pessoa passa do sensor e é reconhecida
    if (dist < 15) movimento++;
    
    Serial.println (dist);

    // Numero de vezes que o programa precisa ler a distância menor para considerar a informação confiável
    if (movimento == 3) {
      while (dist < 15) { // entra num loop até a distancia voltar a ser grande
        microsec = ultrasonic.timing();
        dist = ultrasonic.convert(microsec, Ultrasonic::CM);
        if (dist > 100) dist = 0; // ignora leitura de valores maiores que 100
        Serial.print ("lendo ");
        Serial.println (dist);
      }
      estado = !estado;
      digitalWrite(control_led, estado);
      movimento = 0;
      if (estado == HIGH) {
        Serial.println("entrando");
      }
      else {
        Serial.println("saindo");
      }
    }
  }
 
}
