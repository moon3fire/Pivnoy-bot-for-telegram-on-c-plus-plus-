#include "PivnoyBot.h"

Test::Test()
:test_name("")
,questions_vec(0)
{

}

PivnoyBot::PivnoyBot(std::string token_)
	:m_token(token_)
	,m_bot(token_)
	,m_log_file("log.txt")
	,is_inline_answered(false)
	,inline_keyboard(new TgBot::InlineKeyboardMarkup)
	,m_rc(0)
	,m_tests(0)
	,m_is_passing_test(false)
	,m_test_threads()
	,m_thread_id(0)
	//,reply_keyboard(new TgBot::ReplyKeyboardMarkup)
{
	//database initializing
	m_rc = sqlite3_open("bot.db", &m_db);
	if ( m_rc )
	{
		fprintf(stderr, "Can't open database %s\n", sqlite3_errmsg(m_db));
		throw std::runtime_error("can't open database");
		return;
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}
	
	std::string sql = "CREATE TABLE USERS (" \
	"ID INT PRIMARY KEY		NOT NULL," \
	"NAME			TEXT	NOT NULL," \
	"SURNAME		TEXT	NOT NULL," \
	"USERNAME		TEXT	NOT NULL);";

	try
	{
		m_rc = sqlite3_exec(m_db, sql.c_str(), NULL, 0, &m_err_msg);
		sqlite3_close(m_db);
	}
	catch(std::exception& e)
	{
		fprintf(stderr, "%s", e.what());
	}

	sqlite3_close(m_db);
}

PivnoyBot::PivnoyBot(std::string token_, std::vector<std::string> commandsVec_)
	:m_token(token_)
	,m_bot(token_)
	,m_log_file("log.txt")
	,is_inline_answered(false)
	,inline_keyboard(new TgBot::InlineKeyboardMarkup)
	,m_rc(0)
	,m_tests(0)
	,m_is_passing_test(false)
	,m_test_threads()
	,m_thread_id(0)
//	,reply_keyboard(new TgBot::ReplyKeyboardMarkup)
{
	for (int i = 0; i < commandsVec_.size(); ++i)
	{
		TgBot::BotCommand::Ptr command(new TgBot::BotCommand);
		command->command = commandsVec_[i];
		command->description = commandsVec_[i];
		m_commands.push_back(command);
	}
	
	//database initializing
	m_rc = sqlite3_open("bot.db", &m_db);
	if ( m_rc )
	{
		fprintf(stderr, "Can't open database %s\n", sqlite3_errmsg(m_db));
		throw std::runtime_error("can't open database");
		return;
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}
	
	std::string sql = "CREATE TABLE USERS (" \
	"ID INT PRIMARY KEY		NOT NULL," \
	"NAME			TEXT	NOT NULL," \
	"SURNAME		TEXT	NOT NULL," \
	"USERNAME		TEXT	NOT NULL);";

	try
	{
		m_rc = sqlite3_exec(m_db, sql.c_str(), NULL, 0, &m_err_msg);
		sqlite3_close(m_db);
	}
	catch(std::exception& e)
	{
		fprintf(stderr, "%s", e.what());
	}

	sqlite3_close(m_db);
}

PivnoyBot::PivnoyBot(std::string token_, std::vector<std::pair<std::string, std::string>> commands_descriptionsVec_)
	:m_token(token_)
	,m_bot(token_)
	,m_log_file("log.txt")
	,is_inline_answered(false)
	,inline_keyboard(new TgBot::InlineKeyboardMarkup)
	,m_rc(0)
	,m_tests(0)
	,m_is_passing_test(false)
	,m_test_threads()
	,m_thread_id(0)
//	,reply_keyboard(new TgBot::ReplyKeyboardMarkup)
{
	for (int i = 0; i < commands_descriptionsVec_.size(); ++i)
	{
		TgBot::BotCommand::Ptr command(new TgBot::BotCommand);
		command->command = commands_descriptionsVec_[i].first;
		command->description = commands_descriptionsVec_[i].second;
		m_commands.push_back(command);
	}
	
	//database initializing
	m_rc = sqlite3_open("bot.db", &m_db);
	if ( m_rc )
	{
		fprintf(stderr, "Can't open database %s\n", sqlite3_errmsg(m_db));
		throw std::runtime_error("can't open database");
		return;
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}
	
	std::string sql = "CREATE TABLE USERS (" \
	"ID INT PRIMARY KEY		NOT NULL," \
	"NAME			TEXT	NOT NULL," \
	"SURNAME		TEXT	NOT NULL," \
	"USERNAME		TEXT	NOT NULL);";

	try
	{
		m_rc = sqlite3_exec(m_db, sql.c_str(), NULL, 0, &m_err_msg);
		sqlite3_close(m_db);
	}
	catch(std::exception& e)
	{
		fprintf(stderr, "%s", e.what());
	}

	sqlite3_close(m_db);
}

PivnoyBot::~PivnoyBot()
{
	run_thread.join();
	log_thread.join();
/*	
 	for(auto& thread : m_test_threads)
	{
		thread.join();
	}
*/
}

void PivnoyBot::InitializeKeyboard(std::vector<std::pair<std::string, std::string>> commandsInfoVec_)
{
	std::vector<TgBot::InlineKeyboardButton::Ptr> row0;

	for(int i = 0; i < commandsInfoVec_.size()/2; ++i)
	{
		TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
		button->text = commandsInfoVec_[i].first;
		button->callbackData = commandsInfoVec_[i].second;
		row0.push_back(button);
	}

	inline_keyboard->inlineKeyboard.push_back(row0);


	for(int i = commandsInfoVec_.size() / 2; i < commandsInfoVec_.size(); ++i)
	{
		std::vector<TgBot::InlineKeyboardButton::Ptr> button_vec;
		TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
		button->text = commandsInfoVec_[i].first;
		button->callbackData = commandsInfoVec_[i].second;
		button_vec.push_back(button);
		inline_keyboard->inlineKeyboard.push_back(button_vec);
	}

	return;
}

void PivnoyBot::Runner()
{
	run_thread = std::thread{&PivnoyBot::Run, this};
}

void PivnoyBot::Run()
{
	m_log_file_fd = open(m_log_file.c_str(), O_RDWR | O_CREAT, 0777);
	m_bot.getApi().setMyCommands(m_commands);

	try
	{
		std::string bot_username = "Bot username is " + m_bot.getApi().getMe()->username + "\n";
		write(m_log_file_fd, bot_username.c_str(), strlen(bot_username.c_str()));
		//std::cout << "Bot username is " << m_bot.getApi().getMe()->username << std::endl;
		TgBot::TgLongPoll longPoll(m_bot);
		while (true)
		{
			//write(m_log_file_fd, "Long poll started\n", 18);
			//std::cout << "Long poll started(iteracia ++)" << std::endl;
			longPoll.start();
		}
	}
	catch (TgBot::TgException& e)
	{
		std::string error_message = "error: " + std::string(e.what(), strlen(e.what())) + "\n";
		write(m_log_file_fd, error_message.c_str(), strlen(error_message.c_str()));
		//std::cout << "error: " << e.what() << std::endl;
	}
	
	std::string user_end_info = "The bot has stopped working for user " + m_bot.getApi().getWebhookInfo()->ipAddress;
	write(m_log_file_fd, user_end_info.c_str(), strlen(user_end_info.c_str())); 
	close(m_log_file_fd);
	//std::cout << "The bot has stopped working!" << std::endl;
	return;
}

void PivnoyBot::Call_Logger()
{
	log_thread = std::thread{&PivnoyBot::Logger, this};
}

void PivnoyBot::Logger()
{
	while(true)
	{
		if(m_logs.size() == 0)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}
		mtx.lock();
		std::string current_log_text = m_logs.front() + "\n";
		m_logs.pop_front();
		mtx.unlock();
		write(m_log_file_fd, current_log_text.c_str(), strlen(current_log_text.c_str()));
	}
	return;
}

void PivnoyBot::log(std::string msg)
{
	mtx.lock();
	m_logs.push_back(msg);
	mtx.unlock();
	return;
}

void PivnoyBot::SetInlineKeyboardEvents()
{
	m_bot.getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query)
	{
		if(StringTools::startsWith(query->data, "_row_first")) 
		{
			m_bot.getApi().sendMessage(query->message->chat->id, "row 1");
		}
		if(StringTools::startsWith(query->data, "_row_second")) 
		{
			m_bot.getApi().sendMessage(query->message->chat->id, "row 2");
		}
		if(StringTools::startsWith(query->data, "_row_third")) 
		{
			m_bot.getApi().sendMessage(query->message->chat->id, "row 3");
		}
		if(StringTools::startsWith(query->data, "_row_fourth")) 
		{
			m_bot.getApi().sendMessage(query->message->chat->id, "row 4");
		}
		if(StringTools::startsWith(query->data, "_column_first")) 
		{
			m_bot.getApi().sendMessage(query->message->chat->id, "column 1");
		}
		if(StringTools::startsWith(query->data, "_column_second")) 
		{
			m_bot.getApi().sendMessage(query->message->chat->id, "column 2");
		}
		if(StringTools::startsWith(query->data, "_column_third")) 
		{
			m_bot.getApi().sendMessage(query->message->chat->id, "column 3");
		}
		if(StringTools::startsWith(query->data, "_column_fourth")) 
		{
			m_bot.getApi().sendMessage(query->message->chat->id, "column 4");
		}
	
		std::string msg = "User pressed button " + query->data + " on inline keyboard";
		log(msg);
		m_bot.getApi().editMessageReplyMarkup(query->message->chat->id, query->message->messageId, query->inlineMessageId, NULL);
	});
}

void PivnoyBot::SetCommandsEvents()
{
	SetInlineKeyboardEvents();
	m_bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr message)
	{
		if(message->text[0] != '/')
		{
			m_bot.getApi().sendMessage(message->chat->id, "I understand only commands!");
		}
	});
	for (auto& cmd : m_commands)
	{
		//TODO don't forget to add this to whole list
		m_bot.getEvents().onCommand(cmd->command, [&](TgBot::Message::Ptr message)
		{
			if(cmd->command == "start")
			{
				if(m_is_passing_test)
					return;
				auto it = m_user_active_state.find(message->from->username);
				if(it == m_user_active_state.end())
				{
					m_bot.getApi().sendMessage(message->chat->id, "Welcome to Pivnoy bot!");
					m_user_active_state.emplace(std::pair<std::string, bool>(message->from->username, true));
					std::string msg = "User: " + message->from->username + " entered first time to our bot!";
					log(msg);
				}
				else
				{
					if(false == it->second)
					{
						it->second = true;
						m_bot.getApi().sendMessage(message->chat->id, "Bot started working...");
						std::string msg = "User: " + message->from->username + " restarted the bot!";
						log(msg);
						return;
					}
					else
					{
						m_bot.getApi().sendMessage(message->chat->id, "Bot is already started working");
						std::string msg = "User: " + message->from->username + " tried to /start when bot is already running";
						log(msg);
						return;
					}
				}
				++m_users_id_counter;
				std::string user_id = std::to_string(m_users_id_counter);
				InsertIntoUsers(user_id, message->from->firstName,
								message->from->lastName, message->from->username);
				m_bot.getApi().sendMessage(message->chat->id, "You've been registered to our system!");
				std::string msg = "User: " + message->from->username + " was added to database";
				log(msg);
				return;
			}
			if(cmd->command == "stop")
			{
				if(m_user_active_state.find(message->from->username) == m_user_active_state.end() ||
				   m_user_active_state[message->from->username] == false ||
				   m_is_passing_test)
				{
					return;
				}
				m_bot.getApi().sendMessage(message->chat->id, "Bot is stopped now");
				m_user_active_state[message->from->username] = false;
				std::string msg = "User: " + message->from->username + " typed /stop";
				log(msg);
				return;
			}
			if(cmd->command == "test")
			{
				if(m_user_active_state.find(message->from->username) == m_user_active_state.end() ||
				   m_user_active_state[message->from->username] == false ||
				   m_is_passing_test)
				{
					return;
				}
				std::cout << " 1 " << std::endl;
				Start_RunTest(0, message);
				return;
			}
			if(cmd->command == "end")
			{
				if(m_user_active_state.find(message->from->username) == m_user_active_state.end() ||
				   m_user_active_state[message->from->username] == false ||
				   m_is_passing_test)
				{
					return;
				}
				m_bot.getApi().sendMessage(message->chat->id, "NOT WORKING YET...");
				//RunTest(1, message);
				
				//inline keyboard stuff below
				//is_inline_answered = false;
				//m_bot.getApi().sendMessage(message->chat->id, "choose the variant you want", false, 0, inline_keyboard, "Markdown");
				return;
			}
			//add switch cases to handle any command operations
			//m_bot.getApi().sendMessage(message->chat->id, "You wrote " + cmd->command);\
			//std::string user_message_info = "User: " + message->from->username + " wrote command " + cmd->command + "\n";
			//write(m_log_file_fd, user_message_info.c_str(), strlen(user_message_info.c_str()));
			//std::cout << "User: " << message->from->username << "wrote command " << cmd->command << std::endl;
		});
/*
 @brief getting fileid
		m_bot.getEvents().onAnyMessage([&](TgBot::Message::Ptr message)
		{
			TgBot::File::Ptr my_file = m_bot.getApi().getFile(message->audio->fileId);	
			m_bot.getApi().sendMessage(message->chat->id, my_file->fileId);
		});
*/
	}
}

std::string PivnoyBot::quotesql(const std::string& str)
{
	return std::string("'") + str + std::string("'");
}

void PivnoyBot::InsertIntoUsers(const std::string& id_, const std::string& name_,
								const std::string& surname_, const std::string& username_)
{
	if(sqlite3_open("bot.db", &m_db) == SQLITE_OK)
	{
		std::string sql_statement = "INSERT INTO USERS (ID, NAME, SURNAME, USERNAME) VALUES ("
		+ quotesql(id_) + ","
		+ quotesql(name_) + ","
		+ quotesql(surname_) + ","
		+ quotesql(username_) + ");";

		sqlite3_prepare(m_db, sql_statement.c_str(), -1, &m_stmt, NULL);
		sqlite3_step( m_stmt );
		fprintf(stdout, "Inserted user info into database\n");
	}
	else
	{
		fprintf(stderr, "Failed inserting into database\n");
		throw std::runtime_error("failed inserting into database");
		return;
	}

	sqlite3_finalize(m_stmt);
	sqlite3_close(m_db);
}

void PivnoyBot::AddTest(const std::string& test_name_, const std::vector<std::string>& questions_)
{
	Test test;
	test.test_name = test_name_;
	for(int i = 0; i < questions_.size(); ++i)
	{
		test.questions_vec.push_back(questions_[i]);
	}
	m_tests.push_back(test);
}

void PivnoyBot::Start_RunTest(int index, TgBot::Message::Ptr message)
{
	std::thread t;
	m_test_threads.emplace(std::pair<int, std::thread>(m_thread_id, std::move(t)));
	m_test_threads[m_thread_id] = std::thread{&PivnoyBot::RunTest, this, index, message, m_thread_id};
	++m_thread_id;
	return;
}

void PivnoyBot::RunTest(int index, TgBot::Message::Ptr message, int current_thread_id)
{
	m_user_active_state[message->from->username] = false;
	mtx.lock();
	m_bot.getApi().sendMessage(message->chat->id, "Test: " + m_tests[index].test_name);
	mtx.unlock();
	for(int i = 0; i < m_tests[index].questions_vec.size(); ++i)
	{
		mtx.lock();
		m_bot.getApi().sendMessage(message->chat->id, m_tests[index].questions_vec[i]);
		mtx.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	m_user_active_state[message->from->username] = true;
	return;
}


