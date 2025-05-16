#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <SFML/Audio.hpp>
#include "SoundBufferHandler.h"

using namespace std;
using namespace sf;

class LeaderBoard
{
private:
    struct LeaderboardEntry
    {
        string name;
        int score;

        LeaderboardEntry() : name(""), score(0) {}
        LeaderboardEntry(const string& playerName, int playerScore)
            : name(playerName), score(playerScore) {}
    };

    static const int MAX_ENTRIES = 10;
    LeaderboardEntry heap[MAX_ENTRIES]; // Min-Heap array
    int currentSize; // Current number of players in heap
    static const string leaderboardFile;
    void heapifyUp(int index);    // maintain heap after inserting
    void heapifyDown(int index);  // maintain heap after replacing
    void readLeaderboard();
    void writeLeaderboard();
    int getPlayerScore(const string& playerName);
    void insertIntoHeap(const LeaderboardEntry& entry);
    RenderWindow& window;
    SoundBufferHandler keyPressSound;
public:
    void displayLeaderboard();
    LeaderBoard(RenderWindow& window);
};


