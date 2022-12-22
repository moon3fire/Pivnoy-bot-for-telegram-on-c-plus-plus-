#include "PivnoyBot.h"
#include "sqlite3.h"
#include <iostream>

int main()
{
	std::string hraman;
	std::vector<std::string> commands = { "start", "help", "test", "end", "stop" };
	std::vector<std::pair<std::string, std::string>> inline_keyboard_info;
	inline_keyboard_info.push_back(std::make_pair("row_first", "_row_first"));
	inline_keyboard_info.push_back(std::make_pair("row_second", "_row_second"));
	inline_keyboard_info.push_back(std::make_pair("row_third", "_row_third"));
	inline_keyboard_info.push_back(std::make_pair("row_fourth", "_row_fourth"));
	inline_keyboard_info.push_back(std::make_pair("column_first", "_column_first"));
	inline_keyboard_info.push_back(std::make_pair("column_second", "_column_second"));
	inline_keyboard_info.push_back(std::make_pair("column_third", "_column_third"));
	inline_keyboard_info.push_back(std::make_pair("column_fourth", "_column_fourth"));
	PivnoyBot my_bot("2020641111:AAEU26AWw0aGrSGZmoYWAx1ardQiq0BqG78", commands);
	my_bot.InitializeKeyboard(inline_keyboard_info);
	my_bot.SetCommandsEvents();

	std::vector<std::string> test1(0);
	test1.push_back("Hello?");
	test1.push_back("How are you?");
	test1.push_back("What beer do you like?");
	std::vector<std::string> test2(0);
	test2.push_back("What is your pants color?");
	test2.push_back("What is your name?");
	test2.push_back("What is you?");
	my_bot.AddTest(std::string("test na pivo"), test1);
	my_bot.AddTest(std::string("test na virginity"), test2);

	my_bot.Call_Logger();
	my_bot.Runner();
//	std::cin >> hraman;
//	sqlite3_close(db);
}
