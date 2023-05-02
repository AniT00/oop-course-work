#pragma once
#include "LeafBase.h"

class Triangle :
    public LeafBase
{
public:
	Triangle();

	Triangle(sf::CircleShape* rectangleShape);

	Triangle(const Triangle& obj);

	Prototype* clone();

	void reset() override;
};

