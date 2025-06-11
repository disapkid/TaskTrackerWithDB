#include "helpFunctions.h"

void readMessage(TgBot::Bot& bot, Database& botDB) {
    bot.getEvents().onNonCommandMessage([&bot, &botDB](TgBot::Message::Ptr message) {
        int64_t chatID = message->chat->id;
        std::string currMessage = message->text;

        if(botDB.getUTC(chatID) == 888) {
            if(checkUTC(currMessage)) {
                botDB.setUTC(chatID, std::stoi(currMessage));
                bot.getApi().sendMessage(chatID, "Часовой пояс успешно выставлен!");
                Menu(bot, message);
            }
            else bot.getApi().sendMessage(message->chat->id, "Неверный формат, попробуйте снова");
        }

        std::vector<Task> currTasks = botDB.ShowActiveTasks(chatID);
        if (currTasks.empty()) return;

        if (currTasks.back().waitingTask) {
            botDB.UpdateTask(chatID, currTasks.back().id, currMessage); 
            bot.getApi().sendMessage(chatID, "Введите время дедлайна в виде ЧЧ ММ (например 20 31) ");
            return;
        }

        if (!currTasks.back().waitingTask && currTasks.back().waitingDeadline) {
            if(!checkTime(currMessage,message,bot,botDB)) {
                bot.getApi().sendMessage(message->chat->id, "Неверный ввод дедлайна, попробуйте снова");
                return;
            }
            botDB.UpdateDeadline(chatID, currTasks.back().id, currMessage); 
            bot.getApi().sendMessage(chatID, "Задача успешно добавлена!");
            Menu(bot, message);
            return;
        }
        
    });
}

bool checkTime(const std::string& deadline, TgBot::Message::Ptr& message, TgBot::Bot& bot, Database& botDB) {
    std::tm tm = {};
    std::istringstream ss(deadline);
    ss >> std::get_time(&tm, "%H %M");
    
    if (ss.fail()) {
        bot.getApi().sendMessage(message->chat->id, "⏳ Неверный формат. Используйте ЧЧ ММ (например: 14 30)");
        return false;
    }

    if (tm.tm_hour < 0 || tm.tm_hour > 23 || tm.tm_min < 0 || tm.tm_min > 59) {
        bot.getApi().sendMessage(message->chat->id, "❌ Недопустимое время. Часы: 0-23, минуты: 0-59");
        return false;
    }

    auto now = std::chrono::system_clock::now();
    time_t now_utc = std::chrono::system_clock::to_time_t(now);
    
    int user_utc_offset = botDB.getUTC(message->chat->id);
    
    time_t now_user = now_utc + user_utc_offset * 3600;
    std::tm* user_tm = std::gmtime(&now_user); 
    
    std::tm deadline_tm = *user_tm;
    deadline_tm.tm_hour = tm.tm_hour;
    deadline_tm.tm_min = tm.tm_min;
    deadline_tm.tm_sec = 0;
    
    time_t deadline_utc = timegm(&deadline_tm) - user_utc_offset * 3600;
    
    if (deadline_utc < now_utc) {
        bot.getApi().sendMessage(message->chat->id, "⏰ Это время уже прошло сегодня! Введите будущее время.");
        return false;
    }

    if ((deadline_utc - now_utc) < 600) {
        bot.getApi().sendMessage(message->chat->id, "⏱ Дедлайн должен быть минимум на 10 минут позже текущего времени");
        return false;
    }

    return true;
}

bool checkUTC(const std::string& UTC) {
    if (UTC.empty()) return false;

    try {
        int offset = std::stoi(UTC);
        return offset >= -12 && offset <= 14;
    } catch (...) {
        return false;
    }
}

void checkDeadline(int chatID, TgBot::Bot& bot, Database& botDB) {
    auto taskInfo = botDB.ClosestTask(chatID);
    std::pair<std::string, std::pair<std::string, int>> empty = {"", {"", 0}};

    if(taskInfo == empty) return;

    std::string taskText = std::string("До задачи осталось 10 минут! \n") + "Задача: " + taskInfo.first;
    std::string deadlineStr = taskInfo.second.first;
    
    std::tm tm = {};
    std::istringstream ss(deadlineStr);
    ss >> std::get_time(&tm, "%H %M");
    
    auto now = std::chrono::system_clock::now();
    time_t now_tt = std::chrono::system_clock::to_time_t(now);
    
    int user_utc = botDB.getUTC(chatID);
    tm.tm_hour -= user_utc;  
    
    time_t deadline_tt = std::mktime(&tm);
    double diff_seconds = difftime(deadline_tt, now_tt);

    if(diff_seconds <= 600 and diff_seconds > 0) {
        bot.getApi().sendMessage(chatID, taskText);
        botDB.UpdateTaskStatus(chatID, taskInfo.second.second);
    }

    if(diff_seconds <= 0) {
        bot.getApi().sendMessage(chatID, "Следующая задача будет автоматически удалена по причине истечения дедлайна: \n" + taskInfo.first);
        botDB.DeleteTask(taskInfo.second.second, chatID);
    }
}