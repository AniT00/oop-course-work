#pragma once
#include "LeafBase.h"

class Rectangle : public LeafBase
{
public:
	Rectangle();
	
	Rectangle(sf::RectangleShape* rectangleShape);

	Rectangle(const Rectangle& obj);

	Prototype* clone() override;

	void reset() override;

	std::ostream& write(std::ostream& os) const override;

	//std::istream& read(std::istream& is) override;
};

