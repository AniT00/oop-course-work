#pragma once

#include "MenuOption.h"

#include <unordered_map>
#include <functional>
#include <stack>
#include <SFML/Graphics.hpp>

namespace menu_tools
{
	extern const std::unordered_map<sf::Keyboard::Key, const char*> key_names;
}

using key_option_pair_t = std::pair<const sf::Keyboard::Key, MenuOption*>;
// Use map because of constant accesss time.
using menu_option_map_t = std::unordered_map<sf::Keyboard::Key, MenuOption*>;

class Menu;

std::ostream& operator<<(std::ostream& os, const Menu& obj);

class Menu
{
public:
	Menu(std::initializer_list<key_option_pair_t> list);

	Menu(const std::list<key_option_pair_t>& list);

	void add(std::initializer_list<key_option_pair_t> list);

	void add(int index, std::initializer_list<key_option_pair_t> list);

	void add(const std::list<key_option_pair_t>& list);

	void setTitle(std::string title);

	const std::string& getTitle() const;

	void handleKey(sf::Keyboard::Key key);
private:
	std::string m_title;

	menu_option_map_t m_option_map;

	std::list<sf::Keyboard::Key> m_options_order;

	friend std::ostream& operator<<(std::ostream& os, const Menu& obj);
};

