const SOUND_LIST_SELECTOR = '#sound_list'
const LOUD_TEST_BUTTON_SELECTOR = '#loud_test_button'
const SOFT_TEST_BUTTON_SELECTOR = '#soft_test_button'

var SoundList = function(sounds, audioContext)
{
    this.audioContext = audioContext

    this.sounds = sounds
    this.soundBuffers = []
    this.selectedSoundIndex = -1

    var soundListEl = document.querySelector(SOUND_LIST_SELECTOR)
    soundListEl.innerHTML = 0

    this.sounds.forEach(function(sound) {
        var soundItemEl = document.createElement('option')
        soundItemEl.value = soundItemEl.innerHTML = sound

        soundListEl.appendChild(soundItemEl)
    })

    this.setSelectedSoundIndex(this.sounds.length ? 0 : -1)

    if (this.selectedSoundIndex !== -1)
        soundListEl.value = sounds[0]
}

SoundList.prototype = {
    setSelectedSoundName: function(soundName)
    {
        this.setSelectedSoundIndex(this.sounds.indexOf(soundName))
    },
    setSelectedSoundIndex: function(index)
    {
        this.selectedSoundIndex = index
        var soundItemElChildren = document.querySelector(SOUND_LIST_SELECTOR).children

        for (var idx = 0; idx < soundItemElChildren.length; ++idx)
            soundItemElChildren[idx].disabled = false

        if (this.selectedSoundIndex === -1)
            return

        soundItemElChildren[this.selectedSoundIndex].disabled = true
        this.ensureSoundIsBuffered(this.selectedSoundIndex)
    },
    playSelectedSoundWithIntensity: function(intensity)
    {
        var buffer = this.soundBuffers[this.selectedSoundIndex]
        if (!buffer)
            return this.ensureSoundIsBuffered(this.selectedSoundIndex)

        var gainNode = this.audioContext.createGain()
        gainNode.connect(this.audioContext.destination)
        gainNode.gain.value = intensity

        var source = this.audioContext.createBufferSource()
        source.buffer = buffer
        source.connect(gainNode)

        source.start(0)
    },
    ensureSoundIsBuffered: function(soundIndex)
    {
        if (this.soundBuffers[soundIndex])
            return

        var request = new XMLHttpRequest(),
            this_ = this

        request.open('GET', '/sounds/' + this.sounds[soundIndex], true)
        request.responseType = 'arraybuffer'
        request.onload = function() {
            this_.audioContext.decodeAudioData(request.response, function(buffer) {
                this_.soundBuffers[soundIndex] = buffer
            }, function(error) {
                console.error(error)
                delete this_.soundBuffers[soundIndex]
            })
        }

        this.soundBuffers[soundIndex] = request
        request.send()
    }
}

window.addEventListener('load', function() {
    var socket = io.connect('http://192.168.7.2:3001'),
        audioContext = new (window.AudioContext || window.webkitAudioContext)(),
        soundList = null

    socket.on('set_sounds', function(sounds) {
        soundList = new SoundList(sounds, audioContext)
    }).on('play_sound', function(info) {
        soundList && soundList.playSelectedSoundWithIntensity(info.intensity)
    })

    document.querySelector(SOUND_LIST_SELECTOR).addEventListener('change', function() {
        soundList && soundList.setSelectedSoundName(this.value)
    })

    document.querySelector(LOUD_TEST_BUTTON_SELECTOR).addEventListener('click', function() {
        soundList && soundList.playSelectedSoundWithIntensity(1)
    })

    document.querySelector(SOFT_TEST_BUTTON_SELECTOR).addEventListener('click', function() {
        soundList && soundList.playSelectedSoundWithIntensity(0.25)
    })
})
