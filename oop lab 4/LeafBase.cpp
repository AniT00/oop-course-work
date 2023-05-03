#include "LeafBase.h"

#include <iostream>

void LeafBase::move(const sf::Vector2f& offset)
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

void LeafBase::rememberTransform()
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

void LeafBase::rotate(float degree)
{
    rememberTransform();
    m_shape->rotate(degree);
}

void LeafBase::scale(const sf::Vector2f& absolute_value, sf::Vector2f centre)
{
    auto tmp = m_shape->getGlobalBounds();
    m_shape->scale(sf::Vector2f
    (
        1.f + absolute_value.x / tmp.width,
        1.f + absolute_value.y / tmp.height
    ));
}

void LeafBase::setActive(bool active)
{
    m_active = active;
}

//Figure* LeafBase::TurnToComposite(Figure* figure)
//{
//    return nullptr;
//}

std::pair<Figure*, Figure*> LeafBase::getIntersection(const sf::Vector2f& position)
{
    auto v = m_shape->getTransform().getInverse().transformPoint(position);
    //auto tmp = parent_transform.transformRect(m_shape->getLocalBounds());
    if (m_shape->getLocalBounds().contains(v))
    {
        return { nullptr, this };
    }
    return { nullptr, nullptr };
}

void LeafBase::setPosition(float x, float y)
{
    m_shape->setPosition(x, y);
}

sf::Vector2f LeafBase::getPosition()
{
    return m_shape->getPosition();
}

const sf::Shape& LeafBase::getShape() const
{
    return *m_shape;
}

sf::Shape& LeafBase::getShape()
{
    return *m_shape;
}

void LeafBase::changeColor(sf::Color offset)
{
    sf::Color color = m_shape->getFillColor();
    color.r += offset.r;
    color.g += offset.g;
    color.b += offset.b;
    m_shape->setFillColor(color);
}

void LeafBase::setTail(bool enabled)
{
    m_trail = enabled;
}

void LeafBase::setColor(sf::Color color)
{
    m_shape->setFillColor(color);
}

const sf::Vector2f& LeafBase::getPosition() const
{
	return m_shape->getPosition();
}

const sf::Vector2f& LeafBase::getScale() const
{
	return m_shape->getScale();
}

float LeafBase::getRotation() const
{
	return m_shape->getRotation();
}

const sf::Transform& LeafBase::getTransform() const
{
	return m_shape->getTransform();
}

void LeafBase::draw(sf::RenderTarget& target, sf::RenderStates states) const
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

LeafBase::~LeafBase()
{
    delete m_shape;
}

//std::ostream& LeafBase::write(std::ostream& os) const
//{
//	const char* name = getName();
//	os.write(name, strlen(name) + 1);
//	const sf::Color& color = m_shape->getFillColor();
//	os.write((char*)&color, sizeof(sf::Color));
//	const float* transform_matrix = m_shape->getTransform().getMatrix();
//	os.write((char*)transform_matrix, 16 * sizeof(float));
//	return os;
//}
//
std::istream& LeafBase::read(std::istream& is)
{
	sf::Color color;
	is.read((char*)&color, sizeof(sf::Color));
	m_shape->setFillColor(color);
	float transform_matrix[16];
	is.read((char*)&transform_matrix, 16 * sizeof(float));
	// TODO matrix to transform.
	return is;
}
