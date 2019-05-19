
/*
  Copyright (c) 2019 Ashari Juang. All right reserved.

  QiscusBot - Library to create your own Qiscus Bot using 
  ESP8266 on Arduino IDE.
  Ref. Library at https:github/esp8266/Arduino
  */

#include "ESP8266QiscusBOT.h"

QiscusBOT::QiscusBOT(String token, String appId, String userId)	{
	user.token = token;
  user.appId = appId;
  user.userId = userId;
}

void QiscusBOT::begin(void)	{
    _lastMessageId="0";
}

/**************************************************************************************************
 * function to achieve connection to https://api.qiscus.com and send command to qiscus                *
 * (Argument to pass: Method and URL to address to Qiscus)                                                 *
 **************************************************************************************************/

String QiscusBOT::connectToServer(String method, String path, String param) {
  String mess="";
  String url = _baseUrl + path;
  if (method == "GET") {
    url = url + param;
  }
  Serial.println("[HTTP] Request "+ method + ": " +url);
  if (http.begin(url)) {
    http.addHeader("QISCUS_SDK_USER_ID", user.userId);
    http.addHeader("QISCUS_SDK_APP_ID", user.appId);
    http.addHeader("QISCUS_SDK_TOKEN", user.token);
    http.addHeader("QISCUS_SDK_VERSION", "WEB_2.8.11-beta1");
    http.addHeader("Content-Type","application/json");

    // start connection and send HTTP header
    int httpCode;
    if (method == "POST") {
      Serial.println("Param: "+param);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      httpCode = http.POST(param);
    } else {
      httpCode = http.GET();
    }
    
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] ... code: %d\n", httpCode);
      String payload = http.getString();
      Serial.println(payload);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        mess=payload;
      }
    } else {
      Serial.printf("[HTTP] failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
      Serial.printf("[HTTP] Unable to connect\n");
  }
  return mess;
} 

/***************************************************************
 * Sync - function to receive last messages from qiscus *
 ***************************************************************/
void QiscusBOT::sync()  {
  Serial.println("Sync Messages ");
  String path="/api/v2/sdk/sync";
  String param="?token="+user.token+"&last_received_comment_id="+_lastMessageId+"&limit=1";
  String mess=connectToServer("GET", path, param);

  if (mess=="") {     
    Serial.println("failed to sync");
    return;
  }

  // const size_t capacity = JSON_ARRAY_SIZE(2) + 4*JSON_OBJECT_SIZE(0) + 4*JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(29) + 2048;
  // DynamicJsonDocument doc(capacity);
  StaticJsonDocument<2048> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, mess);

  // Test if parsing succeeds.
  if (error) {
    Serial.println(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    // return;
  }

  JsonArray comments = doc["results"]["comments"];
  for(JsonObject comment : comments){
    long messageID = comment["id"];
    const char* message = comment["message"];
    const char* roomId = comment["room_id_str"];
    const char* sender = comment["username"];
    const char* email = comment["email"];

    Message m;
    m.roomId = roomId;
    m.sender = sender;
    m.message = message;
    m.email = email;

    if (user.userId != email) {
      analizeMessage(m);  
    }
    _lastMessageId=String(messageID);
  }
}

bool QiscusBOT::postMessage(Message m) {
  bool result = false;
  Serial.println("Post Messages ");
  String path="/api/v2/sdk/post_comment";
  String param="topic_id="+ m.roomId +"&type=text&unique_temp_id=bot"+String(random(100,999))+String(random(1,99999))+"&token="+user.token+"&comment="+m.message;
  String mess=connectToServer("POST", path, param);

  if (mess=="") {     
    Serial.println("failed to send Message");
    return result;
  }

  Serial.println("Post Message: "+m.message);
  return result;
}

/******************************************************************************
 * AnalizeMessage - function to detect message and analize behaviour *
 ******************************************************************************/
void QiscusBOT::analizeMessage(Message m) {
  Serial.println("AnalizeMessage: "+ m.message);

  if (m.message.indexOf("@juang") >=0) {
    String hello = "hello "+ m.sender + ". My name is hijuju, i am juang assistant.";
    m.message = hello;
    postMessage(m);
    String hijuju = "Please mention me @hijuju if you want to talk with me.";
    m.message = hijuju;
    postMessage(m);
  }

  if (m.message.indexOf("@hijuju") >=0) {
    String hello = "hi "+ m.sender +", nice to see u. i'm still learning right now.";
    Serial.println(hello);
    m.message = hello;
    postMessage(m);
  }
}

