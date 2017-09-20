#include <SoftwareSerial.h>

SoftwareSerial mySerial(9,10); // Rx, Tx.

int led = 2;

char senderNumber[20];
String AT = "AT";
String READ1 = "AT+CMGR=1";
String gsmBuffer;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(led, OUTPUT);
  boolean handshake = false;
  while (!handshake) {
    Serial.println("SMS-VOTE:v1.0");
    delay(1000);
    if (readSerial() == "OK") {
      handshake = true;
    }
  }
  boolean notConnected = true;
  // Start GSM connection
  while (notConnected) {
    if (sendCommand(AT) == "OK") {
      notConnected = false; 
    }
    delay(1000);
  }
  Serial.println("READY");
  while (readSerial() != "START") {
    delay(1000);
  }
}

void loop() {
  Serial.println("OK");
  // Loop through memory location 1 to 15 of SIM Card to read incoming messages.
  for (int x = 0; x <= 15; x++) {
    if (sendCommand(getMemoryLocationCommand(x) != "OK") { 
      
    }
  }
}

void blinkLED(int times) {
  for (int x = 0; x <= times; x++) {
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(1000);
  }
}

String getMemoryLocationCommand(int x) {
  switch (x) {
    case 0:
      return READ1;
  }
}

String sendCommand(String command) {
  mySerial.println(command);
  gsmBuffer = "";
  char c;
  while(mySerial.available()) {
    c = mySerial.read();
    gsmBuffer += c;
  }
  gsmBuffer.trim();
  return gsmBuffer;
}

String readSerial() {
  static String response;
  static char c;
  static boolean received = false;
  response = "";
  if (Serial.available()) {
    while (Serial.available() && !received) {
    c = Serial.read();
     if (c != ';') {
       response += c;
     } else {
       received = true;
     }
    } 
  }
  response.trim();
  received = false;
  return response;
}

String extractMessage(String message) {
  message = message.substring(message.indexOf("\r\n") + 2);
  return message.substring(0, message.indexOf("\r\n\r\nOK"));
}

