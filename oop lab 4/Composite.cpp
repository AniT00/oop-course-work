#include "Composite.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Triangle.h"

#include <iostream>
#include <numeric>

std::string Composite::m_type_name = "Composite";

FigureFactory* Composite::m_primitiveFigureFactory = nullptr;

Composite::Composite()
{ }

Composite::Composite(Figure* figure)
{
	m_position = figure->getShape().getPosition();
	figure->getShape().setPosition(0, 0);
	add(figure);
}

Composite::Composite(const Composite& obj)
{
    for (auto figure : obj.m_composition)
    {
        add((Figure*)figure->clone());
    }
	m_transform = obj.m_transform;
    /*m_scale = obj.m_scale;
    m_size = obj.m_size;
    m_position = obj.m_position;*/
}

Figure::Memento* Composite::save() const
{
	return new Memento(
		m_transform.getPosition(),
		m_transform.getRotation(),
		m_transform.getScale());
}

void Composite::restore(const Memento* memento)
{
	m_transform.setPosition(memento->getPosition());
	m_transform.setRotation(memento->getRotation());
	m_transform.setScale(memento->getScale());
}

Prototype* Composite::clone() 
{
	return (Prototype*)new Composite(*this);
}

Figure* Composite::create()
{
	return new Composite();
}

void Composite::add(Figure* obj)
{
	obj->setParent(this);
	m_composition.push_back(obj);
}

bool Composite::remove(Figure* figure)
{
	for (auto it = m_composition.begin(); it != m_composition.end(); it++)
	{
		if ((*it) == figure)
		{
			m_composition.erase(it);
			return true;
		}
	}
}

void Composite::setOriginByAverage()
{
	sf::Vector2f average;
	for (Figure* elem : m_composition)
	{
		average += elem->getPosition();
	}
	average /= (float)m_composition.size();
	for (Figure* elem : m_composition)
	{
		elem->move(-average);
	}
	m_position = average;
}

void Composite::move(const sf::Vector2f& offset)
{
	m_transform.move(offset);
}

void Composite::reset()
{
	m_composition.clear();
	
}

void Composite::rotate(float degree)
{
	m_transform.rotate(degree);
}

void Composite::scale(const sf::Vector2f& factor, sf::Vector2f centre)
{
	m_transform.scale(factor);
}

const sf::Shape& Composite::getShape() const
{
	throw std::exception("Composite has no shape member.");

}

sf::Shape& Composite::getShape()
{
	throw std::exception("Composite has no shape member.");

}

std::pair<Figure*, Figure*> Composite::getIntersection(const sf::Vector2f& position)
{
	if (m_composition.size() == 0) { throw std::exception("Composition is empty"); }
	
	auto p = m_transform.getInverseTransform().transformPoint(position);
	for (auto it = m_composition.rbegin(); it != m_composition.rend(); it++)
	{
		std::pair<Figure*, Figure*> intersected = (*it)->getIntersection(p);
		if (intersected.second != nullptr)
		{
			return { this, intersected.second };
		}
	}
	return { nullptr, nullptr };
}

void Composite::setActive(bool active)
{
	for (auto elem : m_composition)
	{
		elem->setActive(active);
	}
}

void Composite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!m_visible) { return; }
		
	states.transform = states.transform.combine(m_transform.getTransform());
	
	for (auto it = m_composition.begin(); it != m_composition.end(); it++)
	{
		(*it)->draw(target, states);
	}
}

const Figure* Composite::getChild(size_t index) const
{
	return *std::next(m_composition.begin(), index);
}

void Composite::setTail(bool enabled)
{
    for (auto it = m_composition.begin(); it != m_composition.end(); it++)
    {
        (*it)->setTail(enabled);
    }
}

void Composite::changeColor(sf::Color offset)
{
	m_color_modifier += offset;
	for (auto var : m_composition)
	{
		var->changeColor(offset);
	}
}

void Composite::setColor(const sf::Color& color)
{
	throw std::exception("Composite can't determine it's color.");
}

const sf::Color& Composite::getColor() const
{
	throw std::exception("Composite can't determine it's color.");
}

sf::Vector2f Composite::getWorldPosition()
{
	sf::Vector2f pos = m_transform.getPosition();
	if (m_parent != nullptr)
	{
		pos = m_parent->getWorldPosition(pos);
	}
	return pos;
}

sf::Vector2f Composite::getWorldPosition(sf::Vector2f point)
{
	point = m_transform.getTransform().transformPoint(point);
	if (m_parent != nullptr)
	{
		point = m_parent->getWorldPosition(point);
	}
	return point;
}

const sf::Vector2f& Composite::getPosition() const
{
	return m_transform.getPosition();
}

void Composite::setPosition(float x, float y)
{
	m_transform.setPosition(x, y);
}

void Composite::setPosition(const sf::Vector2f& position)
{
	setPosition(position.x, position.y);
}

const sf::Vector2f& Composite::getScale() const
{
	return m_transform.getScale();
}

void Composite::setScale(float x, float y)
{
	m_transform.setScale(x, y);
}

void Composite::setScale(const sf::Vector2f& scale)
{
	setScale(scale.x, scale.y);
}

float Composite::getRotation() const
{
	return m_transform.getRotation();
}

void Composite::setRotation(float angle)
{
	m_transform.setRotation(angle);
}

const sf::Transform& Composite::getTransform() const
{
	return m_transform.getTransform();
}

size_t Composite::getSize()
{
	return m_composition.size();
}

void Composite::setPrimitiveFactory(FigureFactory* factory)
{
	m_primitiveFigureFactory = factory;
}

Composite::~Composite()
{
	for (auto it = m_composition.begin(); it != m_composition.end(); it++)
	{
		delete* it;
	}
}

std::ostream& Composite::write(std::ostream& os, short tabSize) const
{
	std::string tab(tabSize, '\t');

	os << tab << '\"' << getName() << '\"' << ": {\n";
	tab += '\t';

	const sf::Vector2f& position = m_transform.getPosition();
	os << tab << "\"position\": {\n";
	os << tab << "\t\"x\": " << position.x << '\n';
	os << tab << "\t\"y\": " << position.y << '\n';
	os << tab << "}\n";

	os << tab << "\"rotation\": " << m_transform.getRotation() << '\n';

	const sf::Vector2f& scale = m_transform.getScale();
	os << tab << "\"scale\": {\n";
	os << tab << "\t\"x\": " << scale.x << '\n';
	os << tab << "\t\"y\": " << scale.y << '\n';
	os << tab << "}\n";

	os << tab << "\"elements\": [\n";
	// Write a number of elements composite consist of.
	for (auto elem : m_composition)
	{
		// 2 because of increasing tab string.
		elem->write(os, tabSize + 2);
		os << '\n';
	}
	tab = tab.substr(0, tabSize + 1);
	os << tab << "]\n";
	tab = tab.substr(0, tabSize);
	os << tab << '}';
	return os;
}

std::istream& Composite::read(std::istream& is)
{
	sf::Vector2f position;
	float rotation = 0;
	sf::Vector2f scale;

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
	// Skip empty line and "}" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Skip to rotation value
	is.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	is >> rotation;
	// Skip empty line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Skip "scale": {" line
	//is >> v;
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

	setPosition(position.x, position.y);
	setRotation(rotation);
	setScale(scale.x, scale.y);

	// Skip "elements": [" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	while (true)
	{
		int len = is.tellg();
		// Read line
		std::string line;
		std::getline(is, line);
		if (line.find(']') != line.npos)
		{
			break;
		} // Return to position before read.
		is.seekg(len, std::ios_base::beg);

		Figure* figure = nullptr;
		// Skip to element type
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\"');

		std::string type;
		std::getline(is, type, '\"');
		figure = m_primitiveFigureFactory->create(type);
		add(figure);
		is >> *figure;
	}
	// Skip "}" line
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return is;
}

const std::string& Composite::getName() const
{
	return m_type_name;
}
