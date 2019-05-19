/*******************************************************************
 *  this is a basic example how to program a Qiscus Bot            *
 *  using QiscusBOT library on ESP8266                             *                                                                
 *                                                                 *
 *  written by Ashari Juang                                        *
 *******************************************************************/


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266QiscusBOT.h>


// Initialize Wifi connection to the router
// your network SSID (name)
char ssid[] = "*********";
// your network key              
char password[] = "*****";                             

// Initialize Qiscus BOT
#define QiscusToken "*************"
#define QiscusAppID "*************"
#define QiscusUserID "************"
QiscusBOT bot(QiscusToken, QiscusAppID, QiscusUserID);

int Bot_mtbs = 1800; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done


/********************************************
 * EchoMessages - function to Echo messages *
 ********************************************/
void Bot_EchoMessages() {

  for (int i = 1; i < bot.message[0][0].toInt() + 1; i++)      {
    bot.sendMessage(bot.message[i][4], bot.message[i][5], "");
  }
  bot.message[0][0] = "";   // All messages have been replied - reset new messages
}


void setup() {

  Serial.begin(74880);
  delay(3000);
  
  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  bot.begin();      // launch Bot functionalities
}



void loop() {

  if (millis() > Bot_lasttime + Bot_mtbs)  {
    bot.sync();
    Bot_lasttime = millis();
    Serial.println("millis");
    Serial.println(Bot_lasttime);
    Serial.println(bot._lastMessageId);
  }
}




