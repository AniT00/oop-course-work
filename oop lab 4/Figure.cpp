#include "Figure.h"

#include <iostream>

sf::BlendMode Figure::pulse_state = sf::BlendMode();
int Figure::pulse = 0;

std::pair<Figure*, Figure*> Figure::getIntersection(const sf::Vector2f& position)
{
    return { nullptr, nullptr };
}

void Figure::changeMovement()
{
    m_move_automatically = !m_move_automatically;
}

size_t Figure::getSize()
{
	throw std::exception("Leaf has no collection.");
}

const Figure* Figure::getParent() const
{
    return m_parent;
}

void Figure::setParent(Figure* parent)
{
    m_parent = parent;
}

const Figure* Figure::getChild(size_t index) const
{
	throw std::exception("Leaf has no childs.");
}

void Figure::setOriginByAverage()
{
    throw std::exception("Leaf does not need this");
}

void Figure::changeTail()
{
	m_tail = !m_tail;
}

void Figure::changeVisibility()
{
	m_visible = !m_visible;
}

void Figure::setVisible(bool _visible)
{
	m_visible = _visible;
}

bool Figure::getVisible(bool) const
{
	return m_visible;
}

std::ostream& operator<<(std::ostream& os, const Figure& obj)
{
	return obj.write(os);
}

std::istream& operator>> (std::istream& is, Figure& obj)
{
	
	return obj.read(is);
};
