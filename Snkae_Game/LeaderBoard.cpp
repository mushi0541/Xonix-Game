#include "LeaderBoard.h"

const string LeaderBoard::leaderboardFile = "../leaderboard.txt";

LeaderBoard::LeaderBoard(RenderWindow& window) : window(window), currentSize(0)
{
    keyPressSound.loadFromFile("../Sound/Key.wav");
}

// Maintain min-heap based on score (heapify up)
void LeaderBoard::heapifyUp(int index) 
{
    while (index > 0) 
    {
        int parentIndex = (index - 1) / 2;
        if (heap[index].score >= heap[parentIndex].score)
            break;
        swap(heap[index], heap[parentIndex]);
        index = parentIndex;
    }
}

// Maintain min-heap based on score (heapify down)
void LeaderBoard::heapifyDown(int index) {
    while (index < currentSize) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int smallest = index;

        if (leftChild < currentSize && heap[leftChild].score < heap[smallest].score)
            smallest = leftChild;
        if (rightChild < currentSize && heap[rightChild].score < heap[smallest].score)
            smallest = rightChild;

        if (smallest == index)
            break;

        swap(heap[index], heap[smallest]);
        index = smallest;
    }
}

// Get score from player's data file
int LeaderBoard::getPlayerScore(const string& playerName) {
    string filename = "../PlayersData/" + playerName + ".txt";
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error loading file for user: " << playerName << endl;
        return -1;
    }

    string line;
    while (getline(file, line)) {
        size_t pos = line.find("Score: ");
        if (pos != string::npos) {
            try {
                int score = stoi(line.substr(pos + 7));
                file.close();
                return score;
            }
            catch (...) {
                cout << "Invalid score format in file: " << filename << endl;
                file.close();
                return -1;
            }
        }
    }

    file.close();
    return 0;
}

// Read all users from file and populate leaderboard
void LeaderBoard::readLeaderboard() {
    currentSize = 0; // Reset leaderboard before loading

    ifstream usersFile("../GameUsers.txt");
    if (!usersFile.is_open()) {
        cout << "Error opening user list file.\n";
        return;
    }

    string playerName;
    while (getline(usersFile, playerName)) {
        int playerScore = getPlayerScore(playerName);
        if (playerScore != -1) {
            LeaderboardEntry entry(playerName, playerScore);
            insertIntoHeap(entry);
        }
    }

    usersFile.close();
}

// Insert new entry into leaderboard min-heap
void LeaderBoard::insertIntoHeap(const LeaderboardEntry& entry) {
    if (currentSize < MAX_ENTRIES) {
        heap[currentSize] = entry;
        heapifyUp(currentSize);
        currentSize++;
    }
    else if (entry.score > heap[0].score) {
        heap[0] = entry;
        heapifyDown(0);
    }
}

void LeaderBoard::writeLeaderboard()
{
    ofstream file(leaderboardFile);
    if (!file.is_open())
    {
        cerr << "Error opening leaderboard file: "
            << leaderboardFile << endl;
        return;
    }
    LeaderboardEntry* temp = new LeaderboardEntry[currentSize];
    for (int i = 0; i < currentSize; ++i)
        temp[i] = heap[i];

    for (int i = 0; i < currentSize - 1; ++i)
    {
        int bestIdx = i;
        for (int j = i + 1; j < currentSize; ++j)
        {
            if (temp[j].score > temp[bestIdx].score)
                bestIdx = j;
        }
        if (bestIdx != i)
        {
            LeaderboardEntry swapTmp = temp[i];
            temp[i] = temp[bestIdx];
            temp[bestIdx] = swapTmp;
        }
    }

    for (int i = 0; i < currentSize; ++i)
        file << temp[i].name << " " << temp[i].score << "\n";

    delete[] temp;
    file.close();
}


void LeaderBoard::displayLeaderboard()
{
    readLeaderboard();
    Texture BackgroundTexture;
    if (!BackgroundTexture.loadFromFile("../images/LeaderBoard.png"))
    {
		cout << "Error loading background texture" << endl;
		return;
	}

    Sprite BackgroundSprite;
    BackgroundSprite.setTexture(BackgroundTexture);
    BackgroundSprite.setPosition(0, 0);
    LeaderboardEntry* temp = new LeaderboardEntry[currentSize];
    for (int i = 0; i < currentSize; ++i) 
    {
        temp[i] = heap[i];
    }

    for (int i = 0; i < currentSize - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < currentSize; ++j) {
            if (temp[j].score > temp[minIdx].score) {
                minIdx = j;
            }
        }
        if (minIdx != i) 
        {
            LeaderboardEntry swapTmp = temp[i];
            temp[i] = temp[minIdx];
            temp[minIdx] = swapTmp;
        }
    }

    writeLeaderboard();
    Texture CancelIconTexture;
    if (!CancelIconTexture.loadFromFile("../images/cancel.png"))
    {
        cout << "Error loading Cancel Icon texture" << endl;
    }
    Sprite CancelIconSprite;
    CancelIconSprite.setTexture(CancelIconTexture);
    CancelIconSprite.setPosition(750, 13);
    CancelIconSprite.setScale(0.9f, 0.9f);

    Font font;
    if (!font.loadFromFile("../Font2/Category.ttf")) 
    {
        cout << "Error loading font." << endl;
        delete[] temp;
        return;
    }

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
					Vector2i mousePos = Mouse::getPosition(window);
                    if (CancelIconSprite.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                       keyPressSound.play();
                       delete[] temp;
                       return;
					}
				}
			}

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                {
					keyPressSound.play();
					delete[] temp;
					return;
				}
			}
        }

        window.clear();
        window.draw(BackgroundSprite);
        window.draw(CancelIconSprite);

        for (int i = 0; i < currentSize; ++i)
        {
            Text entryText;
            entryText.setFont(font);
            entryText.setCharacterSize(24);
            entryText.setFillColor(Color(148, 180, 255));
            entryText.setString(temp[i].name);
            entryText.setPosition(393, 210 + i * 45.f);
            entryText.setStyle(Text::Bold);
            Text ScoreText;
            ScoreText.setFont(font);
            ScoreText.setCharacterSize(24);
            ScoreText.setFillColor(Color(148, 180, 255));
            ScoreText.setString(to_string(temp[i].score));
            ScoreText.setPosition(617, 210 + i * 45.f);
            ScoreText.setStyle(Text::Bold);
            window.draw(entryText);
            window.draw(ScoreText);
        }

       
        window.display();
    }

    delete[] temp;
}


