#pragma once
#include <iostream>
#include <string>

using namespace std;

class Authentication
{
private:
    string userName;
    string password;
    bool isAuthenticated;
    string fileName;

public:
    Authentication();

    int registerUser();
    int loginUser();
    bool isAuthenticationSuccessful();

    void setFileName(string file);
    string getFileName();

    void setUserName(string name);
    void setPassword(string pass);
    string getUserName();
    string getPassword();

    void setAuthenticationStatus(bool status);
    bool getAuthenticationStatus();

    bool inputUserName();
    bool inputPassword();

    bool usernameExists();
    int validatePasswordStrength();
};


