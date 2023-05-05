#pragma once
#include "LeafBase.h"

class Circle : public PrimitiveFigure
{
public:
	Circle();

	Circle(sf::CircleShape* circleShape);

	Circle(const Circle& obj);

	Prototype* clone() override;

	static Figure* create();

	void reset() override;

private:
	const std::string& getName() const override;

	static std::string m_type_name;
	//std::ostream& write(std::ostream& os) const override;

	//std::istream& read(std::istream& is) override;
};

