#include "Triangle.h"

#include <iostream>

std::string Triangle::m_type_name = "Triangle";

Triangle::Triangle()
	: PrimitiveFigure(new sf::CircleShape(50))
{
	m_shape->setFillColor(sf::Color::Cyan);
	m_shape->setPosition(0, 0);
	m_shape->setOrigin(m_shape->getGlobalBounds().height / 2.f, m_shape->getGlobalBounds().width / 2.f);
	((sf::CircleShape*)m_shape)->setPointCount(3);
}

Triangle::Triangle(const Triangle& obj)
	// m_shape should point to new Shape object. 
	: PrimitiveFigure(new sf::CircleShape(*dynamic_cast<const sf::CircleShape*>(&obj.getShape())))
{
	m_tail = obj.m_tail;
}

Prototype* Triangle::clone()
{
    return (Prototype*)new Triangle(*this);
}

Figure* Triangle::create()
{
	return new Triangle();
}

void Triangle::reset()
{
	*m_shape = sf::CircleShape(50);
	m_shape->setFillColor(sf::Color::Cyan);
	m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
	((sf::CircleShape*)m_shape)->setPointCount(3);
	m_prev_positions.clear();

}

const std::string& Triangle::getName() const
{
	return m_type_name;
}
