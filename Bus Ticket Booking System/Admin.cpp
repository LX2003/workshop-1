#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <regex>
#include <vector>
#include "Admin.h"
#include "MySQL.h"
#include "Menu.h"
using namespace std;

#define RESET   "\033[0m"
#define GREEN   "\033[1;32m"
#define RED     "\033[31m" 

const string BLUE = "\033[1;34m";
const string YELLOW = "\033[1;33m";
const string CYAN = "\033[1;36m";

bool isValidReportDate(const std::string& dateStr)
{
	struct tm inputDate = {};
	std::istringstream ss(dateStr);
	ss >> std::get_time(&inputDate, "%Y-%m-%d");

	if (ss.fail())
	{
		return false; // Invalid date format
	}

	// Convert input date to time_t
	time_t inputTime = mktime(&inputDate);

	// Get current time
	time_t now = time(0);
	struct tm current = {};
	localtime_s(&current, &now);

	// Reject if before Oct 1, 2024
	struct tm minDate = {};
	minDate.tm_year = 2024 - 1900;
	minDate.tm_mon = 9;  // October (0-based)
	minDate.tm_mday = 1;
	time_t minAllowed = mktime(&minDate);

	if (difftime(inputTime, minAllowed) < 0)
		return false;

	// Allow up to 6 months ago
	struct tm sixMonthsAgo = current;
	sixMonthsAgo.tm_mon -= 6;
	mktime(&sixMonthsAgo); // Normalize
	time_t sixMonthsAgoTime = mktime(&sixMonthsAgo);

	if (difftime(inputTime, sixMonthsAgoTime) < 0)
		return false;

	// Reject future dates
	if (difftime(inputTime, now) > 0)
		return false;

	return true;
}

void drawBarChart(const std::string& chartTitle, const std::vector<TrendData>& data) 
{
	if (data.empty()) {
		cout << "No data to display.\n";
		return;
	}

	// --- 1. Find Maximum Count ---
	int maxCount = 0;
	for (const auto& item : data) {
		if (item.count > maxCount) maxCount = item.count;
	}

	// --- 2. Chart Settings ---
	const int Y_LABEL_WIDTH = 5;
	const int BAR_WIDTH = 6;    // Wider bars for visibility
	const int BAR_GAP = 1;      // Less gap between bars

	// --- 3. Dynamic Y-axis Scaling ---
	int Y_TICK_INTERVAL = (maxCount <= 10) ? 1 : (maxCount <= 20) ? 2 : 5;
	int TOTAL_ROWS = maxCount / Y_TICK_INTERVAL + 1;

	// --- 4. Print Chart Header ---
	cout << "\n--- " << chartTitle << " ---\n";
	cout << setw(Y_LABEL_WIDTH + 1) << "Count\n";

	// --- 5. Draw Bars ---
	for (int row = TOTAL_ROWS; row >= 0; --row) {
		int currentLevel = row * Y_TICK_INTERVAL;

		// Y-axis label
		cout << setw(Y_LABEL_WIDTH) << currentLevel << "|";

		// Draw centered markers
		for (const auto& item : data) {
			if (item.count == currentLevel) {
				// Calculate center position
				int padding = (BAR_WIDTH - 1) / 2;
				cout << string(padding, ' ') << GREEN << "*" << RESET << string(padding, ' ');
			}
			else {
				cout << string(BAR_WIDTH, ' ');
			}
			cout << string(BAR_GAP, ' ');
		}
		cout << "\n";
	}

	// --- 6. X-axis Line ---
	cout << setw(Y_LABEL_WIDTH + 1) << "+"
		<< string(data.size() * (BAR_WIDTH + BAR_GAP), '-') << "> Bus ID\n";

	// --- 7. Bus ID Labels ---
	cout << setw(Y_LABEL_WIDTH + 1) << " ";
	for (const auto& item : data) {
		string label = item.label.substr(0, BAR_WIDTH); // Trim long IDs
		cout << setw(BAR_WIDTH) << label << string(BAR_GAP, ' ');
	}
	cout << "\n";
}

int getDayDifference(const string& journeyDate) {
	struct tm journey_tm = {};
	sscanf_s(journeyDate.c_str(), "%d-%d-%d", &journey_tm.tm_year, &journey_tm.tm_mon, &journey_tm.tm_mday);
	journey_tm.tm_year -= 1900;
	journey_tm.tm_mon -= 1;

	time_t journey_time = mktime(&journey_tm);
	time_t now = time(nullptr);

	double secondsDiff = difftime(journey_time, now);
	int daysDiff = static_cast<int>(secondsDiff / (60 * 60 * 24));

	return daysDiff;
}

void Admin::manageStaff()
{
	int choice;
	Menu menu;
	
	while (true)
	{
		system("cls");
		cout << CYAN << "\n\t\t\t\t\t\t\t\t----------- Manage Staff -----------\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t\t1. Add New Staff\n\n";
		cout << "\t\t\t\t\t\t\t\t2. Update Staff Infomation\n\n";
		cout << "\t\t\t\t\t\t\t\t3. Delete Staff\n\n";
		cout << "\t\t\t\t\t\t\t\t4. Return\n\n" << RESET;
		cout << GREEN << "\n\t\t\t\t\t\t\t\tEnter your choice: " << RESET;

	
		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\t\t\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 3.\n" << RESET;
			system("pause");
			continue;
		}

		if (!(cin >> choice)) // Check if input is invalid (not an integer)
		{
			cin.clear();  // Clear the error flag
			cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
			cout << RED << "\t\t\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 4!" << RESET << endl;
			system("pause");
			continue;  // Restart the loop
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 1)
		{
			addStaff();
			break;
		}

		else if (choice == 2)
		{
			updateStaff();
			break;
		}
		else if (choice == 3)
		{
			deleteStaff();
			break;
		}
		else if (choice == 4)
		{
			menu.AdminMenu(*this);
			break;
		}
		else
		{
			cout << RED << "\n\t\t\t\t\t\t\t\tInvalid input! Please enter number between 1 - 4.\n" << RESET;
			system("pause");
			continue;
		}
	}
}

void Admin::addStaff()
{
	MySQL db;
	Menu menu;
	string name, contactNo, email, address, S_Username, S_Pswd;
	char choice;
	bool isChoice = 0;

	while (!isChoice)
	{
		system("cls");
		cout << YELLOW << "\t\t\t\t\t\t\t--------------------------------------------------\n\n" << RESET; 
		cout << CYAN << "\t\t\t\t\t\t\t\t\tStaff Registration\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t--------------------------------------------------\n\n" << RESET;
		cout << "\n\t\t\t\t\t\t\t\tEnter Staff Infomation\n";
		while (true)
		{
			cout << "\n\t\t\t\t\t\t\t\tName: ";
			getline(cin, name);

			if (name.empty())
			{
				cout << RED << "\n\t\t\t\t\t\t\tName cannot be empty. Please enter a name.\n" << RESET;
				continue;
			}
			else
				break;
		}

		while (true)
		{
			cout << RESET << "\n\t\t\t\t\t\t\t\tPhone Number (e.g 0XX-XXXXXXXX): ";
			getline(cin, contactNo);
			regex phone_regex("^\\d{3}-\\d{7,8}$");

			if (regex_match(contactNo, phone_regex))
				break;
			else
				cout << RED << "\t\t\t\t\t\t\tInvalid phone number format. Please try again!\n";
		}

		while (true)
		{
			cout << RESET << "\n\t\t\t\t\t\t\t\tEmail Address: ";
			getline(cin, email);
			regex email_regex("^[\\w.%+-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
			if (regex_match(email, email_regex))
				break;
			else
				cout << RED << "\t\t\t\t\t\t\tInvalid email format. Please try again!\n";
		}

		cout << "\n\t\t\t\t\t\t\t\tAddress: ";
		getline(cin, address);

		while (true)
		{
			cout << "\n\t\t\t\t\t\t\t\tUsername: ";
			getline(cin, S_Username);

			if (S_Username.empty())
			{
				cout << RED << "\t\t\t\t\t\t\tUsername cannot be empty. Please enter a username.\n" << RESET;
				continue;
			}
			else
				break;
		}

		while (true) {
			cout << "\n\t\t\t\t\t\t\t\tPassword (8-16 characters): ";
			getline(cin, S_Pswd);

			if (S_Pswd.length() >= 8 && S_Pswd.length() <= 16) {
				break;
			}
			else {
				cout << RED << "\n\t\t\t\t\t\t\t\tPlease enter between 8 and 16 characters.\n\n" << RESET;
			}
		}

		db.prepareStatement("INSERT INTO user (username, password, userType) VALUES (?, ?, 'Staff')");
		db.statement->setString(1, S_Username);
		db.statement->setString(2, S_Pswd);
		db.QueryStatement();

		string user_id;
		db.prepareStatement("SELECT * FROM user ORDER BY userID DESC LIMIT 1");
		db.QueryResult();
		db.result->next();
		user_id = db.result->getString("userID");

		db.prepareStatement("INSERT INTO staff (staffName, contactNo, Email, address, userID, adminID) VALUES (?, ?, ?, ?, ?, ?)");
		db.statement->setString(1, name);
		db.statement->setString(2, contactNo);
		db.statement->setString(3, email);
		db.statement->setString(4, address);
		db.statement->setString(5, user_id);
		db.statement->setString(6, a_id);
		db.QueryStatement();

		cout <<GREEN << "\t\t\t\t\t\t\t\tStaff Registration Successful!\n\n" << RESET;
		while (true)
		{
			cout << BLUE << "\t\t\t\t\t\t\tDo you want to add more staff?(Y/N): " << RESET;
		
			cin >> choice;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (choice == 'y' || choice == 'Y')
			{
				break;
			}
			else if (choice == 'n' || choice == 'N')
			{
				isChoice = 1;
				break;
			}
			else
			{
				cout << RED << "\n\t\t\t\t\t\t\tInvalid input. Please enter Y or N.\n" << RESET;
			}
		}
	}
	manageStaff();
}

void Admin::updateStaff()
{
	MySQL db;
	string phoneNo, email, address, staffID, s_id, name;
	int ch;
	char option;
	bool isChoice = false;

	
	// Always show staff list at the top of the loop
	db.prepareStatement("SELECT * FROM staff");
	db.QueryResult();
	
	while (true)
	{
		system("cls");
		cout << CYAN << "\t\t\t\t\t--------Update Staff Information--------\n" << RESET;
		cout << string(130, '-') << endl;
		cout << left
			<< setw(10) << "Staff ID"
			<< setw(15) << "Name"
			<< setw(15) << "Phone No"
			<< setw(30) << "Email"
			<< setw(50) << "Address" << endl;
		cout << string(130, '-') << endl;

		db.prepareStatement("SELECT * FROM staff");
		db.QueryResult();

		while (db.result->next())
		{
			s_id = db.result->getString("staffID");
			name = db.result->getString("staffName");
			phoneNo = db.result->getString("contactNo");
			email = db.result->getString("Email");
			address = db.result->getString("address");

			cout << left
				<< setw(10) << s_id
				<< setw(15) << name
				<< setw(15) << phoneNo
				<< setw(30) << email
				<< setw(50) << address << endl;
			cout << string(130, '-') << endl;
		}

		// Ask for Staff ID
		cout << "Enter Staff ID that you want to update (or Q to quit): ";
		getline(cin, staffID);
		if (staffID == "q" || staffID == "Q")
		{
			manageStaff();
			return;
		}

		db.prepareStatement("SELECT * FROM staff WHERE staffID = ?");
		db.statement->setString(1, staffID);
		db.QueryResult();

		if (!db.result->next())
		{
			cout << RED << "\n\t\t\t\t\tStaff ID not found. Press any key to try again.\n" << RESET;
			system("pause");
			continue; // Show the table again
		}

		bool done = false;
		while (!done)
		{
			system("cls");
			cout << string(130, '-') << endl;
			cout << left
				<< setw(10) << "Staff ID"
				<< setw(15) << "Name"
				<< setw(15) << "Phone No"
				<< setw(30) << "Email"
				<< setw(50) << "Address" << endl;
			cout << string(130, '-') << endl;

			db.prepareStatement("SELECT * FROM staff");
			db.QueryResult();

			while (db.result->next())
			{
				s_id = db.result->getString("staffID");
				name = db.result->getString("staffName");
				phoneNo = db.result->getString("contactNo");
				email = db.result->getString("Email");
				address = db.result->getString("address");

				cout << left
					<< setw(10) << s_id
					<< setw(15) << name
					<< setw(15) << phoneNo
					<< setw(30) << email
					<< setw(50) << address << endl;
				cout << string(130, '-') << endl;
			}

			cout << CYAN << "\t\t\t\t\t--------Update Options for Staff " << staffID << " --------\n" << RESET;
			cout << YELLOW << "\t\t\t\t\t\t1. Update Contact No\n";
			cout << "\t\t\t\t\t\t2. Update Email Address\n\n";
			cout << "\t\t\t\t\t\t3. Return to Staff Menu\n\n" << RESET;
			cout << GREEN << "Enter your choice: " << RESET;

			if (cin.peek() == '\n') {
				cin.ignore();
				cout << RED << "\nInput cannot be empty! Please enter a number between 1 - 3.\n" << RESET;
				system("pause");
				continue;
			}

			if (!(cin >> ch)) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << RED << "\nInvalid input. Please enter a number.\n" << RESET;
				system("pause");
				continue;
			}
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			switch (ch)
			{
			case 1:
				cout << "\n\t\t\t\t\t\tEnter New Contact No: ";
				getline(cin, phoneNo);
				db.prepareStatement("UPDATE staff SET contactNo = ? WHERE staffID = ?");
				db.statement->setString(1, phoneNo);
				db.statement->setString(2, staffID);
				db.QueryStatement();
				cout << GREEN << "\n\t\t\t\t\t\tContact No Updated Successfully!\n" << RESET;
				system("pause");
				break;

			case 2:
				cout << "\n\t\t\t\t\t\tEnter New Email Address: ";
				getline(cin, email);
				db.prepareStatement("UPDATE staff SET Email = ? WHERE staffID = ?");
				db.statement->setString(1, email);
				db.statement->setString(2, staffID);
				db.QueryStatement();
				cout << GREEN << "\n\t\t\t\t\t\tEmail Updated Successfully!\n" << RESET;
				system("pause");
				break;

			case 3:
				manageStaff();
				return;

			default:
				cout << RED << "\n\t\t\t\t\t\tInvalid input! Please try again.\n" << RESET;
				system("pause");
				continue;
			}

			// Ask to continue update
			while (true)
			{
				cout << BLUE << "\n\t\t\t\t\t\tDo you want to update other info for this staff? (Y/N): " << RESET;
				cin >> option;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (option == 'n' || option == 'N')
				{
					done = true;
					break;
				}
				else if (option == 'Y' || option == 'y')
					break;
				else
				{
					cout << RED << "\t\t\t\t\t\tInvalid input! Please try again.\n" << RESET;
					system("pause");
					continue;
				}
			}
		}

		// After finishing one staff update, ask if continue for others
		while (true)
		{
			cout << BLUE << "\n\t\t\t\t\t\tDo you want to update another staff? (Y/N): " << RESET;
			cin >> option;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (option == 'n' || option == 'N')
			{
				break;
			}
			else if (option == 'Y' || option == 'y')
				break;
			else
			{
				cout << RED << "\t\t\t\t\t\tInvalid input! Please try again.\n" << RESET;
				system("pause");
				continue;
			}
		}
	}

	manageStaff();
}

void Admin::deleteStaff()
{
	MySQL db;
	string staffid, s_id, name, phoneNo, email, address;
	char opt, choice;
	bool istrue = 0, check = 0;

	db.prepareStatement("SELECT * FROM staff");
	db.QueryResult();

	system("cls");
	cout << CYAN << "\t\t\t\t--------Delete Staff--------\n" << RESET;
	cout << string(130, '-') << endl;
	cout << left
		<< setw(10) << "Staff ID"
		<< setw(15) << "Name"
		<< setw(15) << "Phone No"
		<< setw(30) << "Email"
		<< setw(50) << "Address" << endl;

	cout << string(130, '-') << endl; // Separator line

	while (db.result->next())
	{
		s_id = db.result->getString("staffID");
		name = db.result->getString("staffName");
		phoneNo = db.result->getString("contactNo");
		email = db.result->getString("Email");
		address = db.result->getString("address");

		cout << left
			<< setw(10) << s_id
			<< setw(15) << name
			<< setw(15) << phoneNo
			<< setw(30) << email
			<< setw(50) << address << endl;
		cout << string(130, '-') << endl;
	}
	while (!check)
	{
		while (!istrue)
		{
			cout << "\nEnter Staff ID that you want to delete (or 'Q' to cancel): ";
			getline(cin, staffid);

			db.prepareStatement("SELECT * FROM staff WHERE staffID = ?");
			db.statement->setString(1, staffid);
			db.QueryResult();

			// Allow user to quit from this page
			if (staffid == "q" || staffid == "Q") {
				cout << YELLOW << "Returning to Staff Management Menu...\n" << RESET;
				system("pause");
				manageStaff(); // Exit to previous menu
				return;
			}

			if (db.result->next())
			{ 
				while (true)
				{
					cout << BLUE << "Do you sure want to delete this staff?(Y/N): " << RESET;
					cin >> choice;
					cin.ignore();

					if (choice == 'y' || choice == 'Y')
					{
						istrue = 1;
						db.prepareStatement("UPDATE schedule SET staffID = NULL WHERE staffID = ?");
						db.statement->setString(1, staffid);
						db.QueryStatement();

						db.prepareStatement("DELETE FROM staff WHERE staffID = ?");
						db.statement->setString(1, staffid);
						db.QueryStatement();

						cout << GREEN << "Delete Successful!\n" << RESET;
						break;
					}
					else if (choice == 'n' || choice == 'N')
					{
						cout << YELLOW << "Returning to Staff Management Menu...\n" << RESET;
						system("pause");
						manageStaff();
						break;
					}
					else
					{
						cout << RED << "\nInvalid input. Please enter Y or N.\n" << RESET;
						system("pause");

					}
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
				}
			}
			else
			{
				cout << RED << "Staff ID not found!\n" << RESET;
				system("pause");
				continue;
			}
			
			while (true)
			{
				cout << BLUE << "Do You want to delete more? (Y/N): " << RESET;
				cin >> opt;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (opt == 'y' || opt == 'Y')
				{
					break;
				}
				else if (opt == 'n' || opt == 'N')
				{
					check = 1;
					break;
				}
				else
				{
					cout << RED << "\nInvalid input. Please enter Y or N.\n" << RESET;
					continue;
				}
			}

		}
	}
	manageStaff();
}

void Admin::manageBus()
{
	Menu menu;
	int options;
	
	system("cls");
	cout << CYAN << "\n\t\t\t\t\t\t\t\t----------Manage Bus----------\n\n" << RESET;
	cout << YELLOW << "\t\t\t\t\t\t\t\t1. Add New Bus\n\n";
	cout << "\t\t\t\t\t\t\t\t2. Delete Bus\n\n";
	cout << "\t\t\t\t\t\t\t\t3. Return\n\n" << RESET;
	cout << GREEN << "\t\t\t\t\t\t\t\tEnter your choice: " << RESET;

	while (true)
	{
		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 3.\n" << RESET;
			system("pause");
			continue;
		}

		if (!(cin >> options)) // Check if input is invalid (not an integer)
		{
			cin.clear();  // Clear the error flag
			cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
			cout << RED << "\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 3!" << RESET << endl;
			system("pause");
			continue;  // Restart the loop
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (options == 1)
		{
			addBus();
			break;
		}

		else if (options == 2)
		{
			deleteBus();
			break;
		}
		else if (options == 3)
		{
			menu.AdminMenu(*this);
			break;
		}
		else
		{
			cout << RED << "\n\t\t\t\t\t\tInvalid choice! Please enter number between 1 - 3.\n" << RESET;
			system("pause");
			continue;
		}
	}

}

void Admin::addBus()
{
	MySQL db;
	Menu menu;
	string busNumber, input;
	int capacity;
	char choice;
	bool check = 0;

	while(!check)
	{
		system("cls");
		cout << CYAN << "\t\t\t\t\t\t\t\t--------ADD NEW BUS--------\n\n" << RESET;
		while (true)
		{
			cout << "\t\t\t\t\t\t\t\tEnter New Bus Number: ";
			getline(cin, busNumber);

			if (busNumber.empty())
			{
				cout << RED << "\t\t\t\t\t\tInput cannot be empty. Please enter a valid No Plate.\n" << RESET;
				continue;
			}

			if (busNumber.length() >= 0 && busNumber.length() <= 8) {
				break;
			}
			else {
				cout << RED << "\n\t\t\t\t\t\t\tPlease enter 3 alphabet and 4 digit.\n\n" << RESET;
			}
		}

		while (true) {
			cout << "\n\t\t\t\t\t\t\t\tEnter the capacity of bus: ";
			getline(cin, input);  // Use getline to capture the line of input

			if (input.empty()) 
			{
				cout << RED << "\t\t\t\t\t\tInput cannot be empty. Please enter a valid number for capacity.\n" << RESET;
				continue;  // Restart the loop
			}

			try 
			{
				capacity = stoi(input);  // Convert string to integer
				if (capacity <= 0 || capacity != 28) 
				{
					cout << RED << "\t\t\t\t\t\tPlease enter a positive number for capacity and must be 28 seat.\n" << RESET;
					continue;
				}
				break;
			}
			catch (const invalid_argument&) {
				cout << RED << "\t\t\t\t\t\tInvalid input. Please enter a valid number for capacity.\n" << RESET;
			}
		}

		db.prepareStatement("INSERT INTO bus (busnumber, capacity, adminID) VALUES (?, ?, ?)");
		db.statement->setString(1, busNumber);
		db.statement->setInt(2, capacity);
		db.statement->setString(3, a_id);
		db.QueryStatement();

		cout << GREEN << "\n\t\t\t\t\t\t\t\tNew Bus Add Successful!\n\n" << RESET;

		while (true)
		{
			cout << BLUE << "\t\t\t\t\t\t\tDo you want to add more bus? (Y/N): " << RESET;
		
			cin >> choice;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (choice == 'y' || choice == 'Y')
			{
				break;
			}
			else if (choice == 'n' || choice == 'N')
			{
				check = 1;
				break;
			}
			else
			{
				cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter Y or N.\n" << RESET;
			}
		}
	}
	manageBus();
}

void Admin::deleteBus()
{
	MySQL db;
	string busid, b_id, noPlate;
	char opt, choice;
	bool isTrue = 0, check = 0;

	while (!check)
	{
		while (!isTrue)
		{
			db.prepareStatement("SELECT * FROM bus");
			db.QueryResult();

			system("cls");
			cout << CYAN << "\t\t------Delete Bus------\n" << RESET;
			cout << "------------------------------" << endl;
			cout << left << setw(10) << "Bus ID"
				<< setw(15) << "No Plate" << endl;
			cout << "------------------------------" << endl;

			while (db.result->next())
			{
				b_id = db.result->getString("busID");
				noPlate = db.result->getString("busNumber");

				cout << left << setw(10) << b_id << setw(15) << noPlate << endl;
				cout << "------------------------------" << endl;
			}

			cout << "\nEnter bus ID that you want to delete (or 'Q' to cancel): ";
			getline(cin, busid);

			db.prepareStatement("SELECT * FROM bus WHERE busID = ?");
			db.statement->setString(1, busid);
			db.QueryResult();

			// Allow user to quit from this page
			if (busid == "q" || busid == "Q") {
				cout << YELLOW << "Returning to bus Management Menu...\n" << RESET;
				system("pause");
				manageBus(); // Exit to previous menu
				return;
			}

			if (db.result->next())
			{
				while (true)
				{
					cout << BLUE << "Did you sure want to delete this bus?(Y/N): " << RESET;
					cin >> choice;
					cin.ignore(numeric_limits<streamsize>::max(), '\n');

					if (choice == 'y' || choice == 'Y')
					{
						isTrue = 1;
						db.prepareStatement("UPDATE schedule SET busID = NULL WHERE busID = ?");
						db.statement->setString(1, busid);
						db.QueryStatement();

						db.prepareStatement("DELETE FROM bus WHERE busID = ?");
						db.statement->setString(1, busid);
						db.QueryStatement();

						cout << GREEN << "Delete Successful!\n" << RESET;
						break;
					}
					else if (choice == 'n' || choice == 'N')
					{
						cout << YELLOW << "Returning to bus Management Menu...\n" << RESET;
						system("pause");
						manageBus();
						break;
					}
					else
					{
						cout << RED << "\nInvalid input. Please enter Y or N.\n" << RESET;
						continue;

					}
				}
			}
			else
			{
				cout << RED << "Bus ID not found!\n" << RESET;
				system("pause");
				continue;
			}

			while (true)
			{
				cout << BLUE << "Do You want to delete more? (Y/N): " << RESET;
				cin >> opt;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (opt == 'y' || opt == 'Y')
				{
					isTrue = 0;
					break;
				}
				else if (opt == 'n' || opt == 'N')
				{
					check = 1;
					break;
				}
				else
				{
					cout << RED << "\nInvalid input. Please enter Y or N.\n" << RESET;
					continue;
				}
			}
		}
	}
	manageBus();
}

void Admin::refundApp()
{
	MySQL db;
	Menu menu;
	string c_id, p_id, p_status, departTime, journeyDate, b_place, s_id, app;
	double total, balance = 0, refundAmount = 0;
	int count = 0, SeatNo;
	bool hasRequest = 1, check = 0;

	while (true)
	{
		system("cls");
		cout << CYAN << "\t\t\t\t--------Refund Request--------\n\n" << RESET;

		count = 0;
		check = false;

		db.prepareStatement("SELECT * FROM ticket t JOIN payment p ON t.paymentID = p.paymentID "
			"JOIN passenger c ON p.passengerID = c.passengerID JOIN schedule r ON t.scheduleID = r.scheduleID "
			"WHERE p.paymentStatus = 'Pending' AND r.DateOfJourney >= CURDATE()");
		db.QueryResult();

		cout << string(107, '-') << endl;
		cout << left
			<< setw(12) << "PaymentID" << "| "
			<< setw(8) << "SeatNo" << "| "
			<< setw(17) << "Payment Status" << "| "
			<< setw(14) << "PassengerID" << "| "
			<< setw(13) << "ScheduleID" << "| "
			<< setw(15) << "Journey Date" << "| "
			<< setw(15) << "Total Amount" << "|" << endl;
		cout << string(107, '-') << endl;

		while (db.result->next())
		{
			p_id = db.result->getString("paymentID");
			SeatNo = db.result->getInt("seatNo");
			p_status = db.result->getString("paymentStatus");
			c_id = db.result->getString("passengerID");
			s_id = db.result->getString("scheduleID");
			total = db.result->getDouble("totalAmount");
			journeyDate = db.result->getString("DateOfJourney");

			cout << left
				<< setw(12) << p_id << "| "
				<< setw(8) << SeatNo << "| "
				<< setw(17) << p_status << "| "
				<< setw(14) << c_id << "| "
				<< setw(13) << s_id << "| "
				<< setw(15) << journeyDate << "| " 
				<< right << setw(15) << fixed << setprecision(2) << total << "|" << endl;

			cout << string(107, '-') << endl;
			count++;
		}

		if (count == 0)
		{
			cout << "\n\t\t\tNo refund request available.\n";
			system("pause");
			break;
		}

		cout << "Enter payment ID to approve the request (q to quit): ";
		cin >> app;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (app == "q" || app == "Q")
			break;

		db.prepareStatement("SELECT * FROM ticket t JOIN payment p ON t.paymentID = p.paymentID "
			"JOIN schedule s ON t.scheduleID = s.scheduleID "
			"WHERE p.paymentID = ?");
		db.statement->setString(1, app);
		db.QueryResult();

		if (db.result->next())
		{
			total = db.result->getDouble("totalAmount");
			journeyDate = db.result->getString("DateOfJourney");

			int dayDiff = getDayDifference(journeyDate);
			if (dayDiff >= 2)
			{
				refundAmount = total;
				cout << GREEN << "\nRefund 100% = RM " << fixed << setprecision(2) << refundAmount << RESET << endl;
			}
			else
			{
				refundAmount = total * 0.5;
				cout << GREEN << "\nRefund 50% = RM " << fixed << setprecision(2) << refundAmount << RESET << endl;
			}

			// Update payment status and refund datetime
			db.prepareStatement("UPDATE payment SET paymentStatus = 'Refund', refundDate = NOW() WHERE paymentID = ?");
			db.statement->setString(1, app);
			db.QueryStatement();

			// Refund to wallet
			db.prepareStatement("UPDATE payment p JOIN wallet w ON p.passengerID = w.passengerID SET w.balance = w.balance + ? WHERE p.paymentID = ?");
			db.statement->setDouble(1, refundAmount);
			db.statement->setString(2, app);
			db.QueryStatement();

			cout << GREEN << "\n\t\t\tRefund Request Approved\n" << RESET;
			system("pause");
		}
		else
		{
			cout << RED << "\t\tInvalid Payment ID. Please try again.\n" << RESET;
			system("pause");
		}
	}
	menu.AdminMenu(*this);
}

void Admin::generateReport()
{
	Menu menu;
	int choice;
	while (true)
	{
		system("cls");
		cout << CYAN << "\n\t\t\t\t\t\t\t------------Report Generate------------\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t1. Revenue Report\n\n"
			<< "\t\t\t\t\t\t\t2. Bus Usage Report\n\n"
			<< "\t\t\t\t\t\t\t3. Most Schedule Chosen Report\n\n"
			<< "\t\t\t\t\t\t\t4. Return To Admin Menu\n\n" << RESET
			 << GREEN << "\t\t\t\t\t\t\tEnter your choice: " << RESET;

		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 - 4.\n" << RESET;
			system("pause");
			continue;
		}

		if (!(cin >> choice))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\t\t\t\t\t\tInvalid input. Please enter a number 1 - 4!\n" << RESET;
			system("pause");
			continue;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 1)
		{
			revenueReport();
			break;
		}
		else if (choice == 2)
		{
			busUsageReport();
			break;
		}
		else if (choice == 3)
		{
			mostScheduleReport();
			break;
		}
		else if (choice == 4)
		{
			menu.AdminMenu(*this);
			break;
		}
		else
		{
			cout << RED << "\t\t\t\t\t\tInvalid choice! Please Try Again.\n" << RESET;
			system("pause");
			continue;
		}
	}
}

void Admin::revenueReport()
{
	MySQL db;
	Passenger passenger;
	string s_id, t_id, p_id, date, b_place, a_place;
	double refundAmount, totalAmount, revenue = 0;
	int ch, totalTicket, count =0;

	db.prepareStatement("SELECT * FROM schedule s JOIN ticket t ON s.scheduleID = t.scheduleID JOIN payment p ON t.paymentID = p.paymentID");
	db.QueryResult();
	while (true)
	{
		system("cls");
		cout << CYAN << "\t\t\t\t\t\t\t\t--------Choose the report type--------\n\n" << RESET
			<< YELLOW << "\t\t\t\t\t\t\t\t\t1. Daily\n\n"
			<< "\t\t\t\t\t\t\t\t\t2. Monthly\n\n"
			<< "\t\t\t\t\t\t\t\t\t3. Return\n\n" << RESET
			<< GREEN << "\t\t\t\t\t\t\t\t\tEnter your choice: " << RESET;
		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 - 3.\n" << RESET;
			system("pause");
			continue;
		}

		if (!(cin >> ch))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\t\t\t\t\t\t\tInvalid input. Please enter a number 1 - 3!\n" << RESET;
			system("pause");
			continue;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (ch == 1)
		{
			do
			{
				system("cls");
				cout << CYAN << "\n\t\t\t\t\t\t\t\t----------Daily Report----------\n\n" << RESET;
				cout << "\t\t\t\t\t\t\t\tEnter date (YYYY-MM-DD): ";
				getline(cin, date);

				if (!passenger.isValidDateFormat(date))
				{
					cout << RED << "\n\t\t\t\t\tInvalid date format! Please enter the date in the format YYYY-MM-DD.\n\n" << RESET;
					system("pause");
				}
				else if (!isValidReportDate(date))
				{
					cout << RED << "\n\t\t\t\t\tInvalid date! Only past 6 months and not before Sep 2024.\n\n" << RESET;
					system("pause");
				}
			} while (!passenger.isValidDateFormat(date) || !isValidReportDate(date));

			db.prepareStatement(
				"SELECT s.scheduleID, t.ticketID, p.paymentID, p.totalAmount, p.refundAmount "
				"FROM schedule s "
				"JOIN ticket t ON s.scheduleID = t.scheduleID "
				"JOIN payment p ON t.paymentID = p.paymentID "
				"WHERE s.DateOfjourney = ?"
			);
			db.statement->setString(1, date);
			db.QueryResult();

			system("cls");
			cout << "\n\t\tRevenue Report for " << date << endl;
			cout << string(80, '-') << endl;
			cout << left
				<< setw(16) << "Schedule ID"
				<< setw(16) << "Ticket ID"
				<< setw(16) << "Payment ID"
				<< right << setw(16) << "Amount (RM)"
				<< right << setw(16) << "Refund (RM)" << endl;
			cout << string(80, '-') << endl;

			while (db.result->next())
			{
				count++;
				s_id = db.result->getString("scheduleID");
				t_id = db.result->getString("ticketID");
				p_id = db.result->getString("paymentID");
				totalAmount = db.result->getDouble("totalAmount");
				refundAmount = db.result->getDouble("refundAmount");

				cout << left
					<< setw(16) << s_id
					<< setw(16) << t_id
					<< setw(16) << p_id
					<< right << setw(16) << fixed << setprecision(2) << totalAmount
					<< right << setw(16) << fixed << setprecision(2) << refundAmount << endl;

				revenue += (totalAmount - refundAmount);
			}

			cout << string(80, '-') << endl;

			if (count > 0)
			{
				cout << right << setw(48) << "Total Revenue: RM "
					<< right << setw(16) << fixed << setprecision(2) << revenue << endl;
				system("pause");
			}
			else
			{
				cout << "No report available\n";
				system("pause");
			}
		}
		else if (ch == 2)
		{
			double m_revenue = 0;
			int month, year, count1 = 0;

			// Get current system time
			time_t now = time(0);
			tm* ltm = localtime(&now);
			int currentMonth = ltm->tm_mon + 1;  // tm_mon is 0-11
			int currentYear = 1900 + ltm->tm_year;

			
			while (true)
			{
				system("cls");
				cout << CYAN << "\t\t\t\t\t\t\t----------Monthly Report----------\n\n" << RESET;
				cout << "\t\t\t\t\t\t\tEnter report month (1-12): ";
				if (!(cin >> month) || month < 1 || month > 12)
				{
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << RED << "\t\t\t\t\t\tInvalid month! Please enter a number between 1 and 12.\n" << RESET;
					system("pause");
					continue;
				}

				cout << "\n\t\t\t\t\t\t\tEnter report year: ";
				if (!(cin >> year) || year < 2024 || year > currentYear)
				{
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << RED << "\t\t\t\t\t\tInvalid year! Must be between 2024 and current year.\n" << RESET;
					system("pause");
					continue;
				}

				// Check if input date is in the future
				if ((year > currentYear) || (year == currentYear && month > currentMonth))
				{
					cout << RED << "\t\t\t\t\t\tCannot select a future month! Please enter a previous month or year.\n" << RESET;
					system("pause");
					continue;
				}

				break; // valid input
			}

			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			db.prepareStatement(
				"SELECT s.boardingPlace, s.arrivalPlace, "
				"COUNT(t.ticketID) AS TotalTicket, "
				"SUM(p.totalAmount) AS totalAmount, "
				"SUM(p.refundAmount) AS refundAmount, "
				"SUM(p.totalAmount - p.refundAmount) AS revenue "
				"FROM schedule s "
				"JOIN ticket t ON s.scheduleID = t.scheduleID "
				"JOIN payment p ON t.paymentID = p.paymentID "
				"WHERE MONTH(s.DateOfJourney) = ? AND YEAR(s.DateOfJourney) = ? "
				"GROUP BY s.boardingPlace, s.arrivalPlace "
				"ORDER BY totalAmount DESC"
			);
			db.statement->setInt(1, month);
			db.statement->setInt(2, year);
			db.QueryResult();

			system("cls");
			cout << "\nRevenue Report for " << setw(2) << month << "/" << year << endl;
			cout << string(80, '-') << endl;
			cout << left
				<< setw(14) << "Total Ticket"
				<< setw(20) << "Boarding Place"
				<< setw(20) << "Arrival Place"
				<< right << setw(13) << "Amount (RM)"
				<< right << setw(13) << "Refund (RM)" << endl;
			cout << string(80, '-') << endl;

			while (db.result->next())
			{
				totalTicket = db.result->getInt("TotalTicket");
				b_place = db.result->getString("boardingPlace");
				a_place = db.result->getString("arrivalPlace");
				totalAmount = db.result->getDouble("totalAmount");
				refundAmount = db.result->getDouble("refundAmount");
				revenue = db.result->getDouble("revenue");

				cout << left
					<< setw(14) << totalTicket
					<< setw(20) << b_place
					<< setw(20) << a_place
					<< right << setw(13) << fixed << setprecision(2) << totalAmount
					<< right << setw(13) << fixed << setprecision(2) << refundAmount << endl;

				m_revenue += revenue;
				count1++;
			}

			cout << string(80, '-') << endl;

			if (count1 > 0)
			{

				cout << right << setw(54) << "Total Monthly Revenue: RM "
					<< right << setw(13) << fixed << setprecision(2) << m_revenue << endl;
				system("pause");
			}
			else 
			{
				cout << "\nNo report available\n";
				system("pause");
				continue;
			}
			break;
		}
		else if (ch == 3)
		{
			generateReport();
			break;
		}
		else
		{
			cout << RED << "\t\t\t\t\t\t\tInvalid choice! Please Try Again.\n" << RESET;
			system("pause");
			continue;
		}
	}
	generateReport();
}

void Admin::busUsageReport()
{
	MySQL db;
	string b_id, noPlate;
	int usage = 0;
	vector<TrendData> usageData;
	int month, year;
	string mostUsedBusNumber;
	int maxUsage = -1;

	// Get current system time
	time_t now = time(0);
	tm* ltm = localtime(&now);
	int currentMonth = ltm->tm_mon + 1;  // tm_mon is 0-11
	int currentYear = 1900 + ltm->tm_year;

	// Prompt user for month and year
	while (true) {
		system("cls");
		cout << CYAN << "\t\t\t\t\t\t\t----------Bus Usage Report----------\n\n" << RESET;
		cout << "\t\t\t\t\t\t\tEnter report month (1-12): ";
		if (!(cin >> month) || month < 1 || month > 12) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\t\t\t\t\t\tInvalid month! Please enter a number between 1 and 12.\n" << RESET;
			system("pause");
			continue;
		}

		cout << "\n\t\t\t\t\t\t\tEnter report year: ";
		if (!(cin >> year) || year < 2024 || year > currentYear) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\t\t\t\t\t\tInvalid year! Must be between 2024 and current year.\n" << RESET;
			system("pause");
			continue;
		}

		// Check if input date is in the future
		if ((year > currentYear) || (year == currentYear && month > currentMonth)) {
			cout << RED << "\t\t\t\t\t\tCannot select a future month! Please enter a previous month or year.\n" << RESET;
			system("pause");
			continue;
		}

		break; // valid input
	}
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	db.prepareStatement(
		"SELECT s.busID, b.busNumber, COUNT(*) AS usage_count "
		"FROM schedule s "
		"JOIN bus b ON s.busID = b.busID "
		"WHERE MONTH(s.DateOfJourney) = ? AND YEAR(s.DateOfJourney) = ? "
		"GROUP BY s.busID, b.busNumber "
		"HAVING usage_count > 3 "
		"ORDER BY usage_count ASC");
	db.statement->setInt(1, month);
	db.statement->setInt(2, year);
	db.QueryResult();

	system("cls");
	cout << "\nBus Usage Report for " << setw(2) << month << "/" << year << endl;
	cout << string(36, '-') << endl;
	cout << left
		<< setw(12) << "Bus ID"
		<< setw(14) << "No Plate"
		<< setw(10) << "Bus Usage" << endl;
	cout << string(36, '-') << endl;

	// Print data and collect chart data
	while (db.result->next())
	{
		b_id = db.result->getString("busID");
		noPlate = db.result->getString("busNumber");
		usage = db.result->getInt("usage_count");

		cout << left
			<< setw(12) << b_id
			<< setw(14) << noPlate
			<< setw(10) << usage << endl;

		// Track the most used bus
		if (usage > maxUsage) {
			maxUsage = usage;
			mostUsedBusNumber = noPlate;
		}

		// Push to chart data INSIDE the loop
		usageData.push_back({ b_id, usage });
	}
	cout << string(36, '-') << endl;

	// Only draw chart if we have data
	if (!usageData.empty()) {
		drawBarChart("Bus Usage Report", usageData);

		if (!mostUsedBusNumber.empty()) {
			cout << RED << "\nBus " << mostUsedBusNumber << " needs maintenance!" << RESET << endl;
		}
		system("pause");
	}
	else {
		cout << "No bus usage data available for this month." << endl;
		system("pause");
	}
	generateReport();
}

void Admin::mostScheduleReport()
{
	MySQL db;
	string b_place, a_place, most_popular_route;
	int ticket_count = 0, max_tickets = 0;
	int month, year;

	// Get current system time
	time_t now = time(0);
	tm* ltm = localtime(&now);
	int currentMonth = ltm->tm_mon + 1;  // tm_mon is 0-11
	int currentYear = 1900 + ltm->tm_year;

	// Prompt user for month and year
	while (true) {
		system("cls");
		cout << CYAN << "\t\t\t\t\t\t\t----------Most Popular Routes Report----------\n\n" << RESET;
		cout << "\t\t\t\t\t\t\tEnter report month (1-12): ";
		if (!(cin >> month) || month < 1 || month > 12) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\t\t\t\t\t\tInvalid month! Please enter a number between 1 and 12.\n" << RESET;
			system("pause");
			continue;
		}

		cout << "\n\t\t\t\t\t\t\tEnter report year: ";
		if (!(cin >> year) || year < 2024 || year > currentYear) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\t\t\t\t\t\tInvalid year! Must be between 2024 and current year.\n" << RESET;
			system("pause");
			continue;
		}

		// Check if input date is in the future
		if ((year > currentYear) || (year == currentYear && month > currentMonth)) {
			cout << RED << "\t\t\t\t\t\tCannot select a future month! Please enter a previous month or year.\n" << RESET;
			system("pause");
			continue;
		}

		break; // valid input
	}
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	db.prepareStatement(
		"SELECT s.boardingPlace, s.arrivalPlace, COUNT(t.ticketID) AS ticket_count "
		"FROM ticket t "
		"JOIN schedule s ON t.scheduleID = s.scheduleID "
		"WHERE MONTH(s.DateOfJourney) = ? AND YEAR(s.DateOfJourney) = ? "
		"GROUP BY s.boardingPlace, s.arrivalPlace "
		"ORDER BY ticket_count DESC"
	);
	db.statement->setInt(1, month);
	db.statement->setInt(2, year);
	db.QueryResult();

	system("cls");
	// Display header
	cout << "\nMOST POPULAR ROUTES REPORT for " << setw(2) << month << "/" << year << endl;
	cout << string(60, '=') << endl;
	cout << left
		<< setw(20) << "BOARDING"
		<< setw(20) << "ARRIVAL"
		<< setw(20) << "TICKETS SOLD" << endl;
	cout << string(60, '-') << endl;

	while (db.result->next())
	{
		b_place = db.result->getString("boardingPlace");
		a_place = db.result->getString("arrivalPlace");
		ticket_count = db.result->getInt("ticket_count");

		if (ticket_count > max_tickets)
		{
			max_tickets = ticket_count;
			most_popular_route = b_place + " to " + a_place;
		}

		cout << left
			<< setw(20) << b_place
			<< setw(20) << a_place
			<< setw(20) << ticket_count << endl;
	}

	cout << string(60, '=') << endl;
	if (max_tickets > 0) {
		cout << "\n\tMOST POPULAR ROUTE\n";
		cout << "Route: " << most_popular_route << endl;
		cout << "Tickets Sold: " << max_tickets << endl;
	}
	else {
		cout << "\nNo data available for this month.\n";
	}
	cout << string(60, '=') << endl;
	system("pause");

	generateReport();
}