Course project to create a synthetic band using a beagle bone and some accelerometers.

# Installation

1. Refer to `/motion_io/README.md` for how to build the required node.js addon to reading accelerometer data.
2. Install all other dependencies using `npm install`. This will download the [final-fs](https://github.com/finalclass/final-fs) and [socket.io](http://socket.io) libraries required by the server.
3. Run the server on the beaglebone as `node server.js`, this will start the server on port 3001.

# Concept

This project layers several components.

* A [ADXL345](http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf) accelerometer,
* A C++ node.js module to perform polling and data processing,
* A simple node.js server, which uses socket.io to send motion events, and
* A client side app which uses HTML5 WebAudio APIs to play back sound in response to motion events.

In order to actually configure and run this project you will need to duplicate our setup as described on our [project page](http://elinux.org/ECE497_Project_Makeshift_Drums).