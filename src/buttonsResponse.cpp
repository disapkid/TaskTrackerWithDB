#include "buttonsResponse.h"

void SendPic(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query) {
    TgBot::InlineKeyboardMarkup::Ptr board(new TgBot::InlineKeyboardMarkup);
    TgBot::InlineKeyboardButton::Ptr back(new TgBot::InlineKeyboardButton);

    back->text = "back";
    back->callbackData = "goback";

    board->inlineKeyboard.push_back({back});

    bot.getApi().sendPhoto(query->message->chat->id, 
        TgBot::InputFile::fromFile("/home/litie/visual studio/my_bot/photo/anime.jpg", "image/jpeg"),
        "", 0, board);
}

void DoSomething(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query) {
    TgBot::InlineKeyboardMarkup::Ptr board(new TgBot::InlineKeyboardMarkup);
    TgBot::InlineKeyboardButton::Ptr back(new TgBot::InlineKeyboardButton);

    back->text = "back";
    back->callbackData = "goback";

    board->inlineKeyboard.push_back({back});

    bot.getApi().sendMessage(query->message->chat->id, "Something is happened", nullptr ,0, board);
}

void ActiveTasks(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB) {
    TgBot::InlineKeyboardMarkup::Ptr board(new TgBot::InlineKeyboardMarkup);
    std::vector<TgBot::InlineKeyboardButton::Ptr> buttons;

    std::vector<Task> temp;
    int counter = 0;
    for(auto& [id, taskTime, status, task, deadline] : temp) {
        TgBot::InlineKeyboardButton::Ptr currentTask(new TgBot::InlineKeyboardButton);
        currentTask->text = deadline;
        currentTask->callbackData = "taskNumber" + std::to_string(id);
        counter++;

        buttons.push_back(currentTask);
        if(counter !=0 and counter % 3 == 0) {
            board->inlineKeyboard.push_back(buttons);
            buttons.clear();
        }
    };

    
    if(!buttons.empty()) board->inlineKeyboard.push_back(buttons);
    bot.getApi().sendMessage(query->message->chat->id, "Current active tasks: ", nullptr, 0, board);
}

void PetStatus() {

}

void NewTask(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB) {
    std::vector<Task> temp = botDB.ShowActiveTasks(query->message->chat->id);
    if(!temp.empty() and temp.back().awaitingStatus == true) {
        bot.getApi().sendMessage(query->message->chat->id, "Can't add new task, until previous is awaiting");
        return;
    }
    std::string task = "waiting";
    std::string deadline = "waiting";
    botDB.AddTask(query->message->chat->id, task, 0, deadline);

    bot.getApi().sendMessage(query->message->chat->id, "Enter new task: ");
}