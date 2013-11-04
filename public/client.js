socket = io.connect()

socket.on('source add', function(source) {
    console.log('adding source: ', source)
})

socket.on('random event', function(val) {
    console.log('random event: ', val)
})