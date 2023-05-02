#include "Triangle.h"

Triangle::Triangle()
{
	m_shape = new sf::CircleShape(50);
	m_shape->setFillColor(sf::Color::Cyan);
	m_shape->setPosition(0, 0);
	m_shape->setOrigin(m_shape->getGlobalBounds().height / 2.f, m_shape->getGlobalBounds().width / 2.f);
	((sf::CircleShape*)m_shape)->setPointCount(3);
}

Triangle::Triangle(sf::CircleShape* circleShape)
{
	circleShape->setPointCount(3);
	this->m_shape = circleShape;
}

Triangle::Triangle(const Triangle& obj)
{

	m_shape = new sf::CircleShape(*dynamic_cast<const sf::CircleShape*>(&obj.getShape()));
	m_trail = obj.m_trail;
}

Prototype* Triangle::clone()
{
    return (Prototype*)new Triangle(*this);
}

void Triangle::reset()
{
	*m_shape = sf::CircleShape(50);
	m_shape->setFillColor(sf::Color::Cyan);
	m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
	((sf::CircleShape*)m_shape)->setPointCount(3);
	m_prev_positions.clear();

}