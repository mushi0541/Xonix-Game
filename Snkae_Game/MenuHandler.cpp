#include "MenuHandler.h"

MenuHandler::MenuHandler(RenderWindow& gameWindow, Authentication& auth) : window(gameWindow), MultiplayerMode(false), auth(auth)
{
	keyPressSound.loadFromFile("../Sound/Key.wav");
	font.loadFromFile("../Font2/font1.otf");
	loadResources();
}

void MenuHandler::loadResources()
{
	// Load textures and sprites for Game Start Menu
	for (int i = 0; i < 3; ++i)
	{
		if (!GameStartTextures[i].loadFromFile("../images/GameStart" + to_string(i) + ".png"))
		{
			cout << "Error loading Game Start texture " << i << endl;
		}
		GameStartSprites[i].setTexture(GameStartTextures[i]);
		GameStartSprites[i].setPosition(0, 0);
	}

	// Load textures and sprites for Sign In/Sign Up Menu
	for (int i = 0; i < 2; ++i)
	{
		if (!SignInSignUpTextures[i].loadFromFile("../images/SignInSignUp" + to_string(i) + ".png"))
		{
			cout << "Error loading Sign In/Sign Up texture " << i << endl;
		}
		SignInSignUpSprites[i].setTexture(SignInSignUpTextures[i]);
		SignInSignUpSprites[i].setPosition(0, 0); 
	}

	// Load textures and sprites for Sign In Menu
	for (int i = 0; i < 4; ++i)
	{
		if (!SignInTextures[i].loadFromFile("../images/SignIn" + to_string(i) + ".png"))
		{
			cout << "Error loading Sign In texture " << i << endl;
		}
		SignInSprites[i].setTexture(SignInTextures[i]);
		SignInSprites[i].setPosition(0, 0); 
	}

	// Load textures and sprites for Sign Up Menu
	for (int i = 0; i < 5; ++i)
	{
		if (!SignUpTextures[i].loadFromFile("../images/SignUp" + to_string(i) + ".png"))
		{
			cout << "Error loading Sign Up texture " << i << endl;
		}
		SignUpSprites[i].setTexture(SignUpTextures[i]);
		SignUpSprites[i].setPosition(0, 0); 
	}

    // Load textures and sprites for Sign In Username Error
    if (!SignInUserNameErrorTexture.loadFromFile("../images/LoginUserName.png"))
    {
		cout << "Error loading Sign In Username Error texture" << endl;
	}
    SignInUserNameErrorSprite.setTexture(SignInUserNameErrorTexture);
    SignInUserNameErrorSprite.setPosition(160, 170); 
    // Load textures and sprites for Sign In Password Error
    if (!SignInPasswordErrorTexture.loadFromFile("../images/LoginPassword.png"))
    {

        cout << "Error loading Sign In Password Error texture" << endl;
    }
    SignInPasswordErrorSprite.setTexture(SignInPasswordErrorTexture);
    SignInPasswordErrorSprite.setPosition(160, 170); 
    // Load textures and sprites for Sign In Success
    if (!SignInSuccessTexture.loadFromFile("../images/LoginSuccess.png"))
    {
		cout << "Error loading Sign In Success texture" << endl;
	}
    SignInSuccessSprite.setTexture(SignInSuccessTexture);
	SignInSuccessSprite.setPosition(150, 175); 


    // Load textures and sprites for Sign Up Username Error
    if (!SignUpUserNameErrorTexture.loadFromFile("../images/SignUpUserName.png"))
    {

    }

    SignUpUserNameErrorSprite.setTexture(SignUpUserNameErrorTexture);
    SignUpUserNameErrorSprite.setPosition(160, 220); 
    // Load textures and sprites for Sign Up Password Error
    if (!SignUpPasswordErrorTexture[0].loadFromFile("../images/SignUpPassword1.png"))
    {

    }
    SignUpPasswordErrorSprite[0].setTexture(SignUpPasswordErrorTexture[0]);
    SignUpPasswordErrorSprite[0].setPosition(160, 220); 
    // Load textures and sprites for Sign Up Confirm Password Error
    if (!SignUpPasswordErrorTexture[1].loadFromFile("../images/SignUpPassword2.png"))
    {

	}
    SignUpPasswordErrorSprite[1].setTexture(SignUpPasswordErrorTexture[1]);
	SignUpPasswordErrorSprite[1].setPosition(160, 220); 
	// Load textures and sprites for Sign Up Success

    // Load textures and sprites for Sign Up Password Error
    if (!SignUpPasswordErrorTexture[2].loadFromFile("../images/SignUpPassword3.jpg"))
    {

    }
    SignUpPasswordErrorSprite[2].setTexture(SignUpPasswordErrorTexture[2]);
    SignUpPasswordErrorSprite[2].setPosition(160, 220); 
    if (!SignUpSuccessTexture.loadFromFile("../images/SignUpSuccess.png"))
    {
		cout << "Error loading Sign Up Success texture" << endl;
	}
	SignUpSuccessSprite.setTexture(SignUpSuccessTexture);
    SignUpSuccessSprite.setPosition(150, 220); 

    
    // Load textures and sprites for Player Profile Menu
    for (int i = 0; i < 3; ++i)
    {
        if (!PlayerProfileTexture[i].loadFromFile("../images/Profiles" + to_string(i+1) + ".png"))
        {

        }
        PlayerProfileSprite[i].setTexture(PlayerProfileTexture[i]);
        PlayerProfileSprite[i].setPosition(0, 0); 
    }
  

    // Load textures and sprites for Player Profile Themes
    for (int i = 0; i < 16; ++i)
    {
        if (!PlayerProfileThemesTexture[i].loadFromFile("../PlayerProfiles/p" + to_string(i+1) + ".png"))
        {

        }
    }
    for (int i = 0; i < 16; ++i)
    {
		PlayerProfileThemesSprite[i].setTexture(PlayerProfileThemesTexture[i]);
		PlayerProfileThemesSprite[i].setPosition(250, 180); 
	}

    // Load textures and sprites for Friend Request Menu

	MultiplayerMode = false;
}


int MenuHandler::GameStartMenu()
{
	int selectedIndex = 0;
	Clock animationClock; 

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				keyPressSound.getSound().play();
				window.close();
				return -1; // Exit game
			}

			if (event.type == Event::KeyPressed)
			{
				keyPressSound.getSound().play();
					return selectedIndex; 
			}
		}

		if (animationClock.getElapsedTime().asSeconds() >= 0.5f)
		{
			selectedIndex = (selectedIndex + 1) % 3;
			animationClock.restart(); 
		}

		window.clear();
		window.draw(GameStartSprites[selectedIndex]);
		window.display();
	}

	return -1;
}


int MenuHandler::SignInSignUpMenu()
{
	int selectedIndex = 0;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
			{
				keyPressSound.getSound().play();
				window.close();
				return -1; 
			}

			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
			{
				keyPressSound.getSound().play();
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

		window.clear();
		window.draw(SignInSignUpSprites[selectedIndex]);
		window.display();
	}

	return -1;
}

int MenuHandler::SignInMenu()
{
    int selectedIndex = 0;
    bool isUsernameSelected = false;
    bool isPasswordSelected = false;
    bool typing = false;
    bool typingUsername = false;
    bool typingPassword = false;
    bool isLoginSuccessful = false;
    string usernameInput = "";
    string passwordInput = "";

    Clock cursorTimer;
    bool showCursor = true;

    Font font2;
    font2.loadFromFile("../Font2/Category.ttf");

    Text usernameText;
    usernameText.setFont(font2);
    usernameText.setCharacterSize(32);
    usernameText.setFillColor(Color::White);
    usernameText.setPosition(302, 270);

    Text passwordText;
    passwordText.setFont(font2);
    passwordText.setCharacterSize(40);
    passwordText.setFillColor(Color::White);
    passwordText.setPosition(302, 370);

    FloatRect usernameBox(290, 260, 440, 56);
    FloatRect passwordBox(290, 360, 440, 56);
    FloatRect submitBox  (263, 504, 477, 570);
    int result = -1;
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
                return -1;
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (usernameBox.contains(mousePos))
                {
                    keyPressSound.getSound().play();
                    if(passwordInput.empty())
                    selectedIndex = 1;
                    isUsernameSelected = true;
                    isPasswordSelected = false;
                    typing = true;
                    typingUsername = true;
                    typingPassword = false;
                }
                else if (passwordBox.contains(mousePos))
                {
                    keyPressSound.getSound().play();
                    selectedIndex = 2;
                    isUsernameSelected = false;
                    isPasswordSelected = true;
                    typing = true;
                    typingUsername = false;
                    typingPassword = true;
                }
                else if ((submitBox.contains(mousePos) && !usernameInput.empty() && !passwordInput.empty()))
                {
					keyPressSound.getSound().play();
					auth.setUserName(usernameInput);
					auth.setPassword(passwordInput);
                    result = auth.loginUser();

                    if (result == 0 || result == 1)
                    {
                        if(result == 0)
							window.draw(SignInUserNameErrorSprite);
						else
							window.draw(SignInPasswordErrorSprite);
                        window.display();
                        sleep(seconds(5));
                        window.clear();
                        usernameInput = "";
                        passwordInput = "";
                        selectedIndex = 0;
                        isUsernameSelected = false;
                        isPasswordSelected = false;
                        typing = false;
                        typingUsername = false;
                        typingPassword = false;
                    }
                    else
                    {
                        window.draw(SignInSuccessSprite);
                        window.display();
                        sleep(seconds(5));
                        window.clear();
                        isLoginSuccessful = true;
                    }

				}
                else
                {
                    isUsernameSelected = false;
                    isPasswordSelected = false;
                    typing = false;
                    typingUsername = false;
                    typingPassword = false;
                    if(usernameInput.empty() && passwordInput.empty())
						selectedIndex = 0; 
					else if (usernameInput.empty())
						selectedIndex = 0;
					else if (passwordInput.empty())
						selectedIndex = 1; 
					else
						selectedIndex = 2; 
                }
            }

            if (event.type == Event::TextEntered && typing)
            {
                if (event.text.unicode < 128) // Only process normal ASCII
                {
                    char typedChar = static_cast<char>(event.text.unicode);

                    if ((typedChar >= 'A' && typedChar <= 'Z') ||
                        (typedChar >= 'a' && typedChar <= 'z') ||
                        (typedChar >= '0' && typedChar <= '9') ||
                        typedChar == ' ' || typedChar == '_' || typedChar == '.')
                    {
                        keyPressSound.getSound().play();

                        if (typingUsername && usernameInput.length() <20)
                            usernameInput += typedChar;
                        else if (typingPassword && passwordInput.length() < 20)
                            passwordInput += typedChar;
                    }
                }
            }

            if (event.type == Event::KeyPressed && typing)
            {
                if (event.key.code == Keyboard::BackSpace)
                {
                    keyPressSound.getSound().play();
                    if (typingUsername && !usernameInput.empty())
                        usernameInput.pop_back();
                    else if (typingPassword && !passwordInput.empty())
                        passwordInput.pop_back();
                }

                if (event.key.code == Keyboard::Enter)
                {
                    if (!usernameInput.empty() && !passwordInput.empty())
                    {
                        keyPressSound.getSound().play();
                        auth.setUserName(usernameInput);
                        auth.setPassword(passwordInput);
                        result = auth.loginUser();

                        if (result == 0 || result == 1)
                        {
                            if (result == 0)
                                window.draw(SignInUserNameErrorSprite);
                            else
                                window.draw(SignInPasswordErrorSprite);
                            window.display();
                            sleep(seconds(5));
                            window.clear();
                            usernameInput = "";
                            passwordInput = "";
                            selectedIndex = 0;
                            isUsernameSelected = false;
                            isPasswordSelected = false;
                            typing = false;
                            typingUsername = false;
                            typingPassword = false;
                        }
                        else
                        {
                            window.draw(SignInSuccessSprite);
                            window.display();
                            sleep(seconds(5));
                            window.clear();
                            isLoginSuccessful = true;
                        }
                    }
                }
            }
        }

        if (isLoginSuccessful)
        {
			return 2; // Login successful
		}

        // Cursor blinking logic
        if (cursorTimer.getElapsedTime().asSeconds() >= 0.5f)
        {
            showCursor = !showCursor;
            cursorTimer.restart();
        }

        if (!usernameInput.empty() && !passwordInput.empty())
        {
			selectedIndex = 3; 
		}

        window.clear(Color::White);
        window.draw(SignInSprites[selectedIndex]);

        // Username field drawing
        if (isUsernameSelected || !usernameInput.empty())
        {
            if (showCursor && typingUsername)
                usernameText.setString(usernameInput + "_");
            else
                usernameText.setString(usernameInput);

            window.draw(usernameText);
        }

        // Password field drawing
        if (isPasswordSelected || !passwordInput.empty())
        {
            string maskedPassword(passwordInput.length(), '*'); 
            if (showCursor && typingPassword)
                passwordText.setString(maskedPassword + "_");
            else
                passwordText.setString(maskedPassword);

            window.draw(passwordText);
        }

        window.display();
    }

    return -1;
}


int MenuHandler::SignUpMenu()
{
    int selectedIndex = 0;
    bool isUsernameSelected = false;
    bool isPasswordSelected = false;
    bool isConfirmPasswordSelected = false;
    bool typing = false;
    bool typingUsername = false;
    bool typingPassword = false;
    bool typingConfirmPassword = false;
    bool isSignUpSuccessful = false;
    string usernameInput = "";
    string passwordInput = "";
    string confirmPasswordInput = "";

    Clock cursorTimer;
    bool showCursor = true;

    Font font2;
    font2.loadFromFile("../Font2/Category.ttf");

    Text usernameText;
    usernameText.setFont(font2);
    usernameText.setCharacterSize(32);
    usernameText.setFillColor(Color::White);
    usernameText.setPosition(302, 277);

    Text passwordText;
    passwordText.setFont(font2);
    passwordText.setCharacterSize(40);
    passwordText.setFillColor(Color::White);
    passwordText.setPosition(302, 370);

    Text confirmPasswordText;
    confirmPasswordText.setFont(font2);
    confirmPasswordText.setCharacterSize(40);
    confirmPasswordText.setFillColor(Color::White);
    confirmPasswordText.setPosition(302, 470);

    FloatRect usernameBox(290, 268, 440, 60);
    FloatRect passwordBox(290, 360, 440, 60);
    FloatRect submitBox(264, 544, 470, 72);
    FloatRect confirmPasswordBox(290, 454, 440, 58);
    int result = -1;
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
                return -1;
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (usernameBox.contains(mousePos))
                {
                    keyPressSound.getSound().play();
                    if (passwordInput.empty() && confirmPasswordInput.empty())
                        selectedIndex = 1;
                    isUsernameSelected = true;
                    isPasswordSelected = false;
                    typing = true;
                    typingUsername = true;
                    typingPassword = false;
                    typingConfirmPassword = false;
                }
                else if (passwordBox.contains(mousePos))
                {
                    keyPressSound.getSound().play();
                    if(confirmPasswordInput.empty())
						selectedIndex = 2;
                    isUsernameSelected = false;
                    isPasswordSelected = true;
                    typing = true;
                    typingUsername = false;
                    typingPassword = true;
                    typingConfirmPassword = false;
                }
                else if (confirmPasswordBox.contains(mousePos))
                {
                    keyPressSound.getSound().play();
                    selectedIndex = 3;
                    isUsernameSelected = false;
                    isPasswordSelected = false;
                    isConfirmPasswordSelected = true;
                    typing = true;
                    typingUsername = false;
                    typingPassword = false;
                    typingConfirmPassword = true;
                }
                else if ((submitBox.contains(mousePos) && !usernameInput.empty() && !passwordInput.empty()) && !confirmPasswordInput.empty())
                {
                    keyPressSound.getSound().play();
                    auth.setUserName(usernameInput);
                    auth.setPassword(passwordInput);
                    if(confirmPasswordInput==passwordInput)
                    result = auth.registerUser();

                    if (confirmPasswordInput != passwordInput)
                    {
						window.draw(SignUpPasswordErrorSprite[2]);
						window.display();
						sleep(seconds(5));
						window.clear();
						usernameInput = "";
						passwordInput = "";
						confirmPasswordInput = "";
						selectedIndex = 0;
						isUsernameSelected = false;
						isPasswordSelected = false;
						isConfirmPasswordSelected = false;
						typing = false;
						typingUsername = false;
						typingPassword = false;
						typingConfirmPassword = false;

					}
                    else if (result == 0 || result == 1 || result == 2)
                    {
                        if (result == 0)
                            window.draw(SignUpUserNameErrorSprite);
                        else if(result == 2)
                            window.draw(SignUpPasswordErrorSprite[0]);
                        else
                            window.draw(SignUpPasswordErrorSprite[1]);
                        window.display();
                        sleep(seconds(5));
                        window.clear();
                        usernameInput = "";
                        passwordInput = "";
                        confirmPasswordInput = "";
                        selectedIndex = 0;
                        isUsernameSelected = false;
                        isPasswordSelected = false;
                        isConfirmPasswordSelected = false;
                        typing = false;
                        typingUsername = false;
                        typingPassword = false;
                        typingConfirmPassword = false;

                    }
                    else
                    {
                        window.draw(SignUpSuccessSprite);
                        window.display();
                        sleep(seconds(5));
                        window.clear();
                        isSignUpSuccessful = true;
                    }

                }
                else
                {
                    isUsernameSelected = false;
                    isPasswordSelected = false;
                    typing = false;
                    typingUsername = false;
                    typingPassword = false;
                    if (usernameInput.empty() && passwordInput.empty() && confirmPasswordInput.empty())
                        selectedIndex = 0;
					else if (passwordInput.empty() && confirmPasswordInput.empty())
						selectedIndex = 1;
					else if (confirmPasswordInput.empty())
						selectedIndex = 2;
					else
						selectedIndex = 3;
                       
                }
            }

            if (event.type == Event::TextEntered && typing)
            {
                if (event.text.unicode < 128) // Only process normal ASCII
                {
                    char typedChar = static_cast<char>(event.text.unicode);

                    if ((typedChar >= 'A' && typedChar <= 'Z') ||
                        (typedChar >= 'a' && typedChar <= 'z') ||
                        (typedChar >= '0' && typedChar <= '9') ||
                        typedChar == ' ' || typedChar == '_' || typedChar == '.')
                    {
                        keyPressSound.getSound().play();

                        if (typingUsername && usernameInput.length() < 20)
                            usernameInput += typedChar;
                        else if (typingPassword && passwordInput.length() < 20)
                            passwordInput += typedChar;
                        else if (typingConfirmPassword && confirmPasswordInput.length() < 20)
                            confirmPasswordInput += typedChar;
                    }
                }
            }

            if (event.type == Event::KeyPressed && typing)
            {
                if (event.key.code == Keyboard::BackSpace)
                {
                    keyPressSound.getSound().play();
                    if (typingUsername && !usernameInput.empty())
                        usernameInput.pop_back();
                    else if (typingPassword && !passwordInput.empty())
                        passwordInput.pop_back();
                    else if (typingConfirmPassword && !confirmPasswordInput.empty())
                        confirmPasswordInput.pop_back();
                }

                if (event.key.code == Keyboard::Enter)
                {
                    if (!usernameInput.empty() && !passwordInput.empty() && !confirmPasswordInput.empty())
                    {
                        keyPressSound.getSound().play();
                        auth.setUserName(usernameInput);
                        auth.setPassword(passwordInput);
                        if(confirmPasswordInput==passwordInput)
                        result = auth.registerUser();


                        if (confirmPasswordInput != passwordInput)
                        {
							window.draw(SignUpPasswordErrorSprite[2]);
							window.display();
							sleep(seconds(5));
							window.clear();
							usernameInput = "";
							passwordInput = "";
							confirmPasswordInput = "";
							selectedIndex = 0;
							isUsernameSelected = false;
							isPasswordSelected = false;
							isConfirmPasswordSelected = false;
							typing = false;
							typingUsername = false;
							typingPassword = false;
							typingConfirmPassword = false;
						}
                        else if (result == 0 || result == 1 || result == 2)
                        {
                            //show the sprite for 5 second and then restart the thing and donot clear the window it is a popup
                            if (result == 0)
                                window.draw(SignUpUserNameErrorSprite);
                            else if (result == 2)
                                window.draw(SignUpPasswordErrorSprite[0]);
                            else
                                window.draw(SignUpPasswordErrorSprite[1]);
                            window.display();
                            sleep(seconds(5));
                            window.clear();
                            usernameInput = "";
                            passwordInput = "";
                            confirmPasswordInput = "";
                            selectedIndex = 0;
                            isUsernameSelected = false;
                            isPasswordSelected = false;
                            isConfirmPasswordSelected = false;
                            typing = false;
                            typingUsername = false;
                            typingPassword = false;
                            typingConfirmPassword = false;

                        }
                        else
                        {
                            window.draw(SignUpSuccessSprite);
                            window.display();
                            sleep(seconds(5));
                            window.clear();
                            isSignUpSuccessful = true;
                        }
                    }
                }
            }
        }

        if (isSignUpSuccessful)
        {
            return 2; // Login successful
        }

        // Cursor blinking logic
        if (cursorTimer.getElapsedTime().asSeconds() >= 0.5f)
        {
            showCursor = !showCursor;
            cursorTimer.restart();
        }

        if (!usernameInput.empty() && !passwordInput.empty() && !confirmPasswordInput.empty())
        {
            selectedIndex = 4;
        }

        window.clear(Color::White);
        window.draw(SignUpSprites[selectedIndex]);

        // Username field drawing
        if (isUsernameSelected || !usernameInput.empty())
        {
            if (showCursor && typingUsername)
                usernameText.setString(usernameInput + "_");
            else
                usernameText.setString(usernameInput);

            window.draw(usernameText);
        }

        // Password field drawing
        if (isPasswordSelected || !passwordInput.empty())
        {
            string maskedPassword(passwordInput.length(), '*'); // show asterisks '*'
            if (showCursor && typingPassword)
                passwordText.setString(maskedPassword + "_");
            else
                passwordText.setString(maskedPassword);

            window.draw(passwordText);
        }

        // Confirm Password field drawing
        if (isConfirmPasswordSelected || !confirmPasswordInput.empty())
        {
			string maskedConfirmPassword(confirmPasswordInput.length(), '*'); // show asterisks '*'
			if (showCursor && typingConfirmPassword)
				confirmPasswordText.setString(maskedConfirmPassword + "_");
			else
				confirmPasswordText.setString(maskedConfirmPassword);

			window.draw(confirmPasswordText);
		}

        window.display();
    }

    return -1;
}


int MenuHandler::IntroAnimation()
{
    Texture * PlayAnimatedIntro = new Texture[72];
    Sprite * PlayAnimatedIntroSprite = new Sprite[72];
    for (int i = 0; i < 72; ++i)
    {
        string filename = "../IntroAnimated/ezgif-frame-";
        i < 9 ? filename += "00" : filename += "0";
        filename += to_string(i + 1) + ".png";
        if (!PlayAnimatedIntro[i].loadFromFile(filename));
        {
           // cout << "Error loading Play Animated Intro texture " << i << endl;
        }
        PlayAnimatedIntroSprite[i].setTexture(PlayAnimatedIntro[i]);
        PlayAnimatedIntroSprite[i].setPosition(0, 0);
    }

    Clock clock;
    float frameTime = 0.05f;
    int currentFrame = 0;
    int totalFrames = 72; 

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if (clock.getElapsedTime().asSeconds() >= frameTime)
        {
            currentFrame++;
            clock.restart();
        }

        if (currentFrame >= totalFrames)
            break; 

        window.clear();
        window.draw(PlayAnimatedIntroSprite[currentFrame]);
        window.display();
    }

    return 0; 
}

int MenuHandler::getWins()
{
	string fileName = "../PlayersData/" + auth.getUserName() + ".txt";
	ifstream file(fileName);
    if (!file)
    {
		cout << "Error loading user file." << endl;
		return -1;
	}
	string line;
    while (getline(file, line))
    {
        if (line.find("Wins: ") != string::npos)
        {
			int wins = stoi(line.substr(line.find(": ") + 2));
			file.close();
			return wins; 
		}
	}

	file.close();
	return 0; 
}

int MenuHandler::getScore()
{
    string fileName = "../PlayersData/" + auth.getUserName() + ".txt";
	ifstream file(fileName);
    if (!file)
    {
		cout << "Error loading user file." << endl;
		return -1;
	}
	string line;
    while (getline(file, line))
    {
        if (line.find("Score: ") != string::npos)
        {
			int score = stoi(line.substr(line.find(": ") + 2));
			file.close();
			return score; 
		}
	}

	file.close();
	return 0; 

}

int MenuHandler::getProfileIcon()
{
    string fileName = "../PlayersData/" + auth.getUserName() + ".txt";
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

void MenuHandler::setProfileIcon(int iconIndex)
{
    string fileName = "../PlayersData/" + auth.getUserName() + ".txt";
    ifstream inFile(fileName);
    if (!inFile)
    {
        cout << "Error opening user file for reading." << endl;
        return;
    }

    const int MAX_LINES = 50;
    string lines[MAX_LINES]; // assuming file has fewer than 50 lines
    int count = 0;

    while (getline(inFile, lines[count]) && count < MAX_LINES)
    {
        count++;
    }
    inFile.close();

    if (count < 3)
    {
        cout << "Unexpected file format: less than 3 lines." << endl;
        return;
    }

    lines[2] = "ProfileIcon: " + to_string(iconIndex); // Update line 3 (index 2)

    ofstream outFile(fileName);
    if (!outFile)
    {
        cout << "Error opening user file for writing." << endl;
        return;
    }

    for (int i = 0; i < count; i++)
    {
        outFile << lines[i] << endl;
    }

    outFile.close();
}

int MenuHandler::selectProfileIconMenu()
{
    const int totalIcons = 16;
    Sprite iconSprites[totalIcons];

    // Load icon textures
    Texture iconTextures[totalIcons];
    for (int i = 0; i < totalIcons; ++i)
    {
        if (!iconTextures[i].loadFromFile("../PlayerProfiles/p" + to_string(i + 1) + ".png"))
        {
            cout << "Failed to load icon " << i + 1 << endl;
        }
        iconSprites[i].setTexture(iconTextures[i]);
    }

    // Layout logic: positions for 3 rows
    for (int i = 0; i < totalIcons; ++i)
    {
        int row, col;
        if (i < 6) 
        {
            col = i;
            iconSprites[i].setPosition(130 + col * 130, 200);
        }
        else if (i < 11) 
        {
            col = i - 6;
            iconSprites[i].setPosition(200 + col * 130, 330);
        }
        else 
        {
            col = i - 11;
            iconSprites[i].setPosition(200 + col * 130, 455);
        }
    }

    // Cancel Icon
    Texture cancelTexture;
    Sprite cancelSprite;
    if (!cancelTexture.loadFromFile("../images/cancel.png"))
        cout << "Failed to load cancel icon\n";
    cancelSprite.setTexture(cancelTexture);
    cancelSprite.setPosition(865, 30);
    cancelSprite.setScale(0.9f, 0.9f);

    // Background
    Texture BackgroundTexture[2];
    if (!BackgroundTexture[0].loadFromFile("../images/ProfileMenu1.png"))
        cout << "Error loading Background texture 1\n";
    if (!BackgroundTexture[1].loadFromFile("../images/ProfileMenu2.png"))
        cout << "Error loading Background texture 2\n";

    Sprite BackgroundSprite[2];
    BackgroundSprite[0].setTexture(BackgroundTexture[0]);
    BackgroundSprite[1].setTexture(BackgroundTexture[1]);
    BackgroundSprite[0].setPosition(0, 0);
    BackgroundSprite[1].setPosition(0, 0);

    bool isBackground1 = false; 
    FloatRect confirmBox(274,576,475,93); 
    
    CircleShape highlightCircle(65.f); // Radius slightly bigger than 60 (half of 120)
    highlightCircle.setFillColor(Color::Transparent);
    highlightCircle.setOutlineThickness(4.f);
    highlightCircle.setOutlineColor(Color::Red);



    int selectedIndex = getProfileIcon() - 1;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return -1;
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                return -1;
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (cancelSprite.getGlobalBounds().contains(mousePos))
                {
                    keyPressSound.getSound().play();
                    return -1;
                }

                for (int i = 0; i < totalIcons; ++i)
                {
                    if (iconSprites[i].getGlobalBounds().contains(mousePos))
                    {
                        keyPressSound.getSound().play();
                        selectedIndex = i;
                        isBackground1 = true;
                        break;
                    }
                }

                // Confirm click
                if (confirmBox.contains(mousePos))
                {
                    keyPressSound.getSound().play();
                    setProfileIcon(selectedIndex + 1);
                    return selectedIndex;
                }
            }
        }

        // Rendering
        window.clear(Color::White);
        if (!isBackground1)
            window.draw(BackgroundSprite[0]);
        else
            window.draw(BackgroundSprite[1]);

        for (int i = 0; i < 16; ++i)
        {
            if (i == selectedIndex)
            {
                // Center the highlight around the icon
                highlightCircle.setPosition(
                    iconSprites[i].getPosition().x + 60 - 65, // center X - radius
                    iconSprites[i].getPosition().y + 60 - 65  // center Y - radius
                );
                window.draw(highlightCircle);
            }

            window.draw(iconSprites[i]);
        }


        window.draw(cancelSprite);
        window.display();
    }


    return selectedIndex;
}


int MenuHandler::PlayerProfileMenu()
{
    int selectedIndex = getProfileIcon();
    selectedIndex--;
    bool choosingIcon = false;
    int gridIndex = 0;
   // FriendRequest 

    Texture CancelIconTexture;
    if (!CancelIconTexture.loadFromFile("../images/cancel.png"))
    {
		cout << "Error loading Cancel Icon texture" << endl;
	}
    Sprite CancelIconSprite;
    CancelIconSprite.setTexture(CancelIconTexture);
    CancelIconSprite.setPosition(860, 13); 
    CancelIconSprite.setScale(0.9f, 0.9f); // Scale down the icon
    PlayerProfileThemesSprite[selectedIndex].setPosition(250, 180);

    //Also show to name of the user 
    string userName = auth.getUserName();
    Font font;
    if (!font.loadFromFile("../Font2/Category.ttf"))
    {

    }
    Text PlayerProfileText;
    PlayerProfileText.setFont(font);
    PlayerProfileText.setCharacterSize(35);
    PlayerProfileText.setFillColor(sf::Color(148, 180, 255));
    PlayerProfileText.setPosition(380, 212);
    PlayerProfileText.setString(userName);
    PlayerProfileText.setStyle(Text::Bold);
    PlayerProfileText.setOutlineThickness(5);

    Text ScoreText;
    ScoreText.setFont(font);
    ScoreText.setCharacterSize(35);
    ScoreText.setFillColor(sf::Color(148, 180, 255));
    ScoreText.setPosition(590, 380);
    ScoreText.setString(to_string(getScore()));
    ScoreText.setStyle(Text::Bold);
    ScoreText.setOutlineThickness(5);

    Text WinsText;
    WinsText.setFont(font);
    WinsText.setCharacterSize(35);
    WinsText.setFillColor(sf::Color(148, 180, 255));
    WinsText.setPosition(310, 380);
    WinsText.setString(to_string(getWins()));
    WinsText.setStyle(Text::Bold);
    WinsText.setOutlineThickness(5);


    int backgroundIndex = 0;
    FloatRect FriendBox(221, 480, 251, 107); 
    FloatRect LeaderBoardBox(536, 480, 248, 107); 

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
                if (choosingIcon)
                    choosingIcon = false;
                else
                    return -1;
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (CancelIconSprite.getGlobalBounds().contains(mousePos))
                {
					keyPressSound.getSound().play();
					return -1;
				}
                if (PlayerProfileThemesSprite[selectedIndex].getGlobalBounds().contains(mousePos))
                {
                    int selected = selectProfileIconMenu();
                    if (selected != -1)
                    {
                        selectedIndex = selected;
                    }
				}
                if (FriendBox.contains(mousePos))
                {
                    keyPressSound.getSound().play();
                    FriendRequest friendRequest(window);
                    friendRequest.handleMenuLoop(auth.getUserName());
                    keyPressSound.getSound().play();
                }

                if (LeaderBoardBox.contains(mousePos))
                {
					keyPressSound.getSound().play();
                    LeaderBoard leaderboard(window);
					leaderboard.displayLeaderboard();
                    keyPressSound.getSound().play();
				}
            }

            //if mouse cursor comes upon friend box then show the player profile index 1 and if it comes on leaderboard box then show the player profile index 2 else 0
            if (event.type == Event::MouseMoved)
            {
				Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
                if (FriendBox.contains(mousePos))
                {
					backgroundIndex = 1;
				}
                else if (LeaderBoardBox.contains(mousePos))
                {
					backgroundIndex = 2;
				}
                else
                {
					backgroundIndex = 0;
				}
			}


        }

        window.clear(Color::White);
        window.draw(PlayerProfileSprite[backgroundIndex]);
        window.draw(CancelIconSprite);
        window.draw(PlayerProfileThemesSprite[selectedIndex]);
        window.draw(PlayerProfileText);
        window.draw(ScoreText);
        window.draw(WinsText);
        window.display();
    }

    return selectedIndex;
}


int MenuHandler::handleMenuLoop()
{
    IntroAnimation();
	GameStartMenu();
	keyPressSound.play();
	while (true)
	{
			int startOption = SignInSignUpMenu();
			keyPressSound.play();

			if (startOption == -1)
			{
				break; 
			}

            if (startOption == 0)
            {
				SignUpMenu();
				keyPressSound.play();
			}
            else if (startOption == 1)
            {
				int a = SignInMenu();
				keyPressSound.play();
                if (a == 2)
                    break;
			}
	}

    return 0;
}


int MenuHandler::mainMenu()
{
    String themePath = "../Themes/Theme" + to_string(getPlayerThemeID()) + "/MainMenu" ;
    int selectedIndex = 0;
    
    Texture mainMenuTexture[5];
    Sprite mainMenuSprite[5];
    for (int i = 0; i < 5; ++i)
    {
        if (!mainMenuTexture[i].loadFromFile(themePath + to_string(i+1) + ".png"))
        {
			cout << "Error loading Main Menu texture " << i << endl;
		}
		mainMenuSprite[i].setTexture(mainMenuTexture[i]);
		mainMenuSprite[i].setPosition(0, 0);
	}

    int PlayerProfileIcon = getProfileIcon();
    PlayerProfileIcon--;
    PlayerProfileThemesSprite[PlayerProfileIcon].setPosition(7, 160);
    //PlayerProfileThemesSprite[PlayerProfileIcon].setScale(1.17f, 1.17f);

    Font font;
    if (!font.loadFromFile("../Font2/Category.ttf"))
    {

    }
    Text PlayerProfileText;
    PlayerProfileText.setFont(font);
    PlayerProfileText.setCharacterSize(22);
    PlayerProfileText.setFillColor(Color::White);
    PlayerProfileText.setPosition(135, 203);
    PlayerProfileText.setString(auth.getUserName());
    PlayerProfileText.setStyle(Text::Bold);


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
				return -1;
			}

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
				Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (PlayerProfileThemesSprite[PlayerProfileIcon].getGlobalBounds().contains(mousePos))
                {
                    keyPressSound.getSound().play();
                    PlayerProfileMenu();
                    keyPressSound.getSound().play();
                    return 0;
                }
			}
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up)
                {
					keyPressSound.getSound().play();
					selectedIndex = (selectedIndex - 1 + 5) % 5;
				}
                else if (event.key.code == Keyboard::Down)
                {
					keyPressSound.getSound().play();
					selectedIndex = (selectedIndex + 1) % 5;
				}
                else if (event.key.code == Keyboard::Enter)
                {
					keyPressSound.getSound().play();
                    if (selectedIndex == 0)
                        selectedIndex = -2;
                    if (selectedIndex == 4)
                    {
                        selectedIndex = -1;
                        window.close();
                    }
                        
					return selectedIndex; 
				}
			}

		}

        window.clear();
		window.draw(mainMenuSprite[selectedIndex]);
        window.draw(PlayerProfileText);
        window.draw(PlayerProfileThemesSprite[PlayerProfileIcon]);
		window.display();
    }
}

int MenuHandler::handleMainMenuLoop()
{
    Inventory inventory(window, auth.getUserName());
    int choice = 0;
    while (choice != -1 && window.isOpen())
    {
        
        switch (choice)
        {
			case 0:
				choice = mainMenu();
				break;
            case -2:
                choice = startMenu();
                break;
			case 1:
                return 1;
				break;
			case 2:
                choice = inventory.handleInventoryMenu();
                break;
            case 3:
                choice = showRules();
				break;
            case 4:
            {
                MatchSimulator match(window, auth);
                choice = match.matchSimulatorMenu(getPlayerThemeID());
                keyPressSound.getSound().play();
            }
				break;
            case 5:
            {
                int choice2= difficultyMenu();
                if (choice2 == -2)
                    choice = -2;
                else if (choice2 == 0)
                    return 3;
                else if (choice2 == 1)
                    return 4;
                else if (choice2 == 2)
                    return 5;
            }
                break;
            case 6:
                return 6; 
                break;
			default:
				break;
		}
    }
}


int MenuHandler::getPlayerThemeID()
{
    string fileName = "../PlayersData/" + auth.getUserName() + ".txt";
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


int MenuHandler::showRules()
{
    Texture rulesTexture;
    String themePath = "../Themes/Theme" + to_string(getPlayerThemeID()) + "/Rules.png";
    if (!rulesTexture.loadFromFile(themePath))
    {

    }
	Sprite rulesSprite;
	rulesSprite.setTexture(rulesTexture);
	rulesSprite.setPosition(0, 0);

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
		}

		window.clear();
		window.draw(rulesSprite);
		window.display();
	}

    return 0;
}

int MenuHandler::startMenu()
{
    String themePath = "../Themes/Theme" + to_string(getPlayerThemeID()) + "/StartMenu";
    int selectedIndex = 0;

    Texture StartMenuTexture[3];
    Sprite StartMenuSprite[3];
    for (int i = 0; i < 3; ++i)
    {
        if (!StartMenuTexture[i].loadFromFile(themePath + to_string(i + 1) + ".png"))
        {
            cout << "Error loading Main Menu texture " << i << endl;
        }
        StartMenuSprite[i].setTexture(StartMenuTexture[i]);
        StartMenuSprite[i].setPosition(0, 0);
    }

    while(window.isOpen())
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
                    if (selectedIndex == 0)
                        selectedIndex = 5;
                    if(selectedIndex == 1)
                        selectedIndex = 6;
                    if (selectedIndex == 2)
                        selectedIndex = 4;
					return selectedIndex; 
				}
			}
		}

		window.clear(Color::White);
		window.draw(StartMenuSprite[selectedIndex]);
		window.display();
    }
    return 0;
}

int MenuHandler::difficultyMenu()
{
    String themePath = "../Themes/Theme" + to_string(getPlayerThemeID()) + "/Level";
    int selectedIndex = 0;
    Texture LevelTexture[3];
    Sprite LevelSprite[3];
    for (int i = 0; i < 3; ++i)
    {
        if (!LevelTexture[i].loadFromFile(themePath + to_string(i + 1) + ".png"))
        {
			cout << "Error loading Level texture " << i << endl;
		}
		LevelSprite[i].setTexture(LevelTexture[i]);
		LevelSprite[i].setPosition(0, 0);
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

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
				keyPressSound.getSound().play();
				return -2;
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
		window.draw(LevelSprite[selectedIndex]);
		window.display();
	}
	return -2;
}