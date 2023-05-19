#pragma once

#define FRAME_LIMIT	60
#define _USE_MATH_DEFINES
#include "SceneController.h"
#include "FigureEditor.h"
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

class Program
{
protected:
	Program();

	static Program* _instance;
public:

	enum class InputMode
	{
		MAIN,
		ADD_FIGURE,
		FIGURE_SELECTED,
		EXPAND_COMPOSITE,
		UNITE_COMPOSITES,
		VIEW_PROTOTYPES,
		EDIT_FIGURE,
		MOVING,
		ROTATING,
		SCALING,
		COLORING
	};

	Program(Program& other) = delete;

	void operator=(const Program&) = delete;

	static Program* GetInstance();

	void run();

	~Program();

private:
	void handleEvents();

	void handleInput(const sf::Event& event);

	void OnMouseButtonReleased();

	void OnMouseButtonPressed();

	void changeMode(InputMode _mode);

	void setActive(Figure* figure);

	void createPrototype();

	Figure* selectPrototype();

	void printColorHint(bool shortened = false);

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

	void endModifyingActiveFigure();

	void colorize_figure();

	void finish_composite();

	void scale_figure();

	void rotate_figure();

	void move_figure();

	void try_add_figure();

	//void unite_composites();

	sf::Vector2f getMouseWorldPosition();

	sf::RenderWindow m_scene_window;

	SceneController* m_sceneController;

	FigureEditor m_figureEditor;

	FigureEditor::Context m_figureEditor_context;

	FigureFactory m_primitiveFigureFactory;

	InputMode m_mode;

	std::unordered_map<std::string, Prototype*> m_prototypes;

	std::list<std::string> m_prototype_names;

	Figure* m_active_figure = nullptr;

	Figure* m_construct_composite = nullptr;

	sf::Vector2<bool> m_draw_axis;

	const char* m_color_to_change = "";

	// To scale by corresponding axis.
	bool m_xy_pressed[2] = { false, false };

	Figure* m_first_of_union = nullptr;

	bool m_mouse_pressed_in_window = false;

	bool m_active_composite_modified = false;

	std::stack<Menu*> m_menu_stack;

	std::unordered_map<std::string, Menu*> m_menus;
};

