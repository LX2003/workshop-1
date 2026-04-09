#ifndef STAFF_H
#define STAFF_H

class Staff
{
	public:
		std::string staff_id;
		Staff() {};
		~Staff() {};
		void manageSchedule();
		void addSchedule();
		void updateSchedule();
		void deleteSchedule();
		void cancelTicket();
		void managePassenger();
};
#endif
