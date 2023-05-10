#pragma once

#define FRAME_LIMIT	60
#define _USE_MATH_DEFINES
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
	VIEW_PROTOTYPES,
	MOVING,
	ROTATING,
	SCALING,
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
	void handleEvents();

	void handleInput(sf::Event event);

	void OnMouseButtonReleased();

	void OnMouseButtonPressed();

	void OnMouseMoved();

	void changeMode(InputMode _mode);

	void setActive(Figure* figure);

	void createPrototype();

	Figure* selectPrototype();

	void printColorHint();

	void printPrototypes();

	bool addFigure();

	void saveScene();

	void loadScene();

	void openMenu(Menu* menu);
	
	void closeMenu();

	void printMenu();

	void addPrimitivePrototype(const std::string& type_name, std::function<Figure*()> stat_factory);

	void deleteActive();

	void changeActiveFigureColor();

	void discardlActiveFigureChanges();

	void endModifyingActiveFigure();

	sf::Vector2f getMouseWorldPosition();

	sf::RenderWindow scene_window;

	SceneController* m_sceneController;

	FigureFactory m_primitiveFigureFactory;

	InputMode m_mode;

	std::unordered_map<std::string, Prototype*> m_prototypes;

	std::list<std::string> m_prototype_names;

	Figure* m_active_figure = nullptr;

	Figure* m_construct_composite = nullptr;

	sf::RectangleShape m_edit_line_hint;
	//sf::Vector2f m_initial_edit_line_rotation;
	bool show_edit_line_hint = false;

	ObjectManipulation m_active_manipulating_object = ObjectManipulation::NONE, object_manipulation_type = ObjectManipulation::NONE;
	
	sf::Vector2f m_editing_scale_factor = sf::Vector2f(1.f, 1.f);
	sf::Transformable m_initial_active_figure_transform;
	sf::Vector2f m_initial_mouse_position;
	sf::Vector2f m_initial_scale;

	sf::Vector2f m_last_mouse_position;
	sf::Vector2f m_mouse_click_position;

	Figure* m_first_of_union = nullptr;

	bool m_mouse_pressed_in_window = false;

	bool m_active_composite_modified = false;

	std::stack<Menu*> m_menu_stack;

	Menu* m_edit_composite_menu;
	Menu* m_edit_primitive_menu;
	Menu* m_root_menu;
};

