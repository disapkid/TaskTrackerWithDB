#pragma once
#include <tgbot/tgbot.h>
#include "menu.h"
#include "database.h"
#include <algorithm>

void SetUsersUTC(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB);

void TaskNumber(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, int ID, Database& botDB);

void ActiveTasks(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB);

void DeleteCurrentTask(TgBot::Bot& bot,TgBot::CallbackQuery::Ptr& query, int ID, Database& botDB);

void NewTask(TgBot::Bot& bot, TgBot::CallbackQuery::Ptr& query, Database& botDB);