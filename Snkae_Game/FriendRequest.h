#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "SoundBufferHandler.h"
#include <string>
#include <fstream>
using namespace std;
using namespace sf;



struct FriendNode 
{
	string username;
	FriendNode* next;
    FriendNode(string name) : username(name), next(nullptr) {}
};


struct Player 
{
    string username;
    FriendNode* friends;
    FriendNode* pendingRequests;
	Player() : username(""), friends(nullptr), pendingRequests(nullptr) {}
    Player(string name) : username(name), friends(nullptr), pendingRequests(nullptr) {}

	bool isRequestPending(string name)
	{
		FriendNode* current = pendingRequests;
		while (current != nullptr)
		{
			if (current->username == name)
			{
				return true;
			}
			current = current->next;
		}
		return false;
	}
	bool isAlreadyFriend(string name) 
	{
		FriendNode* current = friends;
		while (current != nullptr) {
			if (current->username == name) {
				return true;
			}
			current = current->next;
		}
		return false;
	}
	int getProfileIcon()
	{
		string fileName = "../PlayersData/" + username + ".txt";
		ifstream file(fileName);
		if (!file)
		{
			cout << "Error loading user file." << endl;
			return -1;
		}
		string line;
		while (getline(file, line))
		{
			if (line.find("ProfileIcon: ") != string::npos)
			{
				int profileIcon = stoi(line.substr(line.find(": ") + 2));
				file.close();
				return profileIcon;
			}
		}

		file.close();
		return 1;
	}
};

class FriendRequest 
{
	Player* players;
	int playerCount;
	Player** hashTable;
	RenderWindow& window;
	SoundBufferHandler keyPressSound;
	Font font;
	Texture FriendRequestMenuTextures[3];
	Sprite FriendRequestMenuSprites[3];

	int hashFunction(string username) 
	{
		if (playerCount == 0) return 0;
		int hash = 0;
		for (int i = 0; i < username.length(); i++) {
			hash = (hash * 31 + username[i]) % (playerCount * 2);
		}
		return hash;
	}

public:
	FriendRequest(RenderWindow& window);
	void loadPlayers();
	void loadFriendList(Player& player);
	int sendFriendRequest(string sender, string receiver);
	void acceptFriendRequest(string sender, string receiver);
	void rejectFriendRequest(string sender, string receiver);
	void DrawFriends(string username);
	void viewPendingRequests(string username);
	Player* getPlayer(string username);
	bool isPlayerExists(string username);
	int getNumberOfFriends(string username);
	int handleFriendRequestMenu(string username);
	int pendingRequestMenu(string username);
	int addFriendMenu(string username);
	int handleMenuLoop(string username);
};

