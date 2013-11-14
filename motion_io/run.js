var mio = require('./build/Release/motion_io');

var accel = new mio.MotionIO('0xTEST', function(data) {
    console.log('(got data)=> ' + data)
})

console.log(mio)
console.log(accel)
