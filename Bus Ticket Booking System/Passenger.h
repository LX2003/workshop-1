#include <iostream>
#include <string>
using namespace std;

#ifndef PASSENGER_H
#define PASSENGER_H
class Passenger
{
	public:
		string id,username,password,phoneNo,email,name,gender, s_id;
		static const int rows = 7, column = 4;
		string seats[rows][column];
		int age;
		double balance = 0;

		Passenger() {};
		~Passenger() {};
		bool isValidDateFormat(const string&);
		bool isTodayOrFuture(const string&);
		void searchBus();
		void showSchedule(string&, string&, string&);
		void selectSeat();
		void updateSeat(string seats[][column], int, const string&);
		void payment(int, string[]);
		void showBooking();
		void profile();
		void setting();
		void editInfo();
		void changePass();
		void ReqDeleteAcc();
		void wallet(bool);
};
#endif
