#pragma once

#define FRAME_LIMIT	60
#include "SceneController.h"

#include "Circle.h"
#include "Rectangle.h"
#include "Triangle.h"

#include <SFML/Graphics.hpp>
#include <iostream>

#define SCENE_WINDOW_WIDTH	720
#define SCENE_WINDOW_HEIGHT	480

#define HIERARCHY_WINDOW_WIDTH	320
#define HIERARCHY_WINDOW_HEIGHT	640



enum class InputMode
{
	MAIN,
	ADD_COMPOSITE,
	OBJECT_EDITING,
	EXPAND_COMPOSITE,
	UNITE_COMPOSITES
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

private:
	sf::RenderWindow scene_window;
	SceneController* sceneController;
	InputMode mode;
	//sf::Shape* active_object = nullptr;
	Figure* active_composite = nullptr;
	ObjectManipulation active_manipulating_object = ObjectManipulation::NONE, object_manipulation_type = ObjectManipulation::NONE;
	sf::Vector2i mouse_position;
	Figure* first_of_unity = nullptr;
};

