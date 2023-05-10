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
	t = obj.t;
    /*m_scale = obj.m_scale;
    m_size = obj.m_size;
    m_position = obj.m_position;*/
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
	t.move(offset);
}

void Composite::reset()
{
	m_composition.clear();
	
}

void Composite::rotate(float degree)
{
	t.rotate(degree);
}

void Composite::scale(const sf::Vector2f& factor, sf::Vector2f centre)
{
	t.scale(factor);
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
	
	auto p = t.getInverseTransform().transformPoint(position);
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
		
	states.transform = states.transform.combine(t.getTransform());
	
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

const sf::Vector2f& Composite::getPosition() const
{
	return t.getPosition();
}

void Composite::setPosition(float x, float y)
{
	t.setPosition(x, y);
}

void Composite::setPosition(const sf::Vector2f& position)
{
	setPosition(position.x, position.y);
}

const sf::Vector2f& Composite::getScale() const
{
	return t.getScale();
}

void Composite::setScale(float x, float y)
{
	t.setScale(x, y);
}

void Composite::setScale(const sf::Vector2f& scale)
{
	setScale(scale.x, scale.y);
}

float Composite::getRotation() const
{
	return t.getRotation();
}

void Composite::setRotation(float angle)
{
	t.setRotation(angle);
}

const sf::Transform& Composite::getTransform() const
{
	return t.getTransform();
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

std::ostream& Composite::write(std::ostream& os) const
{
	os << '\n';
	const std::string& name = getName();
	os.write(name.c_str(), name.size());
	os << '\n';

	os.write((char*)&t.getPosition(), sizeof(sf::Vector2f));
	float rotation = t.getRotation();
	os.write((char*)&rotation, sizeof(float));
	os.write((char*)&t.getScale(), sizeof(sf::Vector2f));
	// Write a number of elements composite consist of.
	size_t size = m_composition.size();
	os.write((char*)&size, sizeof(size));
	for (auto elem : m_composition)
	{
		os << *elem;
	}
	return os;
}

std::istream& Composite::read(std::istream& is)
{
	sf::Vector2f position;
	is.read((char*)&position, sizeof(position));

	float rotation = getRotation();
	is.read((char*)&rotation, sizeof(rotation));

	sf::Vector2f scale;
	is.read((char*)&scale, sizeof(scale));

	setPosition(position.x, position.y);
	setRotation(rotation);
	setScale(scale.x, scale.y);

	size_t figure_count;
	is.read((char*)&figure_count, sizeof(figure_count));
	for (size_t i = 0; i < figure_count; i++)
	{
		Figure* figure = nullptr;

		is.ignore(1, '\n');
		std::string type;
		getline(is, type);
		std::cout << is.tellg() << std::endl;
		try
		{
			figure = m_primitiveFigureFactory->create(type);
		}
		catch (const std::exception& e)
		{
			std::cout << "Error while reading. Wrong type string. The rest of the buffer:\n";
			char c;
			while (is >> c)
			{
				std::cout << std::hex << (int)c << ' ';
			}
			throw e;
		}
		add(figure);
		is >> *figure;
	}
	return is;
}

const std::string& Composite::getName() const
{
	return m_type_name;
}
