#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>

struct Task {
    int id;
    int taskTime;
    bool waitingTask;
    bool waitingDeadline;
    std::string task;
    std::string deadline;
};

class Database {
    private: 
        sqlite3* db;
    public:
        Database(const std::string& dbName);
        ~Database();

        void CreateTable();

        bool AddTask(int userID, const std::string& task, int TaskTime, const std::string& deadline);

        bool addUTC(int userID);

        bool setUTC(int userID, int UTC);

        int getUTC(int userID);

        bool UpdateTask(int userID, int taskID, const std::string& newTask);
        
        bool UpdateDeadline(int userID, int taskID, const std::string& newDeadline);

        std::vector<Task> ShowActiveTasks(int userID);

        Task ShowTasksText(int userID, int taskID);

        std::pair<std::string, std::pair<std::string, int>> ClosestTask(int userID);

        bool UpdateTaskStatus(int userID, int taskID);

        bool DeleteTask(int taskID, int chatID);
};