import json

TOPIC_POOL_GLOBAL = "uca/iot/piscine"

def initMqttHandlers(mqttClient, dbClient):
    @mqttClient.on_connect()
    def handle_connect(client, userdata, flags, rc):
      if rc == 0:
          print('Connected successfully')
          mqttClient.subscribe(TOPIC_POOL_GLOBAL)
      else:
          print('Bad connection. Code:', rc)

    @mqttClient.on_log()
    def handle_logging(client, userdata, level, buf):
      if level == mqttClient.MQTT_LOG_ERR:
        print('MQTT ERR: {}'.format(buf))

    @mqttClient.on_message()
    def handle_mqtt_message(client, userdata, msg):
      topic = msg.topic

      if (topic == TOPIC_POOL_GLOBAL):
        try:
          data = dict(topic=msg.topic, payload=msg.payload.decode())
          dic = json.loads(data["payload"])
          who = dic["info"]["ident"]
          hotspot = dic["piscine"]["hotspot"]
          occuped = dic["piscine"]["occuped"]
          t = dic["status"]["temperature"]
          poolsCollection = dbClient.WaterBnb.pools
          poolsCollection.update_one({"ident": who}, {"$set": {"hotspot": hotspot, "occuped": occuped, "temperature": t}}, upsert=True)
          return
        except Exception as e:
          print("Error while handling the message: " + str(e))
