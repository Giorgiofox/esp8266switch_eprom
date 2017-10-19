//#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>


#include <ESP8266WiFi.h>
extern "C" {
  #include "user_interface.h"
}


// per usare tip102 su esp8266 mettere una pullup da 2.2k sul gpio per evitare che non faccia il boot
//////////////////////
// WiFi Definitions //
//////////////////////

#include <EEPROM.h>


const char* ssid = "yourwifi";
const char* password = "yourpassword";
char wiFiHostname[ ] = "ESP94";

/// DA FARE: pulsante e interruttore ON OFF
  
/////////////////////
// Pin Definitions //
/////////////////////

const int GPIO2 = 2; 
const int GPIO0 = 0; 


/**
 * 
 * GPIO 0 GPIO0
 * GPIO 1 TX
 * GPIO 2 GPIO2
 * GPIO 3 RX
 * 
 * 
 *   +3.3         TX
 *   ---------------
 *    O   O   O   O
 *    O   O   O   O
 *  ----------------
 *    3   0   2   GND
 *    
 *        ANT
 *    
 *   ---------------- 
 *    
 */





WiFiServer server(80);




void setup() 
{
    Serial.begin(9600);
  EEPROM.begin(512);
    Serial.print("ciao");
  pinMode(GPIO2, OUTPUT);
  
  int state=EEPROM.read(1);
  if (state == 1) {digitalWrite(GPIO2, HIGH);}
  else
  {digitalWrite(GPIO2, LOW);}

  Serial.print(state);
  
  
  connectWiFi();
  server.begin();
  //setupMDNS();
  
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
 // Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/out/1") != -1)
    val = 1; // Will write LED high
  else if (req.indexOf("/out/0") != -1)
    val = 0; // Will write LED low
  else if (req.indexOf("/status") != -1)
    val = -2; // Will print pin reads
  // Otherwise request will be invalid. We'll say as much in HTML


  

  // Set GPIO5 according to the request
  if (val >= 0)
EEPROM.write(1,val);  
digitalWrite(GPIO2, val);
  if (val==1){ 
    delay=500; //aspetto un po'
  digitalWrite(GPIO2, 0); //spengo di nuovo
  val=0;
  }
EEPROM.commit();  

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
 // s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
  //  s += "LED is now ";
    s += (val);// ?"off":"on";
  }
  else if (val == -2)
  
  { 
    s += int(digitalRead(GPIO2));
  }
  else
  
  
  {
    s += "Invalid Request.<br> Try /out/1, /out/0, or /status.";
  }
  
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);

}




void connectWiFi()
{
  

    WiFi.begin(ssid, password);
    wifi_station_set_auto_connect(true);
    wifi_station_set_hostname(wiFiHostname);
    IPAddress ip(192,168,0,94);
    IPAddress gateway(192,168,0,254);
    IPAddress subnet(255,255,255,0);
    WiFi.config(ip, gateway, subnet);


}




