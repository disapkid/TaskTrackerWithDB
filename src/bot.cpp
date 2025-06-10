#include "bot.h"


void start(TgBot::Bot& bot, Database& botDB) {
    bot.getEvents().onCommand("start", [&bot, &botDB](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет, давай поставим на сегодня несколько задач!");
        botDB.addUTC(message->chat->id);
        Menu(bot,message);
    });
}

void help(TgBot::Bot& bot) {
    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {    
        std::string helpMessage = "Бот имеет несколько функций : \n"
                              "Set UTC - выставление часового пояса в формате UTC \n"
                              "Add Task - добавить новую задачу \n"
                              "Active Tasks - просмотреть список активных задач (выводится только дедлайн задач) \n"
                              "Перед добавлением новой задачи необходимо выставить свой часовой пояс для корректной работы уведомлений \n"
                              "Чтобы отметить задачу выполненной, необходимо нажать на нужную задачу и отметить ее";
        bot.getApi().sendMessage(message->chat->id, helpMessage);
    });
}

void assembled(TgBot::Bot& bot, Database& botDB) {
    start(bot, botDB);
    help(bot);
    CallBackFunc(bot, botDB);
    readMessage(bot, botDB);
}