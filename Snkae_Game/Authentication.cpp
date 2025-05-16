#include "Authentication.h"
#include <fstream>
#include<iostream>

Authentication::Authentication()
{
	isAuthenticated = false;
	fileName = "";
	userName = "";
	password = "";
}

bool Authentication:: inputUserName()
{
	cout << "Enter username: ";
	cin >> userName;

	if (userName.empty())
	{
		cout << "Username cannot be empty." << endl;
		return false;
	}

	fileName = "../PlayersData/" + userName + ".txt";

	return true;
}

bool Authentication::inputPassword()
{
	cout << "Enter password: ";
	cin >> password;

	return !password.empty();
}

bool Authentication::usernameExists()
{	
	ifstream file(fileName);

	return file.good();
}

int Authentication::validatePasswordStrength()
{
	if (password.length() < 7)
	{
		cout << "Password must be at least 7 characters long." << endl;
		return 0;
	}

	bool hasUpper = false, hasLower = false, hasDigit = false;

	for (char c : password)
	{
		if (isupper(c)) hasUpper = true;
		else if (islower(c)) hasLower = true;
		else if (isdigit(c)) hasDigit = true;
	}

	if (!hasUpper || !hasLower || !hasDigit)
	{
		cout << "Password must contain at least one uppercase letter, one lowercase letter, and one digit." << endl;
		return 1;
	}

	return 2;
}

int Authentication::registerUser()
{
	if (usernameExists())
	{
		cout << "Username already exists. Please choose a different username." << endl;
		return 0;
	}

	int passwordStrength = validatePasswordStrength();
	if (passwordStrength == 0)
	{
		cout << "Password is less than 7 characters" << endl;
		return 1;
	}
	else if (passwordStrength == 1)
	{
		cout << "Password does not meet the strength requirements." << endl;
		return 2;
	}
	srand(time(0));
	ofstream file(fileName);
	if (!file)
	{
		cout << "Error creating user file." << endl;
		return false;
	}
	file << "Username: " << userName << endl;
	file << "Password: " << password << endl;
	file << "ProfileIcon: "<<rand()%17<<endl;
	file << "Score: 0" << endl;
	file << "Wins: 0" << endl;
	file << "ThemeID: "<<rand()%5+1<<endl;
	file << "Powerups: 0" << endl;
	file << "HighScore: 0" << endl;
	file.close();

	string fileName2 = "../GameUsers.txt";
	ofstream file2(fileName2, ios::app);
	if (!file2)
	{
		cout << "Error creating user file." << endl;
		return false;
	}
	file2 << userName << endl;
	file2.close();

	string fileName3 = "../Friends/" + userName + "_friends.txt";
	ofstream file3(fileName3);
	if (!file3)
	{
		cout << "Error creating user file." << endl;
		return false;
	}

	fileName = "../Pendings/" + userName + "_pending.txt";
	ofstream file4(fileName);
	if (!file4)
	{
		cout << "Error creating user file." << endl;
		return false;
	}
	file4.close();
	file3.close();
	cout << "Registration successful!" << endl;
	return 3;
}

int Authentication::loginUser()
{
	cout<<fileName<<endl;
	if (!usernameExists())
	{
		cout << "Username does not exist." << endl;
		return 0;
	}

	// Load user data from file
	ifstream file(fileName);
	if (!file)
	{
		cout << "Error loading user file." << endl;
		return 0;
	}
	string line;
	while (getline(file, line))
	{
		if (line.find("Password: ") != string::npos)
		{
			string storedPassword = line.substr(line.find(": ") + 2);
			if (storedPassword != password)
			{
				cout << "Incorrect password." << endl;
				return 1;
			}
		}
	}

	file.close();

	// If we reach here, the password is correct
	isAuthenticated = true;
	cout << "Login successful!" << endl;
	return 2;
}

bool Authentication::isAuthenticationSuccessful()
{
	return isAuthenticated;
}

void Authentication::setFileName(string file)
{
	fileName = file;
}

string Authentication::getFileName()
{
	return fileName;
}

void Authentication::setUserName(string name)
{
	userName = name;
	if (userName.empty())
	{
		cout << "Username cannot be empty." << endl;
	}
	else
	{
		fileName = "../PlayersData/" + userName + ".txt";
	}
}

string Authentication::getUserName()
{
	return userName;
}

void Authentication::setPassword(string pass)
{
	password = pass;
}

string Authentication::getPassword()
{
	return password;
}

void Authentication::setAuthenticationStatus(bool status)
{
	isAuthenticated = status;
}

bool Authentication::getAuthenticationStatus()
{
	return isAuthenticated;
}

