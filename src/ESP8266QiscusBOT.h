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

class QiscusBOT
{
  public:
    QiscusBOT (String, String, String);
  	String message[3][6];  // amount of messages read per time  (update_id, name_id, name, lastname, chat_id, text)
  	String _lastMessageId;
    void begin(void);
  	void analizeMessages(void);
  	void sendMessage(String chat_id, String text, String reply_markup);
  	void getUpdates(String offset);
    void sync();
	const char* fingerprint = "37:21:36:77:50:57:F3:C9:28:D0:F7:FA:4C:05:35:7F:60:C1:20:44";  //Telegram.org Certificate 

  private:
    String connectToQiscus(String command);
    String connectToServer(String command);
    void analizeMessage(String message);
    String _baseUrl = HOST;
    String _token;
    String _appId;
    String _userId;
    String _message;
    WiFiClientSecure client;
    HTTPClient http;
};

struct Message {
  String message;
  String id;
  String roomId;
  String senderName;
  String senderEmail;
};

#endif
