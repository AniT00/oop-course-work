#include "SceneController.h"

SceneController* SceneController::Controller_ = nullptr;


void SceneController::add(Figure* figure)
{
	if (m_new_composite != nullptr)
	{
		m_new_composite->add(figure);
		setCurrentFigure(figure);
	}
	else
	{
		throw std::exception("No composite to which could add figure.");
	}
	/*if (m_current_figure != nullptr)
	{
		Figure* composite = m_current_figure;
		if (dynamic_cast<Composite*>(m_current_figure) == nullptr)
		{
			for (int i = 0; i < m_figures.size(); i++)
			{
				if (m_figures[i] == m_current_figure)
				{
					m_figures[i] = composite = new Composite(m_figures[i]);
				}
				else
				{
					composite = m_figures[i]->TurnToComposite(m_current_figure);
				}
				if (composite) { break; }
			}
		}
		composite->add(figure);
	}
	else
	{
		m_figures.push_back(figure);
	}*/
	/*if (m_current_composite == nullptr)
	{
	m_current_composite = new Composite(new Composite(figure));
	m_figures.push_back(m_current_composite);
	}
	else
	{
	m_current_composite->add(new Composite(figure));
	}*/
	//setCurrentFigure(figure);
}

Figure* SceneController::giveAway(Figure* figure)
{
    for (int i = 0; i < m_figures.size(); i++)
    {
        if (m_figures[i] == figure)
        {
            Figure* tmp = m_figures[i];
            m_figures[i] = nullptr;
            return tmp;
        }
    }
    return nullptr;
}

const Figure* SceneController::getCurrentComposite() const
{
	return m_new_composite;
}

void SceneController::createNewComposite()
{
	m_new_composite = new Composite();
	m_figures.push_back(m_new_composite);
}

void SceneController::endNewComposite()
{
	if (m_new_composite != nullptr)
	{
		if (m_new_composite->getSize() == 0)
		{
			delete m_new_composite;
			m_figures[m_figures.size() - 1] = nullptr;
		}
		else
		{
			m_new_composite->setOriginByAverage();
		}
	}
	m_new_composite = nullptr;
}

void SceneController::update()
{
	for (auto var : m_figures)
	{
		if (var != nullptr)
		{
			var->update();
		}
	}
}

void SceneController::draw()
{
	windowTarget->clear(sf::Color::White);
	for (auto var : m_figures)
	{
		if (var != nullptr)
		{
			windowTarget->draw(*var);
		}
	}
	windowTarget->display();
}

void SceneController::setCurrentFigure(Figure* figure)
{
	if (m_current_figure != nullptr)
		m_current_figure->setActive(false);
	m_current_figure = figure;
	if (m_current_figure != nullptr)
		m_current_figure->setActive(true);
	/*
	if (dynamic_cast<Composite*>(figure) == nullptr)
	{
	sf::FloatRect bounds = figure->getShape().getGlobalBounds();
	sf::FloatRect l_bounds = figure->getShape().getLocalBounds();
	}*/
}

void SceneController::resetActiveToDefault()
{
	m_current_figure->reset();
}

void SceneController::setCurrentComposite(Figure* figure)
{
	if (m_new_composite != nullptr)
		m_new_composite->setActive(false);
	m_new_composite = figure;
	if (m_new_composite != nullptr)
		m_new_composite->setActive(false);
	setCurrentFigure(figure);
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
