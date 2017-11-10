#include <SoftwareSerial.h>

//software serial (TX, RX);
SoftwareSerial esp8266(2, 3);

#define DEBUG true

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


void setup() {
  String test_return;
  String flag_wifi;
  
  Serial.begin(9600);
  
  //esp8266  baudrate previously changed to 19200 (standard is 115200)
  esp8266.begin(19200);
  

  Serial.println("testing ESP8266");
  //enable echo
  sendData("ATE1\r\n", 2000); 
  delay(500);
  // recieves version just for testing
  test_return = sendData("AT+GMR\r\n", 2000); 
  delay(1000);
  Serial.println(test_return);
  

  Serial.println("Setting up connection");

  //changes to server mode
  Serial.println("Changing to server mode");
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  //check connection
  Serial.println("Checking connection");
  flag_wifi = sendData("AT+CWJAP?\r\n", 2000);
  delay(1000);
  //if answer is No AP (it's not connected)
  if (flag_wifi[0] == 'N'){
    Serial.println("Not connected to any AP. Trying to connect");
    //trying to connecto to internet
    //connect to wifi
    //AT command = AT+CWJAP="SSID","password"
    //this part is messy, but if it does not work, you can set it up by hand through arduino tx/rx serial connection
    sendData("AT+CWJAP=\"Vanilda\",\"1234567890123\"\r\n", 2000, DEBUG);
    delay(2000);
  }
  //when its connected
  Serial.println("Connected to wifi");
  flag_wifi = sendData("AT+CWJAP?\r\n", 2000, DEBUG);
  delay(1000);
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
}

void loop(){
  // Checks if ESP8266 is available
  if (esp8266.available()){
    //finds the "IPD, " when ESP sends things to the serial input on arduino board
    if (esp8266.find("+IPD,")){
      delay(300);
      //checks the ID number (the -48 is because of ascii characters)
      int connectionId = esp8266.read() - 48;
 
      String webpage = "<head><meta http-equiv=""refresh"" content=""3"">";
      webpage += "</head><h1><u>ESP8266 - Web Server</u></h1><h2>Porta";
      webpage += "Digital 8: ";
      int a = digitalRead(8);
      webpage += a;
      webpage += "<h2>Porta Digital 9: ";
      int b = digitalRead(9);
      webpage += b;
      webpage += "</h2>";
 
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";
 
      sendData(cipSend, 1000, DEBUG);
      sendData(webpage, 1000, DEBUG);
 
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // append connection id
      closeCommand += "\r\n";
 
      sendData(closeCommand, 3000, DEBUG);
    }
  }
}
