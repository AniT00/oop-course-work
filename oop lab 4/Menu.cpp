#include "Menu.h"

#include <unordered_map>

Menu::Menu(std::initializer_list<key_option_pair_t> list)
	: m_option_map(list)
{
	for (auto elem : list)
	{
		m_options_order.push_back(elem.first);
	}
}

Menu::Menu(const std::list<key_option_pair_t>& list)
{
	add(list);
}

void Menu::add(std::initializer_list<key_option_pair_t> list)
{
	for (auto elem : list)
	{
		m_options_order.push_back(elem.first);
	}
	m_option_map.insert(list);
}

void Menu::add(int index, std::initializer_list<key_option_pair_t> list)
{
	auto it = std::next(m_option_map.cbegin(), index);
	for (auto elem : list)
	{
		m_options_order.insert(m_options_order.cbegin(), elem.first);
		m_option_map.insert(it++, elem);
	}
}

void Menu::add(const std::list<key_option_pair_t>& list)
{
	for (auto elem : list)
	{
		m_options_order.push_back(elem.first);
		m_option_map.insert(elem);
	}
}

void Menu::setTitle(std::string title)
{
	m_title = title;
}

const std::string& Menu::getTitle() const { return m_title; }

void Menu::handleKey(sf::Keyboard::Key key)
{
	if (m_option_map.contains(key))
	{
		m_option_map[key]->execute();
	}
}

Menu::~Menu()
{
	for (auto& elem : m_option_map)
	{
		elem.second.reset();
	}
}

std::ostream& operator<<(std::ostream& os, const Menu& obj)
{
	if (!obj.m_title.empty())
	{
		std::cout << obj.m_title << '\n';
	}
	for (auto& elem : obj.m_options_order)
	{
		os << '(' << menu_tools::key_names.at(elem) << ") " << *obj.m_option_map.find(elem)->second << '\n';
	}
	return os;
}
