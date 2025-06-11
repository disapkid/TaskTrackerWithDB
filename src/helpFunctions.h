#pragma once
#include <tgbot/tgbot.h>
#include "menu.h"
#include "database.h"

void readMessage(TgBot::Bot& bot, Database& botDB);
 
bool checkTime(const std::string& deadline, TgBot::Message::Ptr& message, TgBot::Bot& bot, Database& botDB);

bool checkUTC(const std::string& UTC);

void checkDeadline(int chatID, TgBot::Bot& bot, Database& botDB);
