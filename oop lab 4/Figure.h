#pragma once
#include <SFML/Graphics.hpp>

#ifdef FRAME_LIMIT
	#define ACTIVE_OBJECT_PULSE_TIME FRAME_LIMIT
#else
	#define ACTIVE_OBJECT_PULSE_TIME 60
#endif // FRAME_LIMIT

class Figure : public sf::Drawable
{
public:
	virtual ~Figure() {};

	virtual void add(Figure* figure)
	{
		throw std::exception("Can't add to leaf.");
	}

	virtual bool remove(Figure* figure)
	{
		throw std::exception("Can't remove from leaf.");
	}

	virtual void update()
	{
		if (m_move_automatically)
		{
			move(sf::Vector2f(-1.f, 1.f));
		}
	}

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

	virtual sf::Vector2f getPosition() = 0;

	virtual const sf::Shape& getShape() const = 0;

	virtual sf::Shape& getShape() = 0;

	virtual void setActive(bool active) = 0;

	virtual Figure* TurnToComposite(Figure* figure) = 0;

	virtual std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position);;

	virtual void setPosition(float x, float y) = 0;

	virtual void changeMovement();

	virtual void setTail(bool enabled) = 0;

	virtual void setColor(sf::Color color) = 0;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;

	virtual size_t getSize();

	virtual const Figure* getChild(size_t index) const;

protected:
	bool m_active = false;
	static sf::BlendMode pulse_state;
	static int pulse;
	bool m_visible = true;
	bool m_tail;
	bool m_move_automatically;
};



