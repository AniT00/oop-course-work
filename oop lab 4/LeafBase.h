#pragma once
#include "Figure.h"

#include <list>

class LeafBase : public Figure
{
public:
	void move(const sf::Vector2f& offset) override;

	void rememberTransform();

	void rotate(float degree) override;

	void scale(const sf::Vector2f& absolute_value, sf::Vector2f centre = sf::Vector2f()) override;

	void setActive(bool active) override;

	//virtual Figure* TurnToComposite(Figure* figure);

	std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position) override;

	void setPosition(float x, float y) override;

	sf::Vector2f getPosition() override;

	const sf::Shape& getShape() const override;

	virtual sf::Shape& getShape();

	virtual void changeColor(sf::Color offset);

	void setTail(bool enabled) override;

	void setColor(sf::Color color) override;

	const sf::Transform& getTransform() const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	~LeafBase();

protected:
	std::list<sf::Transform> m_prev_positions;
	sf::Shape* m_shape;
	int m_trail;
};

