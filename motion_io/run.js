var mio = require('./build/Release/motion_io');

var accel = new mio.MotionIO(null, function(data) {
    console.log('[HIT{ intensity: ' + data + '}]')
})

console.log(mio)
console.log(accel)
