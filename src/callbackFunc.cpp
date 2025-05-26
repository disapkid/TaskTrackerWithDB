#include "callbackFunc.h"

void CallBackFunc(TgBot::Bot& bot, Database& botDB) {
    bot.getEvents().onCallbackQuery([&bot, &botDB](TgBot::CallbackQuery::Ptr query) {
        std::string data = query->data;
        std::string response;

        if(data == "setUTC") {
            SetUsersUTC(bot, query, botDB);
        } else if(data == "addNewTask") {
            NewTask(bot,query,botDB); 
        } else if(data == "activeTasks") {
            ActiveTasks(bot,query,botDB);
        } else if(data.find("taskNumber:") != std::string::npos) {
            int id = std::stoi(data.substr(std::string("taskNumber:").size()));
            TaskNumber(bot, query, id, botDB);
        } else if(data.find("deleteTask:") != std::string::npos) {
            int id = std::stoi(data.substr(std::string("deleteTask:").size()));
            DeleteCurrentTask(bot, query, id, botDB );
        } else {
            response = "Unknown button";
        }
    });
}