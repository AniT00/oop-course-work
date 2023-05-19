#include "PrimitiveFigure.h"

#include <iostream>

PrimitiveFigure::PrimitiveFigure(sf::Shape* shape)
	: m_shape(shape)
{ 
	m_shape->setOutlineColor(sf::Color::Red);
}

Figure::Memento* PrimitiveFigure::save() const
{
	return new Memento(
		m_shape->getPosition(),
		m_shape->getRotation(),
		m_shape->getScale());
}

void PrimitiveFigure::restore(const Memento* memento)
{
	m_shape->setPosition(memento->getPosition());
	m_shape->setRotation(memento->getRotation());
	m_shape->setScale(memento->getScale());
}

void PrimitiveFigure::move(const sf::Vector2f& offset)
{
    rememberTransform();
	
	if (m_parent != nullptr)
	{
		sf::Transform n = sf::Transform().rotate(-m_parent->getRotation());
		sf::Vector2f m = n.transformPoint(offset);
		m_shape->move(m);
	}
    
}

void PrimitiveFigure::rememberTransform()
{
    if (m_prev_positions.size() < 10)
    {
        m_prev_positions.push_front(m_shape->getTransform());
    }
    else
    {
        m_prev_positions.pop_back();
    }
}

void PrimitiveFigure::rotate(float degree)
{
    rememberTransform();
    m_shape->rotate(degree);
}

void PrimitiveFigure::scale(const sf::Vector2f& factor, sf::Vector2f centre)
{
    auto tmp = m_shape->getGlobalBounds();
    m_shape->scale(factor);
}

void PrimitiveFigure::setActive(bool active)
{
	if (active)
	{
		m_shape->setOutlineThickness(3.f);
	}
	else
	{
		m_shape->setOutlineThickness(0.f);
	}
}

std::pair<Figure*, Figure*> PrimitiveFigure::getIntersection(const sf::Vector2f& position)
{
    auto v = m_shape->getTransform().getInverse().transformPoint(position);
    if (m_shape->getLocalBounds().contains(v))
    {
        return { nullptr, this };
    }
    return { nullptr, nullptr };
}

const sf::Shape& PrimitiveFigure::getShape() const
{
    return *m_shape;
}

sf::Shape& PrimitiveFigure::getShape()
{
    return *m_shape;
}

void PrimitiveFigure::changeColor(sf::Color offset)
{
    sf::Color color = m_shape->getFillColor();
    color.r += offset.r;
    color.g += offset.g;
    color.b += offset.b;
    m_shape->setFillColor(color);
}

void PrimitiveFigure::setTail(bool enabled)
{
    m_tail = enabled;
}

void PrimitiveFigure::setColor(const sf::Color& color)
{
    m_shape->setFillColor(color);
}

const sf::Color& PrimitiveFigure::getColor() const
{
	return m_shape->getFillColor();
}

sf::Vector2f PrimitiveFigure::getWorldPosition()
{
	sf::Vector2f pos = m_shape->getPosition();
	if (m_parent != nullptr)
	{
		pos = m_parent->getWorldPosition(pos);
	}
	return pos;
}

sf::Vector2f PrimitiveFigure::getWorldPosition(sf::Vector2f point)
{
	// TODO test.
	return m_shape->getTransform().getInverse().transformPoint(point);
}

const sf::Vector2f& PrimitiveFigure::getPosition() const
{
	return m_shape->getPosition();
}

void PrimitiveFigure::setPosition(float x, float y)
{
	m_shape->setPosition(x, y);
}

void PrimitiveFigure::setPosition(const sf::Vector2f& position)
{
	setPosition(position.x, position.y);
}

const sf::Vector2f& PrimitiveFigure::getScale() const
{
	return m_shape->getScale();
}

void PrimitiveFigure::setScale(float x, float y)
{
	m_shape->setScale(x, y);
}

void PrimitiveFigure::setScale(const sf::Vector2f& scale)
{
	setScale(scale.x, scale.y);
}

float PrimitiveFigure::getRotation() const
{
	return m_shape->getRotation();
}

void PrimitiveFigure::setRotation(float angle)
{
	m_shape->setRotation(angle);
}

const sf::Transform& PrimitiveFigure::getTransform() const
{
	return m_shape->getTransform();
}

void PrimitiveFigure::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!m_visible) { return; }
    if (m_tail)
    {
        sf::Vector2f pos = m_shape->getPosition();
        m_shape->setPosition(m_shape->getOrigin().x, m_shape->getOrigin().y);
        float rot = m_shape->getRotation();
        m_shape->setRotation(0);
        sf::Color color = m_shape->getFillColor();

        for (auto& elem : m_prev_positions)
        {
            target.draw(*m_shape, states.transform * elem);
            sf::Color c = m_shape->getFillColor();
            if (m_prev_positions.size() != 1)
                c.a -= 255 / (m_prev_positions.size() - 1);
            m_shape->setFillColor(c);
        }

        m_shape->setPosition(pos);
        m_shape->setRotation(rot);
        m_shape->setFillColor(color);
    }
    target.draw(*m_shape, states);
}

PrimitiveFigure::~PrimitiveFigure()
{
    delete m_shape;
}

std::ostream& PrimitiveFigure::write(std::ostream& os, short tabSize) const
{
	std::string tab(tabSize, '\t');

	os << tab << '\"' << getName() << '\"' << ": {\n";
	tab += '\t';

	const sf::Vector2f& position = m_shape->getPosition();
	os << tab << "\"position\": {\n";
	os << tab << "\t\"x\": " << position.x << '\n';
	os << tab << "\t\"y\": " << position.y << '\n';
	os << tab << "}\n";

	os << tab << "\"rotation\": " << m_shape->getRotation() << '\n';

	const sf::Vector2f& scale = m_shape->getScale();
	os << tab << "\"scale\": {\n";
	os << tab << "\t\"x\": " << scale.x << '\n';
	os << tab << "\t\"y\": " << scale.y << '\n';
	os << tab << "}\n";

	const sf::Color& color = m_shape->getFillColor();
	os << tab << "\"color\": {\n";
	os << tab << "\t\"r\": " << (int)color.r << "\n";
	os << tab << "\t\"g\": " << (int)color.g << "\n";
	os << tab << "\t\"b\": " << (int)color.b << "\n";
	os << tab << "}\n";
	tab = tab.substr(0, tabSize);
	os << tab << '}';

	return os;
}

std::istream& PrimitiveFigure::read(std::istream& is)
{
	sf::Vector2f position;
	float rotation = 0;
	sf::Vector2f scale;
	sf::Color color;
	std::string v;
	// Skip "{" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Skip "position": {" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	// Skip to x value
	is.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	is >> position.x;
	// Skip to y value
	is.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	is >> position.y;
	// Skip "}" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Skip to rotation value
	is.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	is >> rotation;
	// Skip empty line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Skip "scale": {" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	// Skip to x value
	is.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	is >> scale.x;
	// Skip to y value
	is.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	is >> scale.y;
	// Skip empty line and "}" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Skip "color": {" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	// Skip to r value
	int value;
	is.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	is >> value;
	color.r = value;
	// Skip to g value
	is.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	is >> value;
	color.g = value;
	// Skip to b value
	is.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	is >> value;
	color.b = value;
	// Skip empty and "}" lines
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Skip "}" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	
	m_shape->setPosition(position);
	m_shape->setRotation(rotation);
	m_shape->setScale(scale);
	m_shape->setFillColor(color);

	return is;
}
