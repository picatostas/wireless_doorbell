// codigo del receptor
#include "LedControlMS.h"
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#define TIMEOUT_PIR 8000
// wifi connection variables
const char* ssid = "MY_SSID";
const char* password = "MY_PASS";
boolean wifiConnected = false;
// UDP variables
unsigned int localPort = 8888;
WiFiUDP UDP;
boolean udpConnected = false;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
bool timbre = false,pir = false;
// estos dos contadores los uso para usar la variable pir como un token temporal
int pir_timeout = 0, pir_timeout_prev = 0; 
int tim_timeout = 0, tim_timeout_prev = 0;

const char pir_msg[] = "pir";
const char tim_msg[] = "tim";
const char ReplyBuffer[] = "OK"; // a string to send back


// Driver de la matriz con max7219
#define DIN  14
#define CLK  13
#define CS   12
LedControl lc = LedControl(DIN, CLK, CS, 1);

/* we always wait a bit between updates of the display */
unsigned long delaytime = 800;

#define BUZZ 4

void setup() {

  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);
  pinMode(BUZZ, OUTPUT); // para buzz activo
  //noTone(BUZZ); // por si tuivieses un buzz Pasivo
  // Initialise Serial connection
  Serial.begin(115200);
  // Initialise wifi connection
  wifiConnected = connectWifi();
  // only proceed if wifi connection successful
  if (wifiConnected) {
    udpConnected = connectUDP();   
  }
  
}
void loop() {
  // Comprobamos wifi

  if (wifiConnected) {

    
    //comprobamos que haya conexion por UDP
    if (udpConnected) {

      // comprobamos que el paquete sea valido
      int packetSize = UDP.parsePacket();
      if (packetSize)
      {
        Serial.println("");
        Serial.print("Received packet of size :");
        Serial.println(packetSize);
        Serial.print("From :");
        IPAddress remote = UDP.remoteIP();
        for (int i = 0; i < 4; i++)
        {
          Serial.print(remote[i], DEC);
          if (i < 3)
          {
            Serial.print(".");
          }
        }
        Serial.print(", port :");
        Serial.println(UDP.remotePort());
        // leemos el mensaje
        UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Contents:");


        for (int i = 0; i < packetSize; i++)
        {
          Serial.print(packetBuffer[i]);
        }
        Serial.println();     
        
        if( !(strcmp(packetBuffer,pir_msg)) )
        {
          pir = true;
          Serial.println("He recibido PIR");
          pir_timeout = millis();    
        }
        
        if( !(strcmp(packetBuffer,tim_msg)) )
        {
          timbre = true;
          Serial.println("He recibido TIMBRE");
          tim_timeout = millis();
                        
        }
        // Respondemos OK
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        UDP.write(ReplyBuffer);
        UDP.endPacket();
        
      }
      delay(10);
    }
    // esto es mejor que este fuera, para que se pierda el menor tiempo posible en la comm
    if (timbre)
    {

      tim_timeout_prev = millis();
      if( tim_timeout_prev - tim_timeout < 3000)
      {
        for (int i = 0; i < 14; i++)
          writeArduinoOnMatrix();
        lc.clearDisplay(0);
        pir = false;
        timbre = false;
      }
      else
      {
        Serial.println("TIMEOUT TIMBRE");
        timbre = false;
      }
      lc.clearDisplay(0);
      delay(10);
    }
    if (pir)
    {
      pir_timeout_prev = millis();      
      if(pir_timeout_prev - pir_timeout > TIMEOUT_PIR)
      {
        Serial.println("TIMEOUT PIR");
        pir = false;
      }
        
    }
    lc.clearDisplay(0);
    delay(20);
        


  }
} // fin void loop 
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
void writeArduinoOnMatrix() {
  /* animaciones del timbre*/
  // para el timbre de fuera ------------------>
  byte timbre_a[8] = {
    B00011000,
    B00100100,
    B00100100,
    B00100100,
    B01000010,
    B11111111,
    B00001100,
    B00001110
  };
  byte timbre_b[8] = {
    B00011000,
    B00100100,
    B00100100,
    B00100100,
    B01000010,
    B11111111,
    B00011000,
    B00111100
  };
  byte timbre_c[8] = {
    B00011000,
    B00100100,
    B00100100,
    B00100100,
    B01000010,
    B11111111,
    B00110000,
    B01110000
  };
  //---------------------------->
  // para el timbre de dentro ------------->
  byte timbre_d[8] = {
    B00011000,
    B00111100,
    B00111100,
    B00111100,
    B01111110,
    B11111111,
    B00001100,
    B00001100
  };
  byte timbre_e[8] = {
    B00011000,
    B00111100,
    B00111100,
    B00111100,
    B01111110,
    B11111111,
    B00011000,
    B00011000
  };
  byte timbre_f[8] = {
    B00011000,
    B00111100,
    B00111100,
    B00111100,
    B01111110,
    B11111111,
    B00110000,
    B00110000
  };
  // ------------------------------_>

  


  if(pir)
  {  
    Serial.println("TIMBRE CABINA");
    for ( int i = 0 ; i < 8; i++)
      lc.setRow(0, i, timbre_a[i]);
    //tone(BUZZ,440); // para BUZZ pasivo
    digitalWrite(BUZZ, HIGH);// para BUZZ activo
    delay(delaytime / 4);

    for ( int i = 0 ; i < 8; i++)
      lc.setRow(0, i, timbre_b[i]);
    delay(delaytime / 8);

    for ( int i = 0 ; i < 8; i++)
      lc.setRow(0, i, timbre_c[i]);
    //tone(BUZZ,349);// para BUZZ pasivo
    digitalWrite(BUZZ, LOW);// para BUZZ activo
    delay(delaytime / 4);

    for ( int i = 0 ; i < 8; i++)
      lc.setRow(0, i, timbre_b[i]);
    delay(delaytime / 8);
  }
  else
  {
    Serial.println("TIMBRE CALLE");
    for ( int i = 0 ; i < 8; i++)
      lc.setRow(0, i, timbre_d[i]);
    //tone(BUZZ,440); // para BUZZ pasivo
    digitalWrite(BUZZ, HIGH);// para BUZZ activo
    delay(delaytime / 4);

    for ( int i = 0 ; i < 8; i++)
      lc.setRow(0, i, timbre_e[i]);
    delay(delaytime / 8);

    for ( int i = 0 ; i < 8; i++)
      lc.setRow(0, i, timbre_f[i]);
    //tone(BUZZ,349);// para BUZZ pasivo
    digitalWrite(BUZZ, LOW);// para BUZZ activo
    delay(delaytime / 4);

    for ( int i = 0 ; i < 8; i++)
      lc.setRow(0, i, timbre_e[i]);
    delay(delaytime / 8);
  }




}

