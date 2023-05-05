#include "SceneController.h"

#include <limits>

SceneController* SceneController::Controller_ = nullptr;


SceneController::Snapshot SceneController::save()
{
	return Snapshot(m_figures);
}

void SceneController::restore(const Snapshot& snapshot)
{
	m_figures.clear();
	for (auto figure : snapshot.getFigures())
	{
		m_figures.push_back((Figure*)figure->clone());
	}
}

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

SceneController::Snapshot::Snapshot(std::istream& file)
{
	while (file.peek() != EOF)
	{
		// Already know this will be composite. Skip type name.
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		Composite* composite = new Composite();
		m_figures.push_back(composite);
		file >> *composite;
		file.get(); // '\n'
	}
}

SceneController::Snapshot::Snapshot(std::list<Figure*> figures)
{
	for (auto figure : figures)
	{
		m_figures.push_back((Figure*)figure->clone());
	}
}

const std::list<Figure*>& SceneController::Snapshot::getFigures() const
{
	return m_figures;
}
