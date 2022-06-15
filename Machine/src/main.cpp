#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <EthernetUdp.h>
#include <Syslog.h>

// Syslog server connection info
#define SYSLOG_SERVER "192.168.1.101"
#define SYSLOG_PORT 514

// This device info
#define DEVICE_HOSTNAME "machine-001"
#define APP_NAME "PRESIB-RESOURCE-HOLON-MACHINE"

// A UDP instance to let us send and receive packets over UDP
EthernetUDP udpClient;
// Create a new empty syslog instance
Syslog syslog(udpClient, SYSLOG_PROTO_IETF);

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const char *topic = "esp32/test";
const int mqtt_port = 1883;

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

void callback(char *topic, byte *payload, unsigned int length)
{
  syslog.log("Message arrived in topic: ");
  syslog.log(LOG_INFO, topic);
  syslog.log("Message:");
  for (int i = 0; i < length; i++)
  {
    digitalWrite(13, HIGH);
    digitalWrite(12, HIGH);
    syslog.log(LOG_INFO, String(payload[i]));
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
  }
  syslog.log(LOG_INFO, "-----------------------");
}

void setupMqqtClient()
{
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(callback);
  while (!mqttClient.connected())
  {
    String client_id = "esp32-client-arduino";
    syslog.log(LOG_INFO, "The client " + client_id + " connects to the public mqtt broker");
    if (mqttClient.connect(client_id.c_str()))
    {
      syslog.log(LOG_INFO, "Public emqx mqtt broker connected");
    }
    else
    {
      syslog.logf(LOG_ERR, "failed with state %s", mqttClient.state());
      delay(2000);
    }
  }

  // publish and subscribe
  syslog.log(LOG_INFO, "Sending message to MQTT");
  mqttClient.publish(topic, "Hi EMQX I'm ESP32 ^^");
  mqttClient.subscribe(topic);
}

void setup()
{

  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);

  digitalWrite(13, HIGH);
  digitalWrite(12, HIGH);
  Serial.begin(9600); // Start the Serial communication to send messages to the computer
  delay(10);

  // prepare syslog configuration here (can be anywhere before first call of
  // log/logf method)
  syslog.server(SYSLOG_SERVER, SYSLOG_PORT);
  syslog.deviceHostname(DEVICE_HOSTNAME);
  syslog.appName(APP_NAME);
  syslog.defaultPriority(LOG_KERN);

  syslog.log(LOG_INFO, "Start mqtt setup");

  setupMqqtClient();

  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
}

void loop()
{
  mqttClient.loop();
}