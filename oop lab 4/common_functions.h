#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <SFML/Graphics.hpp>
#include <unordered_map>

template<typename First, typename ... T>
inline bool is_in(First&& first, T && ... t)
{
	// Use of fold expression
	return ((first == t) || ...);
}

inline float vector_length(const sf::Vector2f& vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

inline float get_clockwise_angle(sf::Vector2f first, sf::Vector2f second)
{
	float dot = first.x * second.x + first.y * second.y;
	float det = first.x * second.y - first.y * second.x;
	return atan2f(det, dot) * 180 / M_PI;
}
namespace menu_tools
{
	extern const std::unordered_map<sf::Keyboard::Key, const char*> key_names;
}