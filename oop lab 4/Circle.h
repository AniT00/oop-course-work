#pragma once
#include "LeafBase.h"

#include <SFML/Graphics.hpp>

class Circle : public LeafBase
{
public:
	Circle();

	Circle(sf::CircleShape* circleShape);

	Circle(const Circle& obj);

	void reset() override;
};

