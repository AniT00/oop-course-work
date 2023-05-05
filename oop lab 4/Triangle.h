#pragma once
#include "LeafBase.h"

class Triangle : public PrimitiveFigure
{
public:
	Triangle();

	Triangle(sf::CircleShape* rectangleShape);

	Triangle(const Triangle& obj);

	Prototype* clone() override;

	static Figure* create();

	void reset() override;

private:
	const std::string& getName() const override;

	static std::string m_type_name;

	//std::ostream& write(std::ostream& os) const override;

	//std::istream& read(std::istream& is) override;
};

