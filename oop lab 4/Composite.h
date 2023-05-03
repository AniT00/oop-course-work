#pragma once
#include "Figure.h"

#include <list>

class Composite : public Figure
{
public:
	Composite();

	Composite(Figure* figure);

	Composite(const Composite& obj);

	Prototype* clone() override;

	void add(Figure* obj) override;

	bool remove(Figure* figure) override;
	
	void setOriginByAverage() override;

	void move(const sf::Vector2f& offset) override;

	//Figure* TurnToComposite(Figure* figure) override;

	void reset() override;

	void rotate(float degree) override;

	void scale(const sf::Vector2f& absolute_value, sf::Vector2f centre = sf::Vector2f()) override;

	sf::Vector2f getPosition() override;

	virtual const sf::Shape& getShape() const;

	virtual sf::Shape& getShape();

	void setPosition(float x, float y) override;

	std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position) override;

	void setActive(bool active) override;

	void changeColor(sf::Color offset) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	const Figure* getChild(size_t index) const override;

	void setTail(bool enabled) override;

	void setColor(sf::Color color) override;

	const sf::Vector2f& getPosition() const override;

	const sf::Vector2f& getScale() const override;

	float getRotation() const override;

	const sf::Transform& getTransform() const override;

	size_t getSize() override;

	~Composite();

private:
	std::ostream& write(std::ostream& os) const override;

	std::istream& read(std::istream& is) override;

	std::list<Figure*> m_composition;
	//TODO
	//sf::Transform m_transform;
	sf::Vector2f m_scale = sf::Vector2f(1.f,1.f);
	sf::Vector2f m_size = sf::Vector2f(1.f, 1.f);
	sf::Vector2f m_position = sf::Vector2f(0.f, 0.f);
	mutable sf::Transformable t;

	float m_rotation = 0;
};

