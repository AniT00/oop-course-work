#include "SceneController.h"

SceneController* SceneController::Controller_ = nullptr;


void SceneController::add(Figure* figure)
{
	m_figures.push_back(figure);
}

bool SceneController::remove(Figure* figure)
{
	return m_figures.remove(figure) == 1; // TODO
}

void SceneController::update()
{
	for (auto var : m_figures)
	{
		var->update();
	}
}

void SceneController::draw()
{
	windowTarget->clear(sf::Color::White);
	for (auto var : m_figures)
	{
		windowTarget->draw(*var);
	}
	windowTarget->display();
}

std::pair<Figure*, Figure*> SceneController::getIntersection(const sf::Vector2f& position)
{
	for (auto it = m_figures.rbegin(); it != m_figures.rend(); it++)
	{
		std::pair<Figure*, Figure*> intersected = (*it)->getIntersection(position);
		if (intersected.second)
		{
			return intersected;
		}
	}
	return { nullptr, nullptr };
}

Figure* SceneController::getActiveFigure()
{
    return m_current_figure;
}

SceneController* SceneController::GetInstance(sf::RenderWindow* window)
{
	if (Controller_ == nullptr)
	{
		Controller_ = new SceneController(window);
	}
	return Controller_;
}

SceneController::~SceneController()
{
	for (auto it = m_figures.begin(); it != m_figures.end(); it++)
	{
		delete* it;
	}
}
