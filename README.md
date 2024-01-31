# WaterBnb_22012379

## Arduino code and Node-Red flow

- The Arduino code and the Node-Red flow are in the `adhoc` folder.
- The main Arduino code is in the `adhoc/regul.ino` file.
- The Node-Red flow is in the `adhoc/flows.json` file.
- There is a specific README.md file in the `adhoc` folder for more information about the Arduino code and the Node-Red flow.

## What is interesting about this project?

- The architecture of the boilerplate template has been rework to be more modular and scalable.
  - Each kind of feature has its own folder and files.
- The web app has a dashboard page on the root path. (http://217.160.51.173:5000)
  - The dashboard page contains some statistics about the system. (Numbers and a chart of the pools requests) (http://217.160.51.173:5000)
  - I uses the Flask template injection system to pass the data from the server to the HTML and then uses the Chart.js library to display the chart.
- I added a route `/close` to close the pool after being used. It can be called with the same arguments as the `/open` route and it will re-open the pool for other users.


## About the MQTT publish problem on Render

The problem is that I did not find a way to publish on the MQTT channel from the Render server.
The subcription and the receiving of the message works fine but the publish does not work.
On my local environment and a personal server hosted on Ionos, the publish works fine, so I think Render is blocking the protocol behind the MQTT publish (maybe for security reasons).

So I put this project on a personal server hosted on Ionos. My Node-Red is set-up to work by default with this personal environment. (http://217.160.51.173:5000)

## Author

Made by Vasseur Pierre-Adrien