#include "bot.h"


void start(TgBot::Bot& bot) {
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hello, this is test message");
        Menu(bot,message);
    });
}

void help(TgBot::Bot& bot) {
    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {    
        bot.getApi().sendMessage(message->chat->id, "This is test help message");
    });
}

void readTask(TgBot::Bot& bot, Database& botDB) {
    bot.getEvents().onNonCommandMessage([&bot, &botDB](TgBot::Message::Ptr message) {
        auto ChatID = message->chat->id;
        std::string currMessage = message->text;
        int taskSendTime = message->date;

        std::vector<Task> currTasks = botDB.ShowActiveTasks(message->chat->id);
        if(currTasks.back().awaitingStatus == true) {
            botDB.UpdateTask(ChatID, currTasks.back().id, currMessage, "18:30" );
        } else return;
    });
}

void assembled(TgBot::Bot& bot, Database& botDB) {
    start(bot);
    help(bot);
    CallBackFunc(bot, botDB);
    readTask(bot, botDB);
}