#include "FigureFactory.h"

void FigureFactory::registerType(std::string typeName, std::function<Figure*()> ctor) {
	m_types[typeName] = ctor;
}

Figure* FigureFactory::create(const std::string& typeName) {
	return m_types[typeName]();
}
