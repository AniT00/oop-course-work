#pragma once
#include "Figure.h"

#include <list>

class PrimitiveFigure : public Figure
{
public:
	/*
	* When creating new primitive, base class should get shape to initialize common m_shape properties. 
	*/
	PrimitiveFigure(sf::Shape* shape);

	void move(const sf::Vector2f& offset) override;

	void rememberTransform();

	void rotate(float degree) override;

	void scale(const sf::Vector2f& absolute_value, sf::Vector2f centre = sf::Vector2f()) override;

	void setActive(bool active) override;

	std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position) override;

	const sf::Shape& getShape() const override;

	virtual sf::Shape& getShape();

	virtual void changeColor(sf::Color offset);

	void setTail(bool enabled) override;

	void setColor(sf::Color color) override;
	
	const sf::Color& getColor() const override;

	const sf::Vector2f& getPosition() const override;

	void setPosition(float x, float y) override;

	void setPosition(const sf::Vector2f& position) override;

	const sf::Vector2f& getScale() const override;

	void setScale(float x, float y) override;

	void setScale(const sf::Vector2f& scale) override;

	float getRotation() const override;

	void setRotation(float angle) override;

	const sf::Transform& getTransform() const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	~PrimitiveFigure();

protected:
	std::ostream& write(std::ostream& os) const override;

	std::istream& read(std::istream& is) override;

	std::list<sf::Transform> m_prev_positions;

	// Dissallow changing of shape object: ptr can be initialized only in base constructor.
	sf::Shape* const m_shape;
};

