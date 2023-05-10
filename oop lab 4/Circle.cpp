#include "Circle.h"

#include <iostream>

std::string Circle::m_type_name = "Circle";

Circle::Circle()
	: PrimitiveFigure(new sf::CircleShape(50))
{
    m_shape->setFillColor(sf::Color::Blue);
    m_shape->setPosition(0, 0);
    m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
}

Circle::Circle(const Circle& obj)
	// m_shape should point to new Shape object. 
	: PrimitiveFigure(new sf::CircleShape(*dynamic_cast<const sf::CircleShape*>(&obj.getShape())))
{
	m_tail = obj.m_tail;
}

Prototype* Circle::clone()
{
    return (Prototype*)new Circle(*this);
}

Figure* Circle::create()
{
	return new Circle();
}

void Circle::reset()
{
	*m_shape = sf::CircleShape(50);
	m_shape->setFillColor(sf::Color::Blue);
	m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
	m_prev_positions.clear();
}

const std::string& Circle::getName() const
{
	return m_type_name;
}
