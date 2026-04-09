#include <iostream>
#include <mysql/jdbc.h>
#include "MySQL.h"
#include "Login.h"
#include "Menu.h"
using namespace std;

int main()
{
	Login login;
	Menu menu;

	menu.HomeMenu();
	
	return 0;
}