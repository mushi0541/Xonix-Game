#include <iostream>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <ctime>
#include "MenuHandler.h"
#include "SoundBufferHandler.h"
#include "Authentication.h"

using namespace std;
using namespace sf;


const int ts = 32;
const int topOffset = 112;
const int M = 19;
const int N = 32;

class Enemy
{
public:
    int x, y;
    int dx, dy;
    Sprite sprite;
    Texture texture;
    Enemy()
    {
        x = rand() % 750 + 180; // Random x position
        y = rand() % 400 + 100; // Random y position

        // Random non-zero direction
        do
        {
            dx = static_cast<float>(rand() % 7 - 3); // -3 to +3
            dy = static_cast<float>(rand() % 7 - 3);
        } while (dx == 0 && dy == 0);
    }

    bool loadTexture(const string& filePath)
    {
        if (!texture.loadFromFile(filePath))
            return false;
        sprite.setTexture(texture);
        sprite.setOrigin(25, 25);
        return true;
    }

    void move(int grid[][N])
    {
		x += dx;
        if (grid[y / ts][x / ts] == 1 || grid[y / ts][x / ts] == 4)
        {
			dx = -dx;
			x += dx;
		}
		y += dy;
        if (grid[y / ts][x / ts] == 1 || grid[y / ts][x / ts] == 4)
        {
			dy = -dy;
			y += dy;
		}
	}

    void draw(RenderWindow& window, int topOffset = 120)
    {
        sprite.setPosition(x, y + topOffset);
        sprite.rotate(10);
        window.draw(sprite);
    }

    int getGridX(int tileSize) const { return x / tileSize; }
    int getGridY(int tileSize) const { return y / tileSize; }
    int getX() const { return x; }
    int getY() const { return y; }
};

class player {
public:
    string userName;
    int x, y;     
    int dx, dy;   
    int score;
    int powerUps;
    bool isAlive;
    int trailMarkerValue;
    bool isFrozen;
    bool isConstructing;
public:
    player() : userName("Player"), x(10), y(0), dx(0), dy(0), score(0), powerUps(0),isAlive(true) {}

    void move(float& timer, float delay, int M, int N, int grid[][40]) 
    {
       
        x += dx;
        y += dy;

        // Boundary clamp
        if (x < 0) x = 0;
        if (x >= N) x = N - 1;
        if (y < 0) y = 0;
        if (y >= M) y = M - 1;
        if (grid[y][x] == 0) grid[y][x] = 2;

        timer = 0;
    }

    bool usePowerUp() 
    {
        if (powerUps > 0) 
        {
            powerUps--;
            return true;
        }
        return false;
    }

    int getPlayerPowerups()
    {
        string fileName = "../PlayersData/" + userName + ".txt";
        ifstream file(fileName);
        if (!file)
        {
            cout << "Error loading user file." << endl;
            return -1;
        }

        string line;
        int lineNo = 0;
        int powerupCount = 0;
        while (getline(file, line))
        {
            if (line.find("Powerups: ") != string::npos)
            {
                powerupCount = stoi(line.substr(line.find(":") + 2));
                break;
            }
            lineNo++;
        }
        file.close();
        return powerupCount;
    }
    void setPlayerPowerups(int powerups)
    {
        string path = "../PlayersData/" + userName + ".txt";
        ifstream in(path);
        ofstream out("../PlayersData/temp.txt");
        string line;
        int lineNo = 0;

        while (getline(in, line))
        {
            if (lineNo == 6)
                out << "Powerups: " << powerups << endl;
            else
                out << line << endl;
            lineNo++;
        }

        in.close();
        out.close();
        remove(path.c_str());
        rename("../PlayersData/temp.txt", path.c_str());
    }
    void setPlayerScore(int score)
    {
        string path = "../PlayersData/" + userName + ".txt";
        ifstream in(path);
        ofstream out("../PlayersData/temp.txt");
        string line;
        int lineNo = 0;

        while (getline(in, line))
        {
            if (lineNo == 3)
                out << "Score: " << score << endl;
            else
                out << line << endl;
            lineNo++;
        }

        in.close();
        out.close();
        remove(path.c_str());
        rename("../PlayersData/temp.txt", path.c_str());
    }
    int getPlayerScore()
    {
        string fileName = "../PlayersData/" + userName + ".txt";
        ifstream file(fileName);
        if (!file)
        {
            cout << "Error loading user file." << endl;
            return -1;
        }

        string line;
        int lineNo = 0;
        int highScore = 0;
        while (getline(file, line))
        {
            if (line.find("Score: ") != string::npos)
            {
                highScore = stoi(line.substr(line.find(":") + 2));
                break;
            }
            lineNo++;
        }
        file.close();
        return highScore;
    }
    int getPlayerHighScore()
    {
        string fileName = "../PlayersData/" + userName + ".txt";
        ifstream file(fileName);
        if (!file)
        {
            cout << "Error loading user file." << endl;
            return -1;
        }

        string line;
        int lineNo = 0;
        int highScore = 0;
        while (getline(file, line))
        {
            if (line.find("HighScore: ") != string::npos)
            {
                highScore = stoi(line.substr(line.find(":") + 2));
                break;
            }
            lineNo++;
        }
        file.close();
        return highScore;
    }
    void setPlayerHighScore(int highScore)
    {

        string path = "../PlayersData/" + userName + ".txt";
        ifstream in(path);
        ofstream out("../PlayersData/temp.txt");
        string line;
        int lineNo = 0;
        while (getline(in, line))
        {
            if (lineNo == 7)
                out << "HighScore: " << highScore << endl;
            else
                out << line << endl;
            lineNo++;
        }
        in.close();
        out.close();
        remove(path.c_str());
        rename("../PlayersData/temp.txt", path.c_str());
    }

    void updateWins()
    {
        string path = "../PlayersData/" + userName + ".txt";
        ifstream in(path);
        //wins are on line 4 first get the wins and then update them 
        ofstream out("../PlayersData/temp.txt");
        string line;
        int lineNo = 0;
        int wins = 0;
        while (getline(in, line)) {
            if (lineNo == 4)
            {
                wins = stoi(line.substr(line.find(":") + 2));
                wins++;
                out << "Wins: " << wins << endl;
            }
            else
                out << line << endl;
            lineNo++;
        }
        in.close();
        out.close();
        remove(path.c_str());
        rename("../PlayersData/temp.txt", path.c_str());
    }

    void addPowerUp() { powerUps++; }
    void addScore(int s) { score += s; }

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getScore() const { return score; }
    int getPowerUps() const { return powerUps; }
    string getUserName() const { return userName; }
};


struct GamePlayers
{
    string username;
    GamePlayers() : username("") {}
    GamePlayers(string name) : username(name) {}
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


class Game
{
private:
    RenderWindow& window;
    MenuHandler menuHandler;
    Authentication auth;
    Font font;
    SoundBufferHandler keyPressSound;
    SoundBufferHandler bgMusic;
    Texture BackgroundTexture;
    Sprite BackgroundSprite;
    Texture tile, gameOver;
    Sprite sTile, sGameover;
    int grid[M][N];
    player Player1;
    player Player2;
    Enemy * enemies;
    int enemyCount;
    float timer, delay;
    Clock clock;
    bool isRunning;
    bool enemiesFrozen;
    Clock powerUpTimer;
    float freezeDuration;
    bool easy, medium, hard;
    bool isMultiplayer;
    bool isGame;
    int capturedTiles;
    int bonusCounter;
    int tileCaptureThreshold = 10;
    GamePlayers* head;
    int playerCount;
public:
    Game(RenderWindow& window);
    void run();
    int handleGameLoop();
    bool saveGame();
    bool loadGame();
    int pauseMenu();
    int GameOverMenu();
    void InitializeMultiPlayer();
    int handleMultiPlayerLoop();
    int multiPlayerPauseMenu();
    
    void drop(int y, int x)
    {
        if (y < 0 || x < 0 || y >= M || x >= N) return;
        if (grid[y][x] != 0) return;

        grid[y][x] = -1;
        drop(y - 1, x);
        drop(y + 1, x);
        drop(y, x - 1);
        drop(y, x + 1);
    }
    void Initialize();
    void loadPlayers()
    {
        string fileName = "../GameUsers.txt";
        ifstream file(fileName);
        if (!file.is_open())
        {
            cout << "Error opening file.\n";
            return;
        }

        int index = 0;
        string line;
        while (getline(file, line))
        {
            if (line == auth.getUserName()) 
                continue;
            head[index].username = line;
            index++;
        }
        file.close();
    }
    int multiPlayerMenu();
    int MultiPlayerGameOverMenu();
};
