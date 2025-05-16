#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "SoundBufferHandler.h"
#include "Authentication.h"
#include "FriendRequest.h"
#include "Inventory.h"
#include "MatchSimulator.h"
#include "LeaderBoard.h"

using namespace std;
using namespace sf;
class MenuHandler
{
public:
    RenderWindow& window;
    SoundBufferHandler keyPressSound;
    Font font;
    Authentication& auth;
    Texture GameStartTextures[3];
    Sprite GameStartSprites[3];
    Texture SignInSignUpTextures[2];
    Sprite SignInSignUpSprites[2];
    Texture SignInTextures[4];
    Sprite SignInSprites[4];
    Texture SignUpTextures[5];
    Sprite SignUpSprites[5];
    Texture SignInUserNameErrorTexture;
    Sprite SignInUserNameErrorSprite;
    Texture SignInPasswordErrorTexture;
    Sprite SignInPasswordErrorSprite;
    Texture SignInSuccessTexture;
    Sprite SignInSuccessSprite;
    Texture SignUpUserNameErrorTexture;
    Sprite SignUpUserNameErrorSprite;
    Texture SignUpPasswordErrorTexture[3];
    Sprite SignUpPasswordErrorSprite[3];
    Texture SignUpSuccessTexture;
    Sprite SignUpSuccessSprite;
    Texture PlayerProfileTexture[3];
    Sprite PlayerProfileSprite[3];
    Texture PlayerProfileThemesTexture[16];
    Sprite PlayerProfileThemesSprite[16];
    bool MultiplayerMode;

public:
    MenuHandler(RenderWindow& gameWindow, Authentication& auth);
    void loadResources();
    int mainMenu();
    
    int startMenu();
    int difficultyMenu();
    int GameStartMenu();
    int SignInSignUpMenu();
    int SignInMenu();
    int SignUpMenu();
    int IntroAnimation();
    int PlayerProfileMenu();
    int handleMenuLoop();
    int getProfileIcon();
    void setProfileIcon(int iconIndex);
    int selectProfileIconMenu();
    int getWins();
    int getScore();
    int handleMainMenuLoop();
    int getPlayerThemeID();
    int showRules();
};

