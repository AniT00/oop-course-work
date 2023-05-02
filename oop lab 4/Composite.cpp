#include "Composite.h"

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

Prototype* Composite::clone() 
{
	return (Prototype*)new Composite(*this);
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
	m_transform.translate(offset);
}

//Figure* Composite::TurnToComposite(Figure* figure)
//{
//	for (auto elem : m_composition)
//	{
//		if (figure == elem)
//		{
//			auto f = elem;
//			elem = new Composite(elem);
//			f->getShape().setPosition(0, 0);
//			return elem;
//		}
//		Figure* tmp = elem->TurnToComposite(figure);
//		if (tmp)
//		{
//			return tmp;
//		}
//	}
//	return nullptr;
//}

void Composite::reset()
{
	m_composition.clear();
	
}

void Composite::rotate(float degree)
{
	m_transform.rotate(degree);
}

void Composite::scale(const sf::Vector2f& absolute_value, sf::Vector2f centre)
{
	m_transform.scale(
		1.f + absolute_value.x / (abs(m_scale.x) * 100),
		1.f + absolute_value.y / (abs(m_scale.y) * 100)
	);
	//m_scale.x *= 1.f + absolute_value.x / (abs(m_scale.x) * 100);
	//m_scale.y *= 1.f + absolute_value.y / (abs(m_scale.y) * 100);

	/////////////
	/*for (auto it = m_composition.begin(); it != m_composition.end(); it++)
	{
		(*it)->scale(absolute_value, m_position);
	}*/
}

sf::Vector2f Composite::getPosition()
{
	return m_position;
}

const sf::Shape& Composite::getShape() const
{
	throw std::exception("Composite has no shape member.");

}

sf::Shape& Composite::getShape()
{
	throw std::exception("Composite has no shape member.");

}

void Composite::setPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}


std::pair<Figure*, Figure*> Composite::getIntersection(const sf::Vector2f& position)
{
	if (m_composition.size() == 0) { throw std::exception("Composition is empty"); }
	
	auto p = sf::Transform().translate(m_position).rotate(m_rotation).getInverse().transformPoint(position);
	for (auto it = m_composition.rbegin(); it != m_composition.rend(); it++)
	{
		std::pair<Figure*, Figure*> intersected = (*it)->getIntersection(p);
		if (intersected.second != nullptr)
		{
			/*if (intersected.first)
				return intersected;
			else*/
				return { this, intersected.second };
		}
	}
	return { nullptr, nullptr };
}

void Composite::setActive(bool active)
{
	m_active = active;
}

void Composite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!m_visible) { return; }
	if (m_active)
	{
		if (pulse >= ACTIVE_OBJECT_PULSE_TIME)
		{
			if (pulse_state.colorEquation == sf::BlendMode::ReverseSubtract)
			{
				pulse_state.colorDstFactor = sf::BlendMode::Zero;
				pulse_state.colorEquation = sf::BlendMode::Add;
			}
			else
			{
				pulse_state.colorDstFactor = sf::BlendMode::One;
				pulse_state.colorEquation = sf::BlendMode::ReverseSubtract;
			}
			pulse = 0;
		}
		else
		{
			pulse++;
		}
		states.blendMode = pulse_state;
	}
	//states.transform.combine(m_transform);
	states.transform = states.transform.combine(m_transform);
	//states.transform.translate(m_position);
	//states.transform.scale(m_scale);
	//states.transform.rotate(m_rotation);
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
	for (auto var : m_composition)
	{
		var->changeColor(offset);
	}
}

void Composite::setColor(sf::Color color)
{
	for (auto it = m_composition.begin(); it != m_composition.end(); it++)
	{
		(*it)->setColor(color);
	}
}

size_t Composite::getSize()
{
	return m_composition.size();
}

Composite::~Composite()
{
	for (auto it = m_composition.begin(); it != m_composition.end(); it++)
	{
		delete* it;
	}
}
