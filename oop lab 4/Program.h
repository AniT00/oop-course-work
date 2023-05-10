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

	void addFigure(InputMode back_to);

	void printPrototypes();

	bool addFigure();

	void saveScene();

	void loadScene();

	void openMenu(Menu* menu);
	
	void closeMenu();

	void printMenu();

	void addPrimitivePrototype(const std::string& type_name, std::function<Figure*()> stat_factory);

	void deleteActive();

	void discardlActiveFigureChanges();

	void endModifyingActiveFigure();

	sf::Vector2f getMouseWorldPosition();

	sf::RenderWindow m_scene_window;

	SceneController* m_sceneController;

	FigureFactory m_primitiveFigureFactory;

	InputMode m_mode;

	std::unordered_map<std::string, Prototype*> m_prototypes;

	std::list<std::string> m_prototype_names;

	Figure* m_active_figure = nullptr;

	Figure* m_construct_composite = nullptr;

	sf::Vector2<bool> m_draw_axis;

	// Stores initial transform to get back in case user wants to discard changes.
	sf::Transformable m_initial_active_figure_transform;
	sf::Color m_initial_active_figure_color;
	// Stores temp changes that wasn't confirmed.
	sf::Transformable m_temp_scale_transform;
	sf::Color m_temp_color;

	sf::Vector2f m_initial_mouse_position;
	sf::Vector2f m_last_mouse_position;
	sf::Vector2f m_mouse_click_position;

	const char* m_color_to_change = "";

	// To scale by corresponding axis.
	bool m_xy_pressed[2] = { false, false };

	Figure* m_first_of_union = nullptr;

	bool m_mouse_pressed_in_window = false;

	bool m_active_composite_modified = false;

	std::stack<Menu*> m_menu_stack;

	Menu* m_edit_composite_menu;
	Menu* m_edit_primitive_menu;
	Menu* m_root_menu;
};

