#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <iostream>

using namespace sf;
using namespace std;

class SoundBufferHandler {
private:
    SoundBuffer buffer; // Holds the sound buffer
    Sound sound;        // Associated sound object
    string path;        // File path of the sound file

public:
    SoundBufferHandler(const string& filePath = "");
    bool loadFromFile(const string& filePath);
    void play();
    void stop();
    void setVolume(float volume);
    void setLoop(bool loop);
    Sound& getSound();
    SoundBuffer& getBuffer();
};
