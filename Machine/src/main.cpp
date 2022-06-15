#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const char *topic = "esp32/test";
const int mqtt_port = 1883;

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void setupMqqtClient()
{
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(callback);
  while (!mqttClient.connected())
  {
    String client_id = "esp32-client-";
    client_id += String(ethClient.remoteIP());
    Serial.println("The client " + client_id + " connects to the public mqtt broker");
    if (mqttClient.connect(client_id.c_str()))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }

  // publish and subscribe
  mqttClient.publish(topic, "Hi EMQX I'm ESP32 ^^");
  mqttClient.subscribe(topic);
}

void setup()
{

  Serial.begin(9600); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  setupMqqtClient();
}

void loop()
{
  mqttClient.loop();
}