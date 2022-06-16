const mqtt = require("mqtt");
const client = mqtt.connect("mqtt://broker.hivemq.com", {
  port: 1883,
  protocol: "mqtt",
  clientId: "presib-multi-rh-virtual",
});
console.log("connected flag  " + client.connected);

const DEVICE_COMMAND_START_SERVICE = "start_service";
const DEVICE_COMMAND_READY = "ready";
const DEVICE_COMMAND_LOAD = "load";
const DEVICE_COMMAND_UNLOAD = "unload";

const NET_COMMAND_START_SERVICE = "s";
const NET_COMMAND_READY = "r";
const NET_COMMAND_LOAD = "l";
const NET_COMMAND_UNLOAD = "u";
const NET_COMMAND_END_SERVICE = "e";

//handle incoming messages
client.on("message", (topic, message, packet) => {
  const holonId = topic.split("/").pop();
  const id = holonId.split("-").pop();

  if (id > 1) {
    let result = "";
    console.log("message is " + message.toString());
    console.log("topic is " + topic);
    switch (message.toString()) {
      case DEVICE_COMMAND_READY:
        result = NET_COMMAND_READY;
        break;
      case DEVICE_COMMAND_LOAD:
        result = NET_COMMAND_LOAD;
        break;
      case DEVICE_COMMAND_UNLOAD:
        result = NET_COMMAND_UNLOAD;
        break;
      case DEVICE_COMMAND_START_SERVICE:
        publish(`${netTopic + holonId}`, NET_COMMAND_START_SERVICE);
        result = NET_COMMAND_END_SERVICE;
        break;
    }

    if (result) {
      console.log("result is " + result);
      publish(`${netTopic + holonId}`, result);
    }
  }
});

client.on("connect", () => {
  console.log("connected  " + client.connected);
  publish(systemTopic, "Hi from viltual multi RH");
});
//handle errors
client.on("error", (error) => {
  console.log("Can't connect" + error);
  process.exit(1);
});
//publish
function publish(
  topic,
  msg,
  options = {
    retain: true,
    qos: 0,
  }
) {
  console.log("publishing", msg);

  if (client.connected == true) {
    client.publish(topic, msg, options);
  }
}

//////////////

const deviceTopic = "com/nfriacowboy/presib/holon/mqtt/device/#";
const netTopic = "com/nfriacowboy/presib/holon/mqtt/net/";
const systemTopic = "com/nfriacowboy/presib/hermes/management/system";

console.log("subscribing to topics");
client.subscribe(deviceTopic, { qos: 0 }); //topic list

//notice this is printed even before we connect
console.log("end of script");

/*while (true) {

}*/
