#include "helpFunctions.h"

void readMessage(TgBot::Bot& bot, Database& botDB) {
    bot.getEvents().onNonCommandMessage([&bot, &botDB](TgBot::Message::Ptr message) {
        int64_t chatID = message->chat->id;
        std::string currMessage = message->text;

        std::vector<Task> currTasks = botDB.ShowActiveTasks(chatID);
        if (currTasks.empty()) return;

        if (currTasks.back().waitingTask) {
            botDB.UpdateTask(chatID, currTasks.back().id, currMessage); 
            bot.getApi().sendMessage(chatID, "Enter deadline in format hh:mm");
            return;
        }

        if (!currTasks.back().waitingTask && currTasks.back().waitingDeadline) {
            if(!checkTime(currMessage,message,bot,botDB)) {
                bot.getApi().sendMessage(message->chat->id, "Incorrect deadline input, please try again");
                return;
            }
            botDB.UpdateDeadline(chatID, currTasks.back().id, currMessage); 
            bot.getApi().sendMessage(chatID, "Task successfully added!");
            Menu(bot, message);
            return;
        }

        if(botDB.getUTC(chatID) == 20) {
            if(checkUTC(currMessage)) {
                botDB.setUTC(chatID, std::stoi(currMessage));
                bot.getApi().sendMessage(chatID, "Time zone successfully set");
                Menu(bot, message);
            }
            else bot.getApi().sendMessage(message->chat->id, "Incorrect input, please try again");
        }
        
    });
}

bool checkTime(const std::string& deadline, TgBot::Message::Ptr& message, TgBot::Bot& bot,Database& botBD) {
    if(deadline.find(' ')==std::string::npos) return false;
    
    int countSpaces = 0;
    for(const auto& c : deadline) {
        if(c!= ' ') {
            if(!isdigit(c)) return false;
        }
        else countSpaces++;
    }
    if(countSpaces>1) return false;

    int hours = std::stoi(deadline.substr(0, deadline.find(' ')));
    int min = std::stoi(deadline.substr(deadline.find(' ') + 1));

    std::time_t usersTime = parseTime(deadline) - 3600 * botBD.getUTC(message->chat->id);
    if(abs(usersTime - message->date) < 600) {
        bot.getApi().sendMessage(message->chat->id, "Нельзя выставить дедлайн раньше 10ти минут");
        return false;
    }

    if(hours > 23 or min > 59 ) return false;
    return true;
}

bool checkUTC(const std::string& UTC) {
    if (UTC.empty()) return false;

    if (UTC.front() == '+' or UTC.front() == '-') {
        if (UTC.size() == 1) return false;
        for (size_t i = 1; i < UTC.size(); ++i) {
            if (!isdigit(UTC[i])) return false;
        }
    } else {
        for (char c : UTC) {
            if (!isdigit(c)) return false;
        }
    }

    try {
        int res = std::stoi(UTC);
        return (res >= -12 and res <= 14);
    } catch (...) {
        return false;
    }
}

std::time_t parseTime(const std::string& timeStr) {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%H %M");

    std::time_t now = std::time(nullptr);
    std::tm nowTime = *std::localtime(&now);

    nowTime.tm_hour = tm.tm_hour;
    nowTime.tm_min  = tm.tm_min;
    nowTime.tm_sec  = 0;

    std::time_t timestamp = std::mktime(&nowTime);

    return timestamp;
}