from flask import Flask
from flask_mqtt import Mqtt
from database.database import initDatabaseClient, checkDatabase, checkThatCollectionExist, setupUsersCollection
from routes.routes import initRoutes
from mqtt.mqtt import initMqttHandlers


ADMIN = False
app = Flask(__name__)
app.config.from_pyfile('./config/config.py')


# Initialize Database
dbClient = initDatabaseClient(app)
checkDatabase(dbClient)
checkThatCollectionExist(dbClient, "users")
checkThatCollectionExist(dbClient, "pools")
if ADMIN:
    setupUsersCollection(dbClient)

# Initialize MQTT
mqttClient = Mqtt(app)
initMqttHandlers(mqttClient, dbClient)

# Initialize Routes
initRoutes(app, dbClient, mqttClient)

if __name__ == '__main__':
    app.run(debug=False)
