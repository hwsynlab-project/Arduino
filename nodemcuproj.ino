#include <MicroGear.h>
#include <ESP8266WiFi.h>

#define D0 16  
#define ledPin  D0
#define LINE_TOKEN "Mf27ufrAkpM6FUr6wcdkkBEeTbsljzPhuDxoCfqr3Ay"       

const char* ssid     = "McDonan";
const char* password = "mcdonan2539";
String message = "ttttt";

#define APPID   "HWSynLabProjectTest01"
#define KEY     "h0gScFIU1MJSuyC"
#define SECRET  "mhlmt2GThQWWH6dUAzov0i2el"

#define ALIAS   "NodeMCU1"
#define TargetWeb "frontpage"
boolean flag=true;

WiFiClient client;
MicroGear microgear(client);

void Line_Notify(String message) {
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;   
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  // Serial.println(req);
  client.print(req);
    
  delay(20);

  // Serial.println("-------------");
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-------------");
}

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++) 
  {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();

  String stateStr = String(strState).substring(0, msglen);

  if(stateStr == "ON") 
  {
    digitalWrite(ledPin, LOW);
    flag=true;
  } 
  else if (stateStr == "OFF") 
  {
    digitalWrite(ledPin, HIGH);
    flag=false;
  }
}
void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    digitalWrite(ledPin,LOW);
}
void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    digitalWrite(ledPin,HIGH);
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
  digitalWrite(ledPin,HIGH);
}

void setup() 
{
    /* Event listener */
    microgear.on(CONNECTED,onConnected);
    microgear.on(MESSAGE,onMsghandler);

    Serial.begin(115200);
    Serial.println("Starting...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
       delay(250);
       
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);

    pinMode(ledPin,OUTPUT);
    
}

void loop() 
{
  if(microgear.connected()) 
  {
    microgear.loop();
    
  } 
  else 
  {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  if (Serial.available()>0)
  {
    
    Serial.println(Serial.read());
    if(flag){
      if (Serial.read()==1){
        microgear.chat(TargetWeb,"alert");
        Line_Notify("alert");
        while(Serial.read()==1){
          
        }
      }
    }
  } 
  else 
  {
    Serial.println("No data buffer");
  }
  delay(100);
}
