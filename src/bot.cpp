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

void assembled(TgBot::Bot& bot, Database& botDB) {
    start(bot);
    help(bot);
    CallBackFunc(bot, botDB);
    readMessage(bot, botDB);
}