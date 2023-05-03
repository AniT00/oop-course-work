#pragma once
#include "LeafBase.h"

class Triangle : public LeafBase
{
public:
	Triangle();

	Triangle(sf::CircleShape* rectangleShape);

	Triangle(const Triangle& obj);

	Prototype* clone() override;

	void reset() override;

	std::ostream& write(std::ostream& os) const override;

	//std::istream& read(std::istream& is) override;
};

