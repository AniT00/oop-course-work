#include "LeafBase.h"

#include <iostream>

void PrimitiveFigure::move(const sf::Vector2f& offset)
{
    rememberTransform();
	
	if (m_parent != nullptr)
	{
		sf::Transform n = sf::Transform().rotate(-m_parent->getRotation());
		sf::Vector2f m = n.transformPoint(offset);
		// TODO check if statement necessity
		// fix this
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

void PrimitiveFigure::scale(const sf::Vector2f& absolute_value, sf::Vector2f centre)
{
    auto tmp = m_shape->getGlobalBounds();
    m_shape->scale(sf::Vector2f
    (
        1.f + absolute_value.x / tmp.width,
        1.f + absolute_value.y / tmp.height
    ));
}

void PrimitiveFigure::setActive(bool active)
{
    m_active = active;
}

//Figure* LeafBase::TurnToComposite(Figure* figure)
//{
//    return nullptr;
//}

std::pair<Figure*, Figure*> PrimitiveFigure::getIntersection(const sf::Vector2f& position)
{
    auto v = m_shape->getTransform().getInverse().transformPoint(position);
    //auto tmp = parent_transform.transformRect(m_shape->getLocalBounds());
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
    m_trail = enabled;
}

void PrimitiveFigure::setColor(sf::Color color)
{
    m_shape->setFillColor(color);
}

const sf::Vector2f& PrimitiveFigure::getPosition() const
{
	return m_shape->getPosition();
}

void PrimitiveFigure::setPosition(float x, float y)
{
	m_shape->setPosition(x, y);
}

const sf::Vector2f& PrimitiveFigure::getScale() const
{
	return m_shape->getScale();
}

void PrimitiveFigure::setScale(float x, float y)
{
	m_shape->setScale(x, y);
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

std::ostream& PrimitiveFigure::write(std::ostream& os) const
{
	os << '\n';
	const std::string& name = getName();
	os.write(name.c_str(), name.size());
	os << '\n';

	sf::Vector2f position = m_shape->getPosition();
	os.write((char*)&position, sizeof(sf::Vector2f));

	float rotation = m_shape->getRotation();
	os.write((char*)&rotation, sizeof(float));

	sf::Vector2f scale = m_shape->getScale();
	os.write((char*)&scale, sizeof(sf::Vector2f));

	const sf::Color& color = m_shape->getFillColor();
	os.write((char*)&color, sizeof(sf::Color));

	return os;
}

std::istream& PrimitiveFigure::read(std::istream& is)
{
	sf::Vector2f position;
	is.read((char*)&position, sizeof(sf::Vector2f));

	float rotation = m_shape->getRotation();
	is.read((char*)&rotation, sizeof(float));

	sf::Vector2f scale;
	is.read((char*)&scale, sizeof(sf::Vector2f));

	sf::Color color;
	is.read((char*)&color, sizeof(sf::Color));

	m_shape->setPosition(position);
	m_shape->setRotation(rotation);
	m_shape->setScale(scale);
	m_shape->setFillColor(color);

	return is;
}
