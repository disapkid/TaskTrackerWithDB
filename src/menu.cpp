#include "menu.h"

void Menu(TgBot::Bot& bot, TgBot::Message::Ptr& message) {
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        std::vector<TgBot::InlineKeyboardButton::Ptr> buttons;

        TgBot::InlineKeyboardButton::Ptr setUTC(new TgBot::InlineKeyboardButton);
        setUTC->text = "Set UTC";
        setUTC->callbackData = "setUTC";

        TgBot::InlineKeyboardButton::Ptr newTask(new TgBot::InlineKeyboardButton);
        newTask->text = "Add new task";
        newTask->callbackData = "addNewTask";
        
        TgBot::InlineKeyboardButton::Ptr activeTasks(new TgBot::InlineKeyboardButton);
        activeTasks->text = "Active tasks";
        activeTasks->callbackData = "activeTasks";

        buttons.push_back(setUTC);
        buttons.push_back(newTask);
        buttons.push_back(activeTasks);
        keyboard->inlineKeyboard.push_back(buttons);

        bot.getApi().sendMessage(message->chat->id, "Available functions: ", nullptr, 0, keyboard);
}