#include "MenuOption.h"

MenuOption::MenuOption(std::string text, Menu* submenu)
	: m_text(text), m_command(submenu)
{
	m_submenu_command = true;
}

MenuOption::MenuOption(std::string text, std::function<void()> function)
	: m_text(text), m_command(function)
{
	m_submenu_command = false;
}

Menu* MenuOption::execute()
{
	if (m_submenu_command)
	{
		return m_command.m_submenu;
	}
	else
	{
		m_command.m_function();
		return nullptr;
	}
}

const std::string& MenuOption::getText() const
{
	return m_text;
}

MenuOption::~MenuOption()
{

}

MenuOption::Command::Command(std::function<void()> function)
	:m_function(function)
{ }

MenuOption::Command::Command(Menu * submenu)
	: m_submenu(submenu) 
{ }

std::ostream& operator<<(std::ostream& os, const MenuOption& obj)
{
	return os << obj.m_text;
}