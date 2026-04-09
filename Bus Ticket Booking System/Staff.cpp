#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <regex>
#include <conio.h>
#include <chrono>
#include "Staff.h"
#include "Menu.h"
#include "MySQL.h"
using namespace std;

const string RED = "\033[1;31m";
const string GREEN = "\033[1;32m";
const string BLUE = "\033[1;34m";
const string YELLOW = "\033[1;33m";
const string CYAN = "\033[1;36m";
const string RESET = "\033[0m";

void Staff::manageSchedule()
{
	Menu menu;
	int opt;
	while (true)
	{
		system("cls");
		cout << CYAN << "\n\t\t\t\t\t\t\t----------------Manage Schedule----------------\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t\t1. Add New Bus Schedule\n\n";
		cout << "\t\t\t\t\t\t\t\t2. Update Bus Schedule\n\n";
		cout << "\t\t\t\t\t\t\t\t3. Delete Bus Schedule\n\n";
		cout << "\t\t\t\t\t\t\t\t4. Return To Menu\n\n";
		cout << GREEN << "\t\t\t\t\t\t\t\tEnter your option: " << RESET;

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
			cout << RED << "\n\t\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 4!" << RESET << endl;
			system("pause");
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (opt == 1)
		{
			addSchedule();
			break;
		}

		else if (opt == 2)
		{
			updateSchedule();
			break;
		}
		else if (opt == 3)
		{
			deleteSchedule();
			break;
		}
		else if (opt == 4)
		{
			menu.StaffMenu(*this);
			break;
		}
		else
		{
			cout << RED << "\n\t\t\t\t\t\t\tInvalid option! Please enter number between 1 - 4.\n" << RESET;
			system("pause");
			continue;
		}
	}
}

void Staff::addSchedule()
{
	MySQL db;
	Menu menu;
	Passenger passenger;
	string b_place, a_place, depart_time, arrival_time, date, busid, b_id, noPlate;
	int b_choice, a_choice;
	double price;
	char choice;
	bool isChoice = 0, check = 0;

	while(!isChoice)
	{
		system("cls");
		db.prepareStatement("SELECT * FROM bus");
		db.QueryResult();

		cout << "------------------------------" << endl;
		cout << left << setw(10) << "Bus ID" 
			<< setw(15) << "No Plate" << endl;
		cout << "------------------------------" << endl;

		while (db.result->next())
		{
			b_id = db.result->getString("busID");
			noPlate = db.result->getString("busNumber");

			cout << left << setw(10) << b_id 
				<< setw(15) << noPlate << endl;
			cout << "------------------------------" << endl;
		}

		while (true)
		{
			cout << "Enter Bus ID: ";
			getline(cin, busid);

			db.prepareStatement("SELECT * FROM bus WHERE busID = ?");
			db.statement->setString(1, busid);
			db.QueryResult();

			if (db.result->next())
			{
				break;
			}
			else
			{
				cout << RED << "Invalid busID. Please try again.\n" << RESET;
				continue;
			}
		}

		string bPlace, aPlace, date, dTime, aTime;
		int counts = 0;

		db.prepareStatement("SELECT * FROM schedule WHERE busID = ? AND DateOfJourney >= CURDATE()");
		db.statement->setString(1, busid);
		db.QueryResult();

		cout << string(76, '-') << endl;
		cout << left
			<< setw(18) << "Date of Journey"
			<< setw(18) << "Departure Time"
			<< setw(18) << "Arrival Time"
			<< setw(20) << "Boarding Place"
			<< setw(20) << "Arrival Place" << endl;

		cout << string(76, '-') << endl; // Optional separator line

		while (db.result->next())
		{
			counts++;
			bPlace = db.result->getString("boardingPlace");
			aPlace = db.result->getString("arrivalPlace");
			date = db.result->getString("DateOfJourney");
			dTime = db.result->getString("departureTime");
			aTime = db.result->getString("arrivalTime");

			cout << left
				<< setw(18) << date
				<< setw(18) << dTime
				<< setw(18) << aTime
				<< setw(20) << bPlace
				<< setw(20) << aPlace << endl;
			cout << string(76, '-') << endl;
		}
		if (counts == 0)
			cout << "No schedule for this bus. Can assign schedule for this bus.\n\n";		

		do
		{
			cout << "Enter Date Of Journey(YYYY-MM-DD): ";
			getline(cin, date);

			if (!passenger.isValidDateFormat(date))
			{
				cout << RED << "\nInvalid date format! Please enter the date in the format YYYY-MM-DD.\n\n" << RESET;
				system("pause");
			}
			else if (!passenger.isTodayOrFuture(date))
			{
				cout << RED << "\nInvalid date or date already past! Please enter a valid date.\n\n" << RESET;
				system("pause");
			}
		} while (!passenger.isValidDateFormat(date) || !passenger.isTodayOrFuture(date));

		while (true)
		{
			cout << "Enter the departure time(HH:MM): ";
			getline(cin, depart_time);
			regex dTime_regex("^\\d{2}:\\d{2}$");
			
			if (regex_match(depart_time, dTime_regex))
				break;
			else
				cout << RED << "Invalid time format. Please try again!\n" << RESET;
		}

		while (!check)
		{
			// Get valid boarding place
			while (true)
			{
				cout << "\n1. Johor Sentral\n";
				cout << "2. Melaka Sentral\n";
				cout << "3. KL Sentral\n";
				cout << "4. Penang Sentral\n";
				cout << "5. Kedah Sentral\n";
				cout << "Enter Boarding Place: ";

				if (!(cin >> b_choice)) {
					cin.clear(); // clear failbit
					cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
					cout << RED << "\nInvalid input! Please enter a number.\n" << RESET;
					continue;
				}

				switch (b_choice)
				{
				case 1: b_place = "Johor Sentral"; break;
				case 2: b_place = "Melaka Sentral"; break;
				case 3: b_place = "KL Sentral"; break;
				case 4: b_place = "Penang Sentral"; break;
				case 5: b_place = "Kedah Sentral"; break;
				default:
					cout << RED << "\nInvalid choice! Please Try Again.\n" << RESET;
					continue;
				}
				break; // Valid choice, exit boarding place loop
			}

			// Get valid arrival place
			while (true)
			{
				cout << "\n1. Johor Sentral\n";
				cout << "2. Melaka Sentral\n";
				cout << "3. KL Sentral\n";
				cout << "4. Penang Sentral\n";
				cout << "5. Kedah Sentral\n";
				cout << "Enter Arrival Place: ";

				if (!(cin >> a_choice)) {
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << RED << "\nInvalid input! Please enter a number.\n" << RESET;
					continue;
				}

				switch (a_choice)
				{
				case 1: a_place = "Johor Sentral"; break;
				case 2: a_place = "Melaka Sentral"; break;
				case 3: a_place = "KL Sentral"; break;
				case 4: a_place = "Penang Sentral"; break;
				case 5: a_place = "Kedah Sentral"; break;
				default:
					cout << RED << "\nInvalid choice! Please Try Again.\n" <<RESET;
					continue;
				}
				break; // Valid choice, exit arrival place loop
			}

			// Check if both choices are different
			if (a_choice == b_choice)
			{
				cout << RED << "\nBoarding Place and Arrival Place cannot be the same!\nPlease Try Again.\n" << RESET;
				system("pause");
			}
			else
			{
				check = true; // Exit main loop
			}
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		while (true)
		{
			cout << "Enter the Arrival time(HH:MM): ";
			getline(cin, arrival_time);
			regex aTime_regex("^\\d{2}:\\d{2}$");

			if (regex_match(arrival_time, aTime_regex))
				break;
			else
				cout << RED << "Invalid time format. Please try again!\n" << RESET;
		}

		if (b_place == "Johor Sentral" && a_place == "Melaka Sentral" || a_place == "Johor Sentral" && b_place == "Melaka Sentral")
			price = 20.00;
		else if (b_place == "Johor Sentral" && a_place == "KL Sentral" || a_place == "Johor Sentral" && b_place == "KL Sentral")
			price = 30.00;
		else if (b_place == "Johor Sentral" && a_place == "Penang Sentral" || a_place == "Johor Sentral" && b_place == "Penang Sentral")
			price = 75.00;
		else if (b_place == "Johor Sentral" && a_place == "Kedah Sentral" || a_place == "Johor Sentral" && b_place == "Kedah Sentral")
			price = 90.00;
		else if (b_place == "KL Sentral" && a_place == "Melaka Sentral" || a_place == "KL Sentral" && b_place == "Melaka Sentral")
			price = 12.00;
		else if (b_place == "Penang Sentral" && a_place == "Melaka Sentral" || a_place == "Penang Sentral" && b_place == "Melaka Sentral")
			price = 45.00;
		else if (b_place == "Kedah Sentral" && a_place == "Melaka Sentral" || a_place == "Kedah Sentral" && b_place == "Melaka Sentral")
			price = 65.00;
		else if (b_place == "KL Sentral" && a_place == "Penang Sentral" || a_place == "KL Sentral" && b_place == "Penang Sentral")
			price = 35.00;
		else if (b_place == "KL Sentral" && a_place == "Kedah Sentral" || a_place == "KL Sentral" && b_place == "Kedah Sentral")
			price = 45.00;
		else if (b_place == "Penang Sentral" && a_place == "Kedah Sentral" || a_place == "Penang Sentral" && b_place == "Kedah Sentral")
			price = 25.00;
		else
			price = 0.00;

		depart_time += ":00";
		arrival_time += ":00";

		db.prepareStatement("SELECT COUNT(*) AS count FROM schedule WHERE busID = ? AND DateOfJourney = ? AND departureTime = ?");
		db.statement->setString(1, busid);
		db.statement->setDateTime(2, date);
		db.statement->setDateTime(3, depart_time);
		db.QueryResult();

		db.result->next();
		int count = db.result->getInt("count");

		if (count > 0)
		{
			cout << RED << "\nThis schedule already exist. Plaese select other bus or date and time.\n" << RESET;
			system("pause");

			check = false;
			continue; // back to the loop
		}

		db.prepareStatement("INSERT INTO schedule (departureTime, arrivalTime, DateOfJourney, boardingPlace, arrivalPlace, price, busID, staffID) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
		db.statement->setDateTime(1, depart_time);
		db.statement->setDateTime(2, arrival_time);
		db.statement->setDateTime(3, date);
		db.statement->setString(4, b_place);
		db.statement->setString(5, a_place);
		db.statement->setDouble(6, price);
		db.statement->setString(7, busid);
		db.statement->setString(8, staff_id);
		db.QueryStatement();

		cout << GREEN << "New Schedule add Successful!\n\n" << RESET;

		cout << "Do you want to add more schedule?(Y/N): ";
		while (true)
		{
			cin >> choice;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (choice == 'y' || choice == 'Y')
			{
				check = false;
				break;
			}
			else if (choice == 'n' || choice == 'N')
			{
				isChoice = 1;
				break;
			}
			else
			{
				cout << "\nInvalid input. Please enter Y or N: ";
			}
		}
	}
	manageSchedule();
}

void Staff::updateSchedule()
{
	MySQL db;
	string s_ID, d_time, a_time, date, busid;
	string s_id, D_time, A_time, doj, b_place, a_place, b_id;
	int ch;
	double price, s_price;
	char option;
	bool isChoice = 0;

	db.prepareStatement("SELECT * FROM schedule WHERE (DateOfJourney >= CURDATE())");
	db.QueryResult();

	while (!isChoice)
	{
		system("cls");
		cout << string(104, '-') << endl;
		cout << left
			<< setw(12) << "ScheduleID"
			<< setw(15) << "DepartureTime"
			<< setw(12) << "ArrivalTime"
			<< setw(15) << "DateOfJourney"
			<< setw(18) << "BoardingPlace"
			<< setw(18) << "ArrivalPlace"
			<< setw(8) << "Price"
			<< setw(6) << "BusID" << endl;

		cout << string(104, '-') << endl;

		while (db.result->next())
		{
			s_id = db.result->getString("scheduleID");
			D_time = db.result->getString("departureTime");
			A_time = db.result->getString("arrivalTime");
			doj = db.result->getString("DateOfJourney");
			b_place = db.result->getString("boardingPlace");
			a_place = db.result->getString("arrivalPlace");
			s_price = db.result->getDouble("price");
			b_id = db.result->getString("busID");

			cout << left
				<< setw(12) << s_id
				<< setw(15) << D_time
				<< setw(12) << A_time
				<< setw(15) << doj
				<< setw(18) << b_place
				<< setw(18) << a_place
				<< setw(8) << fixed << setprecision(2) << s_price
				<< setw(6) << b_id << endl;
			cout << string(104, '-') << endl;
		}
	
		cout << "Enter Schedule ID that you want to update: ";
		getline(cin, s_ID);
	
		db.prepareStatement("SELECT * FROM schedule WHERE scheduleID = ? AND DateOfJourney >= CURDATE()");
		db.statement->setString(1, s_ID);
		db.QueryResult();

		system("cls");
		cout << string(104, '-') << endl;
		cout << left
			<< setw(12) << "ScheduleID"
			<< setw(15) << "DepartureTime"
			<< setw(12) << "ArrivalTime"
			<< setw(15) << "DateOfJourney"
			<< setw(18) << "BoardingPlace"
			<< setw(18) << "ArrivalPlace"
			<< setw(8) << "Price"
			<< setw(6) << "BusID" << endl;

		cout << string(104, '-') << endl;
		if (db.result->next()) {
			s_id = db.result->getString("scheduleID");
			D_time = db.result->getString("departureTime");
			A_time = db.result->getString("arrivalTime");
			doj = db.result->getString("DateOfJourney");
			b_place = db.result->getString("boardingPlace");
			a_place = db.result->getString("arrivalPlace");
			s_price = db.result->getDouble("price");
			b_id = db.result->getString("busID");

			cout << left
				<< setw(12) << s_id
				<< setw(15) << D_time
				<< setw(12) << A_time
				<< setw(15) << doj
				<< setw(18) << b_place
				<< setw(18) << a_place
				<< setw(8) << fixed << setprecision(2) << s_price
				<< setw(6) << b_id << endl;
			cout << string(104, '-') << endl;
		}
		else {
			cout << RED << "Schedule ID not found!\n" << RESET;
			system("pause");
			continue;
		}

		cout << "1.Update Departure Time\n";
		cout << "2.Update Arrival Time\n";
		cout << "3.Update Date Of Journey\n";
		cout << "4.Update Bus ID\n";
		cout << "5.Update Price\n";
		cout << "6.Return\n";
		cout << "Enter your choice: ";

		if (!(cin >> ch)) {
			cin.clear(); // clear failbit
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
			cout << RED << "\nInvalid input! Please enter a number.\n" << RESET;
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (ch)
		{
		case 1:
			while (true)
			{
				cout << "Enter new departure time(HH:MM): ";
				getline(cin, d_time);
				regex dTime_regex("^\\d{2}:\\d{2}$");

				if (regex_match(d_time, dTime_regex))
					break;
				else
					cout << RED << "Invalid time format. Please try again!\n" << RESET;
			}

			d_time += ":00";

			db.prepareStatement("UPDATE schedule SET departureTime = ? WHERE scheduleID = ?");
			db.statement->setDateTime(1, d_time);
			db.statement->setString(2, s_ID);
			db.QueryStatement();

			cout << GREEN << "Update Successful!\n" << RESET;
			system("pause");
			break;

		case 2:
			while (true)
			{
				cout << "Enter the departure time(HH:MM): ";
				getline(cin, a_time);
				regex aTime_regex("^\\d{2}:\\d{2}$");

				if (regex_match(a_time, aTime_regex))
					break;
				else
					cout << RED << "Invalid time format. Please try again!\n" << RESET;
			}

			a_time += ":00";

			db.prepareStatement("UPDATE schedule SET arrivalTime = ? WHERE scheduleID = ?");
			db.statement->setString(1, a_time);
			db.statement->setString(2, s_ID);
			db.QueryStatement();

			cout << GREEN << "Update Successful!\n" << RESET;
			system("pause");
			break;

		case 3:
			while (true)
			{
				cout << "Enter New Date Of Journey(YYYY-MM-DD): ";
				getline(cin, date);
				regex datePattern("^\\d{4}-\\d{2}-\\d{2}$");

				if (regex_match(date, datePattern))
					break;
				else
					cout << RED << "Invalid date format. Please try again!\n" << RESET;
			}

			db.prepareStatement("UPDATE schedule SET DateOfJourney = ? WHERE scheduleID = ?");
			db.statement->setString(1, date);
			db.statement->setString(2, s_ID);
			db.QueryStatement();

			cout << GREEN << "Update Successful!\n" << RESET;
			system("pause");
			break;

		case 4:
			cout << "Enter New Bus ID: ";
			getline(cin, busid);

			db.prepareStatement("UPDATE schedule SET busID = ? WHERE scheduleID = ?");
			db.statement->setString(1, busid);
			db.statement->setString(2, s_ID);
			db.QueryStatement();

			cout << GREEN << "Update Successful!\n" << RESET;
			system("pause");
			break;

		case 5:
			cout << "Enter New Price: ";
			cin >> price;

			db.prepareStatement("UPDATE schedule SET price = ? WHERE scheduleID = ?");
			db.statement->setDouble(1, price);
			db.statement->setString(2, s_ID);
			db.QueryStatement();

			cout << GREEN << "Update Successful!\n\n" <<RESET;
			system("pause");
			break;

		case 6:
			manageSchedule();
			break;
		default:
			cout << RED << "\nInvalid input! Please try again.\n" << RESET;
			continue;
		}

		while (true)
		{
			cout << BLUE << "Do you want to update other information?(Y/N): " << RESET;
			cin >> option;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (option == 'y' || option == 'Y')
			{
				break;
			}
			else if (option == 'n' || option == 'N')
			{
				isChoice = 1;
				system("pause");
				break;
			}
			else
			{
				cout << RED << "\nInvalid input. Please enter Y or N.\n" << RESET;
			}
		}
	}
	manageSchedule();
}

void Staff::deleteSchedule()
{
	MySQL db;
	string scheduleid, s_id, D_time, A_time, doj, b_place, a_place, b_id;
	double s_price;
	char opt, confirm;
	bool isconfirm = 0,check = 0;

	db.prepareStatement("SELECT * FROM schedule WHERE DateOfJourney >= CURDATE()");
	db.QueryResult();

	system("cls");
	cout << string(104, '-') << endl;
	cout << left
		<< setw(12) << "ScheduleID"
		<< setw(15) << "DepartureTime"
		<< setw(12) << "ArrivalTime"
		<< setw(15) << "DateOfJourney"
		<< setw(18) << "BoardingPlace"
		<< setw(18) << "ArrivalPlace"
		<< setw(8) << "Price"
		<< setw(6) << "BusID" << endl;

	cout << string(104, '-') << endl;

	while (db.result->next())
	{
		s_id = db.result->getString("scheduleID");
		D_time = db.result->getString("departureTime");
		A_time = db.result->getString("arrivalTime");
		doj = db.result->getString("DateOfJourney");
		b_place = db.result->getString("boardingPlace");
		a_place = db.result->getString("arrivalPlace");
		s_price = db.result->getDouble("price");
		b_id = db.result->getString("busID");

		cout << left
			<< setw(12) << s_id
			<< setw(15) << D_time
			<< setw(12) << A_time
			<< setw(15) << doj
			<< setw(18) << b_place
			<< setw(18) << a_place
			<< setw(8) << fixed << setprecision(2) << s_price
			<< setw(6) << b_id << endl;
		cout << string(104, '-') << endl;
	}

	while (!check)
	{
		while (!isconfirm)
		{
			cout << "\nEnter Schedule ID that you want to delete (or 'Q' to cancel): ";
			getline(cin, scheduleid);

			db.prepareStatement("SELECT * FROM schedule WHERE scheduleID = ? AND DateOfJourney >= CURDATE()");
			db.statement->setString(1, scheduleid);
			db.QueryResult();

			if (scheduleid == "q" || scheduleid == "Q") {
				cout << YELLOW << "Returning to Schedule Management Menu...\n" << RESET;
				system("pause");
				manageSchedule(); // Exit to previous menu
				return;
			}

			if (db.result->next())
			{
				while (true)
				{
					cout << BLUE << "Did you sure want to delete this schedule?(Y/N): " << RESET;
					cin >> confirm;
					cin.ignore(numeric_limits<streamsize>::max(), '\n');

					if (confirm == 'y' || confirm == 'Y')
					{
						isconfirm = 1;
						db.prepareStatement("DELETE FROM schedule WHERE scheduleID = ?");
						db.statement->setString(1, scheduleid);
						db.QueryStatement();

						cout << GREEN << "Delete Successful!\n\n" << RESET;
						break;
					}
					else if (confirm == 'n' || confirm == 'N')
					{
						cout << YELLOW << "Returning to Schedule Management Menu...\n" << RESET;
						system("pause");
						manageSchedule();
						break;
					}
					else
					{
						cout << RED << "\nInvalid input. Please enter Y or N.\n" << RESET;
						system("pause");

					}
				}
			}
			else
			{
				cout << RED << "Schedule ID not found or expired!\n" << RESET;
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
					isconfirm = 0;
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
	manageSchedule();
}

void Staff::cancelTicket()
{
	MySQL db;
	Menu menu;
	string t_id, status, p_id, s_id, app;
	int seatNo, count = 0;
	bool check = 0, hasRequest = 1;

	while (true)
	{
		system("cls");
		cout << CYAN << "\n\t--------Cancel Ticket Request--------\n\n" << RESET;

		// Reset count and check at the start of each loop
		count = 0;
		check = false;

		db.prepareStatement("SELECT * FROM ticket WHERE status = 'Pending'");
		db.QueryResult();

		cout << string(59, '-') << endl;
		cout << left
			<< setw(10) << "TicketID" << "| "
			<< setw(7) << "SeatNo" << "| "
			<< setw(9) << "Status" << " | "
			<< setw(13) << "PassengerID" << "| "
			<< setw(11) << "ScheduleID" << "|" << endl;
		cout << string(59, '-') << endl;

		// Display all pending requests
		while (db.result->next())
		{
			t_id = db.result->getString("ticketID");
			seatNo = db.result->getInt("seatNo");
			status = db.result->getString("status");
			p_id = db.result->getString("passengerID");
			s_id = db.result->getString("scheduleID");

			cout << left
				<< setw(10) << t_id << "| "
				<< setw(7) << seatNo << "| "
				<< setw(9) << status << "| "
				<< setw(13) << p_id << "| "
				<< setw(11) << s_id  << "|" << endl;
			cout << string(59, '-') << endl;
			count++;
		}

		if (count == 0)
		{
			cout << "No cancel request available.\n";
			system("pause");
			break; // Exit the loop
		}

		// Prompt to approve one ticket at a time
		cout << GREEN << "\nEnter ticket ID to approve the request (q to quit): " << RESET;
		cin >> app;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (app == "q" || app == "Q")
			break;

		db.prepareStatement("SELECT * FROM ticket WHERE ticketID = ? AND status = 'Pending'");
		db.statement->setString(1, app);
		db.QueryResult();

		if (db.result->next())
		{
			db.prepareStatement("UPDATE ticket SET status = 'Cancelled' WHERE ticketID = ?");
			db.statement->setString(1, app);
			db.QueryStatement();
			cout << GREEN << "Cancel Request Approved\n" << RESET;
			system("pause");
		}
		else
		{
			cout << RED << "Invalid or already processed Ticket ID.\n" << RESET;
			system("pause");
		}
	}
	menu.StaffMenu(*this);
}

void Staff::managePassenger()
{
	MySQL db;
	Menu menu;
	string p_id, status, u_id, name, phoneNo, username, app;
	bool hasRequest = 1, check = 0;
	int count = 0;

	while (true)
	{
		system("cls");
		cout << CYAN << "\n\t\t\t--------Delete Account Request--------\n\n" << RESET;

		// Reset count and check
		count = 0;
		check = false;

		db.prepareStatement("SELECT * FROM passenger p JOIN user u ON p.userID = u.userID WHERE p.status = 'Pending'");
		db.QueryResult();

		cout << string(93, '-') << endl;
		cout << left
			<< setw(12) << "PassengerID" << "| "
			<< setw(20) << "Name" << "| "
			<< setw(15) << "PhoneNo" << "| "
			<< setw(10) << "UserID" << "| "
			<< setw(15) << "Username" << "| "
			<< setw(10) << "Status" << "| " << endl;
		cout << string(93, '-') << endl;

		while (db.result->next())
		{
			p_id = db.result->getString("passengerID");
			name = db.result->getString("name");
			phoneNo = db.result->getString("phoneNo");
			u_id = db.result->getString("userID");
			username = db.result->getString("username");
			status = db.result->getString("status");

			cout << left
				<< setw(12) << p_id << "| "
				<< setw(20) << name << "| "
				<< setw(15) << phoneNo << "| "
				<< setw(10) << u_id << "| "
				<< setw(15) << username << "| "
				<< setw(10) << status << "| "
				<< endl;
			cout << string(93, '-') << endl;
			count++;
		}

		if (count == 0)
		{
			cout << "\nNo request available.\n";
			system("pause");
			break;
		}

		cout << GREEN << "\nEnter passenger ID to approve the request (q to quit): " << RESET;
		cin >> app;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (app == "q" || app == "Q")
			break;

		db.prepareStatement("SELECT * FROM passenger WHERE passengerID = ? AND status = 'Pending'");
		db.statement->setString(1, app);
		db.QueryResult();

		if (db.result->next())
		{
			db.prepareStatement("UPDATE passenger SET status = 'Inactive' WHERE passengerID = ?");
			db.statement->setString(1, app);
			db.QueryStatement();
			cout << GREEN << "Account Delete Request Approved\n" << RESET;
			system("pause");
		}
		else
		{
			cout << RED << "Invalid or already processed Passenger ID.\n" << RESET;
			system("pause");
		}
	}
	menu.StaffMenu(*this);
}