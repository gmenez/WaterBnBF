from flask import jsonify, request
from flask import render_template
import datetime
from bson.json_util import dumps, loads

TOPIC_POOL_PRIVATE = "uca/iot/piscine/22012379"

def initRoutes(app, dbClient, mqttClient):
    @app.route('/')
    def helloWorld():
      poolsCursor = dbClient.WaterBnb.pools.find()
      pools = dumps(list(poolsCursor))
      return render_template('index.html', pools=pools)
    
    @app.route('/open', methods=['GET', 'POST'])
    def openTheDoor():
      idu = request.args.get('clientid') # idu : clientid of the service
      idswp = request.args.get('ident')  #idswp : id of the swimming pool

      print("clientid : " + idu)
      print("ident pool : " + idswp)

      # Check user existence
      if dbClient.WaterBnb.users.find_one({"num": idu}) == None:
          return jsonify({'idu': idu, 'idswp': idswp, "granted" : "NO"}), 404

      # Check pool existence
      if dbClient.WaterBnb.pools.find_one({"ident": idswp}) == None:
          return jsonify({'idu': idu, 'idswp': idswp, "granted" : "NO"}), 404

      # Check if the pool is already occuped
      if dbClient.WaterBnb.pools.find_one({"ident": idswp})["occuped"] == True:
          dbClient.WaterBnb.pools.update_one({"ident": idswp}, {"$push": {"request": {"idu": idu, "time": datetime.datetime.now(), "access": "denied"}}}, upsert=True)
          mqttClient.publish(TOPIC_POOL_PRIVATE, '{"piscine":{"occuped":true, "access":"denied"}, "info": {"ident": "' + idswp + '"}}', qos=2)
          return jsonify({'idu': idu, 'idswp': idswp, "granted" : "NO"}), 200

      # Set the pool as occuped
      mqttClient.publish(TOPIC_POOL_PRIVATE, '{"piscine":{"occuped":true, "access":"granted"}, "info": {"ident": "' + idswp + '"}}', qos=2)
      dbClient.WaterBnb.pools.update_one({"ident": idswp}, {"$set": {"occuped": True}, "$push": {"request": {"idu": idu, "time": datetime.datetime.now(), "access": "granted"}}}, upsert=True)

      return jsonify({'idu': idu, 'idswp': idswp, "granted" : "YES"}), 200

    @app.route('/close', methods=['GET', 'POST'])
    def closeTheDoor():
      idu = request.args.get('clientid') # idu : clientid of the service
      idswp = request.args.get('ident')  #idswp : id of the swimming pool

      # Check user existence
      if dbClient.WaterBnb.users.find_one({"num": idu}) == None:
          return jsonify({'idu': idu, 'idswp': idswp}), 404

      # Check pool existence
      if dbClient.WaterBnb.pools.find_one({"ident": idswp}) == None:
          return jsonify({'idu': idu, 'idswp': idswp}), 404

      mqttClient.publish(TOPIC_POOL_PRIVATE, '{"piscine":{"occuped":false, "access":"granted"}, "info": {"ident": "' + idswp + '"}}', qos=2)
      dbClient.WaterBnb.pools.update_one({"ident": idswp}, {"$set": {"occuped": False}}, upsert=True)
      return jsonify({'idu': idu, 'idswp': idswp, "open": True}), 200

    @app.route('/users')
    def listUsers():
      todos = dbClient.WaterBnb.users.find()
      return jsonify([todo['name'] for todo in todos])

    @app.route('/publish', methods=['POST'])
    def publishMessage():
      request_data = request.get_json()
      publish_result = mqttClient.publish(request_data['topic'], request_data['msg'], qos=2)
      return jsonify({'code': publish_result[0]})
