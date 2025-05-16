#include "FriendRequest.h"
#include <iostream>
#include <string>
using namespace std;

FriendRequest::FriendRequest(RenderWindow& window) : window(window)
{
    keyPressSound.loadFromFile("../Sound/Key.wav");
    if (!font.loadFromFile("../Font2/Category.ttf"))
    {

    }

    for (int i = 0; i < 3; ++i)
    {
        if (!FriendRequestMenuTextures[i].loadFromFile("../images/FriendRequestMenu" + to_string(i + 1) + ".png"))
        {

        }
        FriendRequestMenuSprites[i].setTexture(FriendRequestMenuTextures[i]);
        FriendRequestMenuSprites[i].setPosition(0, 0);
    }

    string fileName = "../GameUsers.txt";
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error opening file.\n";
        playerCount = 0;
        players = nullptr;
        hashTable = nullptr;
        return;
    }

    playerCount = 0;
    string line;
    while (getline(file, line)) {
        playerCount++;
    }

    players = new Player[playerCount];
    hashTable = new Player * [playerCount * 2]; 

    for (int i = 0; i < playerCount * 2; i++) 
    {
        hashTable[i] = nullptr;
    }

    file.close(); 
}

void FriendRequest::loadPlayers() 
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
    while (getline(file, line) && index < playerCount) 
    {
        players[index].username = line;
        players[index].friends = nullptr;
        players[index].pendingRequests = nullptr;
        index++;
    }
    file.close();

    for (int i = 0; i < playerCount; i++) 
    {
        loadFriendList(players[i]);
    }

    for (int i = 0; i < playerCount; i++) 
    {
        int hashIndex = hashFunction(players[i].username);
        while (hashTable[hashIndex] != nullptr) 
        {
            hashIndex = (hashIndex + 1) % (playerCount * 2); 
        }
        hashTable[hashIndex] = &players[i];
    }
}

void FriendRequest::loadFriendList(Player& player) 
{
    string fileName = "../Friends/" + player.username + "_friends.txt";
    ifstream file(fileName);
    if (file.is_open()) 
    {
        string line;
        while (getline(file, line)) 
        {
            FriendNode* newFriend = new FriendNode(line);
            newFriend->next = player.friends;
            player.friends = newFriend;
        }
        file.close();
    }

    // Load pending requests
    fileName = "../Pendings/" + player.username + "_pending.txt";
    ifstream pendingFile(fileName);
    if (pendingFile.is_open()) 
    {
        string line;
        while (getline(pendingFile, line)) 
        {
            if(line.empty()) continue; // Skip empty lines
            FriendNode* newRequest = new FriendNode(line);
            newRequest->next = player.pendingRequests;
            player.pendingRequests = newRequest;
        }
        pendingFile.close();
    }
}

int FriendRequest::sendFriendRequest(string sender, string receiver)
{
    if (!isPlayerExists(receiver))
    {
        cout << "Player does not exist.\n";
        return 0;
    }

    if (sender == receiver)
    {
        cout << "Cannot send friend request to yourself.\n";
        return -1;
    }

    Player* senderPlayer = getPlayer(sender);
    Player* receiverPlayer = getPlayer(receiver);

    // Check if already friends
    FriendNode* temp = receiverPlayer->friends;
    while (temp != nullptr)
    {
        if (temp->username == sender)
        {
            cout << "You are already friends with " << receiver << ".\n";
            return -2;
        }
        temp = temp->next;
    }

    // Check if already sent
    if (receiverPlayer->isRequestPending(sender))
    {
        cout << "Friend request already pending.\n";
        return 2;
    }

    // Add to receiver's pending list
    FriendNode* newRequest = new FriendNode(sender);
    newRequest->next = receiverPlayer->pendingRequests;
    receiverPlayer->pendingRequests = newRequest;

    string fileName = "../Pendings/" + receiver + "_pending.txt";
    ofstream file(fileName, ios::app);
    if (file.is_open())
    {
        file << sender << endl;
        file.close();
    }
    else
    {
        cout << "Error opening pending file.\n";
    }

    cout << "Friend request sent from " << sender << " to " << receiver << ".\n";
    return 1;
}

void FriendRequest::DrawFriends(string username)
{
    int hashIndex = hashFunction(username);
    Player* player = hashTable[hashIndex];

    if (player == nullptr || player->username != username)
    {
        cout << "Player not found.\n";
        return;
    }

    cout << "Friends of " << username << ":\n";
    if (player->friends == nullptr)
    {
        cout << "No friends yet.\n";
        return;
    }

    FriendNode* curr = player->friends;
    while (curr != nullptr)
    {
        cout << curr->username << endl;
        curr = curr->next;
    }
}

void FriendRequest::viewPendingRequests(string username)
{
    int hashIndex = hashFunction(username);
    Player* player = hashTable[hashIndex];

    if (player == nullptr || player->username != username)
    {
        cout << "Player not found.\n";
        return;
    }

    cout << "Pending friend requests for " << username << ":\n";
    if (player->pendingRequests == nullptr)
    {
        cout << "No pending friend requests.\n";
        return;
    }

    FriendNode* curr = player->pendingRequests;
    while (curr != nullptr)
    {
        cout << curr->username << endl;
        curr = curr->next;
    }
}


void FriendRequest::acceptFriendRequest(string sender, string receiver)
{
    if (!isPlayerExists(sender) || !isPlayerExists(receiver))
    {
        cout << "Sender or receiver does not exist.\n";
        return;
    }

    Player* senderPlayer = getPlayer(sender);
    Player* receiverPlayer = getPlayer(receiver);

    // Check if request exists
    if (!receiverPlayer->isRequestPending(sender))
    {
        cout << "No pending request from " << sender << " to accept.\n";
        return;
    }

    // Add both to each other’s friend lists
    FriendNode* newFriend1 = new FriendNode(sender);
    newFriend1->next = receiverPlayer->friends;
    receiverPlayer->friends = newFriend1;

    FriendNode* newFriend2 = new FriendNode(receiver);
    newFriend2->next = senderPlayer->friends;
    senderPlayer->friends = newFriend2;

    // Save to friends files
    ofstream file("../Friends/" + receiver + "_friends.txt", ios::app);
    if (file.is_open())
    {
        file << sender << endl;
        file.close();
    }

    file.open("../Friends/" + sender + "_friends.txt", ios::app);
    if (file.is_open())
    {
        file << receiver << endl;
        file.close();
    }

    // Remove request from receiver’s pending list
    FriendNode* curr = receiverPlayer->pendingRequests;
    FriendNode* prev = nullptr;
    while (curr != nullptr && curr->username != sender)
    {
        prev = curr;
        curr = curr->next;
    }

    if (curr != nullptr)
    {
        if (prev == nullptr)
        {
            receiverPlayer->pendingRequests = curr->next;
        }
        else
        {
            prev->next = curr->next;
        }
        delete curr;
    }

    // Rewrite pending file
    ofstream newPending("../Pendings/" + receiver + "_pending.txt");
    FriendNode* iter = receiverPlayer->pendingRequests;
    while (iter != nullptr)
    {
        newPending << iter->username << endl;
        iter = iter->next;
    }
    newPending.close();

    cout << "Friend request from " << sender << " accepted by " << receiver << ".\n";
}


Player* FriendRequest::getPlayer(string username)
{
    int index = hashFunction(username);
    while (hashTable[index] != nullptr)
    {
        if (hashTable[index]->username == username)
        {
            return hashTable[index];
        }
        index = (index + 1) % (playerCount * 2);
    }
    return nullptr;
}

bool FriendRequest::isPlayerExists(string username)
{
	int index = hashFunction(username);
    while (hashTable[index] != nullptr)
    {
        if (hashTable[index]->username == username)
        {
			return true;
		}
		index = (index + 1) % (playerCount * 2);
	}
	return false;
}

int FriendRequest::getNumberOfFriends(string username)
{
	Player* player = getPlayer(username);
    if (player == nullptr)
    {
		return 0;
	}

	int count = 0;
	FriendNode* curr = player->friends;
    while (curr != nullptr)
    {
		count++;
		curr = curr->next;
	}
	return count;
}


int FriendRequest::handleFriendRequestMenu(string username)
{
    int selectedMenu = 0;
    Player* player = getPlayer(username);
    int numberOfFriends = getNumberOfFriends(username);
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

    if (numberOfFriends == 0) {
        while (window.isOpen()) {
            window.clear();
            infoText.setCharacterSize(80);
            infoText.setPosition(170, 350);
            infoText.setString("NO FRIENDS EXIST");
            window.draw(FriendRequestMenuSprites[selectedMenu]);
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
                    if (AddFriendsBoxBounds.contains(mousePos))
                    {
						keyPressSound.play();
						return 1;
					}
                    if (PendingBoxBounds.contains(mousePos))
                    {
                        keyPressSound.play();
                        return 2;
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

    // Iterate through the friend list
    FriendNode* current = player->friends;
    int index = 0;
    while (current && index < numberOfFriends) {
        // Load rectangle
        playerRectangleTexture[index].loadFromFile("../images/PlayerRectangle.jpg");
        playerRectangleSprite[index].setTexture(playerRectangleTexture[index]);
        playerRectangleSprite[index].setPosition(240, 300 + (index % 2) * 150);

        // Load icon
        string iconPath = "../PlayerProfiles/p" + to_string(getPlayer(current->username)->getProfileIcon()) + ".png";
        profileIconTexture[index].loadFromFile(iconPath);
        profileIconSprite[index].setTexture(profileIconTexture[index]);
        //profileIconSprite[index].setScale(0.3f, 0.3f);  // Scale as needed
        profileIconSprite[index].setPosition(260, 310 + (index % 2) * 150);

        // Prepare username text
        usernameText[index].setFont(font);
        usernameText[index].setCharacterSize(40);
        usernameText[index].setFillColor(Color::White);
        usernameText[index].setStyle(Text::Bold);
        usernameText[index].setString(current->username);
        usernameText[index].setPosition(400, 350 + (index % 2) * 150);

        index++;
        current = current->next;
    }

    // Scroll controls
    int scrollIndex = 0;
    Texture upIconTexture, downIconTexture;
    upIconTexture.loadFromFile("../images/UpIcon.png");
    downIconTexture.loadFromFile("../images/DownIcon.png");
    Sprite upIconSprite(upIconTexture), downIconSprite(downIconTexture);
    upIconSprite.setPosition(800, 330);
    downIconSprite.setPosition(800, 450);
    

    //here i have a pending box icon on the background so if i click him it should go to that menu
    // Main loop
    
    while (window.isOpen()) {
        window.clear();
        window.draw(FriendRequestMenuSprites[selectedMenu]);
        window.draw(CancelIconSprite);

        if (numberOfFriends > 2) {
            window.draw(upIconSprite);
            window.draw(downIconSprite);
        }

        // Draw first visible friend
        if (scrollIndex < numberOfFriends) {
            playerRectangleSprite[scrollIndex].setPosition(240, 300);
            profileIconSprite[scrollIndex].setPosition(260, 310);
            usernameText[scrollIndex].setPosition(400, 350);

            window.draw(playerRectangleSprite[scrollIndex]);
            window.draw(profileIconSprite[scrollIndex]);
            window.draw(usernameText[scrollIndex]);
        }

        // Draw second visible friend
        if (scrollIndex + 1 < numberOfFriends) {
            playerRectangleSprite[scrollIndex + 1].setPosition(240, 450);
            profileIconSprite[scrollIndex + 1].setPosition(260, 460);
            usernameText[scrollIndex + 1].setPosition(400, 500);

            window.draw(playerRectangleSprite[scrollIndex + 1]);
            window.draw(profileIconSprite[scrollIndex + 1]);
            window.draw(usernameText[scrollIndex + 1]);
        }

        else 
        {
            infoText.setCharacterSize(60);
            infoText.setPosition(270, 500);
            infoText.setString("Friend list ends.");
            window.draw(infoText);
        }

        window.display();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return -1;
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Up) {
                    selectedMenu = (selectedMenu - 1 + 3) % 3;
                    keyPressSound.play();
                }
                else if (event.key.code == Keyboard::Down) {
                    selectedMenu = (selectedMenu + 1) % 3;
                    keyPressSound.play();
                }
                else if (event.key.code == Keyboard::Enter) {
                    return selectedMenu;
                }
                else if (event.key.code == Keyboard::Escape) {
					return -1;
				}
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                cout<<"Mouse position: " << mousePos.x << ", " << mousePos.y << endl;
                if (upIconSprite.getGlobalBounds().contains(mousePos) && scrollIndex > 0) {
                    scrollIndex--;
                    keyPressSound.play();
                }
                else if (downIconSprite.getGlobalBounds().contains(mousePos) && scrollIndex + 2 < numberOfFriends) {
                    scrollIndex++;
                    keyPressSound.play();
                }
                else if (PendingBoxBounds.contains(mousePos))
                {
                    keyPressSound.play();
                    return 2;
                }
                else if (AddFriendsBoxBounds.contains(mousePos))
                {
					keyPressSound.play();
					return 1;
				}
                else if (CancelIconSprite.getGlobalBounds().contains(mousePos))
                {
                    keyPressSound.play();
					return -1;
				}
            }
        }
    }

    // Cleanup
    delete[] playerRectangleTexture;
    delete[] playerRectangleSprite;
    delete[] profileIconTexture;
    delete[] profileIconSprite;
    delete[] usernameText;
    return -1;
}

int FriendRequest::pendingRequestMenu(string username)
{
    int selectedMenu = 2;
    Player* player = getPlayer(username);
    int numberOfPendingRequests = 0;
    FriendNode* current = player->pendingRequests;
    FloatRect FriendsBoxBounds(115, 155, 238, 105); // Adjust as needed
    FloatRect AddFriendsBoxBounds(393, 155, 238, 105); // Adjust as needed

    // Count pending requests
    while (current != nullptr) {
        numberOfPendingRequests++;
        current = current->next;
    }


    Texture CancelIconTexture;
    if (!CancelIconTexture.loadFromFile("../images/cancel.png"))
    {
        cout << "Error loading Cancel Icon texture" << endl;
    }
    Sprite CancelIconSprite;
    CancelIconSprite.setTexture(CancelIconTexture);
    CancelIconSprite.setPosition(830, 84);
    CancelIconSprite.setScale(0.9f, 0.9f); // Scale down the icon

    // If no pending requests
    if (numberOfPendingRequests == 0) 
    {
        while (window.isOpen()) {
            window.clear();
            Text infoText;
            infoText.setFont(font);
            infoText.setCharacterSize(75);
            infoText.setFillColor(Color::White);
            infoText.setStyle(Text::Bold);
            infoText.setPosition(103, 350);
            infoText.setString("NO PENDING REQUESTS");

            window.draw(FriendRequestMenuSprites[selectedMenu]);
            window.draw(CancelIconSprite);
            window.draw(infoText);
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
                    cout << "Mouse position: " << mousePos.x << ", " << mousePos.y << endl;
                    if (CancelIconSprite.getGlobalBounds().contains(mousePos))
                    {
						keyPressSound.play();
						return -1;
					}
                    if (FriendsBoxBounds.contains(mousePos))
                    {
                        keyPressSound.play();
                        return 0;
                    }
                    if (AddFriendsBoxBounds.contains(mousePos))
                    {
						keyPressSound.play();
						return 1;
					}
                }
            }
        }
        return -1;
    }

    // Prepare containers
    Texture* playerRectangleTexture = new Texture[numberOfPendingRequests];
    Sprite* playerRectangleSprite = new Sprite[numberOfPendingRequests];
    Texture* profileIconTexture = new Texture[numberOfPendingRequests];
    Sprite* profileIconSprite = new Sprite[numberOfPendingRequests];
    Text* usernameText = new Text[numberOfPendingRequests];
    Texture * AcceptIconTexture = new Texture[numberOfPendingRequests];
    Sprite * AcceptIconSprite = new Sprite[numberOfPendingRequests];
    Texture * RejectIconTexture = new Texture[numberOfPendingRequests];
    Sprite * RejectIconSprite = new Sprite[numberOfPendingRequests];


    // Fill containers
    current = player->pendingRequests;
    int index = 0;
    while (current && index < numberOfPendingRequests) {
        // Load rectangle
        playerRectangleTexture[index].loadFromFile("../images/PlayerRectangle.jpg");
        playerRectangleSprite[index].setTexture(playerRectangleTexture[index]);
        AcceptIconTexture[index].loadFromFile("../images/Accept.png");
        AcceptIconSprite[index].setTexture(AcceptIconTexture[index]);
        RejectIconTexture[index].loadFromFile("../images/Reject.png");
        RejectIconSprite[index].setTexture(RejectIconTexture[index]);

        // Load profile icon
        string iconPath = "../PlayerProfiles/p" + to_string(getPlayer(current->username)->getProfileIcon()) + ".png";
        profileIconTexture[index].loadFromFile(iconPath);
        profileIconSprite[index].setTexture(profileIconTexture[index]);



        // Prepare username text
        usernameText[index].setFont(font);
        usernameText[index].setCharacterSize(40);
        usernameText[index].setFillColor(Color::White);
        usernameText[index].setStyle(Text::Bold);
        usernameText[index].setString(current->username);

        current = current->next;
        index++;
    }

    // Scroll controls
    int scrollIndex = 0;
    Texture upIconTexture, downIconTexture;
    upIconTexture.loadFromFile("../images/UpIcon.png");
    downIconTexture.loadFromFile("../images/DownIcon.png");
    Sprite upIconSprite(upIconTexture), downIconSprite(downIconTexture);
    upIconSprite.setPosition(800, 330);
    downIconSprite.setPosition(800, 450);

    while (window.isOpen()) {
        window.clear();
        window.draw(FriendRequestMenuSprites[selectedMenu]);
        window.draw(CancelIconSprite);

        if (numberOfPendingRequests > 2) {
            window.draw(upIconSprite);
            window.draw(downIconSprite);
        }

        // First friend
        if (scrollIndex < numberOfPendingRequests) 
        {
            playerRectangleSprite[scrollIndex].setPosition(240, 300);
            profileIconSprite[scrollIndex].setPosition(260, 310);
            usernameText[scrollIndex].setPosition(395, 345);
            AcceptIconSprite[scrollIndex].setPosition(650, 345);
            RejectIconSprite[scrollIndex].setPosition(725, 345);
            window.draw(playerRectangleSprite[scrollIndex]);
            window.draw(profileIconSprite[scrollIndex]);
            window.draw(usernameText[scrollIndex]);
            window.draw(AcceptIconSprite[scrollIndex]);
            window.draw(RejectIconSprite[scrollIndex]);
        }

        // Second friend
        if (scrollIndex + 1 < numberOfPendingRequests) 
        {
            playerRectangleSprite[scrollIndex + 1].setPosition(240, 450);
            profileIconSprite[scrollIndex + 1].setPosition(260, 460);
            usernameText[scrollIndex + 1].setPosition(400, 500);
            AcceptIconSprite[scrollIndex + 1].setPosition(650, 500);
            RejectIconSprite[scrollIndex + 1].setPosition(725, 500);

            window.draw(playerRectangleSprite[scrollIndex + 1]);
            window.draw(profileIconSprite[scrollIndex + 1]);
            window.draw(usernameText[scrollIndex + 1]);
            window.draw(AcceptIconSprite[scrollIndex + 1]);
            window.draw(RejectIconSprite[scrollIndex + 1]);

        }

        window.display();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                delete[] playerRectangleTexture;
                delete[] playerRectangleSprite;
                delete[] profileIconTexture;
                delete[] profileIconSprite;
                delete[] usernameText;
                return -1;
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) 
                {
                    delete[] playerRectangleTexture;
                    delete[] playerRectangleSprite;
                    delete[] profileIconTexture;
                    delete[] profileIconSprite;
                    delete[] usernameText;
                    return -1;
                }
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) 
            {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (upIconSprite.getGlobalBounds().contains(mousePos) && scrollIndex > 0) {
                    scrollIndex--;
                    keyPressSound.play();
                }
                else if (downIconSprite.getGlobalBounds().contains(mousePos) && scrollIndex + 2 < numberOfPendingRequests) {
                    scrollIndex++;
                    keyPressSound.play();
                }
                else if (CancelIconSprite.getGlobalBounds().contains(mousePos))
                {
					keyPressSound.play();
					delete[] playerRectangleTexture;
					delete[] playerRectangleSprite;
					delete[] profileIconTexture;
					delete[] profileIconSprite;
					delete[] usernameText;
					return -1;
				}
                else if (FriendsBoxBounds.contains(mousePos))
                {
                    delete[] playerRectangleTexture;
                    delete[] playerRectangleSprite;
                    delete[] profileIconTexture;
                    delete[] profileIconSprite;
                    delete[] usernameText;
					keyPressSound.play();
					return 0;
				}
                else if (AddFriendsBoxBounds.contains(mousePos))
                {
                    delete[] playerRectangleTexture;
                    delete[] playerRectangleSprite;
                    delete[] profileIconTexture;
                    delete[] profileIconSprite;
                    delete[] usernameText;
                    keyPressSound.play();
                    return 1;
                }
                else if (AcceptIconSprite[scrollIndex].getGlobalBounds().contains(mousePos))
                {
					keyPressSound.play();
					acceptFriendRequest(usernameText[scrollIndex].getString(), username); 
					delete[] playerRectangleTexture;
					delete[] playerRectangleSprite;
					delete[] profileIconTexture;
					delete[] profileIconSprite;
					delete[] usernameText;
					return 2; // Return to the main menu
				}
                else if (RejectIconSprite[scrollIndex].getGlobalBounds().contains(mousePos))
                {
					keyPressSound.play();
					rejectFriendRequest(usernameText[scrollIndex].getString(), username); 
					delete[] playerRectangleTexture;
					delete[] playerRectangleSprite;
					delete[] profileIconTexture;
					delete[] profileIconSprite;
					delete[] usernameText;
					return 2; // Return to the main menu
				}
                if (scrollIndex + 1 < numberOfPendingRequests)
                {
                    if (AcceptIconSprite[scrollIndex + 1].getGlobalBounds().contains(mousePos))
                    {
						keyPressSound.play();
						acceptFriendRequest(usernameText[scrollIndex + 1].getString(), username); 
						delete[] playerRectangleTexture;
						delete[] playerRectangleSprite;
						delete[] profileIconTexture;
						delete[] profileIconSprite;
						delete[] usernameText;
						return 2; // Return to the main menu
					}
                    else if (RejectIconSprite[scrollIndex + 1].getGlobalBounds().contains(mousePos))
                    {
						keyPressSound.play();
						rejectFriendRequest(usernameText[scrollIndex + 1].getString(), username); 
						delete[] playerRectangleTexture;
						delete[] playerRectangleSprite;
						delete[] profileIconTexture;
						delete[] profileIconSprite;
						delete[] usernameText;
						return 2; // Return to the main menu
					}
				}
            }
        }
    }

    // Cleanup
    delete[] playerRectangleTexture;
    delete[] playerRectangleSprite;
    delete[] profileIconTexture;
    delete[] profileIconSprite;
    delete[] usernameText;
    return selectedMenu;
}

int FriendRequest::addFriendMenu(string username)
{
    int selectedMenu = 1;
    Player* player = getPlayer(username);
    string friendName;
    bool showCursor = true;
    Clock cursorClock;
    bool searchTriggered = false;
    Player* searchedPlayer = nullptr;
    string message = "";
    int sendStatus = 0;
    FloatRect FriendsBoxBounds(115, 155, 238, 105);
    FloatRect PendingBoxBounds(671, 155, 236, 103); 

    // Load textures and sprites
    Texture CancelIconTexture, SearchIconTexture, SearchBarTexture, RectangleTexture, SendRequestTexture;
    CancelIconTexture.loadFromFile("../images/cancel.png");
    SearchIconTexture.loadFromFile("../images/SearchIcon.png");
    SearchBarTexture.loadFromFile("../images/SearchBar.png");
    RectangleTexture.loadFromFile("../images/PlayerRectangle.jpg");
    SendRequestTexture.loadFromFile("../images/SendRequest.png");

    Sprite CancelIconSprite(CancelIconTexture), SearchIconSprite(SearchIconTexture),
           SearchBarSprite(SearchBarTexture), RectangleSprite(RectangleTexture),
           SendRequestSprite(SendRequestTexture);

    CancelIconSprite.setPosition(830, 84); CancelIconSprite.setScale(0.9f, 0.9f);
    SearchIconSprite.setPosition(690, 293);
    SearchBarSprite.setPosition(240, 300);
    RectangleSprite.setPosition(220, 380);
    SendRequestSprite.setPosition(600, 422);

    // Text setup
    Text inputText, infoText, requestStatusText;
    inputText.setFont(font);
    inputText.setCharacterSize(30);
    inputText.setFillColor(Color::White);
    inputText.setPosition(260, 310);
    inputText.setStyle(Text::Bold);

    infoText.setFont(font);
    infoText.setCharacterSize(25);
    infoText.setFillColor(Color::Red);
    infoText.setPosition(260, 370);
    infoText.setStyle(Text::Bold);

    bool showError = false;
    string errorMessage = "";
    bool showProfile = false;

    while (window.isOpen()) {
        window.clear();

        // Draw base UI
        window.draw(FriendRequestMenuSprites[selectedMenu]);
        window.draw(CancelIconSprite);
        window.draw(SearchBarSprite);
        window.draw(SearchIconSprite);

        // Blinking cursor logic
        if (cursorClock.getElapsedTime().asSeconds() >= 0.5f) {
            cursorClock.restart();
            showCursor = !showCursor;
        }

        // Display input text with blinking cursor
        string displayText = friendName + (showCursor ? "_" : "");
        inputText.setString(displayText);
        window.draw(inputText);

        // Handle search result drawing
        if (showError) {
            Text errorText;
            errorText.setFont(font);
            if (errorMessage == "PLAYER NOT FOUND.")
            {
                errorText.setCharacterSize(70);
                errorText.setPosition(185, 410);
            }
            else if (errorMessage == "You cannot send a request to yourself.")
            {
                errorText.setCharacterSize(35);
                errorText.setPosition(150, 420);
            }
            errorText.setFillColor(Color::White);
            errorText.setString(errorMessage);
            errorText.setStyle(Text::Bold);
            window.draw(errorText);
        }
        else if (showProfile && searchedPlayer != nullptr) {
            // Draw rectangle and profile content
            window.draw(RectangleSprite);

            // Profile icon
            Texture iconTex;
            if (iconTex.loadFromFile("../PlayerProfiles/p" + to_string(searchedPlayer->getProfileIcon()) + ".png")) {
                Sprite iconSprite(iconTex);
                iconSprite.setPosition(255, 388);
                window.draw(iconSprite);
            }

            // Username
            Text usernameText;
            usernameText.setFont(font);
            usernameText.setCharacterSize(35);
            usernameText.setFillColor(Color(148, 180, 255));
            usernameText.setPosition(390, 425);
            usernameText.setString(searchedPlayer->username);
            usernameText.setStyle(Text::Bold);
            usernameText.setOutlineThickness(3);
            window.draw(usernameText);

            // Status message
            Text statusText;
            statusText.setFont(font);
            statusText.setCharacterSize(27);
            statusText.setFillColor(Color::Red);
            statusText.setPosition(385, 470);
            statusText.setStyle(Text::Bold);

            if (player->isRequestPending(friendName)) 
            {
                statusText.setString("Request Already Pending.");
                window.draw(statusText);
            }
            else if (player->isAlreadyFriend(friendName)) 
            {
                statusText.setString("You are already Friends.");
                window.draw(statusText);
            }
            else if (searchedPlayer->isRequestPending(username)) 
            {
				statusText.setString("Request Already Sent.");
				window.draw(statusText);
			}
            else 
            {
                window.draw(SendRequestSprite);
            }
        }

        window.display();

        // --- Handle Events ---
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                return -1;
            }

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) {
                    keyPressSound.play();
                    return -1;
                }
                if (event.key.code == Keyboard::BackSpace && !friendName.empty()) {
                    friendName.pop_back();
                    showProfile = false;  
                    showError = false;     
                }
                if (event.key.code == Keyboard::Enter) {
                    keyPressSound.play();
                    searchTriggered = true;
                }
            }

            if (event.type == Event::TextEntered) {
                if (event.text.unicode < 128 && event.text.unicode != 13 && event.text.unicode != 8) {
                    keyPressSound.play();
                    friendName += static_cast<char>(event.text.unicode);
                    showProfile = false;   
                    showError = false;
                }
            }


            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                if (SearchIconSprite.getGlobalBounds().contains(mousePos)) {
                    keyPressSound.play();
                    searchTriggered = true;
                }
                if (CancelIconSprite.getGlobalBounds().contains(mousePos)) {
                    keyPressSound.play();
                    return -1;
                }

                if (FriendsBoxBounds.contains(mousePos))
                {
					keyPressSound.play();
					return 0;
				}
                if (PendingBoxBounds.contains(mousePos))
				{
                    keyPressSound.play();
                    return 2;
                }

                // Handle send friend request
                if (showProfile && searchedPlayer != nullptr &&
                    SendRequestSprite.getGlobalBounds().contains(mousePos)) {
                    sendFriendRequest(username, friendName);
                    return 1;
                }
            }
        }

        // Process search once per trigger
        if (searchTriggered) {
            searchTriggered = false;
            searchedPlayer = getPlayer(friendName);
            showError = false;
            showProfile = false;

            if (friendName == username) {
                showError = true;

                errorMessage = "You cannot send a request to yourself.";
            }
            else if (!isPlayerExists(friendName)) {
                showError = true;
                errorMessage = "PLAYER NOT FOUND.";
            }
            else if (searchedPlayer != nullptr) {
                showProfile = true;
            }
        }
    }


    return -1;
}

int FriendRequest::handleMenuLoop(string username)
{
    loadPlayers();
	int choice = 0;
    choice = handleFriendRequestMenu(username);
    while (choice != -1) 
    {
        switch (choice)
        {
			case 0:
				choice = handleFriendRequestMenu(username);
				break;
			case 1:
				choice = addFriendMenu(username);
				break;
			case 2:
				choice = pendingRequestMenu(username);
				break;
			default:
				break;
		}
	}
	return 0;
}


void FriendRequest::rejectFriendRequest(string sender, string receiver)
{
    if (!isPlayerExists(sender) || !isPlayerExists(receiver))
    {
		cout << "Sender or receiver does not exist.\n";
		return;
	}

	Player* senderPlayer = getPlayer(sender);
	Player* receiverPlayer = getPlayer(receiver);

	// Check if request exists
    if (!receiverPlayer->isRequestPending(sender))
    {
		cout << "No pending request from " << sender << " to reject.\n";
		return;
	}

	// Remove request from receiver’s pending list
	FriendNode* curr = receiverPlayer->pendingRequests;
	FriendNode* prev = nullptr;
    while (curr != nullptr && curr->username != sender)
    {
		prev = curr;
		curr = curr->next;
	}

    if (curr != nullptr)
    {
        if (prev == nullptr)
        {
			receiverPlayer->pendingRequests = curr->next;
		}
        else
        {
			prev->next = curr->next;
		}
		delete curr;
	}

	// Rewrite pending file
	ofstream newPending("../Pendings/" + receiver + "_pending.txt");
	FriendNode* iter = receiverPlayer->pendingRequests;
    while (iter != nullptr)
    {
		newPending << iter->username << endl;
		iter = iter->next;
	}
	newPending.close();

	cout << "Friend request from " << sender << " rejected by " << receiver << ".\n";
}