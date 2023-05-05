#include "MenuOption.h"

MenuOption::MenuOption(std::string text, Menu* submenu)
{
	m_submenu_command = true;
	m_command.submenu = submenu;
}

MenuOption::MenuOption(std::string text, std::function<void()> function)
{
	m_submenu_command = false;
	m_command.function = function;
}

Menu* MenuOption::execute()
{
	if (m_submenu_command)
	{
		return m_command.submenu;
	}
	else
	{
		m_command.function();
		return nullptr;
	}
}

const std::string& MenuOption::getText() const
{
	return m_text;
}
