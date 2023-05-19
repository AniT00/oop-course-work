#include "SceneController.h"

#include <limits>

SceneController* SceneController::Controller_ = nullptr;

SceneController::SceneController(sf::RenderWindow* window)
{
	m_window_target = window;
	m_axis.setSize(sf::Vector2f(2.f, 1e5));
	m_axis.setOrigin(m_axis.getSize() / 2.f);
}

SceneController::Memento SceneController::save()
{
	return Memento(m_figures);
}

void SceneController::restore(const Memento& snapshot)
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

void SceneController::clear()
{
	m_window_target->clear(sf::Color::White);
}

void SceneController::draw()
{
	for (auto var : m_figures)
	{
		m_window_target->draw(*var);
	}
}

void SceneController::draw(const sf::Shape& shape)
{
	m_window_target->draw(shape);
}

void SceneController::drawAxis(sf::Vector2<bool> axis_to_draw)
{
	if (axis_to_draw.x)
	{
		m_axis.setRotation(90);
		m_axis.setFillColor(sf::Color::Red);
		m_window_target->draw(m_axis);
	}
	if (axis_to_draw.y)
	{
		m_axis.setRotation(0);
		m_axis.setFillColor(sf::Color::Green);
		m_window_target->draw(m_axis);
	}
}

void SceneController::display()
{
	m_window_target->display();
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

SceneController::Memento::Memento(std::istream& file)
{
	// Skip "[" line
	file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	while (file.peek() != EOF)
	{
		int len = file.tellg();
		// Read line
		std::string line;
		std::getline(file, line);
		if (line.find(']') != line.npos)
		{
			break;
		} // Return to position before read.
		file.seekg(len, std::ios_base::beg);

		// Already know this will be composite. Skip type name.
		file.ignore(std::numeric_limits<std::streamsize>::max(), '{');
		Composite* composite = new Composite();
		m_figures.push_back(composite);
		file >> *composite;
	}
}

SceneController::Memento::Memento(std::list<Figure*> figures)
{
	for (auto figure : figures)
	{
		m_figures.push_back((Figure*)figure->clone());
	}
}

const std::list<Figure*>& SceneController::Memento::getFigures() const
{
	return m_figures;
}
