#pragma once
#include <mysql/jdbc.h>

class MySQL
{
	public:
		MySQL();
		~MySQL();

		void prepareStatement(std::string query);
		sql::PreparedStatement* statement;
		sql::ResultSet* result;
		void QueryStatement();
		void QueryResult();

	private:
		sql::Connection* connection;
};

