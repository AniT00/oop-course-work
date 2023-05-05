#pragma once
#include "LeafBase.h"

#include <unordered_map>
#include <string>
#include <functional>

class FigureFactory
{
public:
	void registerType(std::string typeName, std::function<Figure*()> ctor);

	Figure* create(const std::string& typeName);

private:
	std::unordered_map<std::string, std::function<Figure*()>> m_types;
};

