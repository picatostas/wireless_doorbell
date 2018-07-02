#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#define button 2
// wifi connection variables
const char* ssid = "MY_SSID";
const char* password = "MY_PASS";
boolean wifiConnected = false;
// UDP variables
unsigned int localPort = 9999;
WiFiUDP UDP;
boolean udpConnected = false;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "OK"; // a string to send back

IPAddress target_ip(192, 168, 1, 44);
unsigned int target_port = 8888;

//unsigned int target_port = 8888;
int button_state=1, button_state_prev=1;
void setup() {
  // Initialise Serial connection
  Serial.begin(115200);
  // Initialise wifi connection
  wifiConnected = connectWifi();
  // only proceed if wifi connection successful
  if (wifiConnected) {
    udpConnected = connectUDP();
    if (udpConnected) {
      // initialise pins
      pinMode(button, INPUT);
    }
  }
}
void loop() {
  // check if the WiFi and UDP connections were successful
  if (wifiConnected) {
    button_state = digitalRead(button);
    if((button_state != button_state_prev) && !(button_state))
    {
    
        Serial.println("button pressed");
        UDP.beginPacket(target_ip, target_port);
        UDP.write("tim");
        UDP.endPacket();
        
    }
    if (udpConnected) {
      // if there’s data available, read a packet
      int packetSize = UDP.parsePacket();
      if (packetSize)
      {
        Serial.println("");
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
        Serial.print("From ");
        IPAddress remote = UDP.remoteIP();
        for (int i = 0; i < 4; i++)
        {
          Serial.print(remote[i], DEC);
          if (i < 3)
          {
            Serial.print(".");
          }
        }
        Serial.print(", port ");
        Serial.println(UDP.remotePort());
        // read the packet into packetBufffer
        UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Contents:");
        for(int i = 0;i<packetSize;i++)
          Serial.print(packetBuffer[i]);
        //int value = packetBuffer[0] * 256 + packetBuffer[1];
       // Serial.println(value);
        // send a reply, to the IP address and port that sent us the packet we received
//        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
//        UDP.write("tim");
//        UDP.endPacket();
        // turn LED on or off depending on value recieved
        //digitalWrite(5, value);
      }
      delay(10);
    }
  }
  button_state_prev = button_state;
}
// connect to UDP – returns true if successful or false if not
boolean connectUDP() {
  boolean state = false;
  Serial.println("");
  Serial.println("Connecting to UDP");
  if (UDP.begin(localPort) == 1) {
    Serial.println("Connection successful");
    state = true;
  }
  else {
    Serial.println("Connection failed");
  }
  return state;
}
// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  return state;
}

