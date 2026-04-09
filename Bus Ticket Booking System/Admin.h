#ifndef ADMIN_H
#define ADMIN_H
struct TrendData {
	std::string label; // Bus ID
	int count;         // Usage count
};
class Admin
{
	public:
		std::string a_id;
		Admin() {};
		~Admin() {};
		void manageStaff();
		void addStaff();
		void updateStaff();
		void deleteStaff();
		void manageBus();
		void addBus();
		void deleteBus();
		void refundApp();
		void generateReport();
		void revenueReport();
		void busUsageReport();
		void mostScheduleReport();
};
#endif
