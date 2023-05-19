#pragma once
#include <iostream>
#include <string>
#include <functional>

class Menu;
class MenuOption;

std::ostream& operator<<(std::ostream& os, const MenuOption& obj);

class MenuOption
{
public:
	MenuOption(std::string text, Menu* submenu);

	MenuOption(std::string text, std::function<void()> function);

	Menu* execute();

	const std::string& getText() const;

	~MenuOption();
private:
	friend std::ostream& operator<<(std::ostream& os, const MenuOption& obj);

	std::string m_text;

	bool m_submenu_command = false;

	union Command
	{
		Menu* m_submenu = nullptr;
		std::function<void()> m_function;

		Command(std::function<void()> function);

		Command(Menu* submenu);

		~Command() {}
	};

	Command m_command;
};

