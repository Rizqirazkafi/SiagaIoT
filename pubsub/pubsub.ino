// #include <ESP8266WiFi.h>
// #include <WiFiClient.h>
// #include <Arduino.h>
// #include <WiFiClientSecureBearSSL.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>
#include <DHT.h>
#include <SafeString.h>
#define resetPin D0
#define firePin D1
#define DHT11PIN D2
#define MQ2pin A0
int buzzer = D4;
float smokeSensor;
DHT dht11(DHT11PIN, DHT11);
int resetPinState = 0;
void connectWifi();
void connect_mqtt();
void saveConfigCallback ();
String mqttBroker = "test.mosquitto.org";
String randomTemp();
WiFiClient client;
PubSubClient mqtt(client);
String wifiname = WiFi.SSID().c_str();
String pub_temp = wifiname + "/temp";
String pub_humidity = wifiname + "/humidity";
String pub_fireAlert = wifiname + "/fire";
String pub_smokeValue = wifiname + "/smoke";
char username[20];
// char userpass[20];
char room[50];
void setup() {
  Serial.begin(9600);
  dht11.begin();
  pinMode(resetPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(firePin, INPUT);
  connectWifi();
  mqtt.setServer(mqttBroker.c_str(), 1883);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  wifiname.replace(" ","-");
  resetPinState = digitalRead(resetPin);
  int flame_state = digitalRead(firePin);
  smokeSensor = analogRead(MQ2pin);
  Serial.println(flame_state);
  float humi = dht11.readHumidity();
  float tempC = dht11.readTemperature();
  char humiString[10];
  char tempCString[10];
  char smokeChar[10];
  dtostrf(humi, 4, 2, humiString);
  dtostrf(tempC, 4, 2, tempCString);
  dtostrf(smokeSensor, 4, 2, smokeChar);
  Serial.print("Humidity ");
  Serial.print(humi);
  Serial.print("%");
  Serial.print("\n");
  Serial.print("Temperature ");
  Serial.print(tempC);
  Serial.print("C");
  Serial.print("\n");
  Serial.print("Smoke ");
  Serial.print(smokeSensor);
  Serial.println("\n");


  if (resetPinState == HIGH){
    Serial.println("Resetting WiFiManager config....");
    WiFiManager wm;
    wm.resetSettings();
    delay(2000);
    ESP.reset();
  }

  if(!mqtt.connected()){
    connect_mqtt();
    Serial.println("MQTT Connected");
    mqtt.publish(wifiname.c_str(), "ESP8266 Online!");
  }
  mqtt.loop();
  mqtt.publish(pub_temp.c_str(), tempCString);
  mqtt.publish(pub_humidity.c_str(), humiString);
  mqtt.publish(pub_smokeValue.c_str(), smokeChar);
  Serial.println(tempCString);
  Serial.println(humiString);
  Serial.println(smokeChar);
  Serial.println(wifiname.c_str());
  if (flame_state == 0){
    mqtt.publish(pub_fireAlert.c_str(), "Fire");
    Serial.println("Fire");
    delay(250);
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
    delay(250);
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
    delay(250);
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
  } else {
    mqtt.publish(pub_fireAlert.c_str(), "No Fire");
    Serial.println("No Fire");
    digitalWrite(buzzer, LOW);
  }
  delay(3000);
}
String randomTemp(){
  int randTemp = random(20,40);
  Serial.println(randTemp);
  return String(randTemp);

}
void connect_mqtt(){
  while(!mqtt.connected()){
    Serial.println("Connecting MQTT ...");
    if(mqtt.connect("esp8266SiagaDK1")){
      mqtt.subscribe("esp8266SiagaDK1/temphum");
    }
  }
}
void connectWifi() {
  WiFi.hostname("siagadk1");
  // WiFiManagerParameter add_username("name", "username", username, 20);
  // WiFiManagerParameter add_userpass("pass", "password", userpass, 20);
  // WiFiManagerParameter add_room("room", "room placement", room, 50);
  WiFiManager wm;
  // wm.setSaveConfigCallback(saveConfigCallback);
  // wm.addParameter(&add_username);
  // wm.addParameter(&add_userpass);
  // wm.addParameter(&add_room);
  // wm.resetSettings();
  // wm.setSaveConfigCallback(saveConfigCallback);
  // WiFiManagerParameter email("email", "Email Address", email, 50);
  // wm.addParameter(&email);
  bool res;
  res = wm.autoConnect("SIAGADK1", "merdeka45hebat");

  if (!res) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("Connected");
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.RSSI());
    Serial.println(WiFi.macAddress());
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.gatewayIP());
  }
  // strcpy(username, add_username.getValue());
  // strcpy(userpass, add_userpass.getValue());
  // strcpy(room, add_room.getValue());
  // Serial.println("Value");
  // Serial.println("\tusername: " + String(username));
  // Serial.println("\tuserpass: " + String(userpass));
  // Serial.println("\troom: " + String(room));
}
// void saveConfigCallback() {
//   Serial.println("Should save config");
//   shouldSaveConfig = true;
// }
