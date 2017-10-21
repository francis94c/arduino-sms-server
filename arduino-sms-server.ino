#include <SoftwareSerial.h>

/*
 * Global Variables.
 */
SoftwareSerial mySerial(9,10); // Rx, Tx.

int led = 13;

char senderNumber[20];

String AT = "AT";
String READ = "AT+CMGR=";
String DELETE = "AT+CMGD=1,3";
String gsmBuffer;
String outputBuffer = "";

boolean isReady = false;

/**
 * Setup
 */
void setup() {
  Serial.begin(115200);
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
  // Initialize GSM Module.
  String response;
  while (!isReady) {
    response = sendCommand(AT);
    if (response == "AT\r\n\r\nOK" || response == "OK") {
      isReady = true; 
      sendCommand("ATE0");
      Serial.println("READY");
      sendCommand(DELETE);
    }
    delay(1000);
  }
}

/**
 * Loop
 */
void loop() {
  static String response;
  static String phoneNumber;
  static String message;
  static int handle1;
  static int handle2;
  static String request;
  request = Serial.readString();
  if (request == "QUERY VOTES") {
    // Loop through memory location 1 to 15 of SIM Card to read incoming messages.
    for (int x = 0; x < 15; x++) {
      response = sendCommand(READ + (x + 1));
      if (response != "OK") {
        handle1 = response.indexOf("\",\"") + 3;
        phoneNumber = response.substring(handle1, response.indexOf("\"", handle1));
        handle2 = response.indexOf("\r\n") + 2;
        message = response.substring(handle2, response.indexOf("\r\n", handle2));
        message.trim();
        if (phoneNumber.startsWith("+")) {
          blinkLED(2);
          outputBuffer += "ENTRY-" + phoneNumber + "-" + message + ";"; 
        }
      }
    }
    Serial.println("OK"); 
  } else if (request == "GET VOTES") {
    if (outputBuffer == "") {
      Serial.println("NO VOTES;");
    } else {
      Serial.println(outputBuffer);
      outputBuffer = "";
    }
  } else if (request == "CLEAR READ SMSES") {
    sendCommand(DELETE);
    Serial.println("OK");
  } else if (request == "OK") {
    sendCommand("ATE0");
    if (sendCommand(AT) != "READY") {
      Serial.println("NOT READY");
    } else {
      Serial.println("READY");
    }
  }
}

/**
 * to blink the LED
 */
void blinkLED(int times) {
  for (int x = 0; x <= times; x++) {
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(1000);
  }
}

String sendCommand(String command) {
  mySerial.println(command);
  gsmBuffer = "";
  char c;
  delay(1000);
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

