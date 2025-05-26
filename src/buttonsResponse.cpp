#include "buttonsResponse.h"

void SetUsersUTC(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB) {
    bot.getApi().sendMessage(query->message->chat->id, "Enter your time zone: ");
    botDB.setUTC(query->message->chat->id, 20);
}

void TaskNumber(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, int ID, Database& botDB) {
    TgBot::InlineKeyboardMarkup::Ptr board(new TgBot::InlineKeyboardMarkup);
    TgBot::InlineKeyboardButton::Ptr deleteTask(new TgBot::InlineKeyboardButton);

    deleteTask->text = "Mark as achieved and delete task";
    deleteTask->callbackData = "deleteTask:" + std::to_string(ID);

    Task current = botDB.ShowTasksText(query->message->chat->id, ID);
    board->inlineKeyboard.push_back({deleteTask});
    bot.getApi().sendMessage(query->message->chat->id, current.task, nullptr, 0, board);
}

void ActiveTasks(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB) {
    TgBot::InlineKeyboardMarkup::Ptr board(new TgBot::InlineKeyboardMarkup);
    std::vector<TgBot::InlineKeyboardButton::Ptr> buttons;

    std::vector<Task> temp = botDB.ShowActiveTasks(query->message->chat->id);
    int counter = 0;
    
    for(auto& [id, taskTime, taskStatus, deadlineStatus, task, deadline] : temp) {
        counter++;

        TgBot::InlineKeyboardButton::Ptr currentTask(new TgBot::InlineKeyboardButton);

        std::string correctedDeadline = deadline;
        std::replace(correctedDeadline.begin(), correctedDeadline.end(), ' ', ':');

        currentTask->text = std::to_string(counter) + ")" + " " + correctedDeadline;
        currentTask->callbackData = "taskNumber:" + std::to_string(id);

        buttons.push_back(currentTask);
        if(counter !=0 and counter % 3 == 0) {
            board->inlineKeyboard.push_back(buttons);
            buttons.clear();
        }
    };

    
    if(!buttons.empty()) board->inlineKeyboard.push_back(buttons);
    bot.getApi().sendMessage(query->message->chat->id, "Current active tasks: ", nullptr, 0, board);
}

void DeleteCurrentTask(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, int ID, Database& botDB) {
    botDB.DeleteTask(ID, query->message->chat->id);
    bot.getApi().sendMessage(query->message->chat->id, "Task succesfully deleted!");
    Menu(bot, query->message);
}

void NewTask(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB) {
    std::vector<Task> temp = botDB.ShowActiveTasks(query->message->chat->id);

    if(botDB.getUTC(query->message->chat->id) == -404 or botDB.getUTC(query->message->chat->id) == 404 ) {
        bot.getApi().sendMessage(query->message->chat->id, "You didn't enter your time zone");
        Menu(bot, query->message);
        return;
    }

    if(!temp.empty()) {
        if(temp.back().waitingTask == true or temp.back().waitingDeadline == true) {
            bot.getApi().sendMessage(query->message->chat->id, "Can't add new task, until previous is awaiting");
            return;
        }
    }
    std::string task = "WaitingTask";
    std::string deadline = "WaitingDeadline";
    botDB.AddTask(query->message->chat->id, task, query->message->date, deadline);

    bot.getApi().sendMessage(query->message->chat->id, "Enter new task: ");
}