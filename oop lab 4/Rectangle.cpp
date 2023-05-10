#include "Rectangle.h"

#include <iostream>

std::string Rectangle::m_type_name = "Rectangle";

Rectangle::Rectangle()
	: PrimitiveFigure(new sf::RectangleShape(sf::Vector2f(200, 100)))
{
    m_shape->setFillColor(sf::Color::Yellow);
    m_shape->setPosition(0, 0);
    m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
}

//Rectangle::Rectangle(sf::RectangleShape* rectangleShape)
//// m_shape should point to new Shape object. 
//	: PrimitiveFigure(new sf::RectangleShape(*rectangleShape))
//{
//}

Rectangle::Rectangle(const Rectangle& obj)
	// m_shape should point to new Shape object. 
	: PrimitiveFigure(new sf::RectangleShape(*dynamic_cast<const sf::RectangleShape*>(&obj.getShape())))
{
	m_tail = obj.m_tail;
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
