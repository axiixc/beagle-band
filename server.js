const CONFIG_DEFAULT_INDEX = '/index.html'
const CONFIG_SERVER_PORT = 3001
const CONFIG_PUBLIC_DIRECTORY = __dirname + '/public/'
const CONFIG_SOUNDS_DIRECTORY = __dirname + '/public/sounds/'

var fs = require('final-fs'),
    url = require('url'),
    path = require('path'),
    publicFiles = []

fs.readdirRecursive(CONFIG_PUBLIC_DIRECTORY, true, '').then(function(files) {
    publicFiles = files.map(function(name) { return '/' + name })
})

server = require('http').createServer(function (req, res) {
    var filePath = url.parse(req.url).pathname || CONFIG_DEFAULT_INDEX
    if (filePath === '/')
        filePath = CONFIG_DEFAULT_INDEX

    if (publicFiles.indexOf(filePath) < 0) {
        res.writeHead(404)
        res.end('404 Not Found')
        return
    }

    fs.readFile(path.join(CONFIG_PUBLIC_DIRECTORY, filePath)).then(function(data) {
        res.writeHead(200)
        res.write(data)
        res.end()
    })
}).on('error', function(err) {
    console.log(err)
})

io = require('socket.io').listen(server),
sockets = io.sockets.on('connection', function(socket) {
    fs.readdir(CONFIG_SOUNDS_DIRECTORY).then(function(sounds) {
        socket.emit('set_sounds', sounds)
    })
})

server.listen(CONFIG_SERVER_PORT)

// var mio = require('./motion_io/build/Release/motion_io')
// new mio.MotionIO('', function(intensity) {
//     io.sockets.emit('play_sound', { intensity: intensity })
// })