#pragma once

#include "Composite.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>



class SceneController
{
protected:
	SceneController(sf::RenderWindow* window);

	static SceneController* Controller_;
public:
	// Memento class
	class Snapshot
	{
	private:
		std::list<Figure*> m_figures;
	public:
		Snapshot(std::istream& file);

		Snapshot(std::list<Figure*> figures);

		const std::list<Figure*>& getFigures() const;

		friend std::ostream& operator<<(std::ostream& os, const Snapshot& obj)
		{
			for (auto figure : obj.m_figures)
			{
				os << *figure << '\n';
			}
			return os;
		}
	};

	Snapshot save();

	void restore(const Snapshot& snapshot);

	void add(Figure* figure);

	bool remove(Figure* figure);

	void update();

	void clear();

	void draw();

	void draw(const sf::Shape& shape);

	void drawAxis(sf::Vector2<bool> axis_to_draw);

	void display();

	/// <summary>
	/// Returns pair of figures: first pair element - composite,
	/// which contains second pair element - intersected primitive.
	/// </summary>
	/// <param name="position"></param>
	/// <returns></returns>
	std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position);

	SceneController(SceneController& other) = delete;

	void operator=(const SceneController&) = delete;

	static SceneController* GetInstance(sf::RenderWindow* window);

	~SceneController();
		
private:
	std::list<Figure*> m_figures;
	sf::RectangleShape m_axis;

	sf::RenderWindow* m_window_target;
};


