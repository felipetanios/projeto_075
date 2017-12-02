#include <SoftwareSerial.h>
#include <Ultrasonic.h>


/*
*Função sendData obtida no tutorial https://www.filipeflop.com/blog/esp8266-arduino-tutorial/
*Função de configuração tambem baseada nesse tutorial
*Lista de comandos AT (protocolo de comunicação do ESP8266)
*https://room-15.github.io/blog/2015/03/26/esp8266-at-command-reference/
*Biblioteca ultrassonic utilizada: http://www.arduinolibraries.info/libraries/ultrasonic
*/

/* DEFINIÇÕES DO ESP */

//Settando as configurações para comunicação serial entre arduino e ESP8266
//software serial (TX, RX);
SoftwareSerial esp8266(2, 3);
String ultrasound_status;
String ultrasound_status_update;

//Setando variaveis globais para teste do servidor
#define DEBUG true
int control_led = 11;

/* DEFINIÇÕES DO ULTRASSOM */

// Define os pinos para o trigger e echo
#define pino_trigger 4
#define pino_echo 5

// Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

int i;
long microsec;
int flagPosicao = 0; // posicao 0 = parado; posicao 1 = aproxima; posicao 2 = afasta
float distAnteriores[5]; // vetor para armazenar distancias anteriores
float distanciaAtual; // variavel para armazenar distancia atual
float aproximando, afastando; // variaveis contadoras para definir sentido do movimento
int redundanciaAproxima = 0, redundanciaAfasta = 0; // variaveis de redundancia

String IP = "";

void setup() {
  
  Serial.begin(9600);
  //esp8266  baudrate mudado anteriormente para 19200 (o padrão é 115200)
  esp8266.begin(19200);
  //Configura o ESP

  Setting_ESP();
    
  pinMode(control_led, OUTPUT);

  Serial.println("pushing  webpage for the first time");

  ultrasound_status =  check_ultrassom();

}

void loop(){
  //Liga um led quando o servidor esta configurado e funcionando
  digitalWrite(control_led, HIGH);
  
   ultrasound_status =  check_ultrassom();

  if (!(ultrasound_status == "parada")){
    Serial.println(ultrasound_status) ;
    update_page(ultrasound_status);
  }

  delay(1000);

}

/*FUNÇÃO PARA COMUNICAÇÃO SERIAL ENTRE ARDUINO E ESP8266*/

String sendData(String command, const int timeout, boolean debug = false){
  // Send AT commands
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis()){
    while (esp8266.available()){
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug){
    Serial.println(response);
  }
  return response;
}


/*
*
*TODAS AS FUNÇÕES CRIADAS POR NOSSO GRUPO SÃO DEFINIDAS A PARTIR DAQUI
*
*/

/*FUNÇÃO PARA POST DE UMA PAGINA NO SERVIDOR PELO PRÓPRIO SERVIDOR*/

//Essa função faz um post de uma página HTML contendo a palavra _a_ no IP do servidor

void update_page(String a){
  if (esp8266.available()){
    //Acha o começo da resposta do TCP/IP para ver o Id de conexão
    if (esp8266.find("+IPD,")){
      delay(100);
      //Esse ID é a primeira coisa depois de +IPD, na resposta, porem como é uma string, precisamos transformar o numero
      //de ASCII para inteiro
      int connectionId = esp8266.read() - 48;


      //Define a página HTML a ser postada no endereço de IP do próprio servidor
      String webpage = "<head><meta http-equiv=""refresh"" content=""2"">";
      webpage += "</head><h2>";
      webpage += a;
      webpage += "</h2>";

      //Envia a define o que vai ser enviado para o IP do servidor (o comando AT+CIPSEND tem como parametros
      //A o ID de conexão e o tamanho da string a ser enviada
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";
 
      sendData(cipSend, 500, DEBUG);
      sendData(webpage, 500, DEBUG);
      

    }
  }
  Serial.println("updated webapge");
}



/* FUNÇÃO DE CONFIGURAÇÃO INICIAL DO ESP8266 COMO SERVIDOR PARA MULTIPLAS CONEXÕES*/

void Setting_ESP(){  
  String test_return;
  String flag_wifi;
  
  /*
  *inicialmente  se testa simplesmente se o ESP8266 está se comunicando com o arduino
  */
  
  Serial.println("testing ESP8266");
  
  //Comando para que o ESP imprima o comando que foi enviado a ele e a resposta (echo)
  
  sendData("ATE1\r\n", 2000); 
  
  //Comando para imprimir versão do ESP e do firmware dele
  
  test_return = sendData("AT+GMR\r\n", 2000); 
  
  Serial.println(test_return);
  
  
  /*
   * Agora conectamos o ESP a rede e o configuramos como servidor
   */

  Serial.println("Setting up connection");


  //Muda o ESP para o modo 1 (modo Station, não Access Point)
  
  Serial.println("Changing to server mode");
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  //Checa se o ESP esta conectado
  Serial.println("Checking connection");
  flag_wifi = sendData("AT+CWJAP?\r\n", 2000);

  if (flag_wifi[0] == 'N'){
    //Se ele nao estiver conectado a nenhum AP (access point) temos que conecta-lo
    Serial.println("Not connected to any AP. Trying to connect");
    //O comando AT utilizado é o AT+CWJAP="ssid","senha"
    sendData("AT+CWJAP=""tanios"",""bicicleta""\r\n", 2000, DEBUG);
  }
  
  //Quando o ESP Estiver conectado
  Serial.println("Connected to wifi");
  flag_wifi = sendData("AT+CWJAP?\r\n", 2000, DEBUG);
  Serial.println(flag_wifi);
  //Imprime o IP dele (servidor)
  Serial.println("Checking IP");
  sendData("AT+CIFSR\r\n", 1000, DEBUG);
  //E configura para multiplas conexões
  Serial.println("Setting Server to multiple connections");
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
  //Então incia o servidor na porta 80
  Serial.println("Starting server at port 80");
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG);

  Serial.println("printing web status");

  //Por fim desabilita a função que faz com que o ESP imprima o comando mandado para ele (disable echo)
  sendData("ATE0\r\n", 2000);
  Serial.println("End of setting up");
}


//ultrasound chekcer function

String check_ultrassom(){
    // Le as informacoes do sensor em cm
  microsec = ultrasonic.timing();
  distanciaAtual = ultrasonic.convert(microsec, Ultrasonic::CM);

  // Zera variaveis contadoras
  aproximando = 0;
  afastando = 0;
  
  // Condicao para tentar evitar bugs de distancias muito grandes (maiores que 30cm -> maquete)
  if (distanciaAtual < 30) { 

    // Compara a distancia atual com as 5 ultimas distancias
    for (i = 0; i < 5; i++) {
      // Se a atual for menor, pessoa esta se aproximando
      if (distAnteriores[i] - distanciaAtual > 3) {
        aproximando++;
      }
      // Se a atual for maior, pessoa esta se afastando
      else if (distanciaAtual - distAnteriores[i] > 3) {
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

    // Se a flag for 1, pessoa se aproxima. Incrementa a variavel de redundancia para aproximacao e zera a de redundancia para afastamento 
    if (flagPosicao == 1) {
      redundanciaAproxima++;
      redundanciaAfasta = 0;
    }
    
    // Se a flag for 2, pessoa se afasta. Incrementa a variavel de redundancia para afastamento e zera a de redundancia para aproximacao
    else if (flagPosicao == 2) {
      redundanciaAfasta++;
      redundanciaAproxima = 0;
    }

    // Se a flag for 0, pessoa esta parada. Zera as variaveis de redundancia
    else {
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
      //Serial.println("Pessoa se aproximando");
      return "aproximando";
  }

  // Se checar pelo menos 3 vezes seguidas que a pessoa esta se afastando, considera que esta informacao eh confiavel e acende led amarelo
  else if (redundanciaAfasta >= 3) {    
      //Serial.println("Pessoa se afastando");
      return "afastando";
  }
  // Considera que a pessoa esta parada e apaga leds.
  else {
      //Serial.println("Pessoa parada");
      return "parada";
  }
}

