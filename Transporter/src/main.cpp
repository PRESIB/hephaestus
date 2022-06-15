#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define ONBOARD_LED 2
char *NET_COMMAND_START_SERVICE = (char *)"s";
char *NET_COMMAND_READY = (char *)"r";
char *NET_COMMAND_MOVE = (char *)"m";
char *NET_COMMAND_END_SERVICE = (char *)"e";

#define DEVICE_COMMAND_START_SERVICE "start_service"
#define DEVICE_COMMAND_READY "ready"
#define DEVICE_COMMAND_END_SERVICE "end_service"

const char *HOLON_ID = "rht-001";

// WiFi
const char *ssid = "MEO-9ABB30";     // The SSID (name) of the Wi-Fi network you want to connect to
const char *password = "4ff01eb05e"; // The password of the Wi-Fi network

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const char *netTopic = "com/nfriacowboy/presib/holon/mqtt/net/";
const char *deviceTopic = "com/nfriacowboy/presib/holon/mqtt/device/";
const char *systemTopic = "com/nfriacowboy/presib/hermes/management/system";

const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void publish(char *message)
{
  digitalWrite(ONBOARD_LED, HIGH);
  int tl = strlen(netTopic) + strlen(HOLON_ID);
  char topic[tl + 1];
  strcpy(topic, netTopic);
  strcat(topic, HOLON_ID);

  if (mqttClient.publish(topic, message))
  {
    Serial.println("message sent");
  }
  else
  {
    Serial.println("message not sent");
  }

  digitalWrite(ONBOARD_LED, LOW);
}

void callback(char *topic, byte *payload, unsigned int length)
{

  if (strcmp(topic, deviceTopic) == 0)
  {

    digitalWrite(ONBOARD_LED, HIGH);
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");

    char message[length + 1];

    strncpy(message, (char *)payload, length);
    message[length] = '\0';

    Serial.printf("%s", message);
    Serial.println("-----------------------");

    digitalWrite(ONBOARD_LED, LOW);

    if (strcmp(DEVICE_COMMAND_READY, message) == 0)
    {
      digitalWrite(ONBOARD_LED, HIGH);
      publish(NET_COMMAND_READY);
      digitalWrite(ONBOARD_LED, LOW);
    }
    else if (strcmp(DEVICE_COMMAND_END_SERVICE, message) == 0)
    {
      digitalWrite(ONBOARD_LED, HIGH);
      publish(NET_COMMAND_END_SERVICE);
      digitalWrite(ONBOARD_LED, LOW);
    }
    else if (strcmp(DEVICE_COMMAND_START_SERVICE, message) == 0)
    {
      digitalWrite(ONBOARD_LED, HIGH);
      publish(NET_COMMAND_START_SERVICE);
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

    char client_id[20];
    strcpy(client_id, "rh::");
    strcat(client_id, WiFi.macAddress().c_str());

    if (!mqttClient.connect(client_id))
    {
      digitalWrite(ONBOARD_LED, HIGH);
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
      digitalWrite(ONBOARD_LED, LOW);
    }
  }

  // publish and subscribe
  char message[20] = "Hi from ";
  strcpy(message, "Hi from ");
  strcat(message, HOLON_ID);

  int dl = strlen(deviceTopic);
  char device[dl + 1];
  strcpy(device, deviceTopic);
  strcat(device, HOLON_ID);

  // mqttClient.publish(systemTopic, message);
  //  Serial.printf("Subscribe to %s", device);
  //  mqttClient.subscribe(device);
  //   publish(NET_COMMAND_READY);
}

void setup()
{
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, HIGH);
  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  delay(10);
  digitalWrite(ONBOARD_LED, LOW);

  WiFi.begin(ssid, password); // Connect to the network

  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    digitalWrite(ONBOARD_LED, HIGH);
    delay(500);
    digitalWrite(ONBOARD_LED, LOW);
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
