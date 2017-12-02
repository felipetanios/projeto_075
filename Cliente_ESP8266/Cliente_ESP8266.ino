#include <SoftwareSerial.h>

/*
*Função sendData obtida no tutorial https://www.filipeflop.com/blog/esp8266-arduino-tutorial/
*Função de configuração tambem baseada nesse tutorial
*Lista de comandos AT (protocolo de comunicação do ESP8266)
*https://room-15.github.io/blog/2015/03/26/esp8266-at-command-reference/
*
*/


/* DEFINIÇÕES DO ESP */

//Settando as configurações para comunicação serial entre arduino e ESP8266
//software serial (TX, RX);
SoftwareSerial esp8266(2, 3);
String url_response;

//Setando variaveis globais para ativação dos itens dos quartos
#define DEBUG true
int fan = 7;
int luz1 = 8;
int luz2 = 9;
int luz3 = 10;
int luz4 = 11;

void setup() {
   Serial.begin(9600);
  //esp8266  baudrate mudado anteriormente para 19200 (o padrão é 115200)
  esp8266.begin(19200);
  //Configura o  ESP

  Setting_ESP();

  //Define todos os itens do quarto como saída nas portas do arduino.
  pinMode(fan, OUTPUT);
  pinMode(luz1, OUTPUT);
  pinMode(luz2, OUTPUT);
  pinMode(luz3, OUTPUT);
  pinMode(luz4, OUTPUT);
  

}

void loop() {
  //Serial.println("ligando");
  //Nesse loop acendemos ou apagamos a luz de acordo com a string que esta na HTML no IP do servidor.
  
  url_response = get_page();
  Serial.println (url_response);
  if (url_response == "aproximando"){
    digitalWrite(fan, LOW);
    digitalWrite(luz1, LOW);
    digitalWrite(luz2, LOW);
    digitalWrite(luz3, LOW);
    digitalWrite(luz4, LOW);
  }
  if (url_response == "afastando"){
    digitalWrite(fan, HIGH);
    digitalWrite(luz1, HIGH);
    digitalWrite(luz2, HIGH);
    digitalWrite(luz3, HIGH);
    digitalWrite(luz4, HIGH);
  }
  
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


/* FUNÇÃO DE CONFIGURAÇÃO INICIAL DO ESP8266 COMO SERVIDOR PARA MULTIPLAS CONEXÕES*/

void Setting_ESP(){
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
   * Agora conectamos o ESP a rede e o configuramos como cliente
   */
   
  Serial.println("Setting up connection");

  //Muda o ESP para o modo 1 (modo Station, não Access Point)
  Serial.println("Changing to client mode");
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
  
  //Por fim desabilita a função que faz com que o ESP imprima o comando mandado para ele (disable echo)
  sendData("ATE0\r\n", 2000);
  Serial.println("End of setting up");

  //Agora com o cliente configurado e conectado na rede, abrimos a conexão dele com o servidor
  Serial.println("Connecting to server");
  //Primeiramente fechamos qualquer conexão anterior (já que mesmo com o servidor off, eles mantém essa flag como conectado
  sendData("AT+CIPCLOSE=4\r\n",2000,DEBUG);
  String getTCP = "";
  //Então finalmente conectamos diretamente no IP do servidor
  getTCP = "AT+CIPSTART=4,\"TCP\",\"192.168.108.6\",80\r\n";
  sendData(getTCP, 2000, DEBUG);
  Serial.println("Connected to server");
}

/* FUNÇÃO PARA OBTER A STRING NA HTLM POSTADA PELO SERVIDOR*/

String get_page(){
  //Serial.println("fetching data from server");
  String fetchdata = "";
  String response;
  String filtered_html = "";
  String filtered_response = "";
  int begin_html = 0;
  int end_response = 0;

  
  //Primeiro manda uma informação que vai ter uma requisição http para o servidor
  fetchdata = "AT+CIPSEND=4,3\r\n";
  sendData(fetchdata, 300);

  //Então e executado o GET no servidor
  response = sendData("GET",300);
  
  //Serial.println("printing response");
  //Serial.println(response);

  //Essa resposta vem com o prefixo +IPD,<ID de conexão>: Resposta
  //Como precisamos so de uma palavra que está na resposta fazemos um parser tendo em vista de que sabemos exatamente
  //qual é a HTML esperada como resposta. Para aplicações mais complexas o parser deveria ser totalmente diferente
  
  begin_html = response.indexOf(':');
  Serial.println(begin_html);
  if (begin_html > 0){
    filtered_html = response.substring(begin_html+1);
    filtered_response = filtered_html.substring(52);
    end_response = filtered_response.indexOf("<");
    filtered_response = filtered_response.substring(0, end_response);

    //Ao final, retornamos apenas a palavra que gostariamos 
    return filtered_response;
  }
}


