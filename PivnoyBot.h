#include <string>
#include <string.h>
#include <vector>
#include <tgbot/tgbot.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <mutex>
#include <deque>
#include <map>
#include "sqlite3.h"

struct Test {
	Test();
	std::vector<std::string> questions_vec;
	std::string test_name;
};

class PivnoyBot {
public:
	
	//enum class m_states = { first, second, third };

	PivnoyBot(std::string);
	PivnoyBot(std::string, std::vector<std::string>);
	PivnoyBot(std::string, std::vector<std::pair<std::string, std::string>>);
	void Runner();
	void Call_Logger();
	void SetCommandsEvents();
	void InitializeKeyboard(std::vector<std::pair<std::string, std::string>>);
	void SetInlineKeyboardEvents();
	void Start_RunTest(int, TgBot::Message::Ptr);
	void AddTest(const std::string&, const std::vector<std::string>&);
	//DB functionality
	void InsertIntoUsers(const std::string&, const std::string&, const std::string&, const std::string&);
	static std::string quotesql(const std::string&);

	~PivnoyBot();
private:
	void RunTest(int, TgBot::Message::Ptr, int);
	std::vector<Test> m_tests;
	bool m_is_passing_test;

//tg API variables...
	bool is_inline_answered;
	TgBot::InlineKeyboardMarkup::Ptr inline_keyboard;
	//TgBot::ReplyKeyboardMarkup::Ptr	reply_keyboard;
	std::mutex mtx;
	std::deque<std::string> m_logs;
	std::map<int, std::thread> m_test_threads;
	int m_thread_id;
	//std::deque<std::thread> m_test_threads;

// multithreading, logger variables..
	void Logger();
	void log(std::string);
	void Run();
	std::thread run_thread, log_thread;
	TgBot::Bot m_bot;
	void HandleCommand(std::string);
	int m_users_id_counter;

//DB variables...
	sqlite3* m_db;
	sqlite3_stmt* m_stmt;
	char* m_err_msg;
	int m_rc;

	int m_log_file_fd;
	std::string m_log_file;
	std::string m_token;
	std::vector<TgBot::BotCommand::Ptr> m_commands;
	//inline keyboard
	bool m_is_running;
	std::map<std::string, bool> m_user_active_state;
};
