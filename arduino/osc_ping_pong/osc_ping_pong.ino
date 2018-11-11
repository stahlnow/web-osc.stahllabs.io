/*---------------------------------------------------------------------------------------------

  Open Sound Control (OSC) library for the ESP8266

  Example for sending messages from the ESP8266 to a remote computer
  When receiving a message with address "/pong", this example is sending "ping!"
  to the address "/ping" as a reply.
  As a little extra: the example also sends a regularly timed message 
  with the help of little timer class.

  Don't forget to set your ssid / pass in the code below.

  This example code is in the public domain.

--------------------------------------------------------------------------------------------- */
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

#include "Timer.h"

char ssid[] = "****";                     // your network SSID (name)
char pass[] = "**************";           // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192,168,1,11);        // remote IP of your computer
const unsigned int outPort = 57121;         // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)
OSCErrorCode error;

#define LED D6                            // this is our debug LED

// Special ping timer
Timer ping(5000);    // period = 5000 ms

void setup() {

  pinMode(LED, OUTPUT);

  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());

}

void toggleLed() {
  static boolean flag;
  flag ^= 1;
  digitalWrite(LED, flag);
}

void dispatchPong(OSCMessage &msg) {

  // we excpect a message with a single value of type 'String'
  char value[16];
  msg.getString(0, value, 16);

  char output[512];
  sprintf(output, "received msg with value %s", value);
  Serial.println(output);
  
  // toggle LED
  toggleLed();
  
  // send reply
  OSCMessage reply("/ping");
  reply.add("ping!");
  Udp.beginPacket(outIp, outPort);
  reply.send(Udp);
  Udp.endPacket();
  reply.empty();
}


void loop() {

  if (ping.shouldRun()) {
    // send ping
    OSCMessage msg("/timer/ping");
    msg.add("regularly timed ping message!");
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
  }
  
  // handle incoming osc
  OSCBundle bundle;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      bundle.fill(Udp.read());
    }
    if (!bundle.hasError()) {
      bundle.dispatch("/pong", dispatchPong);
    } else {
      error = bundle.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}
