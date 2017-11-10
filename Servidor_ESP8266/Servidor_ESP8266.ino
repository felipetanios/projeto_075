#include <SoftwareSerial.h>

//software serial (TX, RX);
SoftwareSerial esp8266(2, 3);

#define DEBUG true

String sendData(String command, const int timeout, boolean debug = false)
{
  // Send AT commands
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.println(response);
  }
  return response;
}


void setup() {
  String test_return;
  
  Serial.begin(9600);
  
  //esp8266  baudrate previously changed to 19200 (standard is 115200)
  esp8266.begin(19200);
  
  sendData("AT+RST\r\n", 2000, DEBUG); // rst
  delay(500);
  // set echo to print the sent message
  sendData("ATE1\r\n", 2000, DEBUG); 
  delay(500);
  // recieves version just for testing
  test_return = sendData("AT+GMR\r\n", 2000); 
  delay(2000);
  Serial.println(test_return);
  
  
  //trying to connecto to internet
  //show all the access points available
  sendData("AT+CWLAP\r\n", 2000, DEBUG);
  //connect to wifi
  //AT command = AT+CWJAP="SSID","password" 
  sendData("AT+CWJAP=\"Vanilda\",\"1234567890123\"\r\n", 2000, DEBUG);
  delay(3000);
  //test connection
  sendData("AT+CWJAP?\r\n", 2000, DEBUG);
  delay(2000);
  //changes to server mode
  delay(3000);
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  // Shows IP adress
  sendData("AT+CIFSR\r\n", 1000, DEBUG);
  // Sets the server to multiple connections
  sendData("AT+CIPMUX=1rn", 1000, DEBUG);
  // Starts web server at port 80
  sendData("AT+CIPSERVER=1,80rn", 1000, DEBUG);
  
  Serial.println("FIM");
}

void loop() {
  // put your main code here, to run repeatedly:

}
 
//void loop()
//{
//  // Verifica se o ESP8266 esta enviando dados
//  if (esp8266.available())
//  {
//    if (esp8266.find("+IPD,"))
//    {
//      delay(300);
//      int connectionId = esp8266.read() - 48;
// 
//      String webpage = "<head><meta http-equiv=""refresh"" content=""3"">";
//      webpage += "</head><h1><u>ESP8266 - Web Server</u></h1><h2>Porta";
//      webpage += "Digital 8: ";
//      int a = digitalRead(8);
//      webpage += a;
//      webpage += "<h2>Porta Digital 9: ";
//      int b = digitalRead(9);
//      webpage += b;
//      webpage += "</h2>";
// 
//      String cipSend = "AT+CIPSEND=";
//      cipSend += connectionId;
//      cipSend += ",";
//      cipSend += webpage.length();
//      cipSend += "rn";
// 
//      sendData(cipSend, 1000, DEBUG);
//      sendData(webpage, 1000, DEBUG);
// 
//      String closeCommand = "AT+CIPCLOSE=";
//      closeCommand += connectionId; // append connection id
//      closeCommand += "rn";
// 
//      sendData(closeCommand, 3000, DEBUG);
//    }
//  }
//}
// 
//String sendData(String command, const int timeout, boolean debug)
//{
//  // Envio dos comandos AT para o modulo
//  String response = "";
//  esp8266.print(command);
//  long int time = millis();
//  while ( (time + timeout) > millis())
//  {
//    while (esp8266.available())
//    {
//      // The esp has data so display its output to the serial window
//      char c = esp8266.read(); // read the next character.
//      response += c;
//    }
//  }
//  if (debug)
//  {
//    Serial.print(response);
//  }
//  return response;
//}