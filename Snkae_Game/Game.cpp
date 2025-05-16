#include "Game.h"
#include<string>
#include "Authentication.h"
using namespace std;
using namespace sf;

const int P1_TRAIL_MARKER = 2;
const int P2_TRAIL_MARKER = 3;

Game::Game(RenderWindow& window) : window(window), menuHandler(window,auth)
{
	// Load the game resources
	if (!font.loadFromFile("../Font2/Category.ttf"))
	{
		cout << "Error loading font" << endl;
	}
    keyPressSound.loadFromFile("../Sound/Key.wav");
    bgMusic.loadFromFile("../Sound/bgMusic.wav");
	easy = false;
	hard = true;
	medium = false;
	enemyCount = 4;
    isGame = true;
    auth.setUserName("mushahid");
    isMultiplayer = false;

    string fileName = "../GameUsers.txt";
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error opening file.\n";
        playerCount = 0;
        return;
    }

    playerCount = 0;
    string line;
    while (getline(file, line)) 
    {
        if (line == auth.getUserName()) 
            continue;
        playerCount++;
    }

    head = new GamePlayers[playerCount];
    file.close();
}

bool Game::saveGame()
{
    string filename = "../GameSaves/" + Player1.userName + ".txt";
    ofstream out(filename);
    if (!out)
    {
        cerr << "Failed to open save file: " << filename << "\n";
        return false;
    }

    // Player1 state
    out << Player1.x << ' ' << Player1.y << ' '
        << Player1.dx << ' ' << Player1.dy << ' '
        << Player1.score << ' ' << Player1.powerUps << '\n';

    // Bonus state
    out << bonusCounter << ' ' << tileCaptureThreshold << '\n';

    // Enemies
    out << enemyCount << '\n';
    for (int i = 0; i < enemyCount; ++i)
        out << enemies[i].x << ' ' << enemies[i].y << ' '
        << enemies[i].dx << ' ' << enemies[i].dy << '\n';

    // Grid
    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
            out << grid[i][j] << ' ';
        out << '\n';
    }

    return true;
}

bool Game::loadGame()
{
    string filename = "../GameSaves/" + Player1.userName + ".txt";
    ifstream in(filename);
    if (!in)
    {
        cerr << "Failed to open save file: " << filename << "\n";
        return false;
    }

    // Player1 state
    in >> Player1.x >> Player1.y
        >> Player1.dx >> Player1.dy
        >> Player1.score >> Player1.powerUps;

    // Bonus state
    in >> bonusCounter >> tileCaptureThreshold;

    // Enemies
    int savedCount;
    in >> savedCount;
    delete[] enemies;
    enemyCount = savedCount;
    enemies = new Enemy[enemyCount];
    int themeID = menuHandler.getPlayerThemeID();
    string filePath = "../Themes/Theme" + to_string(themeID);
    for (int i = 0; i < enemyCount; ++i)
    {
        in >> enemies[i].x >> enemies[i].y
            >> enemies[i].dx >> enemies[i].dy;
        enemies[i].loadTexture(filePath + "/enemy.png");
    }

    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            in >> grid[i][j];

    timer = 0.f;
    isGame = true;
    enemiesFrozen = false;
    powerUpTimer.restart();
    return true;
}

void Game::Initialize()
{
	// Initialize the game state
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
		{
			grid[i][j] = 0;
		}
	}
	for (int i = 0; i < M; i++)
	{
		grid[i][0] = 1;
		grid[i][N - 1] = 1;
	}
	for (int j = 0; j < N; j++)
	{
		grid[0][j] = 1;
		grid[M - 1][j] = 1;
	}

	if(easy)
		enemyCount = 4;
	else if(medium)
		enemyCount = 5;
	else if(hard)
		enemyCount = 7;

	enemies = new Enemy[enemyCount];
	int themeID = menuHandler.getPlayerThemeID();
    string filePath = "../Themes/Theme" + to_string(themeID);
	for (int i = 0; i < enemyCount; i++)
	{
		enemies[i].loadTexture(filePath + "/enemy.png");
	}

    BackgroundTexture.loadFromFile(filePath + "/GameBackGround.png");
    BackgroundSprite.setTexture(BackgroundTexture);
    BackgroundSprite.setPosition(0, 0);

	Player1.userName = auth.getUserName();
    Player1.powerUps = Player1.getPlayerPowerups();
    Player1.score = 0;
    tile.loadFromFile("../images/tiles.png");
    sTile.setTexture(tile);
    gameOver.loadFromFile("../images/gameover.png");
    sGameover.setTexture(gameOver);
	isGame = true;
	enemiesFrozen = false;
	capturedTiles = 0;
	bonusCounter = 0;
	tileCaptureThreshold = 10;
	delay = 0.07f;
	timer = 0.0f;
	freezeDuration = 3.0f; 
	powerUpTimer.restart();
    Player1.x = 10;
    Player1.y = 0;
    Player1.dx = 0;
    Player1.dy = 0;
}

void Game::run()
{
    bgMusic.play();
    bgMusic.setLoop(true);
    bgMusic.setVolume(30);
	menuHandler.handleMenuLoop();
    int selectedOption = 0;
	while (window.isOpen())
	{
		selectedOption = menuHandler.handleMainMenuLoop();
        if (selectedOption == 1)
        {
            bgMusic.stop();
            bgMusic.play();
            Initialize();
            loadGame();
            handleGameLoop();
        }
        else if (selectedOption == 3)
        {
            bgMusic.stop();
            bgMusic.play();
            easy = true;
            medium = false;
            hard = false;
            Initialize();
            handleGameLoop();
        }
        else if (selectedOption == 4)
        {
            bgMusic.stop();
            bgMusic.play();
			easy = false;
			medium = true;
			hard = false;
			Initialize();
			handleGameLoop();
		}
        else if (selectedOption == 5)
        {
            bgMusic.stop();
            bgMusic.play();
			easy = false;
			medium = false;
			hard = true;
			Initialize();
			handleGameLoop();
		}
        else if (selectedOption == 6)
        {
			isMultiplayer = true;
            selectedOption = multiPlayerMenu();
            if (selectedOption == -1)
                continue;
            bgMusic.stop();
            bgMusic.play();
			InitializeMultiPlayer();
			handleMultiPlayerLoop();
		}   
	}
}


int Game::handleGameLoop()
{
    bool isMainMenuCalled = false;

    Text ScoreText;
    ScoreText.setFont(font);
    ScoreText.setCharacterSize(30);
    ScoreText.setFillColor(Color::White);
    ScoreText.setPosition(40, 35);
    ScoreText.setStyle(Text::Bold);

    Text PowerUpText;
    PowerUpText.setFont(font);
    PowerUpText.setCharacterSize(40);
    PowerUpText.setFillColor(Color::White);
    PowerUpText.setPosition(485, 36);
    PowerUpText.setStyle(Text::Bold);

    Text NameText;
    NameText.setFont(font);
    NameText.setCharacterSize(30);
    NameText.setFillColor(Color::White);
    NameText.setPosition(650, 37);
    NameText.setStyle(Text::Bold);
    NameText.setString(Player1.userName);
   
    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;
        capturedTiles = 0;
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Escape)
                {
                    keyPressSound.getSound().play();
                    int pauseMenuResult = pauseMenu();

                    if (pauseMenuResult == 0)
                    {
                        continue;
					}
                    else if (pauseMenuResult == 1)
                    {
						Initialize();
					}
                    else if (pauseMenuResult == 2)
                    {
                        if (saveGame())
                            cout << "Game saved.\n";
                        else
                            cout << "Save failed.\n";
					}
                    else if (pauseMenuResult == 3)
                    {
                        isMainMenuCalled = true;
                        break;
					}
                }

                if (e.key.code == Keyboard::Space)
                {
                    if (Player1.usePowerUp())
                    {
                        enemiesFrozen = true;
                        powerUpTimer.restart();
                    }
                }
            }
        }

        if (isMainMenuCalled)
            break;

        if (Keyboard::isKeyPressed(Keyboard::Left)) { Player1.dx = -1; Player1.dy = 0; }
        if (Keyboard::isKeyPressed(Keyboard::Right)) { Player1.dx = 1; Player1.dy = 0; }
        if (Keyboard::isKeyPressed(Keyboard::Up)) { Player1.dx = 0; Player1.dy = -1; }
        if (Keyboard::isKeyPressed(Keyboard::Down)) { Player1.dx = 0; Player1.dy = 1; }

        if (!isGame) continue;

        if (timer > delay)
        {
            Player1.x += Player1.dx;
            Player1.y += Player1.dy;

            if (Player1.x < 0) Player1.x = 0; if (Player1.x > N - 1) Player1.x = N - 1;
            if (Player1.y < 0) Player1.y = 0; if (Player1.y > M - 1) Player1.y = M - 1;

            if (grid[Player1.y][Player1.x] == 2) isGame = false;
            if (grid[Player1.y][Player1.x] == 0) grid[Player1.y][Player1.x] = 2;

            timer = 0;
        }

        if (enemiesFrozen)
        {
            if (powerUpTimer.getElapsedTime().asSeconds() > freezeDuration)
                enemiesFrozen = false;
        }
        else
        {
            for (int i = 0; i < enemyCount; i++)
                enemies[i].move(grid);
        }

        if (isGame && grid[Player1.y][Player1.x] == 1) 
    {
        Player1.dx = Player1.dy = 0; 

        for (int k = 0; k < enemyCount; k++)
        {
            drop(enemies[k].y / ts, enemies[k].x / ts);
        }

        int tilesJustCapturedThisTurn = 0; 

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 2) // Player1's trail cell
                {
                    grid[i][j] = 1; // Solidify to border
                    tilesJustCapturedThisTurn++;
                }
                else if (grid[i][j] == 0) // Empty cell (and not marked -1 by drop)
                {
                    grid[i][j] = 1; // Capture it, make it border
                    tilesJustCapturedThisTurn++;
                }
                else if (grid[i][j] == -1) 
                {
                    grid[i][j] = 0; // Revert to normal empty
                }
            }
        }
        
        capturedTiles = tilesJustCapturedThisTurn; 

        if (capturedTiles > 0) // Only apply scoring if something was actually captured
        {
            int pointMultiplier = 1;
            if (capturedTiles > tileCaptureThreshold)
            {
                if (bonusCounter < 5)
                {
                    bonusCounter++;
                    if (bonusCounter == 3)
                        tileCaptureThreshold = 5;
                }

                if (bonusCounter >= 5)
                    pointMultiplier = 4;
                else
                    pointMultiplier = 2;
            }

            int prevScore = Player1.score;
            Player1.score += capturedTiles * pointMultiplier;

            if (Player1.score >= 50 && prevScore < 50) Player1.powerUps++;
            if (Player1.score >= 70 && prevScore < 70) Player1.powerUps++;
            if (Player1.score >= 100 && prevScore < 100) Player1.powerUps++;
            if (Player1.score >= 130 && prevScore < 130) Player1.powerUps++;
            if (Player1.score >= 160 && (Player1.score - prevScore) >= 30) Player1.powerUps++;
        }
    }

        for (int i = 0; i < enemyCount; i++)
            if (grid[enemies[i].y / ts][enemies[i].x / ts] == 2)
                isGame = false;

        window.clear();
        window.draw(BackgroundSprite);
        ScoreText.setString("Score: " + to_string(Player1.score));
        window.draw(ScoreText);
        PowerUpText.setString(to_string(Player1.powerUps));
        window.draw(PowerUpText);
        window.draw(NameText);
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0) continue;
                if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts, ts));
                if (grid[i][j] == 2) sTile.setTextureRect(IntRect(64, 0, ts, ts));
                sTile.setPosition(j * ts, i * ts + 112);
                window.draw(sTile);
            }

        // Player1
        sTile.setTextureRect(IntRect(32, 0, ts, ts));
        sTile.setPosition(Player1.x * ts, Player1.y * ts + 112);
        window.draw(sTile);

        for (int i = 0; i < enemyCount; i++)
        {
            enemies[i].draw(window);
        }

        if (!isGame)
        {
            sleep(seconds(1));
            int GameOverMenuChoice = GameOverMenu();
            if (GameOverMenuChoice == 0)
                Initialize();
            else if (GameOverMenuChoice == 1)
                isMainMenuCalled = true;
        }

        window.display();
    }
    return 0;
}

int Game::pauseMenu()
{
    Texture PauseMenuTexture[4];
    Sprite PauseMenuSprite[4];
    int themeID = menuHandler.getPlayerThemeID();
    string filePath = "../Themes/Theme" + to_string(themeID);
    for (int i = 0; i < 4; i++)
    {
		PauseMenuTexture[i].loadFromFile(filePath + "/PauseMenu" + to_string(i+1) + ".png");
		PauseMenuSprite[i].setTexture(PauseMenuTexture[i]);
        PauseMenuSprite[i].setPosition(0, 0);
	}
    int selectedIndex = 0;
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                keyPressSound.getSound().play();
                window.close();
                return -1;
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                keyPressSound.getSound().play();
                return 0;
            }

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                {
                    keyPressSound.getSound().play();
                    selectedIndex = (selectedIndex - 1 + 4) % 4;
                }
                else if (event.key.code == Keyboard::Down)
                {
                    keyPressSound.getSound().play();
                    selectedIndex = (selectedIndex + 1) % 4;
                }
                else if (event.key.code == Keyboard::Enter)
                {
                    keyPressSound.getSound().play();
                    return selectedIndex;
                }
            }
        }

        window.clear(Color::White);
        window.draw(PauseMenuSprite[selectedIndex]);
        window.display();
    }
    return 0;
}


int Game::GameOverMenu()
{
    Texture GameOverMenuTexture[2];
    Sprite GameOverMenuSprite[2];
    int themeID = menuHandler.getPlayerThemeID();
    string filePath = "../Themes/Theme" + to_string(themeID);
    for (int i = 0; i < 2; i++)
    {
		GameOverMenuTexture[i].loadFromFile(filePath + "/GameOverMenu" + to_string(i+1) + ".png");
		GameOverMenuSprite[i].setTexture(GameOverMenuTexture[i]);
		GameOverMenuSprite[i].setPosition(0, 0);
	}
    int selectedIndex = 0;
    Text ScoreText;
    ScoreText.setFont(font);
    ScoreText.setCharacterSize(34);
    ScoreText.setFillColor(Color::White);
    ScoreText.setPosition(400, 450);
    ScoreText.setStyle(Text::Bold);
    ScoreText.setString("Score: " + to_string(Player1.score));

    int TotalNewScore = Player1.score + Player1.getPlayerScore();
    Player1.setPlayerScore(TotalNewScore);
    Player1.setPlayerPowerups(Player1.powerUps);
    bool isHighScore = false;
    int OldHighScore = Player1.getPlayerHighScore();
    if (Player1.score > OldHighScore)
    {
        isHighScore = true;
        Player1.setPlayerHighScore(Player1.score);
    }
        

    Text NewHighScore;
    NewHighScore.setFont(font);
    NewHighScore.setCharacterSize(34);
    NewHighScore.setFillColor(Color::White);
    NewHighScore.setPosition(310, 380);
    NewHighScore.setStyle(Text::Bold);
    NewHighScore.setString("NEW Player1 HIGHSCORE");
    
    while (window.isOpen())
    {
		Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
				keyPressSound.getSound().play();
				window.close();
				return -1;
			}

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                {
					keyPressSound.getSound().play();
					selectedIndex = (selectedIndex - 1 + 2) % 2;
				}
                else if (event.key.code == Keyboard::Down)
                {
					keyPressSound.getSound().play();
					selectedIndex = (selectedIndex + 1) % 2;
				}
                else if (event.key.code == Keyboard::Enter)
                {
					keyPressSound.getSound().play();
					return selectedIndex;
				}
			}
		}

		window.clear(Color::White);
		window.draw(GameOverMenuSprite[selectedIndex]);
        window.draw(ScoreText);
        if(isHighScore)
        window.draw(NewHighScore);
		window.display();
	}
	return 0;
}


void Game::InitializeMultiPlayer()
{
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            grid[i][j] = 0;
        }
    }
    for (int i = 0; i < M; i++)
    {
        grid[i][0] = 1;
        grid[i][N - 1] = 1;
    }
    for (int j = 0; j < N; j++)
    {
        grid[0][j] = 1;
        grid[M - 1][j] = 1;
    }

    if (easy)
        enemyCount = 4;
    else if (medium)
        enemyCount = 5;
    else if (hard)
        enemyCount = 6;

    delete[] enemies; // Ensure old enemies are deleted if any
    enemies = new Enemy[enemyCount];
    int themeID = menuHandler.getPlayerThemeID();
    string filePath = "../Themes/Theme" + to_string(themeID);
    for (int i = 0; i < enemyCount; i++)
    {
        enemies[i].loadTexture(filePath + "/enemy.png");
    }

    BackgroundTexture.loadFromFile(filePath + "/GameBackGround.png");
    BackgroundSprite.setTexture(BackgroundTexture);
    BackgroundSprite.setPosition(0, 0);

    Player1.userName = auth.getUserName(); 
    Player1.powerUps = Player1.getPlayerPowerups();
    Player1.score = 0;
    Player1.x = N; // Adjusted starting position for P1
    Player1.y = 0;
    Player1.dx = 0;
    Player1.dy = 0;
    Player1.isAlive = true;
    Player1.isFrozen = false;
    Player1.trailMarkerValue = P1_TRAIL_MARKER;
    Player1.isConstructing = false;


    if (isMultiplayer) 
    {
        Player2.powerUps = Player2.getPlayerPowerups(); // Or some default like 0-1
        Player2.score = 0;
        Player2.x = 0; // Adjusted starting position for P2
        Player2.y = 0;
        Player2.dx = 0;
        Player2.dy = 0;
        Player2.isAlive = true;
        Player2.isFrozen = false;
        Player2.trailMarkerValue = P2_TRAIL_MARKER;
        Player2.isConstructing = false;
    }


    tile.loadFromFile("../images/tiles.png"); 
    sTile.setTexture(tile);
    gameOver.loadFromFile("../images/gameover.png");
    sGameover.setTexture(gameOver);

    isGame = true; 
    enemiesFrozen = false;
    capturedTiles = 0;
    bonusCounter = 0; 
    tileCaptureThreshold = 10;
    delay = 0.07f;
    timer = 0.0f;
    freezeDuration = 3.0f;
    powerUpTimer.restart();
}

int Game::handleMultiPlayerLoop()
{
    bool isMainMenuCalled = false;

    Text player1ScoreText;
    player1ScoreText.setFont(font);
    player1ScoreText.setCharacterSize(30);
    player1ScoreText.setFillColor(Color::White);
    player1ScoreText.setPosition(710, 65);
    player1ScoreText.setStyle(Text::Bold);

    Text player1PowerUpText;
    player1PowerUpText.setFont(font);
    player1PowerUpText.setCharacterSize(40);
    player1PowerUpText.setFillColor(Color::White);
    player1PowerUpText.setPosition(610, 36);
    player1PowerUpText.setStyle(Text::Bold);

    Text player1NameText;
    player1NameText.setFont(font);
    player1NameText.setCharacterSize(30);
    player1NameText.setFillColor(Color::White);
    player1NameText.setPosition(710, 25);
    player1NameText.setStyle(Text::Bold);
    player1NameText.setString(Player1.userName);

    Text player2ScoreText;
    player2ScoreText.setFont(font);
    player2ScoreText.setCharacterSize(30);
    player2ScoreText.setFillColor(Color::White);
    player2ScoreText.setPosition(100, 65);
    player2ScoreText.setStyle(Text::Bold);

    Text player2PowerUpText;
    player2PowerUpText.setFont(font);
    player2PowerUpText.setCharacterSize(40);
    player2PowerUpText.setFillColor(Color::White);
    player2PowerUpText.setPosition(340, 36);
    player2PowerUpText.setStyle(Text::Bold);

    Text player2NameText;
    player2NameText.setFont(font);
    player2NameText.setCharacterSize(30);
    player2NameText.setFillColor(Color::White);
    player2NameText.setPosition(100, 25);
    player2NameText.setStyle(Text::Bold);
    player2NameText.setString(Player2.userName);

    isGame = Player1.isAlive || Player2.isAlive;

    while (window.isOpen() && isGame && !isMainMenuCalled)
    {
        float deltaTime = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += deltaTime;

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                {
                    keyPressSound.getSound().play();
                    int pauseMenuResult = multiPlayerPauseMenu();

                    if (pauseMenuResult == 0)
                    {
                        continue;
                    }
                    else if (pauseMenuResult == 1)
                    {
                        InitializeMultiPlayer();
                    }
                    else if (pauseMenuResult == 2)
                    {
                        isMainMenuCalled = true;
                        break;
                    }
                }

                if (Player1.isAlive && event.key.code == Keyboard::RShift)
                {
                    if (Player1.usePowerUp())
                    {
                        enemiesFrozen = true;
                        if (Player2.isAlive)
                        {
                            Player2.isFrozen = true;
                        }
                        powerUpTimer.restart();
                    }
                }
                if (Player2.isAlive && event.key.code == Keyboard::LShift)
                {
                    if (Player2.usePowerUp())
                    {
                        enemiesFrozen = true;
                        if (Player1.isAlive)
                        {
                            Player1.isFrozen = true;
                        }
                        powerUpTimer.restart();
                    }
                }
                if (event.key.code == Keyboard::M)
                {
                    isMainMenuCalled = true;
                }
            }
        }

        if (isMainMenuCalled)
        {
            break;
        }


        // Player 1 controls
        if (Player1.isAlive && !Player1.isFrozen)
        {
            if (Keyboard::isKeyPressed(Keyboard::Left)) { Player1.dx = -1; Player1.dy = 0; }
            else if (Keyboard::isKeyPressed(Keyboard::Right)) { Player1.dx = 1; Player1.dy = 0; }
            else if (Keyboard::isKeyPressed(Keyboard::Up)) { Player1.dx = 0; Player1.dy = -1; }
            else if (Keyboard::isKeyPressed(Keyboard::Down)) { Player1.dx = 0; Player1.dy = 1; }
        }

        if (Player2.isAlive && !Player2.isFrozen)
        {
            if (Keyboard::isKeyPressed(Keyboard::A)) { Player2.dx = -1; Player2.dy = 0; }
            else if (Keyboard::isKeyPressed(Keyboard::D)) { Player2.dx = 1; Player2.dy = 0; }
            else if (Keyboard::isKeyPressed(Keyboard::W)) { Player2.dx = 0; Player2.dy = -1; }
            else if (Keyboard::isKeyPressed(Keyboard::S)) { Player2.dx = 0; Player2.dy = 1; }
        }

        if (enemiesFrozen || Player1.isFrozen || Player2.isFrozen)
        {
            if (powerUpTimer.getElapsedTime().asSeconds() > freezeDuration)
            {
                enemiesFrozen = false;
                Player1.isFrozen = false;
                Player2.isFrozen = false;
            }
        }


        if (timer > delay)
        {
            if (Player1.isAlive && !Player1.isFrozen)
            {
                int nextPlayer1X = Player1.x + Player1.dx;
                int nextPlayer1Y = Player1.y + Player1.dy;

                if (nextPlayer1X >= 0 && nextPlayer1X < N && nextPlayer1Y >= 0 && nextPlayer1Y < M)
                {
                    if (grid[nextPlayer1Y][nextPlayer1X] == Player1.trailMarkerValue)
                    {
                        Player1.isAlive = false;
                        for (int r = 0; r < M; r++) { for (int c = 0; c < N; c++) { if (grid[r][c] == Player1.trailMarkerValue) { grid[r][c] = 0; } } }
                        Player1.isConstructing = false;
                    }
                    else if (grid[nextPlayer1Y][nextPlayer1X] == Player2.trailMarkerValue)
                    {
                        Player1.isAlive = false;
                        if (Player1.isConstructing) { for (int r = 0; r < M; r++) { for (int c = 0; c < N; c++) { if (grid[r][c] == Player1.trailMarkerValue) { grid[r][c] = 0; } } } }
                        Player1.isConstructing = false;
                    }
                    else
                    {
                        Player1.x = nextPlayer1X;
                        Player1.y = nextPlayer1Y;
                        if (grid[Player1.y][Player1.x] == 0)
                        {
                            grid[Player1.y][Player1.x] = Player1.trailMarkerValue;
                            Player1.isConstructing = true;
                        }
                        else
                        {
                            Player1.isConstructing = false;
                        }
                    }
                }
                else
                {
                    Player1.x = (nextPlayer1X < 0) ? 0 : (nextPlayer1X >= N) ? N - 1 : nextPlayer1X;
                    Player1.y = (nextPlayer1Y < 0) ? 0 : (nextPlayer1Y >= M) ? M - 1 : nextPlayer1Y;
                    Player1.isConstructing = false;
                }
            }

            // Player 2 Movement & Trail
            if (Player2.isAlive && !Player2.isFrozen)
            {
                int nextPlayer2X = Player2.x + Player2.dx;
                int nextPlayer2Y = Player2.y + Player2.dy;

                if (nextPlayer2X >= 0 && nextPlayer2X < N && nextPlayer2Y >= 0 && nextPlayer2Y < M)
                {
                    if (grid[nextPlayer2Y][nextPlayer2X] == Player2.trailMarkerValue)
                    {
                        Player2.isAlive = false;
                        for (int r = 0; r < M; r++) { for (int c = 0; c < N; c++) { if (grid[r][c] == Player2.trailMarkerValue) { grid[r][c] = 0; } } }
                        Player2.isConstructing = false;
                    }
                    else if (grid[nextPlayer2Y][nextPlayer2X] == Player1.trailMarkerValue)
                    {
                        Player2.isAlive = false;
                        if (Player2.isConstructing) { for (int r = 0; r < M; r++) { for (int c = 0; c < N; c++) { if (grid[r][c] == Player2.trailMarkerValue) { grid[r][c] = 0; } } } }
                        Player2.isConstructing = false;
                    }
                    else
                    {
                        Player2.x = nextPlayer2X;
                        Player2.y = nextPlayer2Y;
                        if (grid[Player2.y][Player2.x] == 0)
                        {
                            grid[Player2.y][Player2.x] = Player2.trailMarkerValue;
                            Player2.isConstructing = true;
                        }
                        else
                        {
                            Player2.isConstructing = false;
                        }
                    }
                }
                else
                {
                    Player2.x = (nextPlayer2X < 0) ? 0 : (nextPlayer2X >= N) ? N - 1 : nextPlayer2X;
                    Player2.y = (nextPlayer2Y < 0) ? 0 : (nextPlayer2Y >= M) ? M - 1 : nextPlayer2Y;
                    Player2.isConstructing = false;
                }
            }
            if (Player1.x == Player2.x && Player1.y == Player2.y)
            {
                Player1.isAlive = false;
                Player2.isAlive = false;
                sleep(seconds(1));
                isGame = false;
            }
            timer = 0; 
        }

        if (!enemiesFrozen)
        {
            for (int i = 0; i < enemyCount; i++)
                enemies[i].move(grid);
        }

       
        if (Player1.isAlive && grid[Player1.y][Player1.x] == 1 || grid[Player1.y][Player1.x] == 4)
        {
            bool trailToSolidifyPlayer1 = false;
            for (int r = 0; r < M; r++)
            {
                for (int c = 0; c < N; c++)
                {
                    if (grid[r][c] == Player1.trailMarkerValue)
                    {
                        trailToSolidifyPlayer1 = true;
                        break;
                    }
                }
                if (trailToSolidifyPlayer1)
                {
                    break;
                }
            }
        
            if (!trailToSolidifyPlayer1 && Player1.dx == 0 && Player1.dy == 0) 
            { 

            }
            else 
            { // Trail exists or player is moving along border to define new area edge

                Player1.dx = 0;
                Player1.dy = 0;

                int tilesJustCapturedThisTurnPlayer1 = 0;
                for (int k = 0; k < enemyCount; k++)
                {
                    drop(enemies[k].y / ts, enemies[k].x / ts);
                }

                for (int i = 0; i < M; i++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        if (grid[i][j] == Player1.trailMarkerValue)
                        {
                            grid[i][j] = 1;
                            tilesJustCapturedThisTurnPlayer1++;
                        }
                        else if (grid[i][j] == 0)
                        {
                            grid[i][j] = 1;
                            tilesJustCapturedThisTurnPlayer1++;
                        }
                        else if (grid[i][j] == -1)
                        {
                            grid[i][j] = 0;
                        }
                    }
                }

                if (tilesJustCapturedThisTurnPlayer1 > 0)
                {
                    int pointMultiplier = 1;
                    if (tilesJustCapturedThisTurnPlayer1 > tileCaptureThreshold)
                    {
                        if (bonusCounter < 5) { bonusCounter++; if (bonusCounter == 3) { tileCaptureThreshold = 5; } }
                        if (bonusCounter >= 5) { pointMultiplier = 4; }
                        else { pointMultiplier = 2; }
                    }
                    int previousScorePlayer1 = Player1.score;
                    Player1.score += tilesJustCapturedThisTurnPlayer1 * pointMultiplier;
                    if (Player1.score >= 50 && previousScorePlayer1 < 50) { Player1.powerUps++; }
                    if (Player1.score >= 70 && previousScorePlayer1 < 70) { Player1.powerUps++; }
                    if (Player1.score >= 100 && previousScorePlayer1 < 100) { Player1.powerUps++; }
                    if (Player1.score >= 130 && previousScorePlayer1 < 130) { Player1.powerUps++; }
                    if (Player1.score >= 160 && (Player1.score - previousScorePlayer1) >= 30) { Player1.powerUps++; }
                }
            }
            Player1.isConstructing = false;
        }

        // Player 2 Area Capture
        if (Player2.isAlive && (grid[Player2.y][Player2.x] == 1 || grid[Player2.y][Player2.x] == 4))
        {
            bool trailToSolidifyPlayer2 = false;
            for (int r = 0; r < M; r++)
            {
                for (int c = 0; c < N; c++)
                {
                    if (grid[r][c] == Player2.trailMarkerValue)
                    {
                        trailToSolidifyPlayer2 = true;
                        break;
                    }
                }
                if (trailToSolidifyPlayer2)
                {
                    break;
                }
            }
            if (!trailToSolidifyPlayer2 && Player2.dx == 0 && Player2.dy == 0) 
            {

            }
            else 
            {
                Player2.dx = 0;
                Player2.dy = 0;

                int tilesJustCapturedThisTurnPlayer2 = 0;
                for (int k = 0; k < enemyCount; k++)
                {
                    drop(enemies[k].y / ts, enemies[k].x / ts);
                }
                for (int i = 0; i < M; i++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        if (grid[i][j] == Player2.trailMarkerValue)
                        {
                            grid[i][j] = 4;
                            tilesJustCapturedThisTurnPlayer2++;
                        }
                        else if (grid[i][j] == 0)
                        {
                            grid[i][j] = 4;
                            tilesJustCapturedThisTurnPlayer2++;
                        }
                        else if (grid[i][j] == -1)
                        {
                            grid[i][j] = 0;
                        }
                    }
                }

                if (tilesJustCapturedThisTurnPlayer2 > 0)
                {
                    int pointMultiplier = 1;
                    if (tilesJustCapturedThisTurnPlayer2 > tileCaptureThreshold)
                    {
                        if (bonusCounter < 5) { bonusCounter++; if (bonusCounter == 3) { tileCaptureThreshold = 5; } }
                        if (bonusCounter >= 5) { pointMultiplier = 4; }
                        else { pointMultiplier = 2; }
                    }
                    int previousScorePlayer2 = Player2.score;
                    Player2.score += tilesJustCapturedThisTurnPlayer2 * pointMultiplier;
                    if (Player2.score >= 50 && previousScorePlayer2 < 50) { Player2.powerUps++; }
                    if (Player2.score >= 70 && previousScorePlayer2 < 70) { Player2.powerUps++; }
                    if (Player2.score >= 100 && previousScorePlayer2 < 100) { Player2.powerUps++; }
                    if (Player2.score >= 130 && previousScorePlayer2 < 130) { Player2.powerUps++; }
                    if (Player2.score >= 160 && (Player2.score - previousScorePlayer2) >= 30) { Player2.powerUps++; }
                }
            }
            Player2.isConstructing = false;
        }

        // Enemy collision with trails (final check after player moves and before drawing)
        for (int i = 0; i < enemyCount; i++)
        {
            if (Player1.isAlive && grid[enemies[i].y / ts][enemies[i].x / ts] == Player1.trailMarkerValue)
            {
                Player1.isAlive = false;
                for (int r = 0; r < M; r++) { for (int c = 0; c < N; c++) { if (grid[r][c] == Player1.trailMarkerValue) { grid[r][c] = 0; } } }
                Player1.isConstructing = false;
            }
            if (Player2.isAlive && grid[enemies[i].y / ts][enemies[i].x / ts] == Player2.trailMarkerValue)
            {
                Player2.isAlive = false;
                for (int r = 0; r < M; r++) { for (int c = 0; c < N; c++) { if (grid[r][c] == Player2.trailMarkerValue) { grid[r][c] = 0; } } }
                Player2.isConstructing = false;
            }
        }

        isGame = Player1.isAlive || Player2.isAlive;

        window.clear();
        window.draw(BackgroundSprite);

        player1ScoreText.setString("Score: " + to_string(Player1.score));
        window.draw(player1ScoreText);
        player1PowerUpText.setString(to_string(Player1.powerUps));
        window.draw(player1PowerUpText);
        window.draw(player1NameText);

        player2ScoreText.setString("Score: " + to_string(Player2.score));
        window.draw(player2ScoreText);
        player2PowerUpText.setString(to_string(Player2.powerUps));
        window.draw(player2PowerUpText);
        window.draw(player2NameText);

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (grid[i][j] == 0)
                {
                    continue;
                }

                bool tileIsDrawn = true;
                if (grid[i][j] == 1)
                {
                    sTile.setTextureRect(IntRect(0, 0, ts, ts));
                }
                else if (grid[i][j] == Player1.trailMarkerValue && Player1.isAlive)
                {
                    sTile.setTextureRect(IntRect(64, 0, ts, ts));
                }
                else if (grid[i][j] == Player2.trailMarkerValue && Player2.isAlive)
                {
                    sTile.setTextureRect(IntRect(128, 0, ts, ts));
                }
                else if (grid[i][j] == 4)
                {
                    sTile.setTextureRect(IntRect(160, 0, ts, ts));
                }
                else if (grid[i][j] == Player1.trailMarkerValue && !Player1.isAlive)
                {
                    tileIsDrawn = false;
                }
                else if (grid[i][j] == Player2.trailMarkerValue && !Player2.isAlive)
                {
                    tileIsDrawn = false;
                }
                else
                {
                    tileIsDrawn = false;
                }

                if (tileIsDrawn)
                {
                    sTile.setPosition(j * ts, i * ts + 112);
                    window.draw(sTile);
                }
            }
        }

        if (Player1.isAlive)
        {
            sTile.setTextureRect(IntRect(32, 0, ts, ts));
            sTile.setPosition(Player1.x * ts, Player1.y * ts + 112);
            window.draw(sTile);
        }
        else
        {
            Player1.x = -100;
            Player1.y = -100;
        }
        if (Player2.isAlive)
        {
            sTile.setTextureRect(IntRect(96, 0, ts, ts));
            sTile.setPosition(Player2.x * ts, Player2.y * ts + 112);
            window.draw(sTile);
        }
        else
        {
            Player2.x = -200;
            Player2.y = -200;
        }

        for (int i = 0; i < enemyCount; i++)
        {
            enemies[i].draw(window);
        }

        if (!isGame)
        {
            sleep(seconds(1));
            int GameOverMenuChoice = MultiPlayerGameOverMenu();
            if (GameOverMenuChoice == 0)
                InitializeMultiPlayer();
            else if (GameOverMenuChoice == 1)
                isMainMenuCalled = true;
        }
        window.display();
    }
    return 0;
}

int Game::multiPlayerPauseMenu()
{
    Texture PauseMenuTexture[3];
    Sprite PauseMenuSprite[3];
    int themeID = menuHandler.getPlayerThemeID();
    string filePath = "../Themes/Theme" + to_string(themeID);
    for (int i = 0; i < 3; i++)
    {
        PauseMenuTexture[i].loadFromFile(filePath + "/MultiPlayerPauseMenu" + to_string(i + 1) + ".png");
        PauseMenuSprite[i].setTexture(PauseMenuTexture[i]);
        PauseMenuSprite[i].setPosition(0, 0);
    }
    int selectedIndex = 0;
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                keyPressSound.getSound().play();
                window.close();
                return -1;
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                keyPressSound.getSound().play();
                return 0;
            }

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                {
                    keyPressSound.getSound().play();
                    selectedIndex = (selectedIndex - 1 + 3) % 3;
                }
                else if (event.key.code == Keyboard::Down)
                {
                    keyPressSound.getSound().play();
                    selectedIndex = (selectedIndex + 1) % 3;
                }
                else if (event.key.code == Keyboard::Enter)
                {
                    keyPressSound.getSound().play();
                    return selectedIndex;
                }
            }
        }
        window.clear(Color::White);
        window.draw(PauseMenuSprite[selectedIndex]);
        window.display();
    }
    return 0;
}

int Game::multiPlayerMenu()
{
    loadPlayers();
    int selectedMenu = 0;
    int numberOfFriends = playerCount;
    int choice = 0;
    Text infoText;
    infoText.setFont(font);
    infoText.setFillColor(Color::White);
    infoText.setStyle(Text::Bold);
    FloatRect AddFriendsBoxBounds(393, 155, 238, 105);
    FloatRect PendingBoxBounds(671, 155, 236, 103);
    Texture CancelIconTexture;
    if (!CancelIconTexture.loadFromFile("../images/cancel.png"))
    {
        cout << "Error loading Cancel Icon texture" << endl;
    }
    Sprite CancelIconSprite;
    CancelIconSprite.setTexture(CancelIconTexture);
    CancelIconSprite.setPosition(830, 84);
    CancelIconSprite.setScale(0.9f, 0.9f); // Scale down the icon
    string path = "../Themes/Theme" + to_string(menuHandler.getPlayerThemeID());
    Texture MultiplayerMenuTextures;
    MultiplayerMenuTextures.loadFromFile(path + "/MultiPlayerMenu.png");
    Sprite MultiPlayerMenuSprites;
    MultiPlayerMenuSprites.setTexture(MultiplayerMenuTextures);

    if (numberOfFriends == 0) 
    {
        while (window.isOpen()) 
        {
            window.clear();
            infoText.setCharacterSize(80);
            infoText.setPosition(170, 350);
            infoText.setString("NO Available Players");
            window.draw(MultiPlayerMenuSprites);
            window.draw(infoText);
            window.draw(CancelIconSprite);
            window.display();


            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                    return -1;
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                    return -1;

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {

                    Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (CancelIconSprite.getGlobalBounds().contains(mousePos))
                    {
                        keyPressSound.play();
                        return -1;
                    }
                }
            }
        }
        return -1;
    }

    // Prepare containers
    Texture* playerRectangleTexture = new Texture[numberOfFriends];
    Sprite* playerRectangleSprite = new Sprite[numberOfFriends];
    Texture* profileIconTexture = new Texture[numberOfFriends];
    Sprite* profileIconSprite = new Sprite[numberOfFriends];
    Text* usernameText = new Text[numberOfFriends];
    Texture * playIconTexture = new Texture[numberOfFriends];
    Sprite * playIconSprite = new Sprite[numberOfFriends];

    int index = 0;
    while (index < numberOfFriends) 
    {
        playIconTexture[index].loadFromFile("../images/Play.png");
        playIconSprite[index].setTexture(playIconTexture[index]);

        playerRectangleTexture[index].loadFromFile("../images/PlayerRectangle.jpg");
        playerRectangleSprite[index].setTexture(playerRectangleTexture[index]);
        
        string iconPath = "../PlayerProfiles/p" + to_string(head[index].getProfileIcon()) + ".png";
        profileIconTexture[index].loadFromFile(iconPath);
        profileIconSprite[index].setTexture(profileIconTexture[index]);

        usernameText[index].setFont(font);
        usernameText[index].setCharacterSize(40);
        usernameText[index].setFillColor(Color::White);
        usernameText[index].setStyle(Text::Bold);
        usernameText[index].setString(head[index].username);
        index++;
    }

    int scrollIndex = 0;
    Texture upIconTexture, downIconTexture;
    upIconTexture.loadFromFile("../images/UpIcon.png");
    downIconTexture.loadFromFile("../images/DownIcon.png");
    Sprite upIconSprite(upIconTexture), downIconSprite(downIconTexture);
    upIconSprite.setPosition(770, 375);
    downIconSprite.setPosition(770, 495);

    while (window.isOpen()) {
        window.clear();
        window.draw(MultiPlayerMenuSprites);
        window.draw(CancelIconSprite);

        if (numberOfFriends > 2) 
        {
            window.draw(upIconSprite);
            window.draw(downIconSprite);
        }

        if (scrollIndex < numberOfFriends) 
        {
            playerRectangleSprite[scrollIndex].setPosition(210, 345);
            profileIconSprite[scrollIndex].setPosition(230, 355);
            usernameText[scrollIndex].setPosition(370, 395);
            playIconSprite[scrollIndex].setPosition(590, 390);
            window.draw(playerRectangleSprite[scrollIndex]);
            window.draw(profileIconSprite[scrollIndex]);
            window.draw(usernameText[scrollIndex]);
            window.draw(playIconSprite[scrollIndex]);
        }

        if (scrollIndex + 1 < numberOfFriends) 
        {
            playerRectangleSprite[scrollIndex + 1].setPosition(210, 495);
            profileIconSprite[scrollIndex + 1].setPosition(230, 505);
            usernameText[scrollIndex + 1].setPosition(370, 545);
            playIconSprite[scrollIndex + 1].setPosition(590, 545);
            window.draw(playerRectangleSprite[scrollIndex + 1]);
            window.draw(profileIconSprite[scrollIndex + 1]);
            window.draw(usernameText[scrollIndex + 1]);
            window.draw(playIconSprite[scrollIndex + 1]);
        }

        else
        {
            infoText.setCharacterSize(60);
            infoText.setPosition(270, 500);
            infoText.setString("Players list ends.");
            window.draw(infoText);
        }

        window.display();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) 
            {
                window.close();
                return -1;
            }

            if (event.type == Event::KeyPressed) 
            {
                if (event.key.code == Keyboard::Escape) 
                {
                    return -1;
                }
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                cout << "Mouse position: " << mousePos.x << ", " << mousePos.y << endl;
                if (upIconSprite.getGlobalBounds().contains(mousePos) && scrollIndex > 0) 
                {
                    scrollIndex--;
                    keyPressSound.play();
                }
                else if (downIconSprite.getGlobalBounds().contains(mousePos) && scrollIndex + 2 < numberOfFriends) 
                {
                    scrollIndex++;
                    keyPressSound.play();
                }
                else if (CancelIconSprite.getGlobalBounds().contains(mousePos))
                {
                    keyPressSound.play();
                    return -1;
                }
                else if (playIconSprite[scrollIndex].getGlobalBounds().contains(mousePos))
                {
					keyPressSound.play();
					Player2.userName = head[scrollIndex].username;
                    return 1; // Proceed to multiplayer game
                }
                if (scrollIndex + 1 < numberOfFriends)
                {
                    if (playIconSprite[scrollIndex + 1].getGlobalBounds().contains(mousePos))
                    {
                        keyPressSound.play();
                        Player2.userName = head[scrollIndex + 1].username;
                        return 1; // Proceed to multiplayer game
                    }
                }
            }
        }
    }

    delete[] playerRectangleTexture;
    delete[] playerRectangleSprite;
    delete[] profileIconTexture;
    delete[] profileIconSprite;
    delete[] usernameText;
    return -1;
}

int Game::MultiPlayerGameOverMenu()
{
    Texture GameOverMenuTexture[2];
    Sprite GameOverMenuSprite[2];
    int themeID = menuHandler.getPlayerThemeID();
    string filePath = "../Themes/Theme" + to_string(themeID);
    for (int i = 0; i < 2; i++)
    {
        GameOverMenuTexture[i].loadFromFile(filePath + "/MultiPlayerGameOverMenu" + to_string(i + 1) + ".png");
        GameOverMenuSprite[i].setTexture(GameOverMenuTexture[i]);
        GameOverMenuSprite[i].setPosition(0, 0);
    }
    int selectedIndex = 0;

    Text MatchResult;
    MatchResult.setFont(font);
    MatchResult.setCharacterSize(45);
    MatchResult.setFillColor(Color::White);
    MatchResult.setStyle(Text::Bold);

    int TotalNewScore = Player1.score + Player1.getPlayerScore();
    Player1.setPlayerScore(TotalNewScore);
    Player1.setPlayerPowerups(Player1.powerUps);
    int TotalNewScore2 = Player2.score + Player2.getPlayerScore();
    Player2.setPlayerScore(TotalNewScore2);
    Player2.setPlayerPowerups(Player2.powerUps);
    int OldHighScore = Player1.getPlayerHighScore();
    if (Player1.score > OldHighScore)
    {
        Player1.setPlayerHighScore(Player1.score);
    }
    OldHighScore = Player2.getPlayerHighScore();
    if (Player2.score > OldHighScore)
    {
		Player2.setPlayerHighScore(Player2.score);
	}
    if (Player1.score > Player2.score)
    {
        Player1.updateWins();
        MatchResult.setPosition(300, 420);
        MatchResult.setString("Winner: " + Player1.userName);

    }
    else if (Player2.score > Player1.score)
    {
		Player2.updateWins();
        MatchResult.setPosition(300, 420);
		MatchResult.setString("Winner: " + Player2.userName);
	}
    else
    {
        MatchResult.setPosition(380, 420);
		MatchResult.setString("It's a Tie!");
	}

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                keyPressSound.getSound().play();
                window.close();
                return -1;
            }

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                {
                    keyPressSound.getSound().play();
                    selectedIndex = (selectedIndex - 1 + 2) % 2;
                }
                else if (event.key.code == Keyboard::Down)
                {
                    keyPressSound.getSound().play();
                    selectedIndex = (selectedIndex + 1) % 2;
                }
                else if (event.key.code == Keyboard::Enter)
                {
                    keyPressSound.getSound().play();
                    return selectedIndex;
                }
            }
        }

        window.clear(Color::White);
        window.draw(GameOverMenuSprite[selectedIndex]);
        window.draw(MatchResult);
        window.display();
    }
    return 0;
}

int main()
{
	RenderWindow window(VideoMode(1024, 720), "Snake Game");
	window.setFramerateLimit(60);
	Game game(window);
	game.run();
	return 0;
}

