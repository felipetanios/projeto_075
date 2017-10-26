// Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>
 
// Define os pinos para o trigger e echo
#define pino_trigger 4
#define pino_echo 5

// Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

// Seta os valores da saida digital
int LEDVERDE = 8;
int LEDAMARELO = 9;

int i;
long microsec;
int flagPosicao = 0; // posicao 0 = parado; posicao 1 = aproxima; posicao 2 = afasta
float distAnteriores[5]; // vetor para armazenar distancias anteriores
float distanciaAtual; // variavel para armazenar distancia atual
float aproximando, afastando; // variaveis contadoras para definir sentido do movimento
int redundanciaAproxima = 0, redundanciaAfasta = 0; // variaveis de redundancia
 
void setup()
{
  Serial.begin(9600);
  Serial.println("Lendo dados do sensor...");
  pinMode(LEDVERDE, OUTPUT);
  pinMode(LEDAMARELO, OUTPUT);
}
 
void loop() {
  
  // Le as informacoes do sensor em cm
  microsec = ultrasonic.timing();
  distanciaAtual = ultrasonic.convert(microsec, Ultrasonic::CM);

  // Zera variaveis contadoras
  aproximando = 0;
  afastando = 0;
  
  // Condicao para tentar evitar bugs de distancias muito grandes
  if (distanciaAtual < 500) { 

    // Compara a distancia atual com as 5 ultimas distancias
    for (i = 0; i < 5; i++) {
      // Se a atual for menor, pessoa esta se aproximando
      if (distAnteriores[i] > distanciaAtual) {
        aproximando++;
      }
      // Se a atual for maior, pessoa esta se afastando
      else if (distanciaAtual > distAnteriores[i]) {
        afastando++;
      }
    }

    // Opta pela opcao mais provavel e altera a flag de estado
    if (aproximando > afastando) {
      flagPosicao = 1;
    }
    else if (afastando > aproximando) {
      flagPosicao = 2;
    }
    else {
      flagPosicao = 0;
    }

    // Imprime a distancia atual
    Serial.print("Distancia em cm: ");
    Serial.println(distanciaAtual);

    // Se a flag for 1, pessoa se aproxima. Incrementa a variavel de redundancia para aproximacao e zera a de redundancia para afastamento 
    if (flagPosicao == 1) {
      Serial.println("Pessoa se aproximando");
      redundanciaAproxima++;
      redundanciaAfasta = 0;
    }
    
    // Se a flag for 2, pessoa se afasta. Incrementa a variavel de redundancia para afastamento e zera a de redundancia para aproximacao
    else if (flagPosicao == 2) {
      Serial.println("Pessoa se afastando");
      redundanciaAfasta++;
      redundanciaAproxima = 0;
    }

    // Se a flag for 0, pessoa esta parada. Zera as variaveis de redundancia
    else {
      Serial.println("Pessoa parada");
      redundanciaAfasta = 0;
      redundanciaAproxima = 0;
    }

    // Armazena distancias anteriores no vetor
    for (i = 0; i < 4; i++) {
      distAnteriores[i] = distAnteriores[i+1];
    }
    distAnteriores[4] = distanciaAtual;

  }

  // Se checar pelo menos 3 vezes seguidas que a pessoa esta se aproximando, considera que esta informacao eh confiavel e acende led verde
  if (redundanciaAproxima >= 3) {
    digitalWrite (LEDVERDE, HIGH);
    digitalWrite (LEDAMARELO, LOW);
  }

  // Se checar pelo menos 3 vezes seguidas que a pessoa esta se afastando, considera que esta informacao eh confiavel e acende led amarelo
  else if (redundanciaAfasta >= 3) {
    digitalWrite (LEDVERDE, LOW);
    digitalWrite (LEDAMARELO, HIGH);
  }
  // Considera que a pessoa esta parada e apaga leds.
  else {
    digitalWrite (LEDVERDE, LOW);
    digitalWrite (LEDAMARELO, LOW);
  }
  
  delay(100);
}
