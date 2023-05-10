#pragma once
#include "PrimitiveFigure.h"

class Circle : public PrimitiveFigure
{
public:
	Circle();

	Circle(const Circle& obj);

	Prototype* clone() override;

	static Figure* create();

	void reset() override;

private:
	const std::string& getName() const override;

	static std::string m_type_name;
};

