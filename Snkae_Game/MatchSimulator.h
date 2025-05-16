#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Authentication.h"
#include "SoundBufferHandler.h"
using namespace std;
using namespace sf;


struct MatchPlayer 
{
    string userName;
    int score;
    int arrivalTime;

    MatchPlayer() {}
    MatchPlayer(string name, int s, int time = 0) : userName(name), score(s), arrivalTime(time) {}
};


class PriorityQueue 
{
    MatchPlayer data[500];
    int size; // initialize size

public:
    PriorityQueue() 
    {
        size = 0; // Initialize size to 0
        currentTime = 0; // Initialize current time
    }
    int currentTime;
    string username;
    void insert(MatchPlayer p);        // Insert MatchPlayer based on max-heap
    MatchPlayer extractMax();          // Remove and return highest-score MatchPlayer
    bool isEmpty() { return size == 0; }
    int getSize() { return size; }
    void display(sf::RenderWindow& window, sf::Font& font);
    void clear() { size = 0; } // Clear the queue
    void deletePlayer(MatchPlayer p); // Delete a player from the queue
};


struct GameRoom 
{
    MatchPlayer player1, player2;
    GameRoom() : player1("", 0), player2("", 0) {}
};


class GameRoomQueue {
    GameRoom data[250];
    int front = 0, rear = 0;

public:
    void enqueue(GameRoom g);
    GameRoom dequeue();
    bool isEmpty() { return front == rear; }
    void display(RenderWindow& window,Font& font);
    void clear() { front = rear = 0; } // Clear the queue
};



class MatchSimulator 
{
    RenderWindow& window;
    PriorityQueue playerQueue;
    GameRoomQueue gameRooms;
    Font font; 
    Authentication& auth;
    bool loaded;
    MatchPlayer* pendingPlayer;
    SoundBufferHandler keyPressSound;
public:
    MatchSimulator(RenderWindow& window,Authentication& a); 
    int getScoreFromFile(const string& username);
    void loadPlayers(); 
    void updateWins(const MatchPlayer& p);
    void draw(); // use font and SFML to draw names, matches
    void updatePlayerScoreFile(const MatchPlayer& p);
    string simulateMatchesAndUpdateScores();
    int matchSimulatorMenu(int PlayerThemeID);
};


