# Vasseur Pierre-Adrien - Master M1 Info

There is also a README in the github repository dedicated to the WaterBnB web app.

## Some notes about the project in this MQTT part

- Each features has been separated in different files, so it's easier to add new features and understand the code.
- You can choose in the `regul.ino` file if you want to use HTTP or MQTT to transfer data (`#define USE_MQTT and #define USE_HTTP`).
- The MQTT client is connected to 2 topics, one to receive pools data and another to receive order from the web app.

## Node Red

The Node Red flow is in the `flow.json` file. You can import it in your Node Red instance.
