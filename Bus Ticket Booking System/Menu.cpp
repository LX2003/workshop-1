#include <iostream>
#include <string>
#include <iomanip>
#include "Menu.h"
#include "Login.h"
#include "Admin.h"
#include "Staff.h"
#include "Passenger.h"
using namespace std;

const string RED = "\033[1;31m";
const string GREEN = "\033[1;32m";
const string BLUE = "\033[1;34m";
const string YELLOW = "\033[1;33m";
const string CYAN = "\033[1;36m";
const string RESET = "\033[0m";

void Menu::HomeMenu()
{
	Login login;
	int opt;

	while (true)
	{
		system("cls");
		cout << CYAN << "\t\t\t\t\t\t----------------------------------------------------\n\n";
		cout << "\t\t\t\t\t\t\tWELCOME TO BUS TICKET BOOKING SYSTEM\n\n";
		cout << "\t\t\t\t\t\t----------------------------------------------------\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t\t1. REGISTER\n\n";
		cout << "\t\t\t\t\t\t\t\t2. LOGIN\n\n";
		cout << "\t\t\t\t\t\t\t\t3. EXIT\n\n" << RESET;
		cout << GREEN << "\t\t\t\t\t\t\t\tEnter Your Choice: " << RESET;

		if (!(cin >> opt))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 3!\n" << RESET;
			system("pause");
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (opt)
		{
		case 1:
			login.RegisterPage();
			return;
		case 2:
			login.LoginPage();
			return;
		case 3:
			exit(0);
		default:
			cout << RED << "\n\t\t\t\t\t\tInvalid value. Please enter number between 1 - 3!\n" << RESET;
			system("pause");
		}
	}
}

void Menu::AdminMenu(Admin admin)
{
	int ch;
	char choice;

	while (true)
	{
		system("cls");
		cout << CYAN << "\t\t\t\t\t\t\t-----------------------------------------\n\n";
		cout << "\t\t\t\t\t\t\t\tBUS TICKET BOOKING SYSTEM\n\n";
		cout << "\t\t\t\t\t\t\t\t\tADMIN MENU\n\n";
		cout << "\t\t\t\t\t\t\t-----------------------------------------\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t\t1. Manage Staff\n\n";
		cout << "\t\t\t\t\t\t\t\t2. Manage Bus\n\n";
		cout << "\t\t\t\t\t\t\t\t3. Refund Approval\n\n";
		cout << "\t\t\t\t\t\t\t\t4. Generate Report\n\n";
		cout << "\t\t\t\t\t\t\t\t5. Log Out\n\n" << RESET;
		cout << GREEN << "\t\t\t\t\t\t\t\tEnter Your Choice: " << RESET;

		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 5.\n" << RESET;
			system("pause");
			continue;
		}


		if (!(cin >> ch))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 5!\n" << RESET;
			system("pause");
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (ch)
		{
		case 1: admin.manageStaff(); return;
		case 2: admin.manageBus(); return;
		case 3: admin.refundApp(); return;
		case 4: admin.generateReport(); break;
		case 5:
			while (true)
			{
				cout << BLUE << "\n\t\t\t\t\t\t\tDo you confirm want to log out? (Y/N): " << RESET;
				cin >> choice;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (choice == 'y' || choice == 'Y')
				{
					HomeMenu();
					return;
				}
				else if (choice == 'n' || choice == 'N')
					break;
				else
				{
					cout << RED << "\n\t\t\t\t\t\t\tInvalid input. Please enter Y or N\n" << RESET;
					continue;
				}
			}
			break;
		default:
			cout << RED << "\n\t\t\t\t\t\tInvalid choice! Please enter number between 1 - 5.\n" << RESET;
			system("pause");
		}
	}
}

void Menu::StaffMenu(Staff staff)
{
	int opt;
	char choice;

	while (true)
	{
		system("cls");
		cout << CYAN << "\t\t\t\t\t\t\t-----------------------------------------\n\n";
		cout << "\t\t\t\t\t\t\t\t\tSTAFF MENU\n\n";
		cout << "\t\t\t\t\t\t\t-----------------------------------------\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t\t1. Manage Bus Schedule\n\n";
		cout << "\t\t\t\t\t\t\t\t2. Manage Ticket\n\n";
		cout << "\t\t\t\t\t\t\t\t3. Manage Passenger\n\n";
		cout << "\t\t\t\t\t\t\t\t4. Log Out\n\n" << RESET;
		cout << GREEN << "\t\t\t\t\t\t\t\tEnter Your Choice: " << RESET;

		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 4.\n" << RESET;
			system("pause");
			continue;
		}

		if (!(cin >> opt))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 4!\n" << RESET;
			system("pause");
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (opt)
		{
		case 1: staff.manageSchedule(); return;
		case 2: staff.cancelTicket(); return;
		case 3: staff.managePassenger(); return;
		case 4:
			while (true)
			{
				cout << BLUE << "\t\t\t\t\t\t\tDo you confirm want to log out? (Y/N): " << RESET;
				cin >> choice;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (choice == 'y' || choice == 'Y')
				{
					HomeMenu();
					return;
				}
				else if (choice == 'n' || choice == 'N')
					break;
				else
				{
					cout << RED << "\n\t\t\t\t\t\t\tInvalid input. Please enter Y or N\n" << RESET;
					continue;
				}
			}
			break;
		default:
			cout << RED << "\n\t\t\t\t\t\tInvalid choice! Please enter number between 1 - 4.\n" << RESET;
			system("pause");
		}
	}
}

void Menu::CustMenu(Passenger passenger)
{
	int ch;
	char choice;

	while (true)
	{
		system("cls");
		cout << CYAN << "\t\t\t\t\t\t\t-----------------------------------------\n\n";
		cout << "\t\t\t\t\t\t\t\tBUS TICKET BOOKING SYSTEM\n\n";
		cout << "\t\t\t\t\t\t\t\t\tMAIN MENU\n\n";
		cout << "\t\t\t\t\t\t\t-----------------------------------------\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t\t1. Search Bus\n\n";
		cout << "\t\t\t\t\t\t\t\t2. My Bookings\n\n";
		cout << "\t\t\t\t\t\t\t\t3. Profile\n\n";
		cout << "\t\t\t\t\t\t\t\t4. Log Out\n\n" << RESET;
		cout << GREEN << "\t\t\t\t\t\t\t\tEnter Your Choice: " << RESET;

		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 4.\n" << RESET;
			system("pause");
			continue;
		}

		if (!(cin >> ch))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 4!\n" << RESET;
			system("pause");
			continue;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (ch)
		{
		case 1: passenger.searchBus(); return;
		case 2: passenger.showBooking(); return;
		case 3: passenger.profile(); return;
		case 4:
			while (true)
			{
				cout << BLUE << "\t\t\t\t\t\t\tDo you confirm want to log out? (Y/N): " << RESET;
				cin >> choice;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (choice == 'y' || choice == 'Y')
				{
					HomeMenu();
					return;
				}
				else if (choice == 'n' || choice == 'N')
					break;
				else
				{
					cout << RED << "\n\t\t\t\t\t\t\tInvalid input. Please enter Y or N\n" << RESET;
					continue;
				}
			}
			break;
		default:
			cout << RED << "\n\t\t\t\t\t\tInvalid choice! Please enter number between 1 - 4.\n" << RESET;
			system("pause");
		}
	}
}

