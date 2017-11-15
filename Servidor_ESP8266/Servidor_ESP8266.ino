#include <SoftwareSerial.h>

//software serial (TX, RX);
SoftwareSerial esp8266(2, 3);

#define DEBUG true
int control_led = 11;
int push_1 = 0;
int push_2 = 0;
int flag_1 = 0;
int flag_2 = 0;

String IP = "";

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

void update_page(int push_1, int push_2){
  if (esp8266.available()){
    //finds the "IPD, " when ESP sends things to the serial input on arduino board
    if (esp8266.find("+IPD,")){
      delay(100);
      //checks the connection ID number (the -48 is because of ascii characters)
      int connectionId = esp8266.read() - 48;
 
      String webpage = "<head><meta http-equiv=""refresh"" content=""2"">";
      webpage += "</head><h1><u>ESP8266 - Web Server</u></h1><h2>Porta";
      webpage += "Digital 8: ";
      webpage += push_1;
      webpage += "<h2>Porta Digital 9: ";
      webpage += push_2;
      webpage += "</h2>";
 
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";
 
      sendData(cipSend, 500, DEBUG);
      sendData(webpage, 500, DEBUG);
 
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // append connection id
      closeCommand += "\r\n";
 
      sendData(closeCommand, 3000, DEBUG);
    }
  }
  Serial.print("updated webapge");
}

void setup() {
  String test_return;
  String flag_wifi;
  
  Serial.begin(9600);
  
  //esp8266  baudrate previously changed to 19200 (standard is 115200)
  esp8266.begin(19200);
  

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

  //disable echo
  sendData("ATE0\r\n", 2000);
  Serial.println("End of setting up");

  
  pinMode(control_led, OUTPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  push_1 = digitalRead(8);
  Serial.print("PINO 8: ");
  Serial.println(push_1);
  push_2 = digitalRead(9);
  Serial.print("PINO 9: ");
  Serial.println(push_2);

  Serial.println("pushing  webpage for the first time");
  update_page(push_1, push_2);
}

void loop(){
  //lights up the control led so you can see that the configuration step is over
  digitalWrite(control_led, HIGH);
  //Serial.println(digitalRead(8));
  Serial.print("PINO 8: ");
  Serial.println(push_1);
  Serial.print("PINO 9: ");
  Serial.println(push_2);
  flag_1 = push_1;
  flag_2 = push_2;
  update_page(push_1, push_2);
  
  while((flag_1 == push_1) && (flag_2 == push_2)){
    push_1 = digitalRead(8);
    push_2 = digitalRead(9);
  }
}
