#include "Rectangle.h"

#include <iostream>

std::string Rectangle::m_type_name = "Rectangle";

Rectangle::Rectangle()
{
	m_type_name = "Rectangle";
    m_shape = new sf::RectangleShape(sf::Vector2f(200, 100));
    m_shape->setFillColor(sf::Color::Yellow);
    m_shape->setPosition(0, 0);
    m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
}

Rectangle::Rectangle(sf::RectangleShape* rectangleShape)
{
	m_type_name = "Rectangle";
	this->m_shape = rectangleShape;
}

Rectangle::Rectangle(const Rectangle& obj)
{
	m_type_name = "Rectangle";
	m_shape = new sf::RectangleShape(*dynamic_cast<const sf::RectangleShape*>(&obj.getShape()));
	m_trail = obj.m_trail;
}

Prototype* Rectangle::clone()
{
    return (Prototype*)new Rectangle(*this);
}

Figure* Rectangle::create()
{
	return new Rectangle();
}

void Rectangle::reset()
{
	*m_shape = sf::RectangleShape(sf::Vector2f(200, 100));
	m_shape->setFillColor(sf::Color::Yellow);
	m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
	m_prev_positions.clear();
}

const std::string& Rectangle::getName() const
{
	return m_type_name;
}

//std::ostream& Rectangle::write(std::ostream& os) const
//{
//	const char* m_name = "Rectangle";
//	return os;
//}

//std::istream& Rectangle::read(std::istream& is)
//{
//	sf::Color color;
//	is.read((char*)&color, sizeof(sf::Color));
//	m_shape->setFillColor(color);
//	float transform_matrix[16];
//	is.read((char*)&transform_matrix, 16 * sizeof(float));
//	return is;
//}