#include <iostream>
#include <regex>
#include <conio.h>
#include <iomanip>
#include <chrono>
#include "Passenger.h"
#include "Menu.h"
#include "MySQL.h"
using namespace std;

const string RED = "\033[1;31m";
const string GREEN = "\033[1;32m";
const string BLUE = "\033[1;34m";
const string YELLOW = "\033[1;33m";
const string CYAN = "\033[1;36m";
const string RESET = "\033[0m";

bool Passenger::isValidDateFormat(const string& date)
{
	// Regular expression for date format YYYY-MM-DD
	const regex datePattern("^\\d{4}-\\d{2}-\\d{2}$");

	if (!regex_match(date, datePattern))
	{
		return false; // If it doesn't match the pattern
	}
	else
		return true; // All checks passed
}

bool Passenger::isTodayOrFuture(const string& date)
{
	// Extract year, month, and day from the date string
	int year = stoi(date.substr(0, 4));
	int month = stoi(date.substr(5, 2));
	int day = stoi(date.substr(8, 2));

	// Get the current date using chrono
	using namespace std::chrono;
	auto now = system_clock::now();
	time_t currentTime = system_clock::to_time_t(now);
	tm currentDate;

	if (month < 1 || month > 12)
		return false; // Month must be 1-12

	if (day < 1 || day > 31)
		return false; // Day must be 1-31

	// Check days in each month
	if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
		return false; // Apr, Jun, Sep, Nov have 30 days

	if (month == 2)
	{
		// February check for leap year
		bool isLeapYear = ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));

		if ((isLeapYear && day > 29) || (!isLeapYear && day > 28))
			return false;
	}

#if defined(_WIN32) || defined(_WIN64)
	localtime_s(&currentDate, &currentTime); // For Windows
#endif

	int currentYear = 1900 + currentDate.tm_year;
	int currentMonth = 1 + currentDate.tm_mon;
	int currentDay = currentDate.tm_mday;

	// Compare input date with current date
	if (year > currentYear)
		return true;

	if (year == currentYear)
	{
		if (month > currentMonth)
			return true;

		if (month == currentMonth && day >= currentDay)
			return true;
	}

	return false;
}

bool isSeatBooked(const string& seatNo, const string& scheduleID)
{
	MySQL db;
	Passenger passenger;
	db.prepareStatement("SELECT * FROM ticket WHERE seatNo = ? AND scheduleID = ? AND (status = 'Booked' OR status = 'Cancelled')");
	db.statement->setString(1, seatNo);
	db.statement->setString(2, scheduleID);
	db.QueryResult();
	return db.result->next();
}

bool isNumeric(const string& str) {
	for (char ch : str) {
		if (!isdigit(ch)) return false;
	}
	return !str.empty(); // Ensure it's not an empty string
}

void Passenger::searchBus()
{
	MySQL db;
	string date, a_place, b_place;
	int b_choice, a_choice;
	bool check = 0;
	
	do
	{
		system("cls");
		cout << CYAN << "\t\t\t\t\t\t\t\t--------Search Bus--------\n\n" << RESET;
		cout << "\t\t\t\t\t\t\t\tEnter Date Of Journey(YYYY-MM-DD): ";
		getline(cin, date);
 
		if (!isValidDateFormat(date))
		{
			cout << RED << "\n\t\t\t\t\tInvalid date format! Please enter the date in the format YYYY-MM-DD.\n\n" << RESET;
			system("pause");
		}
		else if (!isTodayOrFuture(date))
		{
			cout << RED << "\n\t\t\t\t\tInvalid date or date already past! Please enter a valid date.\n\n" << RESET;
			system("pause");
		}
	} while (!isValidDateFormat(date) || !isTodayOrFuture(date));

	while (!check)
	{
		// Get valid boarding place
		while (true)
		{
			system("cls");
			cout << GREEN << "\t\t\t\t\t\t\t\tYour Selected Date is " << date  << RESET << endl;
			cout << CYAN << "\n\t\t\t\t\t\t\t\tBoarding Place\n" << RESET;
			cout << "\t\t\t\t\t\t\t\t--------------";
			cout << "\n\t\t\t\t\t\t\t\t1. Johor Sentral\n";
			cout << "\t\t\t\t\t\t\t\t2. Melaka Sentral\n";
			cout << "\t\t\t\t\t\t\t\t3. KL Sentral\n";
			cout << "\t\t\t\t\t\t\t\t4. Penang Sentral\n";
			cout << "\t\t\t\t\t\t\t\t5. Kedah Sentral\n";
			cout << "\t\t\t\t\t\t\t\tEnter Boarding Place: ";
			if (cin.peek() == '\n') {
				cin.ignore(); // consume the newline
				cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 - 5.\n" << RESET;
				system("pause");
				continue;
			}

			if (!(cin >> b_choice))
			{
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a number between 1 - 5!\n" << RESET;
				system("pause");
				continue;
			}

			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			switch (b_choice)
			{
			case 1: b_place = "Johor Sentral"; break;
			case 2: b_place = "Melaka Sentral"; break;
			case 3: b_place = "KL Sentral"; break;
			case 4: b_place = "Penang Sentral"; break;
			case 5: b_place = "Kedah Sentral"; break;
			default:
				cout << RED << "\n\t\t\t\t\tInvalid choice for boarding place! Please Try Again.\n" << RESET;
				system("pause");
				continue;
			}
			break; // Valid choice, exit boarding place loop
		}

		// Get valid arrival place
		while (true)
		{
			system("cls");
			cout << GREEN << "\t\t\t\t\t\t\t\tYour Selected Date is " << date << endl;
			cout << "\t\t\t\t\t\t\t\tYour Selected Boarding Place is " << b_place  << RESET << endl;
			cout << CYAN << "\n\t\t\t\t\t\t\t\tArrival Place" << RESET;
			cout << "\n\t\t\t\t\t\t\t\t-------------";
			cout << "\n\t\t\t\t\t\t\t\t1. Johor Sentral\n";
			cout << "\t\t\t\t\t\t\t\t2. Melaka Sentral\n";
			cout << "\t\t\t\t\t\t\t\t3. KL Sentral\n";
			cout << "\t\t\t\t\t\t\t\t4. Penang Sentral\n";
			cout << "\t\t\t\t\t\t\t\t5. Kedah Sentral\n";
			cout << "\t\t\t\t\t\t\t\tEnter Arrival Place: ";
			if (cin.peek() == '\n') {
				cin.ignore(); // consume the newline
				cout << RED << "\n\t\t\t\t\tInput cannot be empty! Please enter a number between 1 - 5.\n" << RESET;
				system("pause");
				continue;
			}

			if (!(cin >> a_choice))
			{
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a number between 1 - 5!\n" << RESET;
				system("pause");
				continue;
			}
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			switch (a_choice)
			{
			case 1: a_place = "Johor Sentral"; break;
			case 2: a_place = "Melaka Sentral"; break;
			case 3: a_place = "KL Sentral"; break;
			case 4: a_place = "Penang Sentral"; break;
			case 5: a_place = "Kedah Sentral"; break;
			default:
				cout << RED << "\n\t\t\t\t\t\tInvalid choice for arrival place! Please Try Again.\n" << RESET;
				system("pause");
				continue;
			}
			break; // Valid choice, exit arrival place loop
		}

		// Check if both choices are different
		if (a_choice == b_choice)
		{
			cout << RED << "\n\t\t\t\t\tBoarding Place and Arrival Place cannot be the same!\nPlease Try Again.\n" << RESET;
			system("pause");
		}
		else
		{
			check = true; // Exit main loop
		}
	}
	showSchedule(date, b_place, a_place);
}

void Passenger::showSchedule(string& date, string& b_place, string& a_place)
{
	MySQL db;
	string d_time, a_time;
	int count = 0, availableSeat;
	double price;

	while (true) {
		system("cls");
		db.prepareStatement("SELECT s.scheduleID,"
			"TIME_FORMAT(s.departureTime, '%H:%i') AS departureTime,"
			"TIME_FORMAT(s.arrivalTime, '%H:%i') AS arrivalTime,"
			"s.boardingPlace,"
			"s.arrivalPlace,"
			"s.price,"
			"(28 - IFNULL(("
				"SELECT COUNT(*) "
				"FROM ticket t "
				"WHERE t.scheduleID = s.scheduleID AND (t.status = 'Booked' OR t.status = 'Cancelled')"
				"), 0)) AS availableSeats "
			"FROM schedule s "
			"WHERE s.DateOfJourney = ? "
			"AND((s.DateOfJourney = CURDATE() AND s.departureTime > CURTIME()) "
				"OR s.DateOfJourney > CURDATE())"
			"AND s.boardingPlace = ? "
			"AND s.arrivalPlace = ? "
		"ORDER BY s.departureTime ASC");

		db.statement->setDateTime(1, date);
		db.statement->setString(2, b_place);
		db.statement->setString(3, a_place);
		db.QueryResult();
		cout << CYAN << "\t\t\t--------Schedule Available--------\n" << RESET;
		cout << string(125, '-') << endl;
		cout << left
			<< setw(15) << "ScheduleID"
			<< setw(20) << "Departure Time"
			<< setw(20) << "Arrival Time"
			<< setw(20) << "Boarding Place"
			<< setw(20) << "Arrival Place"
			<< setw(12) << "Price (RM)"
			<< setw(15) << "Available Seats" << endl;
		cout << string(125, '-') << endl;

		count = 0;
		while (db.result->next()) {
			s_id = db.result->getString("scheduleID");
			d_time = db.result->getString("departureTime");
			a_time = db.result->getString("arrivalTime");
			b_place = db.result->getString("boardingPlace");
			a_place = db.result->getString("arrivalPlace");
			price = db.result->getDouble("price");
			availableSeat = db.result->getInt("availableSeats");

			cout << left
				<< setw(15) << s_id
				<< setw(20) << d_time
				<< setw(20) << a_time
				<< setw(20) << b_place
				<< setw(20) << a_place
				<< fixed << setprecision(2)
				<< setw(12) << price
				<< setw(15) << availableSeat << endl;
			count++;
		}
		cout << string(125, '-') << endl;

	
		if (count <= 0)
		{
			cout << RED << "\nNo schedule available. Please enter other date or place.\n" << RESET << endl;
			system("pause");
			searchBus();
		}
		else {
			string choice;
			while (true)
			{
				cout << "\nEnter the scheduleID that you want to choose (R0XX): ";
				getline(cin, choice);

				if (choice.empty())
				{
					cout << RED << "\n\t\t\t\t\t\tSchedule ID cannot be empty. Please enter a Schedule ID.\n" << RESET;
					break;
				}
				else
					break;
			}

			db.prepareStatement("SELECT * FROM schedule WHERE scheduleID = ? AND boardingPlace = ? AND arrivalPlace = ? AND DateOfJourney >= CURDATE()");
			db.statement->setString(1, choice);
			db.statement->setString(2, b_place);
			db.statement->setString(3, a_place);
			db.QueryResult();

			if (db.result->next())
			{
				s_id = choice;
				break;
			}
			else
			{
				cout << RED << "Invalid schedule ID. Please Try Again." << RESET << endl;
				system("pause");
				continue;
			}
		}
	}
	selectSeat();
}

void Passenger::updateSeat(string seats[][column], int rows, const string& scheduleID)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (isSeatBooked(seats[i][j], scheduleID))
				seats[i][j] = "X ";
		}
	}
}

void Passenger::selectSeat()
{
	MySQL db;
	char choice;
	string selectedSeats[28]; // Array to store selected seats
	int seatCount = 0;

	do {
		system("cls");
		int seatCounter = 1;

		for (int i = 0; i < rows; i++) 
		{
			for (int j = 0; j < column; j++) 
			{
				seats[i][j] = (seatCounter < 10 ? "0" : "") + to_string(seatCounter);
				seatCounter++;
			}
		}

		updateSeat(seats, rows, s_id);
		cout << string(40, '-') << endl;
		cout << "\tAvailable Seats\n\n";
		cout << string(40, '-') << endl;
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < column; j++)
			{
				cout << seats[i][j] << " ";
				if (j == 1)
					cout << " ";
			}
			cout << endl << endl;
		}

		while (true)
		{
			string chooseSeat;
			cout << "Enter the seat number: ";
			cin >> chooseSeat;

			if (!isNumeric(chooseSeat)) {
				cout << RED << "Invalid input. Please enter a number only.\n\n" << RESET;
				system("pause");
				continue;
			}

			int seatNum = stoi(chooseSeat);
			if (seatNum < 1 || seatNum > rows * column)
			{
				cout << RED << "Invalid seat number. Please try again!\n\n" << RESET;
				system("pause");
				continue;
			}

			if (isSeatBooked(chooseSeat, s_id))
			{
				cout << RED << "Seat number " << chooseSeat << " is already booked.\n"
					"Please select another seats! Thank you.\n" << RESET;
				system("pause");
			}
			else
			{
				db.prepareStatement("INSERT INTO ticket (seatNo, status, passengerID, scheduleID, paymentID) VALUES (?, 'Booked',?, ?, ?)");
				db.statement->setInt(1, seatNum);
				db.statement->setString(2, id);
				db.statement->setString(3, s_id);
				db.statement->setNull(4, sql::DataType::VARCHAR);
				db.QueryStatement();

				selectedSeats[seatCount] = chooseSeat;
				seatCount++;

				cout << GREEN << "You have selected seat no ";
				for (int i = 0; i < seatCount; ++i) {
					cout << selectedSeats[i];
					if (i < seatCount - 1) cout << ", ";
				}
				cout << endl << RESET;

				break;
			}
		}
		while (true) {
			cout << BLUE << "\nDo you want to book another seat? (Y/N): " << RESET;
			cin >> choice;

			if (choice == 'Y' || choice == 'y' || choice == 'N' || choice == 'n') {
				break;
			}
			else {
				cout << RED << "Invalid input. Please enter 'Y' or 'N' only.\n" << RESET;
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
		}
	} while (choice == 'Y' || choice == 'y');
	payment(seatCount, selectedSeats);
}

void Passenger::payment(int seatCount, string selectedSeats[])
{
	MySQL db;
	Menu menu;
	string payID;
	char choice, pay;
	bool check = 0;
	double totalPrice, price;

	db.prepareStatement("SELECT price FROM schedule WHERE scheduleID = ?");
	db.statement->setString(1, s_id);
	db.QueryResult();

	db.result->next();
	price = db.result->getDouble("price");

	totalPrice = price * seatCount;
	system("cls");
	cout << CYAN << "\t\t\t\t\t\t\t--------Proceed To Payment--------\n\n" << RESET;
	cout << "\t\t\t\t\t\t\tYour Booking Seat No: ";
	for (int i = 0; i < seatCount; ++i) 
	{
		cout << selectedSeats[i];
		if (i != seatCount - 1) 
			cout << ", ";
	}
	cout << "\n\n\t\t\t\t\t\t\tTicket Price: RM" << right << fixed << setprecision(2) << price << endl;
	cout << "\n\t\t\t\t\t\t\tQuantity: " << right << seatCount << endl;
	cout << "\n\t\t\t\t\t\t\t-------------------------\n";
	cout << "\n\t\t\t\t\t\t\tTotal Price: RM" << right << fixed << setprecision(2) << totalPrice << endl;

	cout << BLUE << "\n\t\t\t\t\t\tDo you want to proceed to payment? (Y/N): " << RESET;
	cin >> choice;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	if (choice != 'Y' && choice != 'y' && choice == 'N' || choice == 'n') 
		goto cancel_payment;

	while (true)
	{
		cout << "\n\t\t\t\t\t\t\tProceed to payment...\n";
		system("cls");
		cout << "\n\t\t\t\t\t\t\tYour Wallet Balance: RM" << balance << endl;
		cout << "\n\t\t\t\t\t\t\tTotal Price: RM" << totalPrice << endl;

		if (balance >= totalPrice)
		{
			cout << BLUE << "\n\t\t\t\t\t\tDo you confirm to do this payment? (Y/N): " << RESET;
			cin >> pay;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (pay == 'Y' || pay == 'y')
			{
				for (int i = 0; i < seatCount; i++)
				{
					int seatNo = stoi(selectedSeats[i]);
					db.prepareStatement("INSERT INTO payment (p_dateTime, TotalAmount, paymentStatus, passengerID) VALUES (NOW(), ?, 'Successful', ?)");
					db.statement->setDouble(1, price);
					db.statement->setString(2, id);
					db.QueryStatement();

					db.prepareStatement("SELECT * FROM payment ORDER BY paymentID DESC LIMIT 1");
					db.QueryResult();
					if (db.result && db.result->next()) {
						payID = db.result->getString("paymentID");

						db.prepareStatement("UPDATE ticket SET paymentID = ? WHERE passengerID = ? AND seatNo = ? AND scheduleID = ?");
						db.statement->setString(1, payID);
						db.statement->setString(2, id);
						db.statement->setInt(3, seatNo);     // seat number as int
						db.statement->setString(4, s_id);
						db.QueryStatement();
					}
				}
				balance = balance - totalPrice;
				db.prepareStatement("SELECT balance FROM wallet WHERE passengerID = '" + id + "'");
				db.QueryResult();

				db.prepareStatement("UPDATE wallet SET balance = ? WHERE passengerID = '" + id + "'");
				db.statement->setDouble(1, balance);
				db.QueryStatement();


				cout << GREEN << "\n\n\t\t\t\t\t\t\tPayment done Successful! Thank You.\n";
				cout << RESET << "\t\t\t\t\t\t\tYou can check your booking at My Booking.\n";
				system("pause");
				break;
			}
			else if (pay == 'N' || pay == 'n')
				goto cancel_payment;
			else
			{
				cout << RED << "\t\t\t\t\t\t\tInvalid input. Please try again.\n" << RESET;
				system("pause");
				continue;
			}

		}
		else
		{
			cout << RED << "\n\t\t\t\t\t\t\tInsufficient Fund!\n" << RESET;
			cout << BLUE << "\n\t\t\t\t\t\t\tDo you want to top up? (Y/N): " << RESET;
			cin >> choice;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (choice == 'Y' || choice == 'y')
			{
				wallet(true);
				continue;
			}
			else if (choice == 'N' || choice == 'n')
				goto cancel_payment;
			else
			{
				cout << "\t\t\t\t\t\tInvalid input. Please try again.\n";
				system("pause");
				continue;
			}
		}
	}
	menu.CustMenu(*this);

	cancel_payment:
	for (int i = 0; i < seatCount; ++i) {
		int seatNo = stoi(selectedSeats[i]);
		db.prepareStatement("INSERT INTO payment (p_dateTime, totalAmount, paymentStatus, passengerID) VALUES (NOW(), ?, 'Failed', ?)");
		db.statement->setDouble(1, totalPrice);
		db.statement->setString(2, id);
		db.QueryStatement();

		string paymentId;
		db.prepareStatement("SELECT paymentID FROM payment WHERE passengerID = ? ORDER BY p_dateTime DESC LIMIT 1");
		db.statement->setString(1, id);
		db.QueryResult();

		if (db.result && db.result->next()) {
			paymentId = db.result->getString("paymentID");

			db.prepareStatement("UPDATE ticket SET paymentID = ?, status = 'Uncomplete' WHERE passengerID = ? AND seatNo = ? AND scheduleID = ?");
			db.statement->setString(1, paymentId);
			db.statement->setString(2, id);
			db.statement->setInt(3, seatNo);     // seat number as int
			db.statement->setString(4, s_id);
			db.QueryStatement();
		}
	}
	cout << RED << "\n\t\t\t\t\t\tPayment Failed\n" << RESET;
	cout << "\n\t\t\t\t\t\tGo Back to Main Menu...\n\n";
	system("pause");
	menu.CustMenu(*this);
}

void Passenger::showBooking()
{
	MySQL db;
	Menu menu;
	string boarding, arrival, date_of_journey, depart_time, arrive_time, status, t_ID, choice;
	int seatNo;
	char cancel;
	bool hasRecords = false;

	db.prepareStatement("SELECT * FROM schedule WHERE CONCAT(DateOfJourney, ' ', departureTime) < NOW()");
	db.QueryResult();
	if (db.result->next()) 
	{
		s_id = db.result->getString("scheduleID");

		db.prepareStatement("UPDATE ticket SET status = 'Completed' WHERE status = 'Booked' AND scheduleID = ?");
		db.statement->setString(1, s_id);
		db.QueryStatement();
	}

	db.prepareStatement("SELECT * FROM ticket t JOIN schedule s ON t.scheduleID = s.scheduleID WHERE t.passengerID = ? AND s.DateOfJourney > CURDATE() AND t.status IN ('Booked', 'Pending', 'Cancelled') ORDER BY s.departureTime ASC");
	db.statement->setString(1, id);
	db.QueryResult();

	system("cls");
	cout << CYAN << "\t\t\t\t\t--------Upcoming Schedule--------\n\n" << RESET;
	cout << string(104, '-') << endl;
	cout << left
		<< setw(12) << "Ticket ID"
		<< setw(10) << "Status"
		<< setw(18) << "Boarding"
		<< setw(18) << "Arrival"
		<< setw(14) << "Date"
		<< setw(10) << "Dep Time"
		<< setw(10) << "Arr Time"
		<< setw(12) << "Seat No" << endl;

	cout << string(104, '-') << endl;

	while (db.result->next())
	{
		hasRecords = true;

		boarding = db.result->getString("boardingPlace");
		arrival = db.result->getString("arrivalPlace");
		date_of_journey = db.result->getString("DateOfJourney");
		depart_time = db.result->getString("departureTime");
		arrive_time = db.result->getString("arrivalTime");
		status = db.result->getString("status");
		t_ID = db.result->getString("ticketID");
		seatNo = db.result->getInt("seatNo");

		cout << left
			<< setw(12) << t_ID
			<< setw(10) << status
			<< setw(18) << boarding
			<< setw(18) << arrival
			<< setw(14) << date_of_journey
			<< setw(10) << depart_time
			<< setw(10) << arrive_time
			<< setw(12) << seatNo << endl;
		cout << string(104, '-') << endl;
	}

	if (!hasRecords) {
		cout << RED << "\t\tNo schedules or tickets found.\n" << RESET;
		cout << "\t\t\t\t\t\tGo Back to Main Menu...\n\n";
		system("pause");
		menu.CustMenu(*this);
	}

	while (true) 
	{
		cout << BLUE << "Do you want to cancel your upcoming booking? (Y/N): " << RESET;
		cin >> cancel;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (cancel == 'Y' || cancel == 'y')
		{
			while (true)
			{
				cout << "Which ticket that you want to cancel?\n";
				cout << GREEN << "\nEnter the ticket ID (T0XX): " << RESET;
				getline(cin, choice);

				if (choice.empty())
				{
					cout << RED << "\nTicket ID cannot be empty. Please enter a Ticket ID.\n" << RESET;
					continue;
				}

				db.prepareStatement("SELECT ticketID FROM ticket WHERE ticketID = ? AND status = 'Booked'");
				db.statement->setString(1, choice);
				db.QueryResult();

				if (db.result->next())
				{
					db.prepareStatement("UPDATE ticket SET status = 'Pending' WHERE ticketID = ?");
					db.statement->setString(1, choice);
					db.QueryStatement();


					db.prepareStatement("UPDATE payment p JOIN ticket t ON p.paymentID = t.paymentID "
						"SET p.paymentStatus = 'Pending', p.reqRefundDate = NOW() WHERE t.ticketID = ?");
					db.statement->setString(1, choice);
					db.QueryStatement();

					cout << GREEN << "\nYour cancellation already send to our staff. Please wait for staff approve.\n Thank you.\n";
					cout << RESET << "\nGo Back to Main Menu...\n";
					system("pause");
					break;
				}
				else
				{
					cout << RED << "\nTicket ID is not exist. Please try again.\n" << RESET;
					continue;
				}
			}
			break;
		}
		else if (cancel == 'N' || cancel == 'n')
			break;
		else
		{
			cout << RED << "Invalid choice! Please Try Again.\n" << RESET;
			continue;
		}
	}
	menu.CustMenu(*this);
}

void Passenger::profile()
{
	Menu menu;
	int action;
	char choice;

	while (true)
	{
		system("cls");
		cout << CYAN << "\n\t\t\t\t\t\t\t\t--------My Account Info--------\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t\t\t1. Settings\n\n";
		cout << "\t\t\t\t\t\t\t\t\t2. Wallet\n\n";
		cout << "\t\t\t\t\t\t\t\t\t3. Return to Menu\n\n";
		cout << "\t\t\t\t\t\t\t\t\t4. Sign Out\n\n" << RESET;
		cout << GREEN << "\t\t\t\t\t\t\t\t\tEnter your action: " << RESET;

		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 4.\n" << RESET;
			system("pause");
			continue;
		}

		if (!(cin >> action))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\n\t\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 4!" << RESET << endl;
			system("pause");
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (action == 1)
		{
			setting();
			break;
		}

		else if (action == 2)
		{
			wallet(false);
			break;
		}
		else if (action == 3)
		{
			menu.CustMenu(*this);
			break;
		}
		else if (action == 4)
		{
			bool confirmedLogout = false;
			while (true)
			{
				cout << BLUE << "\t\t\t\t\t\t\tAre you sure to sign out from this account? (Y/N): " << RESET;
				if (cin.peek() == '\n') {
					cin.ignore(); // consume the newline
					cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 4.\n" << RESET;
					system("pause");
					continue;
				}
				cin >> choice;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (choice == 'y' || choice == 'Y')
				{
					confirmedLogout = true;
					break;
				}
				else if (choice == 'n' || choice == 'N')
				{
					break; // Go back to main menu loop
				}
				else
				{
					cout << RED << "\n\t\t\t\t\t\t\tInvalid input. Please enter Y or N.\n" << RESET;
				}
			}

			if (confirmedLogout)
			{
				menu.HomeMenu();
				break; // Exit outer loop
			}
		}
		else
		{
			cout << RED << "\n\t\t\t\t\t\t\tInvalid action! Please enter number between 1 - 4.\n" << RESET;
			system("pause");
			continue;
		}
	}
}

void Passenger::setting()
{
	int choice;

	while (true)
	{
		system("cls");
		cout << CYAN << "\n\t\t\t\t\t\t\t\t----------Settings----------\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t\t1. Edit Account Info\n\n";
		cout << "\t\t\t\t\t\t\t\t2. Change Password\n\n";
		cout << "\t\t\t\t\t\t\t\t3. Delete Account\n\n";
		cout << "\t\t\t\t\t\t\t\t4. Return\n\n" << RESET;
		cout << GREEN << "\t\t\t\t\t\t\t\tEnter your choice: " << RESET;

		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 and 4.\n" << RESET;
			system("pause");
			continue;
		}

		if (!(cin >> choice))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a number between 1 and 4!" << RESET << endl;
			system("pause");
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 1)
		{
			editInfo();
			break;
		}

		else if (choice == 2)
		{
			changePass();
			break;
		}
		else if (choice == 3)
		{
			ReqDeleteAcc();
			break;
		}
		else if (choice == 4)
		{
			profile();
			break;
		}
		else
		{
			cout << RED << "\n\t\t\t\t\t\tInvalid choice! Please enter number between 1 - 4.\n" << RESET;
			system("pause");
			continue;
		}
	}
}

void Passenger::editInfo()
{
	MySQL db;
	int choice, age;
	string input, userid;
	bool isChoice = false;
	char option;

	// Always run this at the top of each loop since the result only fetches once
start:
	db.prepareStatement("SELECT * FROM passenger p JOIN user u ON p.userID = u.userID WHERE passengerID = ?");
	db.statement->setString(1, id);
	db.QueryResult();

	if (!db.result || !db.result->next()) {
		cout << RED << "Failed to retrieve passenger info.\n" << RESET;
		return;
	}

	while (!isChoice)
	{
		system("cls");
		cout << "\t\t\t\t\t\t\t" << string(40, '-') << endl;
		cout << "\t\t\t\t\t\t\t" << left << setw(20) << "Field" << " | " << setw(20) << "Value" << endl;
		cout << "\t\t\t\t\t\t\t" << string(40, '-') << endl;

		cout << "\t\t\t\t\t\t\t" << left << setw(20) << "Name" << "| " << setw(20) << db.result->getString("name") << endl;
		cout << "\t\t\t\t\t\t\t" << left << setw(20) << "Age" << "| " << setw(20) << db.result->getInt("age") << endl;
		cout << "\t\t\t\t\t\t\t" << left << setw(20) << "Gender" << "| " << setw(20) << db.result->getString("gender") << endl;
		cout << "\t\t\t\t\t\t\t" << left << setw(20) << "Phone Number" << "| " << setw(20) << db.result->getString("phoneNo") << endl;
		cout << "\t\t\t\t\t\t\t" << left << setw(20) << "Email" << "| " << setw(20) << db.result->getString("email") << endl;
		cout << "\t\t\t\t\t\t\t" << left << setw(20) << "Username" << "| " << setw(20) << db.result->getString("username") << endl;
		cout << "\t\t\t\t\t\t\t" << string(40, '-') << endl;

		cout << CYAN << "\n\t\t\t\t\t\t\tWhat information would you like to update?\n\n" << RESET;
		cout << YELLOW << "\t\t\t\t\t\t\t1. Age\n\n";
		cout << "\t\t\t\t\t\t\t2. Phone Number\n\n";
		cout << "\t\t\t\t\t\t\t3. Email\n\n";
		cout << "\t\t\t\t\t\t\t4. Username\n\n";
		cout << "\t\t\t\t\t\t\t5. Return\n\n" << RESET;
		cout << GREEN << "\t\t\t\t\t\t\tEnter your choice: " << RESET;
		
		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter a number between 1 - 5.\n" << RESET;
			system("pause");
			continue;
		}

		if (!(cin >> choice))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\n\t\t\t\t\t\tInvalid input. Please enter a number between 1 - 5!\n" << RESET;
			system("pause");
			continue;
		}

		if (choice < 1 || choice > 5) {
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << RED << "\n\t\t\t\t\t\tInvalid choice. Please enter number between 1 - 5.\n" << RESET;
			system("pause");
			continue;
		}

		cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

		switch (choice)
		{
		case 1:
			while (true)
			{
				cout << "\t\t\t\t\t\t\tAge: ";
				getline(cin, input);
				if (input.empty()) {
					cout << RED << "\t\t\t\t\t\t\tInput cannot be empty.\n" << RESET;
					continue;
				}
				try {
					age = stoi(input);
					if (age <= 0) {
						cout << RED << "\t\t\t\t\t\t\tPlease enter a positive number.\n" << RESET;
						continue;
					}
					break;
				}
				catch (...) {
					cout << RED << "Invalid input. Please enter a number.\n" << RESET;
				}
			}
			db.prepareStatement("UPDATE passenger SET age = ? WHERE passengerID = ?");
			db.statement->setInt(1, age);
			db.statement->setString(2, id);
			db.QueryStatement();
			break;

		case 2:
			while (true)
			{
				cout << "\t\t\t\t\t\t\tEnter New Phone Number (0XX-XXXXXXX): ";
				getline(cin, phoneNo);
				if (regex_match(phoneNo, regex("^\\d{3}-\\d{7,8}$")))
					break;
				else
					cout << RED << "\t\t\t\t\t\t\tInvalid phone format.\n" << RESET;
			}
			db.prepareStatement("UPDATE passenger SET phoneNo = ? WHERE passengerID = ?");
			db.statement->setString(1, phoneNo);
			db.statement->setString(2, id);
			db.QueryStatement();
			break;

		case 3:
			while (true)
			{
				cout << "\t\t\t\t\t\t\tEnter New Email Address: ";
				getline(cin, email);
				if (regex_match(email, regex("^[\\w.%+-]+@[\\w.-]+\\.[a-zA-Z]{2,}$")))
					break;
				else
					cout << RED << "\n\t\t\t\t\t\t\tInvalid email format.\n" << RESET;
			}
			db.prepareStatement("UPDATE passenger SET email = ? WHERE passengerID = ?");
			db.statement->setString(1, email);
			db.statement->setString(2, id);
			db.QueryStatement();
			break;

		case 4:
			cout << "\t\t\t\t\t\t\tEnter New Username: ";
			getline(cin, username);
			db.prepareStatement("UPDATE user SET username = ? WHERE userID = ?");
			db.statement->setString(1, username);
			db.statement->setString(2, userid);
			db.QueryStatement();
			break;

		case 5:
			setting();
			break;

		default:
			cout << RED << "\n\t\t\t\t\t\t\tInvalid choice. Please enter number between 1 - 5.\n" << RESET;
			system("pause");
		}

		cout << GREEN << "\t\t\t\t\t\t\tUpdate successful!\n" << RESET;

		// Ask to continue
		char option;
		while (true)
		{
			cout << BLUE << "\t\t\t\t\t\tDo you want to update more information? (Y/N): " << RESET;
			if (cin.peek() == '\n') {
				cin.ignore(); // consume the newline
				cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter Y or N only.\n" << RESET;
				system("pause");
				continue;
			}
			cin >> option;
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush newline

			if (option == 'Y' || option == 'y') {
				goto start; // Re-fetch and re-display updated info
			}
			else if (option == 'N' || option == 'n') {
				isChoice = true;
				break; // exit this loop
			}
			else {
				cout << RED << "\t\t\t\t\t\t\tInvalid input. Please enter 'Y' or 'N' only.\n" << RESET;
				system("pause"); // pause before prompting again
			}
		}

	}
	setting();
}

void Passenger::changePass()
{
	MySQL db;
	string userid, pswd, newpswd, reenterpswd;
	bool match = 0, pswdmatch = 0;

	db.prepareStatement("SELECT * FROM passenger p JOIN user u ON p.userID = u.userID WHERE passengerID = '" + id + "'");
	db.QueryResult();

	db.result->next();
	db.result->getString("password");
	userid = db.result->getString("userID");

	system("cls");
	cout << CYAN << "\t\t\t\t\t\t\t\t--------Change Password--------\n" << RESET;
	while (!match)
	{
		pswd.clear();
		cout << "\n\t\t\t\t\t\t\t\tEnter Current Password: ";
		while (true)
		{
			char ch;
			ch = _getch();

			// exit the loop if the user presses 'Enter'
			if (ch == '\r' || ch == '\n')
			{
				break;
			}

			if (ch == '\b')
			{
				if (!pswd.empty()) {
					cout << "\b \b";  // Move back, erase character, move back again
					pswd.pop_back();
				}
			}

			else if (ch == ' ')
			{
				cout << RED << "\n\t\t\t\t\t\t\tPassword cannot have space. Please Try Again!\n" << RESET;
				cout << "\n\t\t\t\t\t\t\t\tEnter current password: ";
				pswd.clear();
			}

			else
			{
				pswd += ch;
				cout << '*';
			}
		}

		db.prepareStatement("SELECT * FROM user WHERE password = ?");
		db.statement->setString(1, pswd);
		db.QueryResult();

		if (db.result->next())
		{
			match = true;
		}
		else
		{
			cout << RED << "\n\n\t\t\t\t\t\t\tPassword Incorrect. Please Try Again!\n" << RESET;
			system("pause");
		}
	}

	while (!pswdmatch)
	{
		newpswd.clear();
		reenterpswd.clear();

		cout << "\n\n\t\t\t\t\t\t\t\tEnter New Password: ";
		while (true)
		{
			char ch;
			ch = _getch();

			// exit the loop if the user presses 'Enter'
			if (ch == '\r' || ch == '\n')
			{
				break;
			}

			if (ch == '\b')
			{
				if (!newpswd.empty()) {
					cout << "\b \b";  // Move back, erase character, move back again
					newpswd.pop_back();
				}
			}

			else if (ch == ' ')
			{
				cout << RED << "\n\t\t\t\t\t\t\tPassword cannot have space. Please Try Again!\n" << RESET;
				cout << "\n\t\t\t\t\t\t\t\tEnter New Password: ";
				newpswd.clear();
			}

			else
			{
				newpswd += ch;
				cout << '*';
			}
		}
		
		cout << "\n\t\t\t\t\t\t\t\tReenter new password: ";
		while (true)
		{
			char ch;
			ch = _getch();

			// exit the loop if the user presses 'Enter'
			if (ch == '\r' || ch == '\n')
			{
				break;
			}

			if (ch == '\b')
			{
				if (!reenterpswd.empty()) {
					cout << "\b \b";  // Move back, erase character, move back again
					reenterpswd.pop_back();
				}
			}

			else if (ch == ' ')
			{
				cout << RED << "\n\t\t\t\t\t\t\tPassword cannot have space. Please Try Again!\n" << RESET;
				cout << "\n\t\t\t\t\t\t\t\tReenter new password: ";
				reenterpswd.clear();
			}

			else
			{
				reenterpswd += ch;
				cout << '*';
			}
		}

		if (newpswd == reenterpswd)
		{
			pswdmatch = true;
			db.prepareStatement("UPDATE user SET password = ? WHERE userID = '" + userid + "'");
			db.statement->setString(1, newpswd);
			db.QueryStatement();

			cout << GREEN << "\n\t\t\t\t\t\t\t\tPassword Update Successful!\n" << RESET;
			system("pause");
		}
		else
		{
			cout << RED << "\n\n\t\t\t\t\t\t\tPassword Not Match! Please Try Again.\n" << RESET;
			system("pause");
		}
	}
	setting();
}

void Passenger::ReqDeleteAcc()
{
	MySQL db;
	Menu menu;
	string userid;
	char choice;
	bool istrue = 0;

	db.prepareStatement("SELECT * FROM passenger p JOIN user u ON p.userID = u.userID WHERE passengerID = '" + id + "'");
	db.QueryResult();

	db.result->next();
	userid = db.result->getString("userID");

	while (!istrue)
	{
		system("cls");
		cout << CYAN << "\n\t\t\t\t\t\t\t--------Request to Delete Account--------\n" << RESET;

		while (true)
		{
			cout << BLUE << "\n\t\t\t\t\t\t\tDid you confirm want to request delete your account?(Y/N): " << RESET;
			cin >> choice;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (choice == 'y' || choice == 'Y')
			{
				istrue = 1;

				// Simplified query without unnecessary JOIN
				db.prepareStatement("UPDATE passenger SET status = 'Pending' WHERE userID = ?");
				db.statement->setString(1, userid);
				db.QueryStatement();

				cout << GREEN << "\t\t\t\t\t\t\tYour request is sent to our staff successfully!\n\n" << RESET;
				system("pause");

				menu.HomeMenu();  // Only go to home after request is made
				break;
			}
			else if (choice == 'n' || choice == 'N')
			{
				setting();  // Return to previous menu
				return;     // Exit the function to avoid going to HomeMenu
			}
			else
			{
				cout << RED << "\n\t\t\t\t\t\t\t\tInvalid input. Please enter Y or N.\n" << RESET;
				system("pause");
			}
		}
	}

}

void Passenger::wallet(bool returnToPayment)
{
	MySQL db;
	string walletid;
	char choice;
	int option;
	double amount = 0;
	bool check = 0;

	db.prepareStatement("SELECT * FROM passenger p JOIN user u ON p.userID = u.userID JOIN wallet w ON p.passengerID = w.passengerID WHERE p.passengerID = '" + id + "'");
	db.QueryResult();

	db.result->next();
	balance = db.result->getDouble("balance");
	walletid = db.result->getString("walletID");

	while (!check)
	{
		system("cls");
		cout << CYAN << "\n\t\t\t\t\t\t\t--------Total Wallet Balance--------\n" << RESET;
		cout << "\n\t\t\t\t\t\t\t\t\tRM " << right << fixed << setprecision(2) << balance << endl;

		cout << BLUE << "\n\t\t\t\t\t\t\tDo you want to top up? (Y/N): " << RESET;
		if (cin.peek() == '\n') {
			cin.ignore(); // consume the newline
			cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter Y or N only.\n" << RESET;
			system("pause");
			continue;
		}
		cin >> choice;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (choice == 'N' || choice == 'n') {
			if (returnToPayment) {
				cout << "\t\t\t\t\t\t\tReturning to payment...\n";
				system("pause");
				return;
			}
			else {
				profile();
				return;
			}
		}
		else if (choice != 'Y' && choice != 'y') {
			cout << RED << "\n\t\t\t\t\t\t\tInvalid input. Please enter Y or N.\n" << RESET;
			system("pause");
			continue;
		}

		while (true)
		{
			system("cls");
			cout << CYAN << "\n\t\t\t\t\t\t\t--------Total Wallet Balance--------\n" << RESET;
			cout << "\n\t\t\t\t\t\t\t\t\tRM " << right << fixed << setprecision(2) << balance << endl;
			cout << "\n\t\t\t\t\t\t\tEnter the amount you want to top up\n\n";
			cout << "\t\t\t\t\t\t\t1. RM 10\n\n\t\t\t\t\t\t\t2. RM 20\n\n\t\t\t\t\t\t\t3. RM 50\n\n\t\t\t\t\t\t\t4. Other amount\n\n";
			cout << GREEN << "\t\t\t\t\t\t\tEnter your choice: " << RESET;

			if (cin.peek() == '\n') {
				cin.ignore(); // consume the newline
				cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter number between 1 - 4.\n" << RESET;
				system("pause");
				continue;
			}

			if (!(cin >> option)) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << RED << "\n\t\t\t\t\t\t\tInvalid input. Please enter a number.\n" << RESET;
				system("pause");
				continue;
			}
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			switch (option)
			{
			case 1: amount = 10; break;
			case 2: amount = 20; break;
			case 3: amount = 50; break;
			case 4:
				cout << "\n\t\t\t\t\t\t\tEnter custom amount: RM ";

				if (!(cin >> amount) || amount <= 0) {
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << RED << "\t\t\t\t\t\tInvalid amount. Please enter a number.\n" << RESET;
					system("pause");
					continue;
				}
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				break;
			default:
				cout << RED << "\t\t\t\t\t\tInvalid choice. Please enter number between 1 - 4.\n" << RESET;
				system("pause");
				continue;
			}
			break;
		}
		check = 1;
	}
	db.prepareStatement("UPDATE wallet SET balance = balance + ? WHERE walletID = ?");
	db.statement->setDouble(1, amount);
	db.statement->setString(2, walletid);
	db.QueryStatement();

	cout << GREEN << "\t\t\t\t\t\t\tTop Up Successful!\n" << RESET;
	balance += amount;
	system("pause");

	if (returnToPayment) {
		char continuePay, ch;
		while (true)
		{
			cout << BLUE << "\n\t\t\t\t\t\tDo you want to continue payment? (Y/N): " << RESET;
			if (cin.peek() == '\n') {
				cin.ignore(); // consume the newline
				cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter Y or N only.\n" << RESET;
				system("pause");
				continue;
			}

			cin >> continuePay;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (continuePay == 'Y' || continuePay == 'y')
				return;
			else if (continuePay == 'N' || continuePay == 'n')
			{
				while (true)
				{
					cout << "\t\t\t\t\t\tYour payment will be cancelled. \n\t\t\t\t\t\tDo you sure want to cancel payment? (Y/N): ";
					if (cin.peek() == '\n') {
						cin.ignore(); // consume the newline
						cout << RED << "\n\t\t\t\t\t\tInput cannot be empty! Please enter Y or N only.\n" << RESET;
						system("pause");
						continue;
					}

					cin >> ch;
					cin.ignore(numeric_limits<streamsize>::max(), '\n');


					if (ch == 'Y' || ch == 'y')
						profile();
					else if (ch == 'N' || ch == 'n')
						return;
					else
					{
						cout << RED << "\t\t\t\t\t\tInvalid input. Please enter 'Y' or 'N' only.\n\n" << RESET;
						continue;
					}
				}
			}
			else
			{
				cout << RED << "\t\t\t\t\t\tInvalid input. Please enter 'Y' or 'N' only.\n\n" << RESET;
				continue;
			}
		}
	}
	else {
		profile();
	}
	return;
}