var motion_io = require('./build/Release/motion_io');

var accel = new motion_io('0xTEST', function(data) {
    console.log('(got data)=> ' + data)
})

console.log(motion_io)
console.log(accel)