#include "SoundBufferHandler.h"

SoundBufferHandler::SoundBufferHandler(const string& filePath) : path(filePath) 
{
    if (!filePath.empty()) {
        if (!buffer.loadFromFile(filePath)) {
            cerr << "Failed to load sound buffer from: " << filePath << endl;
        }
        else {
            sound.setBuffer(buffer);
        }
    }
}

bool SoundBufferHandler::loadFromFile(const string& filePath) 
{
    path = filePath;
    if (!buffer.loadFromFile(filePath)) {
        cerr << "Failed to load sound buffer from: " << filePath << endl;
        return false;
    }
    sound.setBuffer(buffer);
    return true;
}

void SoundBufferHandler::play() 
{
    sound.play();
}

void SoundBufferHandler::stop() 
{
    sound.stop();
}

void SoundBufferHandler::setVolume(float volume) 
{
    sound.setVolume(volume);
}

void SoundBufferHandler::setLoop(bool loop) 
{
    sound.setLoop(loop);
}

Sound& SoundBufferHandler::getSound() 
{
    return sound;
}

SoundBuffer& SoundBufferHandler::getBuffer() 
{
    return buffer;
}
