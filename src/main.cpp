#include "bot.h"
#include "database.h"

int main() {
    TgBot::Bot bot("///");

    std::string TGBotDB = "data.db";
    Database botDB(TGBotDB);
    botDB.CreateTable();
    
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
