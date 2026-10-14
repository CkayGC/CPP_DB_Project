#include "Database.h"
#include <iostream>

Database::Database() {
	db = nullptr;
}

bool Database::open(const std::string& filename) {
	int result = sqlite3_open(filename.c_str(), &db);

	if (result != SQLITE_OK) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	std::cout << "Opened database successfully" << std::endl;
	return true;
}

void Database::close() {
	if (db) {
		sqlite3_close(db);
		db = nullptr;
		std::cout << "Closed database successfully" << std::endl;
	}
}

Database::~Database() {
	close();
}

bool Database::execute(const std::string& sql) {
	char* errMsg = nullptr;

	int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

	if (result != SQLITE_OK) {
		std::cerr << "Error executing SQL: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		return false;
	}

	return true;
}

void Database::printAllUsers() {
    // 1. Updated query to match your exact columns
    std::string sql = "SELECT id, name, email, score FROM users;";
    sqlite3_stmt* stmt = nullptr;

    // 2. Compile the SQL statement
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "\n--- USER RECORDS ---" << std::endl;

    // 3. Loop through your table rows
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Extract Column 0 (id) as an Integer
        const int id = sqlite3_column_int(stmt, 0);

        // Extract Column 1 (name) as Text
        const unsigned char* rawName = sqlite3_column_text(stmt, 1);
        std::string name = rawName ? reinterpret_cast<const char*>(rawName) : "";

        // Extract Column 2 (email) as Text
        const unsigned char* rawEmail = sqlite3_column_text(stmt, 2);
        std::string email = rawEmail ? reinterpret_cast<const char*>(rawEmail) : "";

        // Extract Column 3 (score) as Int
        const int score = sqlite3_column_int(stmt, 3);
		std::string scoreStr = std::to_string(score);

        // 4. Print your columns cleanly to the console
        std::cout << "ID: " << id
            << " | Name: " << name
            << " | Email: " << email
            << " | Score: " << score << std::endl;
    }
    std::cout << "--------------------\n" << std::endl;

    // 5. Clean up statement memory
    sqlite3_finalize(stmt);
}

std::string Database::getUserDataByID(int targetUserID) {
    // 1. Updated query to match your exact columns
    std::string sql = "SELECT id, name, email, score FROM users WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;

    std::string data = "";

    // 2. Compile the SQL statement
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return data;
    }

    //std::cout << "\n--- USER DATA READ ---" << std::endl;

	// Bind the targetUserID parameter to the SQL statement
    sqlite3_bind_int(stmt, 1, targetUserID);
    sqlite3_step(stmt);

    // Extract Column 0 (id) as an Integer
    const int id = sqlite3_column_int(stmt, 0);
	data.append("ID: " + std::to_string(id) + " / ");

    // Extract Column 1 (name) as Text
    const unsigned char* rawName = sqlite3_column_text(stmt, 1);
    std::string name = rawName ? reinterpret_cast<const char*>(rawName) : "";
	data.append("Name: " + name + " / ");

    // Extract Column 2 (email) as Text
    const unsigned char* rawEmail = sqlite3_column_text(stmt, 2);
    std::string email = rawEmail ? reinterpret_cast<const char*>(rawEmail) : "";
	data.append("Email: " + email + " / ");

    // Extract Column 3 (score) as Int
    const int score = sqlite3_column_int(stmt, 3);
    std::string scoreStr = std::to_string(score);
	data.append("Score: " + scoreStr);

    // 4. Print your columns cleanly to the console
    /*std::cout << "ID: " << id
        << " | Name: " << name
        << " | Email: " << email
        << " | Score: " << score << std::endl;
    
    std::cout << "--------------------\n" << std::endl;*/

    // 5. Clean up statement memory
    sqlite3_finalize(stmt);

    return data;
}

std::vector<std::vector<std::string>> Database::getAllUserData() {
    // 1. Updated query to match your exact columns
    std::string sql = "SELECT id, name, email, score FROM users;";
    sqlite3_stmt* stmt = nullptr;

    std::vector<std::vector<std::string>> userDataArray;



    // 2. Compile the SQL statement
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return userDataArray;
    }

    // 3. Loop through your table rows
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> userEntryData;

        // Extract Column 0 (id) as an Integer
        const int id = sqlite3_column_int(stmt, 0);
        userEntryData.push_back(std::to_string(id));

        // Extract Column 1 (name) as Text
        const unsigned char* rawName = sqlite3_column_text(stmt, 1);
        std::string name = rawName ? reinterpret_cast<const char*>(rawName) : "";
        userEntryData.push_back(name);

        // Extract Column 2 (email) as Text
        const unsigned char* rawEmail = sqlite3_column_text(stmt, 2);
        std::string email = rawEmail ? reinterpret_cast<const char*>(rawEmail) : "";
        userEntryData.push_back(email);

        // Extract Column 3 (score) as Int
        const int score = sqlite3_column_int(stmt, 3);
        std::string scoreStr = std::to_string(score);
        userEntryData.push_back(scoreStr);

        // 4. Insert user into array
        userDataArray.push_back(userEntryData);
    }

   

    // 5. Clean up statement memory
    sqlite3_finalize(stmt);
    return userDataArray;
}

void Database::addUser(const std::vector<std::string>) {
    // 1. Updated query to match your exact columns
    std::string sql = "INSERT INTO users (name, email, score) VALUES "
                      "('Tobi', 'Tobi@example.com', 20),";

    sqlite3_stmt* stmt = nullptr;

    // 2. Compile the SQL statement
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    //Clean up the statement pointer to avoid memory leaks!
    sqlite3_finalize(stmt);
}

void Database::deleteUser(const int targetUserID) {
    // 1. Updated query to match your exact columns
    std::string sql = "DELETE FROM users WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;

    // 2. Compile the SQL statement
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind the userID parameter to the SQL statement
    sqlite3_bind_int(stmt, 1, targetUserID);

    // 3. FIX: Capture the return value of sqlite3_step!
    int stepResult = sqlite3_step(stmt);

    // 4. FIX: Check stepResult instead of result
    if (stepResult != SQLITE_DONE) {
        std::cerr << "Failed to delete user: " << sqlite3_errmsg(db) << "\n";
    }
    else {
        std::cout << "User with ID " << targetUserID << " successfully deleted.\n";
    }

    // 5. Clean up the statement pointer to avoid memory leaks!
    sqlite3_finalize(stmt);
}
