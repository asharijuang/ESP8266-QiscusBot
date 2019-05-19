/*
  Copyright (c) 2019 Ashari Juang. All right reserved.

  QiscusBot - Library to create your own Qiscus Bot using 
  ESP8266 on Arduino IDE.
  Ref. Library at https:github/esp8266/Arduino
  */

#ifndef ESP8266QiscusBOT_h
#define ESP8266QiscusBOT_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#define HOST "http://api.qiscus.com"
#define SSL_PORT 80
#define HANDLE_MESSAGES 1

struct Message {
    String message;
    long id;
    String roomId;
    String sender;
    String email;
};

struct User {
    String token;
    String appId;
    String userId;
};

class QiscusBOT
{
  public:
    QiscusBOT (String, String, String);
    void begin(void);
  	void getUpdates(String offset);
    bool postMessage(Message m);
    void sync();
	// const char* fingerprint = "37:21:36:77:50:57:F3:C9:28:D0:F7:FA:4C:05:35:7F:60:C1:20:44";  //Telegram.org Certificate 

  private:
    String connectToQiscus(String command);
    String connectToServer(String method, String path, String param);
    void analizeMessage(Message m);
    User user;
    String _lastMessageId;
    String _baseUrl = HOST;
    WiFiClientSecure client;
    HTTPClient http;
};

#endif
