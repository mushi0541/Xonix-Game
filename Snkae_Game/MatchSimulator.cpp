#include "MatchSimulator.h"
#include <iostream>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Authentication.h"
using namespace std;
using namespace sf;

void PriorityQueue::insert(MatchPlayer p)
{
	for (int i = 0; i < size; i++) {
		if (data[i].userName == p.userName) return; 
	}

	if(username== p.userName) return; // Prevent inserting the current player's own data

	p.arrivalTime = currentTime++; 
	data[size] = p;

	int i = size;
	while (i > 0) {
		int parent = (i - 1) / 2;

		if (data[i].score > data[parent].score ||
			(data[i].score == data[parent].score && data[i].arrivalTime < data[parent].arrivalTime)) 
		{
			swap(data[i], data[parent]);
			i = parent;
		}
		else {
			break;
		}
	}
	size++;
}

void PriorityQueue::deletePlayer(MatchPlayer p)
{
	for (int i = 0; i < size; i++) 
	{
		if (data[i].userName == p.userName) 
		{
			data[i] = data[size - 1];
			size--;
			int j = i;
			while (j < size) {
				int leftChild = 2 * j + 1;
				int rightChild = 2 * j + 2;
				int largest = j;
				if (leftChild < size && data[leftChild].score > data[largest].score) 
				{
					largest = leftChild;
				}
				if (rightChild < size && data[rightChild].score > data[largest].score) 
				{
					largest = rightChild;
				}
				if (largest != j) 
				{
					swap(data[j], data[largest]);
					j = largest;
				}
				else 
				{
					break;
				}
			}
			break;
		}
	}
}


MatchPlayer PriorityQueue::extractMax()
{
	if (size == 0) {
		throw runtime_error("Queue is empty");
	}
	MatchPlayer maxPlayer = data[0];
	data[0] = data[size - 1];
	size--;
	int i = 0;
	while (i < size) 
	{
		int leftChild = 2 * i + 1;
		int rightChild = 2 * i + 2;
		int largest = i;
		if (leftChild < size && data[leftChild].score > data[largest].score) 
		{
			largest = leftChild;
		}
		if (rightChild < size && data[rightChild].score > data[largest].score) 
		{
			largest = rightChild;
		}
		if (largest != i)
		{
			swap(data[i], data[largest]);
			i = largest;
		}
		else 
		{
			break;
		}
	}
	return maxPlayer;
}


void PriorityQueue::display(sf::RenderWindow& window, sf::Font& font)
{
	for (int i = 0; i < size; i++)
	{
		Text text;
		text.setFont(font);
		text.setString(data[i].userName + " " + to_string(data[i].score));
		text.setCharacterSize(20);
		text.setFillColor(Color::White);
		text.setPosition(10, 10 + i * 30);
		window.draw(text);
	}
}

void GameRoomQueue::enqueue(GameRoom g)
{
	data[rear] = g;
	rear = (rear + 1) % 250;
}

GameRoom GameRoomQueue::dequeue()
{
	if (front == rear) 
	{
		cout<<"Queue is empty" << endl;
	}
	GameRoom g = data[front];
	front = (front + 1) % 250;
	return g;
}

void GameRoomQueue::display(RenderWindow& window,Font& font)
{
	for (int i = front; i != rear; i = (i + 1) % 250)
	{
		Text text;
		text.setFont(font);
		text.setString(data[i].player1.userName + " vs " + data[i].player2.userName);
		text.setCharacterSize(20);
		text.setFillColor(Color::White);
		text.setPosition(10, 10 + (i - front) * 30);
		window.draw(text);
	}
}


MatchSimulator::MatchSimulator(RenderWindow& window,Authentication& a) : auth(a),window(window)
{
	loaded = false;
	pendingPlayer = nullptr;
	playerQueue.username = auth.getUserName();
	font.loadFromFile("../Font2/Category.ttf");
	keyPressSound.loadFromFile("../Sound/Key.wav");
	loadPlayers();
	
}


int MatchSimulator::getScoreFromFile(const string& username) 
{
	ifstream file("../PlayersData/" + username + ".txt");
	string line;
	int score = 0;
	for (int i = 0; i < 4 && getline(file, line); ++i) {
		if (i == 3) {
			score = stoi(line.substr(line.find(":") + 2));
		}
	}
	file.close();
	return score;
}

void MatchSimulator::loadPlayers()
{
	if (loaded) return;
	ifstream file("../GameUsers.txt");
	if (!file.is_open()) {
		cout << "Error opening file.\n";
		return;
	}

	string line;
	while (getline(file, line)) {
		string username = line.substr(0, line.find(" "));
		int score = getScoreFromFile(username);
		playerQueue.insert(MatchPlayer(username, score));
	}
	file.close();
	loaded = true;
}

void MatchSimulator::draw()
{
	gameRooms.display(window,font);
	playerQueue.display(window,font);
}

void MatchSimulator::updatePlayerScoreFile(const MatchPlayer& p)
{
	string path = "../PlayersData/" + p.userName + ".txt";
	ifstream in(path);
	ofstream out("../PlayersData/temp.txt");
	string line;
	int lineNo = 0;

	while (getline(in, line)) {
		if (lineNo == 3)
			out << "Score: " << p.score << endl;
		else
			out << line << endl;
		lineNo++;
	}

	in.close();
	out.close();
	remove(path.c_str());
	rename("../PlayersData/temp.txt", path.c_str());
}

void MatchSimulator::updateWins(const MatchPlayer& p)
{
	string path = "../PlayersData/" + p.userName + ".txt";
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

string MatchSimulator::simulateMatchesAndUpdateScores()
{
		GameRoom room = gameRooms.dequeue();

		cout << "Match: " << room.player1.userName << " vs " << room.player2.userName << endl;

		// Simulate match logic (simple random increase)
		int p1Delta = rand() % 10 + 1;
		int p2Delta = rand() % 10 + 1;
		room.player1.score += p1Delta;
		room.player2.score += p2Delta;

		cout << "Updated Scores: " << room.player1.userName << " +" << p1Delta
			<< " | " << room.player2.userName << " +" << p2Delta << endl;

		// Update the score file
		updatePlayerScoreFile(room.player1);
		updatePlayerScoreFile(room.player2);

		string winner;
		if (room.player1.score > room.player2.score)
		{
			updateWins(room.player1);
			winner ="Winner: "+ room.player1.userName;
		}
		else if (room.player2.score > room.player1.score)
		{
			updateWins(room.player2);
		    winner ="Winner: " + room.player2.userName;
		}
		else
		{
			winner = "Match Draw";
		}
		playerQueue.insert(room.player1);
		playerQueue.insert(room.player2);
		return winner;
}

int MatchSimulator::matchSimulatorMenu(int PlayerThemeID)
{
	String themePath = "../Themes/Theme" + to_string(PlayerThemeID) + "/MatchSimulation.png";
	int selectedIndex = 0;
	Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile(themePath))
	{
		cout << "Error loading background texture." << endl;
		return -1;
	}
	Sprite backgroundSprite(backgroundTexture);
	backgroundSprite.setPosition(0, 0);
	Texture CancelIconTexture;
	if (!CancelIconTexture.loadFromFile("../images/cancel.png"))
	{
		cout << "Error loading Cancel Icon texture" << endl;
	}
	Sprite CancelIconSprite;
	CancelIconSprite.setTexture(CancelIconTexture);
	CancelIconSprite.setPosition(940, 10);
	CancelIconSprite.scale(0.6f, 0.6f);

	bool isSimulationRunning = true;
	bool isPendingPlayers = false;
	string noPendingPlayersText = "No Waiting Players";

	Text noPendingPlayers;
	noPendingPlayers.setFont(font);
	noPendingPlayers.setFillColor(Color::White);
	noPendingPlayers.setString(noPendingPlayersText);
	noPendingPlayers.setCharacterSize(30);
	noPendingPlayers.setPosition(130, 300);
	noPendingPlayers.setStyle(Text::Bold);

	Texture PendingPlayersRect[2];
	if (!PendingPlayersRect[0].loadFromFile("../images/PlayerRectangle.jpg"))
	{
		cout << "Error loading Pending Player texture." << endl;
		return -1;
	}
	if (!PendingPlayersRect[1].loadFromFile("../images/PlayerRectangle.jpg"))
	{
		cout << "Error loading Matched Player texture." << endl;
		return -1;
	}

	Sprite PendingPlayersSprite[2];
	PendingPlayersSprite[0].setTexture(PendingPlayersRect[0]);
	PendingPlayersSprite[1].setTexture(PendingPlayersRect[1]);
	PendingPlayersSprite[0].scale(0.75f, 0.75f);
	PendingPlayersSprite[1].scale(0.75f, 0.75f);
	Texture WaitingIconTexture[2];
	if (!WaitingIconTexture[0].loadFromFile("../images/waiting.png"))
	{
		cout << "Error loading Waiting Icon texture." << endl;
		return -1;
	}

	if (!WaitingIconTexture[1].loadFromFile("../images/waiting.png"))
	{
		cout << "Error loading Waiting Icon texture." << endl;
		return -1;
	}
	Sprite WaitingIconSprite[2];
	WaitingIconSprite[0].setTexture(WaitingIconTexture[0]);
	WaitingIconSprite[1].setTexture(WaitingIconTexture[1]);

	Text UserNameText[2];
	UserNameText[0].setFont(font);
	UserNameText[0].setFillColor(Color::White);
	UserNameText[0].setCharacterSize(40);
	UserNameText[0].setStyle(Text::Bold);
	UserNameText[0].setString(auth.getUserName());
	UserNameText[1].setFont(font);
	UserNameText[1].setFillColor(Color::White);
	UserNameText[1].setCharacterSize(40);
	UserNameText[1].setStyle(Text::Bold);


	while (isSimulationRunning)
	{
		bool isMatchEnded = false;
		int PlayingPlayers = playerQueue.getSize();
		if (PlayingPlayers % 2 != 0)
		{
			PlayingPlayers--;
		}

		Texture * PlayerBoxTexture = new Texture[PlayingPlayers/2];
		Sprite * PlayerBoxSprite = new Sprite[PlayingPlayers/2];
		Text* userNameText = new Text[PlayingPlayers/2];
		//here every player will have a different clock so do it accordingly
		Clock* playerClock = new Clock[PlayingPlayers];
		Time* playerTime = new Time[PlayingPlayers];

		for (int i = 0; i < PlayingPlayers/2; i++)
		{
			PlayerBoxTexture[i].loadFromFile("../images/PlayerRectangle.jpg");
			PlayerBoxSprite[i].setTexture(PlayerBoxTexture[i]);
			PlayerBoxSprite[i].scale(0.6f, 0.6f);
			userNameText[i].setFont(font);
			userNameText[i].setFillColor(Color::White);
			userNameText[i].setCharacterSize(25);
			userNameText[i].setStyle(Text::Bold);
		}

		// Scroll controls
		int scrollIndex = 0;
		Texture upIconTexture, downIconTexture;
		upIconTexture.loadFromFile("../images/UpIcon.png");
		downIconTexture.loadFromFile("../images/DownIcon.png");
		Sprite upIconSprite(upIconTexture), downIconSprite(downIconTexture);
		upIconSprite.setPosition(880, 290);
		downIconSprite.setPosition(880, 410);

		int index = 0;
		while (playerQueue.getSize() >= 2)
		{
			MatchPlayer player1;
			if (pendingPlayer)
			{
				player1 = *pendingPlayer;
				playerQueue.deletePlayer(*pendingPlayer);
				pendingPlayer = nullptr;
			}
			else
			{
				player1 = playerQueue.extractMax();
			}
			MatchPlayer player2 = playerQueue.extractMax();
			userNameText[index].setString(player1.userName + "  VS  " + player2.userName);
			GameRoom room;
			room.player1 = player1;
			room.player2 = player2;
			index++;
			gameRooms.enqueue(room);
		}

		if (playerQueue.getSize() == 1)
		{
			MatchPlayer leftover = playerQueue.extractMax();
			UserNameText[1].setString(leftover.userName);
			pendingPlayer = &leftover;
			playerQueue.insert(leftover);  
		}

		Clock matchClock;
		int MAX_TIME = 15; // seconds
		int elapsedTime = 0;
		Text timerText;
		timerText.setFont(font);
		timerText.setCharacterSize(26);
		timerText.setFillColor(Color::White);
		timerText.setPosition(540, 195);
		Clock resultClock;
		int ResultTime = 18;
		int resultElapsedTime = 0;
		while (window.isOpen() && isSimulationRunning)
		{
			window.clear();
			window.draw(backgroundSprite);
			window.draw(CancelIconSprite);

			if (PlayingPlayers / 2 > 2)
			{
				window.draw(upIconSprite);
				window.draw(downIconSprite);
			}


			if (!playerQueue.isEmpty())
			{
				isPendingPlayers = true;
			}
			else
			{
				isPendingPlayers = false;
			}

			elapsedTime = matchClock.getElapsedTime().asSeconds();
			if (elapsedTime >= MAX_TIME)
			{
				isMatchEnded = true;
				int index = 0;
				while (!gameRooms.isEmpty() && index < PlayingPlayers / 2)
				{
					// Simulate the match and update scores
					string result = simulateMatchesAndUpdateScores();
					userNameText[index].setString(result);
					index++;
				}
			}

			

			if (scrollIndex < PlayingPlayers / 2)
			{
				//show the timer after which matches will ends 
				
				PlayerBoxSprite[scrollIndex].setPosition(530, 245);
				userNameText[scrollIndex].setPosition(555, 265);
				timerText.setString("Match Ending in: " + to_string(MAX_TIME - elapsedTime) + " seconds");
				if(!isMatchEnded)
				window.draw(timerText);
				window.draw(PlayerBoxSprite[scrollIndex]);
				window.draw(userNameText[scrollIndex]);
			}

			if (scrollIndex + 1 < PlayingPlayers / 2)
			{
				PlayerBoxSprite[scrollIndex + 1].setPosition(530, 445);
				userNameText[scrollIndex + 1].setPosition(555, 465);
				window.draw(PlayerBoxSprite[scrollIndex + 1]);
				window.draw(userNameText[scrollIndex + 1]);
			}


			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
					window.close();
				if (event.type == Event::MouseButtonPressed)
				{
					if (event.mouseButton.button == Mouse::Left)
					{
						Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
						if (CancelIconSprite.getGlobalBounds().contains(mousePos))
						{
							keyPressSound.play();
							isSimulationRunning = false;
							break;
						}
						if (upIconSprite.getGlobalBounds().contains(mousePos) && scrollIndex > 0) 
						{
							scrollIndex--;
							keyPressSound.play();
						}
						else if (downIconSprite.getGlobalBounds().contains(mousePos) && scrollIndex + 2 < PlayingPlayers/2) 
						{
							scrollIndex++;
							keyPressSound.play();
						}
					}
				}

				if (event.type == Event::KeyPressed)
				{
					if (event.key.code == Keyboard::Escape)
					{
						keyPressSound.play();
						isSimulationRunning = false;
						break;
					}
				}

			}

			if (isPendingPlayers && !isMatchEnded)
			{
				PendingPlayersSprite[0].setPosition(80, 230);
				UserNameText[0].setPosition(105, 250);
				WaitingIconSprite[0].setPosition(380, 230);
				PendingPlayersSprite[1].setPosition(80, 430);
				UserNameText[1].setPosition(105, 450);
				WaitingIconSprite[1].setPosition(380, 430);
				window.draw(PendingPlayersSprite[0]);
				window.draw(UserNameText[0]);
				window.draw(PendingPlayersSprite[1]);
				window.draw(UserNameText[1]);
				window.draw(WaitingIconSprite[0]);
				window.draw(WaitingIconSprite[1]);
			}
			if (!isPendingPlayers || isMatchEnded)
			{
				PendingPlayersSprite[0].setPosition(80, 230);
				UserNameText[0].setPosition(105, 250);
				WaitingIconSprite[0].setPosition(380, 230);
				window.draw(PendingPlayersSprite[0]);
				window.draw(WaitingIconSprite[0]);
				window.draw(UserNameText[0]);
			}


			resultElapsedTime = resultClock.getElapsedTime().asSeconds();
			if (isMatchEnded && resultElapsedTime >= ResultTime)
			{
				window.clear();
				keyPressSound.play();
				break;
			}
			window.display();
		}
	}
	return -2;
}


