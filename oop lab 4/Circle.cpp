#include "Circle.h"

#include <iostream>

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
	m_shape = new sf::CircleShape(*dynamic_cast<const sf::CircleShape*>(obj.m_shape));
	m_trail = obj.m_trail;
}

Prototype* Circle::clone()
{
    return (Prototype*)new Circle(*this);
}

void Circle::reset()
{
	*m_shape = sf::CircleShape(50);
	m_shape->setFillColor(sf::Color::Blue);
	m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
	m_prev_positions.clear();
}

std::ostream& Circle::write(std::ostream& os) const
{
	const char* m_name = "Circle";
	os.write(m_name, sizeof(m_name));
	os << '\0';
	const sf::Color& color = m_shape->getFillColor();
	os.write((char*)&color, sizeof(sf::Color));
	const float* transform_matrix = m_shape->getTransform().getMatrix();
	os.write((char*)transform_matrix, 16 * sizeof(float));
	return os;
}

//std::istream& Circle::read(std::istream& is)
//{
//	sf::Color color;
//	is.read((char*)&color, sizeof(sf::Color));
//	m_shape->setFillColor(color);
//	float transform_matrix[16];
//	is.read((char*)&transform_matrix, 16 * sizeof(float));
//	return is;
//}