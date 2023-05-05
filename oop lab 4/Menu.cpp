#include "Menu.h"

Menu::Menu(std::stack<Menu*>* menu_stack, 
	std::initializer_list<std::pair<const sf::Keyboard::Key, MenuOption*>> list)
	: m_option_map(list)
{
	m_menu_stack = menu_stack;
}

void Menu::handleKey(sf::Keyboard::Key key)
{
	if (m_option_map.contains(key))
	{
		m_option_map[key]->execute();
	}
}
