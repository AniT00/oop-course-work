#pragma once
#include "Prototype.h"

#include <SFML/Graphics.hpp>

#ifdef FRAME_LIMIT
	#define ACTIVE_OBJECT_PULSE_TIME FRAME_LIMIT
#else
	#define ACTIVE_OBJECT_PULSE_TIME 60
#endif // FRAME_LIMIT

class Figure;

std::ostream& operator<< (std::ostream& os, const Figure& obj);

std::istream& operator>> (std::istream& is, Figure& obj);

class Figure :
	public sf::Drawable,
	public Prototype
{
public:
	virtual ~Figure() {};

	virtual void add(Figure* figure);

	virtual bool remove(Figure* figure);

	virtual void update();

	virtual void setOriginByAverage();

	virtual void move(const sf::Vector2f& offset) = 0;

	virtual void rotate(float degree) = 0;

	virtual void scale(const sf::Vector2f& absolute_value, sf::Vector2f centre = sf::Vector2f()) = 0;

	virtual void reset() = 0;

	virtual void changeTail();

	virtual void changeVisibility();

	virtual void setVisible(bool _visible);

	virtual bool getVisible(bool) const;

	virtual void changeColor(sf::Color offset) = 0;

	virtual const sf::Shape& getShape() const = 0;

	virtual sf::Shape& getShape() = 0;

	virtual void setActive(bool active) = 0;

	virtual std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position);;

	virtual void changeMovement();

	virtual void setTail(bool enabled) = 0;

	virtual void setColor(sf::Color color) = 0;

	virtual const sf::Color& getColor() const = 0;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;

	virtual size_t getSize();

	virtual const sf::Vector2f& getPosition() const = 0;

	virtual void setPosition(float x, float y) = 0;

	virtual void setPosition(const sf::Vector2f& position) = 0;

	virtual const sf::Vector2f& getScale() const = 0;

	virtual void setScale(float x, float y) = 0;

	virtual void setScale(const sf::Vector2f& scale) = 0;

	virtual float getRotation() const = 0;

	virtual void setRotation(float angle) = 0;

	virtual const sf::Transform& getTransform() const = 0;

	virtual const Figure* getParent() const;

	virtual void setParent(Figure* parent);

	virtual const Figure* getChild(size_t index) const;

	friend std::ostream& operator<< (std::ostream& os, const Figure& obj);

	friend std::istream& operator>> (std::istream& is, Figure& obj);

protected:
	virtual std::ostream& write(std::ostream& os) const = 0;
	
	virtual std::istream& read(std::istream& is) = 0;

	virtual const std::string& getName() const = 0;

	Figure* m_parent = nullptr;
	bool m_visible = true;
	bool m_tail;
	bool m_move_automatically;
};



