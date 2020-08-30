#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <MicroGear.h>
char ssid[] = "vivo 1904";
char pass[] = "12325812";
#define APPID "smartgpsid"
#define KEY "moY0ToP4IKdm5qd"
#define SECRET "BzYKZNLV1fe0uhPl4J5eQ01nI"
#define ALIAS "alcohol"
#define FEEDID "Alcohollevel"
int alcohol;
long lastalcoholread = 0;
long lastTimeWriteFeed = 0;
WiFiClient client;
MicroGear microgear(client);
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE");
  microgear.setAlias(ALIAS);
}
void setup(){
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.print("."); 
    delay(500);
  }
  Serial.println(""); 
  Serial.println("Wi-Fi connected"); 
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP()); 
  microgear.on(CONNECTED,onConnected);
  microgear.init(KEY,SECRET,ALIAS);
  microgear.connect(APPID);
}
void loop(){
  if (microgear.connected()){
    microgear.loop();
    if (millis() - lastalcoholread > 2000){
      alcohol = analogRead(A0);
      lastalcoholread = millis();
      microgear.writeFeed("Alcohollevel","alcohol:225");
      Serial.print("Alcohol :");Serial.print(alcohol);Serial.print("mg%");
    }
  }
  else{
    Serial.println("Connection lost");
    microgear.connect(APPID);
  }
}
