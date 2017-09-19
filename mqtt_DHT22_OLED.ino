#include "DHT.h"
#include <Wire.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
//#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include "images.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

bool light;
const char* ssid = "EE-se6k4x";
const char* password = "photoshop2020";
char* inTopic = "landing/light";
char* topic = "landing/status";
char* server = "192.168.1.75";
char* hellotopic = "hello_topic";
char message_buff[100];
void callback(char* topic, byte* payload, unsigned int length);
#define DHTPIN 14     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define REPORT_INTERVAL 30 // in sec
String clientName;
unsigned long previousTimer = -1;
DHT dht(DHTPIN, DHTTYPE, 15);
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback , wifiClient);
Adafruit_SSD1306 display(0x3c);
int led0Pin = 16;
float t;
float h;
float oldH ;
float oldT ;
void drawTextAlignmentDemo();
void setup() {

  display.begin(0x3C);
  display.display();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 13);
  display.print("MQTT NODE");
  display.display();
  delay(3000);

  client.setServer(server, 1883);
  client.setCallback(callback);
  client.subscribe(inTopic);

  Serial.begin(38400);
  Serial.println("DHTxx test!");
  delay(20);
  Serial.println("Subscribed to ");
  Serial.print(inTopic);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  display.clearDisplay();
  display.setCursor(2, 2);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    display.setTextSize(1);
    display.print(".");
    display.display();

  }
  //display.clearDisplay();

  Serial.println("");
  Serial.println("WiFi UP!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  clientName += "esp8266-landing";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);

  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic is: ");
    Serial.println(topic);
    display.clearDisplay();
    display.setCursor(2, 2);
    display.println("WiFi connected");
    display.setCursor(2, 14);
    display.println(WiFi.localIP());
    display.setCursor(2, 25);
    display.println("T:");
    display.setCursor(58, 25);
    display.println("H:");
    display.setCursor(100, 15);
    // display.drawBitmap(100, 20,wifi_logo1,20,20 ,WHITE);
    display.display();

    if (client.publish(hellotopic, "hello from ESP8266")) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }

  dht.begin();
  oldH = -1;
  oldT = -1;
}

void loop() {
  unsigned long timer = millis();



  if (!client.connected()) {
    reconnect();
  }

  client.subscribe(inTopic);
  delay(10);


  if (timer - previousTimer >= 60000) {




    sendTemperature();
    previousTimer = timer;
  }

  client.loop();
  delay(100);

}

void sendTemperature() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hi = dht.computeHeatIndex(f, h);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");

  String payload = "{\"Humidity\":";
  payload += h;
  payload += ",\"Temperature\":";
  payload += t;
  payload += "}";

  if (t != oldT || h != oldH )
  {

    oldT = t;
    oldH = h;

  }
  display.setTextColor(WHITE, BLACK);
  display.setCursor(22, 25);
  display.println(t);
  display.setCursor(73, 25);
  display.println(h);
  display.display();

  //int cnt = REPORT_INTERVAL;

  // while (cnt--)
  //  delay(1000);
  if (!client.connected()) {
    // reconnect();
    if (client.connect((char*) clientName.c_str())) {
      Serial.println("Connected to MQTT broker again");
      Serial.print("Topic is: ");
      Serial.println(topic);
    }
    else {
      Serial.println("MQTT connect failed");
      Serial.println("Will reset and try again...");
      abort();
    }
  }

  if (client.connected()) {
    Serial.print("Sending payload: ");
    Serial.println(payload);

    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  }
 display.clearDisplay();
  display.setCursor(2, 2);
  display.println("WiFi UP!");
  display.setCursor(2, 14);
  display.println(WiFi.localIP());
  display.setCursor(2, 25);
  display.println("T:");
  display.setCursor(22, 25);
  display.println(t);
  display.setCursor(58, 25);
  display.println("H:");
  display.setCursor(73, 25);
  display.println(h);
  if(light == true){
    display.setCursor(10,100);
      display.println("ON");
       display.display();
  }
  else if(light == false){
      display.setCursor(10,100);
      display.println("OFF");
       display.display();
  }
  display.display();
}


void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived

  int state = digitalRead(led0Pin);
  String topicStr = topic;

  Serial.println("Callback Update");
  Serial.println("Topic: ");
  Serial.println(topicStr);
  Serial.println(payload[0]);

  if (topicStr == "landing/light") {
    if (payload[0] == 49) {
      if (state == 0) {
        Serial.println("new status");
        digitalWrite(led0Pin, HIGH);
       bool light =true;
      }
      if (state == 1) {
        digitalWrite(led0Pin, LOW);
         bool light =false;

      }

    }

  }
  testscrolltext();
  delay(2000);
  display.clearDisplay();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      // client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void testscrolltext(void) {
  //display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.clearDisplay();
  display.println("Light Changed: ");
  display.print( light );
  display.display();
  delay(1);

  display.startscrollright(0x00, 0x0F);
  delay(8500);
  display.stopscroll();
 // delay(3000);

 
  /*
    display.startscrollleft(0x00, 0x0F);
    delay(2000);
    display.stopscroll();
    delay(1000);
    display.startscrolldiagright(0x00, 0x07);
    delay(2000);
    display.startscrolldiagleft(0x00, 0x07);
    delay(2000);
    display.stopscroll();
  */
}
