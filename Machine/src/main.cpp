#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
char *NET_COMMAND_START_SERVICE = (char *)"s";
char *NET_COMMAND_READY = (char *)"r";
char *NET_COMMAND_LOAD = (char *)"l";
char *NET_COMMAND_UNLOAD = (char *)"u";
char *NET_COMMAND_END_SERVICE = (char *)"e";

#define DEVICE_COMMAND_START_SERVICE "start_service"
#define DEVICE_COMMAND_READY "ready"
#define DEVICE_COMMAND_LOAD "load"
#define DEVICE_COMMAND_UNLOAD "unload"

const String HOLON_ID = "rhm-001";

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const char *netTopic = "com/nfriacowboy/presib/holon/mqtt/net/rhm-001";
const char *deviceTopic = "com/nfriacowboy/presib/holon/mqtt/device/rhm-001";
const char *systemTopic = "com/nfriacowboy/presib/hermes/management/system";
const int mqtt_port = 1883;

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

void publish(char *message)
{
  digitalWrite(13, HIGH);
  digitalWrite(12, HIGH);
  Serial.println("sending to: ");
  Serial.println(netTopic);

  if (mqttClient.publish(netTopic, message))
  {
    Serial.println("message sent");
  }
  else
  {
    Serial.println("message not sent");
  }

  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
}

void callback(char *topic, byte *payload, unsigned int length)
{

  if (strcmp(topic, deviceTopic) == 0)
  {

    digitalWrite(13, HIGH);
    digitalWrite(12, HIGH);
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");

    char message[length + 1];

    strncpy(message, (char *)payload, length);
    message[length] = '\0';

    Serial.println(String(message));
    Serial.println("-----------------------");

    digitalWrite(13, LOW);
    digitalWrite(12, LOW);

    if (strcmp(DEVICE_COMMAND_READY, message) == 0)
    {
      digitalWrite(13, HIGH);
      digitalWrite(12, HIGH);
      Serial.println("ready command received");
      Serial.println("sending ready command to net");
      delay(random(100) * 10); // random delay for operation
      publish(NET_COMMAND_READY);
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
    }
    else if (strcmp(DEVICE_COMMAND_START_SERVICE, message) == 0)
    {
      digitalWrite(13, HIGH);
      digitalWrite(12, HIGH);
      Serial.print("start service command received");
      publish(NET_COMMAND_START_SERVICE);
      delay(random(200) * 200); // random delay for operation
      publish(NET_COMMAND_END_SERVICE);
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
    }
    else if (strcmp(DEVICE_COMMAND_UNLOAD, message) == 0)
    {
      digitalWrite(13, HIGH);
      digitalWrite(12, HIGH);
      Serial.print("end service command received");
      delay(random(20) * 10); // random delay for operation
      publish(NET_COMMAND_UNLOAD);
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
    }
    else if (strcmp(DEVICE_COMMAND_LOAD, message) == 0)
    {
      digitalWrite(13, HIGH);
      digitalWrite(12, HIGH);
      Serial.print("start service command received");
      delay(random(20) * 10); // random delay for operation
      publish(NET_COMMAND_LOAD);
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
    }
    else
    {
      Serial.println("Is not ready command");
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
    client_id += String(Ethernet.localIP());
    Serial.println("The client " + client_id + " connects to the public mqtt broker");
    if (mqttClient.connect(client_id.c_str()))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      digitalWrite(13, HIGH);
      digitalWrite(12, HIGH);
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
    }
  }

  // publish and subscribe
  mqttClient.publish(systemTopic, String("Hi from " + HOLON_ID).c_str());
  mqttClient.subscribe(deviceTopic);
}

void setup()
{

  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);

  digitalWrite(13, HIGH);
  digitalWrite(12, HIGH);
  Serial.begin(9600); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while (true)
      ;
  }

  setupMqqtClient();

  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
}

void loop()
{
  mqttClient.loop();
}