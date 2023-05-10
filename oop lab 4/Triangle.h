#pragma once
#include "PrimitiveFigure.h"

class Triangle : public PrimitiveFigure
{
public:
	Triangle();

	Triangle(const Triangle& obj);

	Prototype* clone() override;

	static Figure* create();

	void reset() override;

private:
	const std::string& getName() const override;

	static std::string m_type_name;
};

