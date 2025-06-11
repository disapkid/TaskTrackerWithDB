#include "bot.h"
#include "database.h"
#include <chrono>
#include <thread>

void StartCheckingDeadline(TgBot::Bot& bot, Database& botDB) {
    while(true) {
        auto allUsers = botDB.UsersID();

        for(auto& id : allUsers) {
            checkDeadline(id,bot,botDB);
        }

        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

int main() {
    TgBot::Bot bot(" . . . ");

    std::string TGBotDB = "data.db";
    Database botDB(TGBotDB);
    botDB.CreateTable();

    std::thread deadlineThread(StartCheckingDeadline, std::ref(bot), std::ref(botDB));
    deadlineThread.detach();

	assembled(bot, botDB);
    try {
        bot.getApi().deleteWebhook(); 
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    
    return 0;
}
