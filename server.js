const CONFIG_DEFAULT_INDEX = 'index.html'
const CONFIG_SERVER_PORT = 3000
const CONFIG_PUBLIC_DIRECTORY = __dirname + '/public/'

var fs = require('fs'),
    url = require('url'),
    path = require('path'),
    public_files = fs.readdirSync(CONFIG_PUBLIC_DIRECTORY)

server = require('http').createServer(function (req, res) {
    var filename = path.basename(url.parse(req.url).pathname) || CONFIG_DEFAULT_INDEX
    if (public_files.indexOf(filename) < 0) {
        res.writeHead(404)
        res.end('404 Not Found')
        return
    }

    fs.readFile(path.join(CONFIG_PUBLIC_DIRECTORY, filename), function(err, data) {
        res.writeHead(200)
        res.write(data)
        res.end()
    })
}).on('error', function(err) {
    console.log(err)
})

io = require('socket.io').listen(server),
sockets = io.sockets.on('connection', function(socket) {
    socket.emit('source add', { name: '0x42' })
})

server.listen(CONFIG_SERVER_PORT)