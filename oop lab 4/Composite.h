#pragma once
#include "Figure.h"
#include "FigureFactory.h"

#include <list>

class Composite : public Figure
{
public:
	Composite();

	Composite(Figure* figure);

	Composite(const Composite& obj);

	Prototype* clone() override;

	static Figure* create();

	void add(Figure* obj) override;

	bool remove(Figure* figure) override;
	
	void setOriginByAverage() override;

	void move(const sf::Vector2f& offset) override;

	void reset() override;

	void rotate(float degree) override;

	void scale(const sf::Vector2f& absolute_value, sf::Vector2f centre = sf::Vector2f()) override;

	virtual const sf::Shape& getShape() const;

	virtual sf::Shape& getShape();

	std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position) override;

	void setActive(bool active) override;

	void changeColor(sf::Color offset) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	const Figure* getChild(size_t index) const override;

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

	const sf::Transform& getTransform() const override;

	size_t getSize() override;

	static void setPrimitiveFactory(FigureFactory* factory);

	~Composite();

private:
	std::ostream& write(std::ostream& os) const override;

	std::istream& read(std::istream& is) override;

	const std::string& getName() const override;

	static std::string m_type_name;

	std::list<Figure*> m_composition;
	//TODO
	sf::Vector2f m_scale = sf::Vector2f(1.f,1.f);
	sf::Vector2f m_size = sf::Vector2f(1.f, 1.f);
	sf::Vector2f m_position = sf::Vector2f(0.f, 0.f);
	sf::Color m_color_modifier;
	mutable sf::Transformable t;

	static FigureFactory* m_primitiveFigureFactory;

	float m_rotation = 0;
};

