#include "Inventory.h"
#include <iostream>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace	std;
using namespace sf;


int AVLTree::height(AVLNode* node)
{
	if (node == nullptr)
	{
		return 0;
	}
	return node->height;
}

int AVLTree::getBalance(AVLNode* node)
{
	if (node == nullptr)
	{
		return 0;
	}
	return height(node->left) - height(node->right);
}

AVLNode* AVLTree::rightRotate(AVLNode* y)
{
	AVLNode* x = y->left;
	AVLNode* T2 = x->right;

	x->right = y;
	y->left = T2;

	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

	return x;
}

AVLNode* AVLTree::leftRotate(AVLNode* x)
{
	AVLNode* y = x->right;
	AVLNode* T2 = y->left;

	y->left = x;
	x->right = T2;

	x->height = max(height(x->left), height(x->right)) + 1;
	y->height = max(height(y->left), height(y->right)) + 1;

	return y;
}

AVLNode* AVLTree::insert(AVLNode* node, Theme t)
{
	if (node == nullptr)
	{
		return new AVLNode(t);
	}

	if (t.themeID < node->data.themeID)
	{
		node->left = insert(node->left, t);
	}
	else if (t.themeID > node->data.themeID)
	{
		node->right = insert(node->right, t);
	}
	else
	{
		return node; // Duplicate keys are not allowed
	}

	node->height = 1 + max(height(node->left), height(node->right));

	int balance = getBalance(node);

	if (balance > 1 && t.themeID < node->left->data.themeID)
	{
		return rightRotate(node);
	}

	if (balance < -1 && t.themeID > node->right->data.themeID)
	{
		return leftRotate(node);
	}

	if (balance > 1 && t.themeID > node->left->data.themeID)
	{
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}

	if (balance < -1 && t.themeID < node->right->data.themeID)
	{
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}

	return node;
}


AVLNode* AVLTree::searchByID(AVLNode* node, int id)
{
	if (node == nullptr || node->data.themeID == id)
	{
		return node;
	}

	if (id < node->data.themeID)
	{
		return searchByID(node->left, id);
	}
	else
	{
		return searchByID(node->right, id);
	}
}

void AVLTree::insert(Theme t)
{
	root = insert(root, t);
}


Inventory::Inventory(RenderWindow& window, string username) : window(window), currentPlayerUsername(username)
{
	keyPressSound.loadFromFile("../Sound/Key.wav");
	if (!font.loadFromFile("../Font2/Category.ttf"))
	{

	}
	loadThemes();
}

void Inventory::loadThemes()
{
	Theme themes[5] = 
	{
		{1, "Ocean Blue", "Cool ocean tones", "../Themes/Theme1/"},
		{2, "Sunset Blaze", "Warm sunset colors", "../Themes/Theme2/"},
		{3, "Forest Whisper", "Green forest style", "../Themes/Theme3/"},
		{4, "Cyber Pulse", "Futuristic vibe", "../Themes/Theme4/"},
		{5, "Classic Mode", "Default visual style", "../Themes/Theme5/"}
	};

	for (int i = 0; i < 5; i++)
	{
		tree.insert(themes[i]);
	}

	int id = loadPlayerTheme();
	if (id != 0)
	{
		selectThemeByID(id);
	}
	else
	{
		selectedTheme = themes[0]; 
	}
}

void Inventory::savePlayerTheme(int themeID)
{
	string fileName = "../PlayersData/" + currentPlayerUsername + ".txt";
	ifstream file(fileName);
	if (!file)
	{
		cout << "Error loading user file." << endl;
		return;
	}

	string line;
	int lineNo = 0;
	string lines[100];

	while (getline(file, line))
	{
		lines[lineNo] = line;
		lineNo++;
	}
	file.close();
	lines[5] = "ThemeID: " + to_string(themeID); 
	ofstream outFile(fileName);
	if (!outFile)
	{
		cout << "Error opening user file for writing." << endl;
		return;
	}
	for (int i = 0; i < lineNo; i++)
	{
		outFile << lines[i] << endl;
	}
	outFile.close();
}

int Inventory::loadPlayerTheme()
{
	string fileName = "../PlayersData/" + currentPlayerUsername + ".txt";
	ifstream file(fileName);
	if (!file)
	{
		cout << "Error loading user file." << endl;
		return -1;
	}

	string line;
	int lineNo = 0;
	int themeID = 0;
	while (getline(file, line))
	{
		if (line.find("ThemeID: ") != string::npos)
		{
			themeID = stoi(line.substr(line.find(":") + 2));
			break;
		}
		lineNo++;
	}
	file.close();
	return themeID;
}

bool Inventory::selectThemeByID(int themeID)
{
	AVLNode* node = tree.searchByID(tree.root, themeID);
	if (node != nullptr)
	{
		selectedTheme = node->data;
		return true;
	}
	else
	{
		cout << "Theme not found!" << endl;
	}
	return false;
}
 
int Inventory::handleInventoryMenu()
{
	string themeMenuPath = selectedTheme.folderPath + "ThemesMenu.png";
	Texture themeMenuTexture;
	if (!themeMenuTexture.loadFromFile(themeMenuPath))
	{
		cout << "Error loading theme menu image." << endl;
		return 0;
	}
	Sprite themeMenuSprite(themeMenuTexture);
	themeMenuSprite.setPosition(0, 0);

	bool showCursor = true;
	Clock cursorClock;
	bool searchTriggered = false;
	string input = "";
	Texture CancelIconTexture, SearchIconTexture, SearchBarTexture;
	CancelIconTexture.loadFromFile("../images/cancel.png");
	SearchIconTexture.loadFromFile("../images/SearchIcon.png");
	SearchBarTexture.loadFromFile("../images/SearchBar.png");
	Sprite CancelIconSprite(CancelIconTexture), SearchIconSprite(SearchIconTexture),
		SearchBarSprite(SearchBarTexture);

	CancelIconSprite.setPosition(830, 50);
	SearchIconSprite.setPosition(288, 281);
	SearchIconSprite.setScale(0.8f, 0.8f);
	SearchBarSprite.setPosition(20, 280);
	SearchBarSprite.setScale(0.6f, 0.6f);
	bool themeBoxSelected = false;
	bool themeFound = false;
	Theme foundTheme;

	RectangleShape themeBox;
	themeBox.setSize(Vector2f(315, 50));
	themeBox.setFillColor(Color::Black);
	themeBox.setOutlineColor(Color::White);
	themeBox.setOutlineThickness(5);
	themeBox.setPosition(20, 360);

	Text themeNameText;
	themeNameText.setFont(font);
	themeNameText.setCharacterSize(20);
	themeNameText.setFillColor(Color(148, 180, 255));
	themeNameText.setStyle(Text::Bold);
	themeNameText.setPosition(23, 370);

	Texture AcceptIconTexture;
	AcceptIconTexture.loadFromFile("../images/Accept.png");
	Sprite AcceptIconSprite(AcceptIconTexture);
	AcceptIconSprite.setPosition(290, 360); 

	Text inputText;
	inputText.setFont(font);
	inputText.setCharacterSize(20);
	inputText.setFillColor(Color::White);
	inputText.setPosition(30, 290);
	inputText.setStyle(Text::Bold);


	Text DescriptionText;
	DescriptionText.setFont(font);
	DescriptionText.setCharacterSize(20);
	DescriptionText.setFillColor(Color::White);
	DescriptionText.setPosition(420, 200);
	DescriptionText.setStyle(Text::Bold);

	bool showError = false;
	string errorMessage = "";
	
	Text PowerupText;
	PowerupText.setFont(font);
	PowerupText.setCharacterSize(20);
	PowerupText.setFillColor(Color::White);
	PowerupText.setPosition(695, 200);
	PowerupText.setStyle(Text::Bold);
	PowerupText.setString("Time Lock: " + to_string(getPlayerPowerups()));

	Clock previewClock;
	float enemyIconAngle = 0.0f;
	float previewRotationSpeed = 90.0f; // degrees per second

	while (window.isOpen()) 
	{
		window.clear();

		// Draw base UI
		window.draw(themeMenuSprite);
		window.draw(CancelIconSprite);
		window.draw(SearchBarSprite);
		window.draw(SearchIconSprite);

		// Blinking cursor logic
		if (cursorClock.getElapsedTime().asSeconds() >= 0.5f) {
			cursorClock.restart();
			showCursor = !showCursor;
		}

		// Display input text with blinking cursor
		string displayText = input + (showCursor ? "_" : "");
		inputText.setString(displayText);
		window.draw(inputText);
		window.draw(PowerupText);

		if (showError) 
		{
			Text errorText;
			errorText.setFont(font);
			if (errorMessage == "Theme Not Found.")
			{
				errorText.setCharacterSize(35);
				errorText.setPosition(13, 410);
			}
			else if (errorMessage == "Theme Already Selected")
			{
				errorText.setCharacterSize(25);
				errorText.setPosition(5, 410);
			}
			errorText.setFillColor(Color::White);
			errorText.setString(errorMessage);
			errorText.setStyle(Text::Bold);
			window.draw(errorText);
		}

		if (themeFound)
		{
			// Highlight if selected
			if (themeBoxSelected)
				themeBox.setOutlineColor(Color::Green);
			else
				themeBox.setOutlineColor(Color::White);

			themeNameText.setString("Theme: " + foundTheme.name);
			DescriptionText.setString(foundTheme.description);
			window.draw(themeBox);
			window.draw(themeNameText);

			Texture themeIconTexture;
			string themeIconPath = foundTheme.folderPath + "ThemesMenu.png";
			themeIconTexture.loadFromFile(themeIconPath);
			Sprite themeIconSprite(themeIconTexture);
			themeIconSprite.setPosition(365, 445);
			themeIconSprite.setScale(0.3f, 0.3f);

			Texture enemyIconTexture;
			string enemyIconPath = foundTheme.folderPath + "enemy.png";
			enemyIconTexture.loadFromFile(enemyIconPath);
			Sprite enemyIconSprite(enemyIconTexture);
			enemyIconSprite.setPosition(700, 445);
			enemyIconSprite.setScale(3.0f, 3.0f);
			enemyIconSprite.setOrigin(enemyIconTexture.getSize().x / 2.f, enemyIconTexture.getSize().y / 2.f);
			enemyIconSprite.setPosition(700 + enemyIconTexture.getSize().x * 1.5f, 445 + enemyIconTexture.getSize().y * 1.5f);

			float dt = previewClock.restart().asSeconds();
			enemyIconAngle += previewRotationSpeed * dt;

			if (enemyIconAngle > 360.f)
				enemyIconAngle -= 360.f;

			enemyIconSprite.setRotation(enemyIconAngle);

			// Show Accept icon only if selected
			if (themeBoxSelected)
			{
				window.draw(themeIconSprite);
				window.draw(enemyIconSprite);
				window.draw(DescriptionText);
				window.draw(AcceptIconSprite);
			}
				
		}

		// --- Handle Events ---
		Event event;
		while (window.pollEvent(event)) 
		{
			if (event.type == Event::Closed) 
			{
				window.close();
				return -1;
			}

			if (event.type == Event::KeyPressed) 
			{
				if (event.key.code == Keyboard::Escape) 
				{
					keyPressSound.play();
					return 0;
				}
				if (event.key.code == Keyboard::BackSpace && !input.empty()) 
				{
					input.pop_back();
					showError = false;
				}
				if (event.key.code == Keyboard::Enter) 
				{
					keyPressSound.play();
					searchTriggered = true;
				}
			}

			if (event.type == Event::TextEntered) 
			{ 
				if(event.text.unicode >= 48 && event.text.unicode <= 57) 
				{
					input += static_cast<char>(event.text.unicode);
					keyPressSound.play();
				}
			}


			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) 
			{
				Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

				if (SearchIconSprite.getGlobalBounds().contains(mousePos))
				{
					keyPressSound.play();
					searchTriggered = true;
				}
				else if (CancelIconSprite.getGlobalBounds().contains(mousePos))
				{
					keyPressSound.play();
					return 0;
				}
				else if (themeBox.getGlobalBounds().contains(mousePos) && themeFound)
				{
					keyPressSound.play();
					themeBoxSelected = true;
				}
				if (themeBoxSelected) 
				{
					window.draw(AcceptIconSprite);

					FloatRect iconBounds = AcceptIconSprite.getGlobalBounds();
					if (iconBounds.contains(mousePos)) {
						keyPressSound.play();
						selectedTheme = foundTheme;
						savePlayerTheme(selectedTheme.themeID);
						return 2;
					}
				}

			}
		}
		if (searchTriggered && input!="")
		{
			searchTriggered = false;
			showError = false;
			themeBoxSelected = false;
			themeFound = false;

			int id = stoi(input);

			if (id == selectedTheme.themeID)
			{
				showError = true;
				errorMessage = "Theme Already Selected";
			}
			else
			{
				AVLNode* result = tree.searchByID(tree.root, id); // Adjust AVL object name
				if (result == nullptr)
				{
					showError = true;
					errorMessage = "Theme Not Found.";
				}
				else
				{
					foundTheme = result->data;
					themeFound = true;
				}
			}

		}

		window.display();
	}
	return 0;
}

int Inventory::getPlayerPowerups()
{
	string fileName = "../PlayersData/" + currentPlayerUsername + ".txt";
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

