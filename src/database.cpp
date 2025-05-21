#include "database.h"
#include <iostream>

Database::Database(std::string& dbName) : db(nullptr) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
};

Database::~Database() {
    if(db) {
        sqlite3_close(db);
    }
}

void Database::CreateTable() {
    const char* table = 
            "CREATE TABLE IF NOT EXISTS tasks("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "chatID INTEGER,"
            "task TEXT,"
            "taskTime INTEGER,"
            "awaitingStatus BOOLEAN,"
            "deadline TEXT);";
    char* errMsg = nullptr;
    if(sqlite3_exec(db,table,nullptr,nullptr,&errMsg) != SQLITE_OK) {
        std::cerr<<" SQL Error: "<< errMsg << std::endl;
        sqlite3_free(errMsg);
    };
}

bool Database::AddTask(int chatID, std::string& task, int taskTime, std::string& deadline) {
    const char* sqlRequest = "INSERT INTO tasks (chatID, task, taskTime, awaitingStatus, deadline) VALUES (?, ?, ?, 1, ?);";
    sqlite3_stmt* statement = nullptr;
    if(sqlite3_prepare_v2(db, sqlRequest, -1, &statement, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(statement, 1, chatID);
    sqlite3_bind_text(statement, 2, task.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 3, taskTime);
    sqlite3_bind_text(statement, 4, deadline.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(statement) == SQLITE_DONE);
    sqlite3_finalize(statement);
    return success;
}

bool Database::UpdateTask(int userID, int taskID, std::string newTask, std::string newDeadline) {
    const char* SQLREquest = "UPDATE tasks SET task = ?, deadline = ?, awaitingStatus = 0 WHERE chatID = ? AND id = ?";
    sqlite3_stmt* statement = nullptr;

    if(sqlite3_prepare_v2(db, SQLREquest, -1, &statement, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(statement, 1, newTask.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, newDeadline.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 3, userID);
    sqlite3_bind_int(statement, 4, taskID);

    bool success = (sqlite3_step(statement) == SQLITE_DONE);
    sqlite3_finalize(statement);

    return success;
}

std::vector<Task> Database::ShowActiveTasks(int chatID) {
    std::vector<Task> allTasks;
    const char* SQLRequest = "SELECT id, taskTime, task, awaitingStatus, deadline FROM tasks WHERE chatID = ?; ";
    sqlite3_stmt* statement = nullptr;

    if(sqlite3_prepare_v2(db,SQLRequest, -1, &statement, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(statement,1,chatID);

        while(sqlite3_step(statement) == SQLITE_ROW) {
            Task temp;
            temp.id = sqlite3_column_int(statement,0);
            temp.TaskTime = sqlite3_column_int(statement,1);
            temp.task = reinterpret_cast<const char*>(sqlite3_column_text(statement,2));
            temp.awaitingStatus = sqlite3_column_int(statement,3);
            temp.deadline = reinterpret_cast<const char*>(sqlite3_column_text(statement,4));

            allTasks.push_back(temp);
        }
    }

    sqlite3_finalize(statement);
    return allTasks;
}

bool Database::DeleteTask(int taskID, int chatID) {
    const char* SQLRequest = "DELETE FROM tasks WHERE id = ? AND chatID = ?;";
    sqlite3_stmt* statement = nullptr;
    if(sqlite3_prepare_v2(db, SQLRequest, -1, &statement, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(statement,1, taskID);
    sqlite3_bind_int(statement,2, chatID);

    bool success = (sqlite3_step(statement) == SQLITE_DONE);
    sqlite3_finalize(statement);
    return success;
}