#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>

struct Task {
    int id;
    int TaskTime;
    bool awaitingStatus;
    std::string task;
    std::string deadline;
};

class Database {
    private: 
        sqlite3* db;
    public:
        Database(std::string& dbName);
        ~Database();

        void CreateTable();

        bool AddTask(int userID, std::string& task, int TaskTime, std::string& deadline);

        bool UpdateTask(int userID, int taskID, std::string newTask, std::string newDeadline);

        std::vector<Task> ShowActiveTasks(int userID);

        bool DeleteTask(int taskID, int chatID);
};