#include <SoftwareSerial.h>

//ESP_SET UP DEFINITIONS
//software serial (TX, RX);
SoftwareSerial esp8266(2, 3);
String url_response;

//Setando variaveis globais para teste do servidor
#define DEBUG true
int fan = 7;
int luz1 = 8;
int luz2 = 9;
int luz3 = 10;
int luz4 = 11;

void setup() {
   Serial.begin(9600);
  //esp8266  baudrate previously changed to 19200 (standard is 115200)
  esp8266.begin(19200);
  //set up ESP

  Setting_ESP();
  pinMode(fan, OUTPUT);
  pinMode(luz1, OUTPUT);
  pinMode(luz2, OUTPUT);
  pinMode(luz3, OUTPUT);
  pinMode(luz4, OUTPUT);
  

}

void loop() {
  //Serial.println("ligando");
  
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

void Setting_ESP(){
  String flag_wifi;
  
  Serial.println("testing ESP8266");
  //enable echo
  sendData("ATE1\r\n", 2000); 
  // recieves version just for testing
  sendData("AT+GMR\r\n", 2000); 
  

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
  // Sets the client to multiple connections
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
  //disable echo
  sendData("ATE0\r\n", 2000);
  Serial.println("End of setting up");

  Serial.println("Connecting to server");
  sendData("AT+CIPCLOSE=4\r\n",2000,DEBUG);
  String getTCP = "";
  getTCP = "AT+CIPSTART=4,\"TCP\",\"192.168.108.6\",80\r\n";
  sendData(getTCP, 2000, DEBUG);
  Serial.println("Connected to server");
}

String get_page(){
  //Serial.println("fetching data from server");
  String fetchdata = "";
  String response;
  String filtered_html = "";
  String filtered_response = "";
  int begin_html = 0;
  int end_response = 0;
  //send a get request to 
  fetchdata = "AT+CIPSEND=4,3\r\n";
  sendData(fetchdata, 300);
  response = sendData("GET",300);
  
  //Serial.println("imprimindo resposta");
  //Serial.println(response);
  //Gets the begining of payloads html from get request
  begin_html = response.indexOf(':');
  Serial.println(begin_html);
  if (begin_html > 0){
    filtered_html = response.substring(begin_html+1);
    filtered_response = filtered_html.substring(52);
    end_response = filtered_response.indexOf("<");
    filtered_response = filtered_response.substring(0, end_response);

    //returns the string fetched on servers posted url
    return filtered_response;
  }
}


