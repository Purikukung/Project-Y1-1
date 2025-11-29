#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <NewPing.h>

#define MAX_DISTANCE 1000

const int trigPin1 = D1;
const int echoPin1 = D2;
const int trigPin2 = D8;
const int echoPin2 = D7;
const int servoPin = D3;

Servo myServo;
NewPing sonar1(trigPin1, echoPin1, MAX_DISTANCE);
NewPing sonar2(trigPin2, echoPin2, MAX_DISTANCE);

const char* ssid = " ";
const char* password = " ";

const char* mqtt_server1 = "broker.hivemq.com";
const int mqtt_port1 = 1883;
const char* mqtt_Client1 = " ";
const char* mqtt_username1 = "";
const char* mqtt_password1 = "";

WiFiClient espClient;
PubSubClient client1(espClient);
PubSubClient client2(espClient);

long lastMsg = 0;
int value = 0;
char msg[100];
String DataString;
int l1, l2;

void reconnect1() {
  while (!client1.connected()) {
    Serial.print("Attempting MQTT connection Node-Red...");
    if (client1.connect(mqtt_Client1, mqtt_username1, mqtt_password1)) {
      Serial.println("connected to Node-Red");
      client1.subscribe("Marple");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client1.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback1(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived from Node-Red [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    message = message + char(payload[i]);
  }
  Serial.println(message);
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  myServo.attach(servoPin);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client1.setServer(mqtt_server1, mqtt_port1);
  client1.setCallback(callback1);
  client1.subscribe("Marple");
}

void loop() {
  if (!client1.connected()) {
    reconnect1();
  }
  client1.loop();

  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    ++value;

    l1 = sonar1.ping_cm();
    l2 = sonar2.ping_cm();

    DataString = "{\"length1\" : " + (String)l1 + ", \"length2\" : " + (String)l2 + "}";
    DataString.toCharArray(msg, 100);
    Serial.print("Publish message to Node-Red : ");
    Serial.println(msg);
    client1.publish("Marple", msg);

    delay(1);
  }

  if (l1 > 0 and l1 < 5) {
    myServo.write(180);
    delay(5000);
  } else
    myServo.write(0);

  delay(500);

  if (l2 > 0 and l2 < 5) {
    myServo.write(180);
    delay(5000);
  } else
    myServo.write(0);

  delay(500);
}