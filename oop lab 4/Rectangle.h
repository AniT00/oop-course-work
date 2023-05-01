#pragma once
#include "LeafBase.h"

#include <SFML/Graphics.hpp>

class Rectangle : public LeafBase
{
public:
	Rectangle();
	
	Rectangle(sf::RectangleShape* rectangleShape);

	Rectangle(const Rectangle& obj);

	void reset() override;
};

