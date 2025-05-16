#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "SoundBufferHandler.h"
#include "Authentication.h"
#include "FriendRequest.h"
using namespace std;
using namespace sf;


struct Theme 
{
    int themeID;
    string name;
    string description;
    string folderPath; 
};

struct AVLNode 
{
    Theme data;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(Theme t) : data(t), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree
{
    public:
	AVLNode* root;
	AVLTree() : root(nullptr) {}
	int height(AVLNode* node);
	int getBalance(AVLNode* node);
	AVLNode* rightRotate(AVLNode* y);
	AVLNode* leftRotate(AVLNode* x);
	AVLNode* insert(AVLNode* node, Theme t);
    AVLNode* searchByID(AVLNode* node, int id);
    void insert(Theme t);
};



class Inventory 
{
    AVLTree tree;
    string currentPlayerUsername;
    RenderWindow& window;
    SoundBufferHandler keyPressSound;
    Font font;
    Theme selectedTheme;
public:
    Inventory(RenderWindow& window,string username);
    void loadThemes(); 
    bool selectThemeByID(int id);
    void savePlayerTheme(int themeID);
    int loadPlayerTheme();
    int handleInventoryMenu();
    int getPlayerPowerups();
};


