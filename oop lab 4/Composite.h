#pragma once
#include "Figure.h"

class Composite : public Figure
{
public:
	Composite();

	Composite(Figure* figure);

	void add(Figure* obj) override;

	bool remove(Figure* figure) override;
	
	void setOriginByAverage() override;

	void move(const sf::Vector2f& offset) override;

	Figure* TurnToComposite(Figure* figure) override;

	void reset() override;

	virtual void rotate(float degree) override;

	virtual void scale(const sf::Vector2f& absolute_value, sf::Vector2f centre = sf::Vector2f()) override;

	virtual sf::Vector2f getPosition() override;

	virtual const sf::Shape& getShape() const;

	virtual sf::Shape& getShape();

	virtual void setPosition(float x, float y) override;

	std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position) override;

	void setActive(bool active) override;

	virtual void changeColor(sf::Color offset) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	virtual const Figure* getChild(size_t index) const override;

	void setTail(bool enabled) override;

	void setColor(sf::Color color) override;

	virtual size_t getSize() override
	{
		return m_composition.size();
	}

	~Composite();

private:
	std::vector<Figure*> m_composition;
	sf::Vector2f m_scale = sf::Vector2f(1.f,1.f);
	sf::Vector2f m_size = sf::Vector2f(1.f, 1.f);
	sf::Vector2f m_position;
	float m_rotation = 0;
};

