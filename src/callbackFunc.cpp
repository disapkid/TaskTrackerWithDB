#include "callbackFunc.h"

void CallBackFunc(TgBot::Bot& bot, Database& botDB) {
    bot.getEvents().onCallbackQuery([&bot, &botDB](TgBot::CallbackQuery::Ptr query) {
        std::string data = query->data;
        std::string response;

        if(data == "sendPicButton") {
            SendPic(bot, query);
        } else if(data == "doSmthng") {
            DoSomething(bot,query);
        } else if(data == "goback") {
            Menu(bot,query->message);
        } else if(data == "addNewTask") {
            NewTask(bot,query,botDB); 
        } else if(data == "activeTasks") {
            ActiveTasks(bot,query,botDB);
        } else {
            response = "Unknown button";
        }
    });
}