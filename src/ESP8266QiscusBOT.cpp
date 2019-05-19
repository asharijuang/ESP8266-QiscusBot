
/*
  Copyright (c) 2019 Ashari Juang. All right reserved.

  QiscusBot - Library to create your own Qiscus Bot using 
  ESP8266 on Arduino IDE.
  Ref. Library at https:github/esp8266/Arduino
  */


#include "ESP8266QiscusBOT.h"

QiscusBOT::QiscusBOT(String token, String id, String user)	{
	_token=token;
	_appId=id;
  _userId=user;
}


void QiscusBOT::begin(void)	{
	  message[0][0]="0";   // number of received messages
	  message[1][0]="";    
    message[0][1]="0";  // code of last read Message
    _message="";
    _lastMessageId="0";
}


String QiscusBOT::connectToServer(String command) {
  String mess="";
  // client.connect(HOST, 80);
  // client.println("GET /"+command);
  String url = _baseUrl + command;
  Serial.println("[HTTP] Request: "+url);
  // Serial.println("[HTTP]_userId: "+_userId);
  // Serial.println("[HTTP]_appId: "+_appId);
  // Serial.println("[HTTP]_token: "+_token);
  if (http.begin(url)) {
    http.addHeader("QISCUS_SDK_USER_ID", _userId);
    http.addHeader("QISCUS_SDK_APP_ID", _appId);
    http.addHeader("QISCUS_SDK_TOKEN", _token);
    http.addHeader("QISCUS_SDK_VERSION", "WEB_2.8.11-beta1");
    http.addHeader("Content-Type","application/json");

    Serial.println("[HTTP] GET...");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
        mess=payload;
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
      Serial.printf("[HTTP] Unable to connect\n");
  }
  return mess;
} 

/**************************************************************************************************
 * function to achieve connection to https://api.qiscus.com and send command to qiscus                *
 * (Argument to pass: URL to address to Qiscus)                                                 *
 **************************************************************************************************/
String QiscusBOT::connectToQiscus(String command)  {
    String mess="";
    long now;
    bool avail;
    // Connect with https://api.qiscus.com       
    // IPAddress server(52,74,133,145);
    String server = HOST;
    if (client.connect(server, 80)) {  
        Serial.println(".... connected to server");
        String a="";
        char c;
	      int ch_count=0;
        client.println("GET /"+command);
        client.println();

        now=millis();
        avail=false;
        while (millis()-now<1500) {  
          while (client.available()) {
	           char c = client.read();
              Serial.write(c);
              if (ch_count<700)  {
                 mess=mess+c;
	               ch_count++;
	            }
              avail=true;
          }
          if (avail) {
      		  Serial.println();
      		  Serial.println(mess);
      		  Serial.println();
		        break;
	        }
        }
    } else {
      Serial.println("Failed connect to server"+ server);
    }
    return mess;
}

/***************************************************************
 * Sync - function to receive last messages from qiscus *
 ***************************************************************/
void QiscusBOT::sync()  {
  Serial.println("Sync Messages ");
  String command="/api/v2/sdk/sync?token="+_token+"&last_received_comment_id="+_lastMessageId+"&limit=1";
  String mess=connectToServer(command);

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
    analizeMessage(message);
    Serial.println("Sync Message:");
    Serial.println(messageID);
    Serial.println(message);
    _message=message;
    _lastMessageId=String(messageID);
  }
  
}


/***********************************************************************
 * SendMessage - function to send message to telegram                  *
 * (Arguments to pass: chat_id, text to transmit and markup(optional)) *
 ***********************************************************************/
void QiscusBOT::sendMessage(String chat_id, String text, String reply_markup)  {

    bool sent=false;
   // Serial.println("SEND Message ");
    long sttime=millis();
    if (text!="") {
	    while (millis()<sttime+8000) {    // loop for a while to send the message
		String command="bot"+_token+"/sendMessage?chat_id="+chat_id+"&text="+text+"&reply_markup="+reply_markup;
		String mess=connectToQiscus(command);
		Serial.println(mess);
		int messageLenght=mess.length();
		for (int m=5; m<messageLenght+1; m++)  {
		    if (mess.substring(m-10,m)=="{\"ok\":true")     {  //Chek if message has been properly sent
		        sent=true;
		        break;
		    }
		}
		if (sent==true)   {
		 Serial.print("Message delivred: \"");
		 Serial.print(text);
		 Serial.println("\"");
		 Serial.println();
		  break;
		}
		delay(1000);
		Serial.println("Retry");

	    }
    }
   // if (sent==false) Serial.println("Message not delivered");
}


/******************************************************************************
 * AnalizeMessage - function to detect message and analize behaviour *
 ******************************************************************************/
void QiscusBOT::analizeMessage(String message) {
  Serial.println("AnalizeMessage: "+ message);
  if (message.indexOf("@juang") >=0) {
    Serial.println("hello my name hijuju, i'm juang assistant.");
  }
}

