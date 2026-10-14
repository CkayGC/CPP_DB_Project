#pragma once
#include "sqlite3.h"
#include <string>
#include <vector>

class Database {
private:
	sqlite3* db;

public:
	Database ();
	~Database ();

	bool open(const std::string& filename);
	void close();
	bool execute(const std::string& sql);
	void printAllUsers();
	std::string getUserDataByID(int userID);
	std::vector<std::vector<std::string>> getAllUserData();

	void addUser(const std::vector<std::string>);
	void deleteUser(const int);
};