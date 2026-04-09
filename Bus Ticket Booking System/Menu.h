#include "Passenger.h"
#include "Staff.h"
#include "Admin.h"
#ifndef MENU_H
#define MENU_H
class Menu
{
	public:
		Menu() {};
		~Menu() {};
		void HomeMenu();
		void AdminMenu(Admin admin);
		void StaffMenu(Staff staff);
		void CustMenu(Passenger passenger);
};
#endif

