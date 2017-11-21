#include <SoftwareSerial.h>
#include <Ultrasonic.h>


//ESP_SET UP DEFINITIONS
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
int dist;
int movimento = 0;
boolean estado = LOW;

String IP = "";

void setup() {
  
  Serial.begin(9600);
  //esp8266  baudrate previously changed to 19200 (standard is 115200)
  esp8266.begin(19200);
  //set up ESP

  Setting_ESP();
    
  pinMode(control_led, OUTPUT);

  Serial.println("pushing  webpage for the first time");

  ultrasound_status =  check_ultrassom();

}

void loop(){
  //lights up the control led so you can see that the configuration step is over
  digitalWrite(control_led, HIGH);
  ultrasound_status_update = ultrasound_status;
  //ultrasound_status =  check_ultrassom();

  Serial.println(ultrasound_status);

  update_page(ultrasound_status);

  delay(1000);

  while(ultrasound_status == ultrasound_status_update){
      ultrasound_status =  check_ultrassom();
  }
}

/*
*
*ALL THE FUNCTIONS USED ARE SET HERE
*
*/


//function to communicate arduino to ESP
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


//Function that pushes a html to server ip adress
void update_page(String a){
  if (esp8266.available()){
    //finds the "IPD, " when ESP sends things to the serial input on arduino board
    if (esp8266.find("+IPD,")){
      delay(100);
      //checks the connection ID number (the -48 is because of ascii characters)
      int connectionId = esp8266.read() - 48;
 
      String webpage = "<head><meta http-equiv=""refresh"" content=""2"">";
      webpage += "</head><h2>";
      webpage += a;
      webpage += "</h2>";
 
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";
 
      sendData(cipSend, 500, DEBUG);
      sendData(webpage, 500, DEBUG);
 
//      String closeCommand = "AT+CIPCLOSE=";
//      closeCommand += connectionId; // append connection id
//      closeCommand += "\r\n";
// 
//      sendData(closeCommand, 3000, DEBUG);
    }
  }
  Serial.println("updated webapge");
}

//function that sets ESP as a server to a single network
void Setting_ESP(){  
  String test_return;
  String flag_wifi;
  
  Serial.println("testing ESP8266");
  //enable echo
  sendData("ATE1\r\n", 2000); 
  // recieves version just for testing
  test_return = sendData("AT+GMR\r\n", 2000); 
  Serial.println(test_return);
  

  Serial.println("Setting up connection");

  //changes to server mode
  Serial.println("Changing to server mode");
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  //check connection
  Serial.println("Checking connection");
  flag_wifi = sendData("AT+CWJAP?\r\n", 2000);
  //if answer is No AP (it's not connected)
  if (flag_wifi[0] == 'N'){
    Serial.println("Not connected to any AP. Trying to connect");
    //trying to connecto to internet
    //connect to wifi
    //AT command = AT+CWJAP="SSID","password"
    //this part is messy, but if it does not work, you can set it up by hand through arduino tx/rx serial connection
    sendData("AT+CWJAP=""tanios"",""bicicleta""\r\n", 2000, DEBUG);
  }
  //when its connected
  Serial.println("Connected to wifi");
  flag_wifi = sendData("AT+CWJAP?\r\n", 2000, DEBUG);
  Serial.println(flag_wifi);
    // Shows IP adress
  Serial.println("Checking IP");
  sendData("AT+CIFSR\r\n", 1000, DEBUG);
  // Sets the server to multiple connections
  Serial.println("Setting Server to multiple connections");
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
  // Starts web server at port 80
  Serial.println("Starting server at port 80");
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG);

  Serial.println("printing web status");

  //disable echo
  sendData("ATE0\r\n", 2000);
  Serial.println("End of setting up");
}


//ultrasound chekcer function

String check_ultrassom(){
  // Le as informacoes do sensor em cm
  microsec = ultrasonic.timing();
  dist = ultrasonic.convert(microsec, Ultrasonic::CM);
  
  // Condicao para tentar evitar bugs de distancias muito grandes
  if (dist < 100) { // *distancia da parede deve ser menor que essa*

    // Distancia máxima que a pessoa passa do sensor e é reconhecida
    if (dist < 15) movimento++;
    
  //  Serial.println (dist);

    // Numero de vezes que o programa precisa ler a distância menor para considerar a informação confiável
    if (movimento == 3) {
      while (dist < 15) { // entra num loop até a distancia voltar a ser grande
        microsec = ultrasonic.timing();
        dist = ultrasonic.convert(microsec, Ultrasonic::CM);
        if (dist > 100) dist = 0; // ignora leitura de valores maiores que 100
       // Serial.print ("lendo ");
        //Serial.println (dist);
      }
      estado = !estado;
      //digitalWrite(control_led, estado);
      movimento = 0;
      if (estado == HIGH) {
        return "entrando";
      }
      else {
        return "saindo";
      }
    }
  }
}



