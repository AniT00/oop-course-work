#pragma once
#include "PrimitiveFigure.h"

class Rectangle : public PrimitiveFigure
{
public:
	Rectangle();
	
	Rectangle(const Rectangle& obj);

	Prototype* clone() override;

	static Figure* create();

	void reset() override;

private:
	const std::string& getName() const override;

	static std::string m_type_name;
};

