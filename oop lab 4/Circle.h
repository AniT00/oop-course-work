#pragma once
#include "LeafBase.h"

class Circle : public LeafBase
{
public:
	Circle();

	Circle(sf::CircleShape* circleShape);

	Circle(const Circle& obj);

	Prototype* clone() override;

	void reset() override;

	std::ostream& write(std::ostream& os) const override;

	//std::istream& read(std::istream& is) override;
};

