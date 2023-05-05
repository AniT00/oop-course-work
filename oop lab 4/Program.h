#pragma once

#define FRAME_LIMIT	60
#include "SceneController.h"
#include "Menu.h"

#include "Circle.h"
#include "Rectangle.h"
#include "Triangle.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <fstream>

#define SCENE_WINDOW_WIDTH	720
#define SCENE_WINDOW_HEIGHT	480

#define HIERARCHY_WINDOW_WIDTH	320
#define HIERARCHY_WINDOW_HEIGHT	640



enum class InputMode
{
	MAIN,
	ADD_FIGURE,
	EDIT_OBJECT,
	EXPAND_COMPOSITE,
	UNITE_COMPOSITES,
	VIEW_PROTOTYPES
};

enum class ObjectManipulation
{
	NONE,
	MOVING,
	ROTATING,
	SCALING,
	COLORING
};

class Program
{
public:
	Program();

	void run();

	~Program();

private:
	void handleObjectManipulation();

	void handleEvents();

	void handleInput(sf::Event event);

	void OnKeyboardPress(sf::Event::KeyEvent key);

	void changeMode(InputMode _mode);

	void setActive(Figure* figure);

	void updateModeHint(InputMode m_mode);

	void createPrototype();

	Figure* selectPrototype();

	void addFigure(InputMode back_to);

	void addFigure();

	void saveScene();

	void loadScene();

	void deleteActive();

	void changeActiveFigureColor()
	{
		if (m_active_figure == m_construct_composite)
		{
			return;
		}
		// TODO
		object_manipulation_type = ObjectManipulation::COLORING;
		system("cls");
		sf::Color color = m_active_figure->getShape().getFillColor();
		std::cout << "(F) Red:\t" << (int)color.r << '\n';
		std::cout << "(G) Green\t" << (int)color.g << '\n';
		std::cout << "(B) Blue:\t" << (int)color.b << '\n';
		m_active_figure->setActive(false);
	}

	sf::Vector2f getMouseWorldPosition();

	sf::RenderWindow scene_window;

	SceneController* m_sceneController;

	FigureFactory* m_primitiveFactory;

	InputMode m_mode;

	std::unordered_map<std::string, Prototype*> m_prototypes;

	std::list<std::string> m_prototype_names;

	Figure* m_active_figure = nullptr;

	Figure* m_construct_composite = nullptr;

	ObjectManipulation active_manipulating_object = ObjectManipulation::NONE, object_manipulation_type = ObjectManipulation::NONE;
	
	sf::Vector2f mouse_position;

	Figure* first_of_union = nullptr;

	bool m_mouse_pressed_in_window = false;

	bool m_active_composite_modified = false;

	Menu* m_root_menu;
	std::stack<Menu*>* m_menu_stack;
};

