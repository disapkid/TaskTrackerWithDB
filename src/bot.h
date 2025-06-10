#pragma once
#include <tgbot/tgbot.h>
#include "helpFunctions.h"
#include "buttonsResponse.h"
#include "menu.h"
#include "callbackFunc.h"
#include "database.h"

void start(TgBot::Bot& bot, Database& botDB);

void help(TgBot::Bot& bot);

void assembled(TgBot::Bot& bot, Database& botDB); 