#pragma once

#include "Composite.h"

#include <SFML/Graphics.hpp>
#include <iostream>

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

	Figure* giveAway(Figure* figure);

	const Figure* getCurrentComposite() const;

	void createNewComposite();

	void deleteNewComposite()
	{
		delete m_new_composite;
		m_figures[m_figures.size() - 1] = nullptr;
		m_new_composite = nullptr;
		m_current_figure = nullptr;
	}

	void endNewComposite();

	void update();

	void draw();

	void setCurrentFigure(Figure* figure);

	void resetActiveToDefault();

	void setCurrentComposite(Figure* figure);

	std::pair<Figure*, Figure*> getIntersection(const sf::Vector2f& position);

	Figure* getActiveFigure();

	SceneController(SceneController& other) = delete;

	void operator=(const SceneController&) = delete;

	static SceneController* GetInstance(sf::RenderWindow* window);

	~SceneController();
		
private:
	std::vector<Figure*> m_figures;

	sf::RenderWindow* windowTarget;

	Figure* m_current_figure = nullptr;

	Figure* m_new_composite = nullptr;
};


