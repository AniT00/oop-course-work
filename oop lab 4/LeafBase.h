#pragma once
#include "Figure.h"

#include <list>

class LeafBase : public Figure
{
public:
	virtual void move(const sf::Vector2f& offset) override;

	void rememberTransform();

	virtual void rotate(float degree) override;

	virtual void scale(const sf::Vector2f& absolute_value, sf::Vector2f centre = sf::Vector2f()) override;

	virtual void setActive(bool active) override;

	virtual Figure* TurnToComposite(Figure* figure);

	virtual std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position) override;

	virtual void setPosition(float x, float y) override
	{
		m_shape->setPosition(x, y);
	}

	virtual sf::Vector2f getPosition() override;

	const sf::Shape& getShape() const override;

	virtual sf::Shape& getShape();

	virtual void changeColor(sf::Color offset);

	virtual void setTail(bool enabled) override;

	virtual void setColor(sf::Color color) override;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


	~LeafBase();

protected:
	std::list<sf::Transform> m_prev_positions;
	sf::Shape* m_shape;
	int m_trail;
};

