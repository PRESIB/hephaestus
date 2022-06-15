#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define ONBOARD_LED 2
#define NET_COMMAND_START_SERVICE 's'
#define NET_COMMAND_READY 'r'
#define NET_COMMAND_MOVE 'm'
#define NET_COMMAND_END_SERVICE 'e'

#define DEVICE_COMMAND_START_SERVICE 's'
#define DEVICE_COMMAND_READY "ready"
#define DEVICE_COMMAND_MOVE 'm'
#define DEVICE_COMMAND_END_SERVICE 'e'

const String HOLON_ID = "rht-001";

// WiFi
const char *ssid = "MEO-9ABB30";     // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "4ff01eb05e"; // The password of the Wi-Fi network

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const char *netTopic = String("com/nfriacowboy/presib/holon/mqtt/net/" + HOLON_ID).c_str();
const char *deviceTopic = String("com/nfriacowboy/presib/holon/mqtt/device/" + HOLON_ID).c_str();
const char *systemTopic = "com/nfriacowboy/presib/hermes/management/system";

const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void publish(char *message)
{
  mqttClient.publish(netTopic, message);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

  if (strcmp(topic, deviceTopic) == 0)
  {
    char message[length + 1];
    memcpy(message, payload, length);

    digitalWrite(ONBOARD_LED, HIGH);

    Serial.println(message);
    Serial.println("-----------------------");

    digitalWrite(ONBOARD_LED, LOW);

    if (DEVICE_COMMAND_READY == message)
    {
      digitalWrite(ONBOARD_LED, HIGH);
      Serial.print("ready command received");
      delay(500);
      digitalWrite(ONBOARD_LED, LOW);
    }
  }
}

void setupMqqtClient()
{
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(callback);
  while (!mqttClient.connected())
  {
    String client_id = "rh::";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (mqttClient.connect(client_id.c_str()))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      digitalWrite(ONBOARD_LED, HIGH);
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
      digitalWrite(ONBOARD_LED, LOW);
    }
  }

  // publish and subscribe
  mqttClient.publish(systemTopic, String("Hi from " + HOLON_ID).c_str());
  mqttClient.subscribe(deviceTopic);
}

void setup()
{
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, HIGH);
  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  digitalWrite(ONBOARD_LED, LOW);

  WiFi.begin(ssid, password); // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    digitalWrite(ONBOARD_LED, HIGH);
    delay(500);
    digitalWrite(ONBOARD_LED, LOW);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer

  setupMqqtClient();
}

void loop()
{
  mqttClient.loop();
}
