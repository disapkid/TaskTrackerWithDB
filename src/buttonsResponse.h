#pragma once
#include <tgbot/tgbot.h>
#include "menu.h"
#include "database.h"

void SendPic(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query);

void DoSomething(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query);

void ActiveTasks(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB);

void PetStatus(TgBot::Bot& bot);

void NewTask(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB);