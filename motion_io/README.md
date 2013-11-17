Async IO Controller for Accelerometers

# Build Instructions

1. Install `node-gyp` (`sudo npm install -g node-gyp`)
2. Full clean and compile: `node-gyp clean configure build`

This build was tested under OS X 10.9 and Angstrom Linux for the BeagleBone Black.

# Usage

    var motion_io = require('motion_io')
    
The `motion_io` module exposes a single class `MotionIO` which takes as its constructor and address (currently unused, pass `null` or `''`) and a motion callback `function (intensity) -> void`.

    new motion_io.MotionIO(null, function(intensity) {
        console.log('Hit: ' + intensity)
    })

Motion events begin being dispatched upon object creation.

# Testing

You can run the included `run.js` script after building the module to test the product. If successful, it will simply print motion events to the console.

# Notes

There appears to be a segmentation fault error somewhere in here that I was never able to quite identify. It seems to manifest sporadically, but more often when used in conjunction with the actual server than in the test script. If you happen to know why, I'd love to hear.