#pragma once

#include <string>
#include <functional>

class Menu;

class MenuOption
{
public:
	MenuOption(std::string text, Menu* submenu);

	MenuOption(std::string text, std::function<void()> function);

	Menu* execute();

	const std::string& getText() const;

	~MenuOption()
	{

	}
private:
	std::string m_text;

	bool m_submenu_command = false;

	union Command
	{
		Menu* submenu = nullptr;
		std::function<void()> function;

		~Command()
		{

		}
	};

	Command m_command;
};

