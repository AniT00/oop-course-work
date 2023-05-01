#include "Circle.h"

Circle::Circle()
{
    m_shape = new sf::CircleShape(50);
    m_shape->setFillColor(sf::Color::Blue);
    m_shape->setPosition(0, 0);
    m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
}

Circle::Circle(sf::CircleShape* circleShape)
{
	this->m_shape = circleShape;
}

Circle::Circle(const Circle& obj)
{
	m_shape = new sf::CircleShape(*dynamic_cast<const sf::CircleShape*>(&obj.getShape()));
	m_trail = obj.m_trail;
}

void Circle::reset()
{
	*m_shape = sf::CircleShape(50);
	m_shape->setFillColor(sf::Color::Blue);
	m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
	m_prev_positions.clear();
}
