#pragma once

#include "Composite.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>

class SceneController
{
protected:
	SceneController(sf::RenderWindow* window)
	{
		windowTarget = window;
	}

	static SceneController* Controller_;


public:
	void add(Figure* figure);

	bool remove(Figure* figure);

	void update();

	void draw();

	/// <summary>
	/// Returns pair of figures: first pair element - composite,
	/// which contains second pair element - intersected primitive.
	/// </summary>
	/// <param name="position"></param>
	/// <returns></returns>
	std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position);

	Figure* getActiveFigure();

	SceneController(SceneController& other) = delete;

	void operator=(const SceneController&) = delete;

	static SceneController* GetInstance(sf::RenderWindow* window);

	~SceneController();
		
private:
	std::list<Figure*> m_figures;

	sf::RenderWindow* windowTarget;

	Figure* m_current_figure = nullptr;

	Figure* m_new_composite = nullptr;
};


