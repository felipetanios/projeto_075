//Autor: FILIPEFLOP
 
//Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>
 
//Define os pinos para o trigger e echo
#define pino_trigger 4
#define pino_echo 5

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

// Seta os valores da saída digital
int LEDVERDE = 8;
int LEDAMARELO = 9;

int i;
long microsec;
int flagPosicao = 0; // Posicao 0 = parado; Posicao 1 = aproxima, Posicao 2 = afasta
float distAnteriores[5]; // Vetor para armazenar distancias anteriores
float distanciaAtual; // variável para armazenar distancia atual
float aproximando, afastando; 
int redundanciaAproxima = 0, redundanciaAfasta = 0;
 
void setup()
{
  Serial.begin(9600);
  pinMode(LEDVERDE, OUTPUT);
  pinMode(LEDAMARELO, OUTPUT);
  Serial.println("Lendo dados do sensor...");
}
 
void loop() {
  
  //Le as informacoes do sensor em cm
  microsec = ultrasonic.timing();
  distanciaAtual = ultrasonic.convert(microsec, Ultrasonic::CM);

  aproximando = 0;
  afastando = 0;
  
  //Exibe informacoes no serial monitor
  if (distanciaAtual < 500) { 
    
    for (i = 0; i < 5; i++) {
      if (distAnteriores[i] > distanciaAtual) {
        aproximando++;
      }
      else if (distanciaAtual > distAnteriores[i]) {
        afastando++;
      }
    }

    if (aproximando > afastando) {
      flagPosicao = 1;
    }
    else if (afastando > aproximando) {
      flagPosicao = 2;
    }
    else {
      flagPosicao = 0;
    }
    
    Serial.print("Distancia em cm: ");
    Serial.println(distanciaAtual);
    
    if (flagPosicao == 1) {
      Serial.println("Pessoa se aproximando");
      redundanciaAproxima++;
      redundanciaAfasta = 0;
    }
    else if (flagPosicao == 2) {
      Serial.println("Pessoa se afastando");
      redundanciaAfasta++;
      redundanciaAproxima = 0;
    }
    else {
      Serial.println("Pessoa parada");
    }
    
    for (i = 0; i < 4; i++) {
      distAnteriores[i] = distAnteriores[i+1];
    }
    distAnteriores[4] = distanciaAtual;

  }

  if (redundanciaAproxima > 2) {
    digitalWrite (LEDVERDE, HIGH);
    digitalWrite (LEDAMARELO, LOW);
  }

  else if (redundanciaAfasta > 2) {
    digitalWrite (LEDVERDE, LOW);
    digitalWrite (LEDAMARELO, HIGH);
  }
  
  delay(1000);
}
