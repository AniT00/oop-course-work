#pragma once

#include "MenuOption.h"

#include <unordered_map>
#include <functional>
#include <stack>
#include <SFML/Graphics.hpp>

using menu_option_map_t = std::unordered_map<sf::Keyboard::Key, MenuOption*>;

class Menu
{
public:
	Menu(std::stack<Menu*>* menu_stack,
		std::initializer_list<std::pair<const sf::Keyboard::Key, MenuOption*>> list);

	void handleKey(sf::Keyboard::Key key);
private:
	menu_option_map_t m_option_map;

	std::stack<Menu*>* m_menu_stack;
};

