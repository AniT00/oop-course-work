#include "Rectangle.h"

#include <iostream>

Rectangle::Rectangle()
{
    m_shape = new sf::RectangleShape(sf::Vector2f(200, 100));
    m_shape->setFillColor(sf::Color::Yellow);
    m_shape->setPosition(0, 0);
    m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
}

Rectangle::Rectangle(sf::RectangleShape* rectangleShape)
{
	this->m_shape = rectangleShape;
}

Rectangle::Rectangle(const Rectangle& obj)
{

	m_shape = new sf::RectangleShape(*dynamic_cast<const sf::RectangleShape*>(&obj.getShape()));
	m_trail = obj.m_trail;
}

Prototype* Rectangle::clone()
{
    return (Prototype*)new Rectangle(*this);
}

void Rectangle::reset()
{
	*m_shape = sf::RectangleShape(sf::Vector2f(200, 100));
	m_shape->setFillColor(sf::Color::Yellow);
	m_shape->setOrigin(m_shape->getGlobalBounds().width / 2.f, m_shape->getGlobalBounds().height / 2.f);
	m_prev_positions.clear();
}

std::ostream& Rectangle::write(std::ostream& os) const
{
	const char* m_name = "Rectangle";
	os.write(m_name, sizeof(m_name));
	os << '\0';
	const sf::Color& color = m_shape->getFillColor();
	os.write((char*)&color, sizeof(sf::Color));
	const float* transform_matrix = m_shape->getTransform().getMatrix();
	os.write((char*)transform_matrix, 16 * sizeof(float));
	return os;
}

//std::istream& Rectangle::read(std::istream& is)
//{
//	sf::Color color;
//	is.read((char*)&color, sizeof(sf::Color));
//	m_shape->setFillColor(color);
//	float transform_matrix[16];
//	is.read((char*)&transform_matrix, 16 * sizeof(float));
//	return is;
//}