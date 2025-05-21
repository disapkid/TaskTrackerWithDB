#pragma once
#include <tgbot/tgbot.h>
#include "buttonsResponse.h"
#include "menu.h"
#include "callbackFunc.h"
#include "database.h"

void start(TgBot::Bot& bot);

void help(TgBot::Bot& bot);

void readTask(TgBot::Bot& bot, Database& botDB);

void assembled(TgBot::Bot& bot, Database& botDB); 