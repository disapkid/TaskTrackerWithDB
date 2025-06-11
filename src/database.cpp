#include "database.h"
#include <iostream>

Database::Database(const std::string& dbName) : db(nullptr) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

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
            "waitingTask BOOLEAN,"
            "waitingDeadline BOOLEAN,"
            "isReminded BOOLEAN,"
            "deadline TEXT);";
    char* errMsg = nullptr;
    if(sqlite3_exec(db,table,nullptr,nullptr,&errMsg) != SQLITE_OK) {
        std::cerr<<" SQL Error: "<< errMsg << std::endl;
        sqlite3_free(errMsg);
    };

    const char* utc = "CREATE TABLE IF NOT EXISTS utc("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "chatID INTEGER,"
                      "userUTC INTEGER);";
    char* secondErrMsg = nullptr;
    if(sqlite3_exec(db,utc,nullptr,nullptr,&secondErrMsg) != SQLITE_OK) {
        std::cerr<<" SQL Error: "<< errMsg << std::endl;
        sqlite3_free(errMsg);
    };
}

bool Database::AddTask(int chatID, const std::string& task, int taskTime, const  std::string& deadline) {
    const char* sqlRequest = "INSERT INTO tasks (chatID, task, taskTime, waitingTask, waitingDeadline," 
                             "isReminded, deadline) VALUES (?, ?, ?, 1, 1, 0, ?);";
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

bool Database::addUTC(int userID) {
    const char* SQLRequest = "INSERT INTO utc (chatID, userUTC) VALUES (?,999);";

    sqlite3_stmt* statement = nullptr;
    if(sqlite3_prepare_v2(db,SQLRequest, -1, &statement, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(statement,1,userID);

    bool success = (sqlite3_step(statement) == SQLITE_DONE);
    sqlite3_finalize(statement);
    return success;
}

bool Database::setUTC(int userID, int UTC) {

    // 888 : waiting for UTC
    // 999 : default 

    const char* SQLRequest = "UPDATE utc SET userUTC = ? WHERE chatID = ?;";
    sqlite3_stmt* statement = nullptr;
    if(sqlite3_prepare_v2(db, SQLRequest, -1, &statement, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(statement, 1, UTC);
    sqlite3_bind_int(statement, 2, userID);

    bool success = (sqlite3_step(statement) == SQLITE_DONE);
    sqlite3_finalize(statement);
    return success;
}

int Database::getUTC(int userID) {
    const char* SQLRequest = "SELECT userUTC FROM utc WHERE chatID = ?;";
    sqlite3_stmt* statement = nullptr;

    int result = 0;
    if(sqlite3_prepare_v2(db, SQLRequest, -1, &statement, nullptr) != SQLITE_OK) return 999;

    sqlite3_bind_int(statement, 1, userID);

    if(sqlite3_step(statement) == SQLITE_ROW) {
        result = sqlite3_column_int(statement,0);
    }

    sqlite3_finalize(statement);
    return result;
}

bool Database::UpdateTask(int userID, int taskID, const std::string& newTask) {
    const char* SQLREquest = "UPDATE tasks SET task = ?, waitingTask = 0 WHERE chatID = ? AND id = ?";
    sqlite3_stmt* statement = nullptr;

    if(sqlite3_prepare_v2(db, SQLREquest, -1, &statement, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(statement, 1, newTask.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 2, userID);
    sqlite3_bind_int(statement, 3, taskID);

    bool success = (sqlite3_step(statement) == SQLITE_DONE);
    sqlite3_finalize(statement);

    return success;
}

bool Database::UpdateDeadline(int userID, int taskID, const std::string& newDeadline) {
    const char* SQLRequest = "UPDATE tasks SET deadline = ?, waitingDeadline = 0 WHERE chatID = ? AND id = ?";
    sqlite3_stmt* statement = nullptr;


    if(sqlite3_prepare_v2(db, SQLRequest, -1, &statement, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(statement, 1, newDeadline.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 2 , userID);
    sqlite3_bind_int(statement, 3 , taskID);

    bool success = (sqlite3_step(statement) == SQLITE_DONE);
    sqlite3_finalize(statement);
    return success;
}

std::vector<Task> Database::ShowActiveTasks(int chatID) {
    std::vector<Task> allTasks;
    const char* SQLRequest = "SELECT id, taskTime, task, waitingTask," 
                             "waitingDeadline, deadline FROM tasks WHERE chatID = ?; ";
    sqlite3_stmt* statement = nullptr;

    if(sqlite3_prepare_v2(db,SQLRequest, -1, &statement, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(statement,1,chatID);

        while(sqlite3_step(statement) == SQLITE_ROW) {
            Task temp;
            temp.id = sqlite3_column_int(statement,0);
            temp.taskTime = sqlite3_column_int(statement,1);
            temp.task = reinterpret_cast<const char*>(sqlite3_column_text(statement,2));
            temp.waitingTask = sqlite3_column_int(statement,3);
            temp.waitingDeadline = sqlite3_column_int(statement, 4);
            temp.deadline = reinterpret_cast<const char*>(sqlite3_column_text(statement,5));

            allTasks.push_back(temp);
        }
    }

    sqlite3_finalize(statement);
    return allTasks;
}

Task Database::ShowTasksText(int userID, int taskID) {
    Task result;
    const char* SQLRequest = "SELECT taskTime, task, waitingTask, waitingDeadline, deadline FROM tasks WHERE chatID = ? AND id = ?; ";
    sqlite3_stmt* statement = nullptr;

    if(sqlite3_prepare_v2(db, SQLRequest, -1, &statement, nullptr) != SQLITE_OK) return result;

    sqlite3_bind_int(statement, 1, userID);
    sqlite3_bind_int(statement, 2, taskID);

    if(sqlite3_step(statement) == SQLITE_ROW) {
        result.taskTime = sqlite3_column_int(statement,0);
        result.task = reinterpret_cast<const char*>(sqlite3_column_text(statement, 1));
        result.waitingTask = sqlite3_column_int(statement, 2);
        result.waitingDeadline = sqlite3_column_int(statement,3);
        result.deadline = reinterpret_cast<const char*>(sqlite3_column_text(statement,4));
        result.id = taskID;
    }

    sqlite3_finalize(statement);
    return result;
}

std::pair<std::string, std::pair<std::string, int>> Database::ClosestTask(int userID)
{
    // <task text, <deadline, task id>>
    const char* SQLRequest = "SELECT task, deadline, id FROM tasks "
                             "WHERE chatID = ? AND isReminded = 0 "
                             "ORDER BY deadline ASC "
                             "LIMIT 1";
    sqlite3_stmt* statement = nullptr;

    std::pair<std::string, std::pair<std::string, int>> result = {"", {"", 0}};

    if (sqlite3_prepare_v2(db, SQLRequest, -1, &statement, nullptr) != SQLITE_OK)
        return result;

    sqlite3_bind_int(statement, 1, userID);

    if (sqlite3_step(statement) == SQLITE_ROW) {
        const unsigned char* task_text = sqlite3_column_text(statement, 0);
        const unsigned char* deadline_text = sqlite3_column_text(statement, 1);
        result.first = task_text ? reinterpret_cast<const char*>(task_text) : "";
        result.second.first = deadline_text ? reinterpret_cast<const char*>(deadline_text) : "";
        result.second.second = sqlite3_column_int(statement, 2);
    }

    sqlite3_finalize(statement);

    return result;
}

std::vector<int64_t> Database::UsersID() {
    const char* SQLRequest = "SELECT DISTINCT chatID FROM tasks";
    sqlite3_stmt* statement = nullptr;

    std::vector<int64_t> result;

    if(sqlite3_prepare_v2(db, SQLRequest, -1, &statement, nullptr) != SQLITE_OK) { return result; }

    while(sqlite3_step(statement) == SQLITE_ROW) {
        result.emplace_back(sqlite3_column_int64(statement,0));
    }
    sqlite3_finalize(statement);

    return result;
}

bool Database::UpdateTaskStatus(int userID, int taskID) {
    const char* SQLRequest = "UPDATE tasks SET isReminded = 1 WHERE chatID = ? AND ID = ?";
    sqlite3_stmt* statement = nullptr;

    if(sqlite3_prepare_v2(db, SQLRequest, -1, &statement, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_int(statement, 1, userID);
    sqlite3_bind_int(statement, 2, taskID);

    bool success = (sqlite3_step(statement) == SQLITE_DONE);
    sqlite3_finalize(statement);

    return success;
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