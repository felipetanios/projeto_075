#include <SoftwareSerial.h>

//software serial (TX, RX);
SoftwareSerial esp8266(2, 3);

#define DEBUG true

String sendData(String command, const int timeout, boolean debug = false)
{
  // Envio dos comandos AT para o modulo
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
  delay(1000);
  
  Serial.begin(9600);
  //esp8266 anteriormente mudado para baudrate = 19200
  esp8266.begin(19200);
  sendData("ATE1\r\n", 2000, DEBUG); // set echo to sent message
  delay(2000);
  sendData("AT\r\n", 2000, DEBUG); // AT just to recieve OK
  delay(2000);
  test_return = sendData("AT+GMR\r\n", 2000); // recieves version just for testing
  delay(2000);
  Serial.println(test_return);
  Serial.println("FIM");
}

void loop() {
  // put your main code here, to run repeatedly:

}
