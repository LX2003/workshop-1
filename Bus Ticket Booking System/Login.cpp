#include <iostream>
#include <string>
#include <conio.h>
#include <regex>
#include <iomanip>
#include "Login.h"
#include "MySQL.h"
#include "Passenger.h"
#include "Menu.h"
#include "Staff.h"
#include "Admin.h"
using namespace std;

const string RED = "\033[1;31m";
const string GREEN = "\033[1;32m";
const string BLUE = "\033[1;34m";
const string YELLOW = "\033[1;33m";
const string CYAN = "\033[1;36m";
const string RESET = "\033[0m";

void Login::RegisterPage()
{
	MySQL db;
    Menu menu;
	string Username, pswd, name, phoneNum, email, gender, input;
	int age, choice;

    while (true)
    {
        while (true)
        {
            system("cls");
            cout << YELLOW << "\t\t\t\t\t\t\t-------------------------------------------\n\n";
            cout << CYAN << "\t\t\t\t\t\t\t\t\tREGISTRATION\n\n";
            cout << YELLOW << "\t\t\t\t\t\t\t-------------------------------------------\n\n";
            cout << GREEN << "\t\t\t\t\t\t\t\t1. Register As Customer\n\n";
            cout << GREEN << "\t\t\t\t\t\t\t\t2. Return to Main Page\n\n";
            cout << RESET << "\t\t\t\t\t\t\t\tEnter your Choice: ";

            if (cin.peek() == '\n') {
                cin.ignore(); // consume the newline
                cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 3.\n" << RESET;
                system("pause");
                continue;
            }

            cin >> choice;

            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a valid input!\n" << RESET;
                system("pause");
            }
            else
            {
                break;
            }
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1)
        {
            system("cls");
            cout << CYAN << "\t\t\t\t\t\t-----------------------------------------------------\n\n";
            cout << "\t\t\t\t\t\t\t\tWELCOME TO REGISTRATION\n\n";
            cout << "\t\t\t\t\t\t-----------------------------------------------------\n\n";
            cout << RESET << "\t\t\t\t\t\t\tPlease Enter Your Personal Information\n\n";

            while (true)
            {
                cout << "\t\t\t\t\t\t\tName: ";
                getline(cin, name);

                if (name.empty())
                {
                    cout << RED << "\n\t\t\t\t\t\tName cannot be empty. Please enter a name.\n" << RESET;
                    continue;
                }
                else
                    break;
            }

            while (true) {
                cout << RESET << "\n\t\t\t\t\t\t\tAge: ";
                getline(cin, input);  // Use getline to capture the line of input

                if (input.empty()) {
                    cout << RED << "\n\t\t\t\t\t\tInput cannot be empty. Please enter a valid number for age.\n" << RESET;
                    continue;  // Restart the loop
                }

                try {
                    age = stoi(input);  // Convert string to integer
                    if (age <= 0 || age > 100) {
                        cout << RED << "\n\t\t\t\t\t\tAge must be between 0-100. Please try again.\n" << RESET;
                        continue;
                    }
                    break;
                }
                catch (const invalid_argument&) {
                    cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a valid number for age.\n" << RESET;
                }
            }

            while (true)
            {
                cout << RESET << "\n\t\t\t\t\t\t\tGender(M for Male, F for Female): ";
                getline(cin, gender);

                if (gender == "m" || gender == "M")
                {
                    break;
                }
                else if (gender == "f" || gender == "F")
                {
                    break;
                }
                else
                {
                    cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter 'M' for Male or 'F' for Female.\n" << RESET;
                    continue;
                }
            }

            //set phone number format
            while (true) 
            {
                cout << RESET << "\n\t\t\t\t\t\t\tPhone Number (e.g 0XX-XXXXXXXX): ";
                getline(cin, phoneNum);
                regex phone_regex("^\\d{3}-\\d{7,8}$");

                if (regex_match(phoneNum, phone_regex)) 
                    break;
                else 
                    cout << RED << "\n\t\t\t\t\t\tInvalid phone number format. Please try again!\n" << RESET;
            }

            // set email format
            while (true) 
            {
                cout << RESET << "\n\t\t\t\t\t\t\tEmail Address: ";
                getline(cin, email);
                regex email_regex("^[\\w.%+-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
                if (regex_match(email, email_regex)) 
                    break;
                else 
                    cout << RED << "\n\t\t\t\t\t\tInvalid email format. Please try again!\n";
            }

            while (true)
            {
                cout << "\n\t\t\t\t\t\t\tUsername: ";
                getline(cin, Username);

                if (Username.empty())
                {
                    cout << RED << "\n\t\t\t\t\t\tUsername cannot be empty. Please enter a username.\n" << RESET;
                    continue;
                }
                else
                    break;
            }

            while (true) {
                cout << "\n\t\t\t\t\t\t\tPassword (8-16 characters): ";
                getline(cin, pswd);

                if (pswd.length() >= 8 && pswd.length() <= 16) {
                    break;
                }
                else {
                    cout << RED << "\n\t\t\t\t\t\t\tPlease enter between 8 and 16 characters.\n\n" << RESET;
                }
            }

            db.prepareStatement("INSERT INTO user (username,password,userType) VALUES(?, ?, 'Passenger')");
            db.statement->setString(1, Username);
            db.statement->setString(2, pswd);
            db.QueryStatement();

            string userId;
            db.prepareStatement("SELECT * FROM user ORDER BY userID DESC LIMIT 1");
            db.QueryResult();
            db.result->next();
            userId = db.result->getString("userID");

            db.prepareStatement("INSERT INTO passenger (name,age,gender,phoneNo,email,userID) VALUES(?, ?, ?, ?, ?, ?)");
            db.statement->setString(1, name);
            db.statement->setInt(2, age);
            db.statement->setString(3, gender);
            db.statement->setString(4, phoneNum);
            db.statement->setString(5, email);
            db.statement->setString(6, userId);
            db.QueryStatement();

            string pId;
            db.prepareStatement("SELECT * FROM passenger ORDER BY passengerID DESC LIMIT 1");
            db.QueryResult();
            db.result->next();
            pId = db.result->getString("passengerID");

            db.prepareStatement("INSERT INTO wallet (passengerID) VALUES (?)");
            db.statement->setString(1, pId);
            db.QueryStatement();

            cout << GREEN << "\n\t\t\t\t\t\t\tCustomer Registration Successful!\n" << RESET;
            system("pause");
            menu.HomeMenu();
            break;
        }

        else if (choice == 2)
        {
            menu.HomeMenu();
            break;
        }
        
        else
        {
            cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a valid input!\n" << RESET;
            system("pause");
            continue;
        }
    }
}

void Login::LoginPage()
{
    MySQL db;
    Menu menu;
    string UserName, UserPassword, Usertype;
    int options;
    bool loginSucess = 0;

    while (true)
    {
        while (true)
        {
            system("cls");
            cout << CYAN << "\t\t\t\t\t\t\t-------------------------------------\n\n";
            cout << RED << "\t\t\t\t\t\t\t\t\tLOGIN\n\n";
            cout << CYAN << "\t\t\t\t\t\t\t-------------------------------------\n\n";
            cout << BLUE << "\t\t\t\t\t\t\t\t1.User Login\n\n";
            cout << "\t\t\t\t\t\t\t\t2.Admin Mode\n\n";
            cout << "\t\t\t\t\t\t\t\t3.Return to Main Page\n\n";
            cout << setw(68) << GREEN << "Enter your option: " << RESET;

            if (cin.peek() == '\n') {
                cin.ignore(); // consume the newline
                cout << RED << "\n\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 3.\n" << RESET;
                system("pause");
                continue;
            }

            if (!(cin >> options)) // Check if input is invalid (not an integer)
            {
                cin.clear();  // Clear the error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
                cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 3!" << RESET << endl;
                system("pause");
                continue;  // Restart the loop
            }
            else
            {
                break;
            }
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (options == 1)
        {
            while (!loginSucess)
            {
                system("cls");
                cout << CYAN << "\n\t\t\t\t\t\t\t\t----------- USER LOGIN -----------\n\n";
                cout << RESET << "\t\t\t\t\t\t\t\tUsername: ";
                getline(cin, UserName);

                char ch;

                UserPassword.clear();
                cout << "\n\n\t\t\t\t\t\t\t\tPassword: ";
                while (true)
                {
                    ch = _getch();

                    // Break the loop if the user presses 'Enter' (both '\r' or '\n')
                    if (ch == '\r' || ch == '\n')
                    {
                        break;
                    }

                    //detect backspace
                    if (ch == '\b')
                    {
                        if (!UserPassword.empty()) {
                            cout << "\b \b";  // Move back, erase character, move back again
                            UserPassword.pop_back();
                        }
                    }

                    // Display error message for spaces
                    else if (ch == ' ')
                    {
                        cout << RED << "\n\t\t\t\t\tPassword cannot have space. Please Try Again!\n";
                        cout << RESET << "Enter password: ";
                        UserPassword.clear();
                        break;
                    }

                    else
                    {
                        UserPassword += ch;
                        cout << '*';
                    }
                }

                db.prepareStatement("SELECT userID, userType FROM user WHERE username = ? AND password = ?");
                db.statement->setString(1, UserName);
                db.statement->setString(2, UserPassword);
                db.QueryResult();

                if (db.result->next())
                {
                    string userID = db.result->getString("userID");
                    Usertype = db.result->getString("userType");
                    loginSucess = true;

                    if (Usertype == "Passenger")
                    {
                        db.prepareStatement("SELECT * FROM passenger p JOIN user u ON p.userID = u.userID JOIN wallet w ON p.passengerID = w.passengerID WHERE u.userID = '" + userID + "' AND p.status IN('Active', 'Pending')");
                        db.QueryResult();
                        db.result->next();

                        Passenger passenger;
                        passenger.id = db.result->getString("passengerID");
                        passenger.name = db.result->getString("name");
                        passenger.age = db.result->getInt("age");
                        passenger.gender = db.result->getString("gender");
                        passenger.phoneNo = db.result->getString("phoneNo");
                        passenger.email = db.result->getString("email");
                        passenger.username = db.result->getString("username");
                        passenger.password = db.result->getString("password");
                        passenger.balance = db.result->getDouble("balance");

                        menu.CustMenu(passenger);
                        break;
                    }
                    else if (Usertype == "Staff")
                    {
                        db.prepareStatement("SELECT * FROM staff s JOIN user u ON s.userID = u.userID WHERE u.userID = '" + userID + "'");
                        db.QueryResult();
                        db.result->next();

                        Staff staff;
                        staff.staff_id = db.result->getString("staffID");
                        menu.StaffMenu(staff);
                        break;
                    }
                    else
                    {
                        cout << RED << "\n\t\t\t\t\tUser Not Found! Please Try Again.\n" << RESET;
                        system("pause");
                        loginSucess = 0;
                    }
                }
                else
                {
                    cout << RED << "\n\t\t\t\t\tInvalid username or password. Please Try Again!" << RESET << endl;
                    system("pause");
                }
            }
            break;
        }
        else if (options == 2)
        {
            string adminUser, adminPass;
            char ch;

            while (true)
            {
                system("cls");
                cout << CYAN << "\n\t\t\t\t\t\t\t\t----------- ADMIN MODE -----------\n\n";
                cout << RESET << "\t\t\t\t\t\t\t\tAdmin Username: ";
                getline(cin, adminUser);

                adminPass.clear();

                cout << "\n\t\t\t\t\t\t\t\tAdmin Password: ";
                while (true)
                {
                    ch = _getch();

                    // break the loop if the user presses 'Enter' (both '\r' or '\n')
                    if (ch == '\r' || ch == '\n')
                    {
                        break;
                    }

                    //detect backspace
                    if (ch == '\b')
                    {
                        if (!adminPass.empty())
                        {
                            cout << "\b \b";  // Move back, erase character, move back again
                            adminPass.pop_back();
                        }
                    }

                    // display error message for spaces
                    else if (ch == ' ')
                    {
                        cout << RED << "\n\t\t\t\t\t\t\tPassword cannot have space. Please Try Again!\n";
                        cout << RESET << "\n\t\t\t\t\t\t\tEnter Admin Password: ";
                        adminPass.clear();
                        break;
                    }

                    else
                    {
                        adminPass += ch;
                        cout << '*';
                    }
                }

                db.prepareStatement("SELECT * FROM admin WHERE username = ? AND password = ?");
                db.statement->setString(1, adminUser);
                db.statement->setString(2, adminPass);
                db.QueryResult();

                Admin admin;
                if (db.result->next())
                {
                    admin.a_id = db.result->getString("adminID");
                    menu.AdminMenu(admin);
                    break;
                }
                else
                {
                    cout << RED << "\n\t\t\t\t\t\t\tAdmin verification failed. Access denied.\n" << RESET;
                    system("pause");
                    continue;
                }
            }
            break;
        }
        else if (options == 3)
        {
            menu.HomeMenu();
            break;
        }
        else
        {
            cout << RED << "\n\t\t\t\t\t\tInvalid value! Please enter number between 1 - 3.\n" << RESET;
            system("pause");
            continue;
        }
    }
}

