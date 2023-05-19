#pragma once
#include "common_functions.h"
#include "Figure.h"
#include "Menu.h"

class Program;

class FigureEditor
{
public:
	enum EditMode
	{
		NONE,
		MOVING,
		ROTATING, 
		SCALING,
		COLORING
	};

	struct Context
	{
		Program* m_program_instance;
		Menu* m_edit_menu;
		sf::Vector2f m_mouse_world_position;
		std::function<void()> m_quit_callback;
		sf::Vector2<bool>* m_draw_axis;

		Context(Program* program_instance, std::function<void()> quit_callback, sf::Vector2<bool>* draw_axis);
	};

	FigureEditor(Context* context);

	void setInitialTransform(sf::Transformable transform);

	void setInitialMousePosition(const sf::Vector2f& pos);

	void activate(EditMode mode);

	EditMode getMode();

	void setFigure(Figure* figure);

	const Figure* getFigure() const;

	bool handleInput(const sf::Event& event);

	void OnMouseMoved();

	void discardlActiveFigureChanges();

	void printColorHint(bool shortened = false);

private:
	Figure* m_edit_figure = nullptr;

	Context* const m_context;

	EditMode m_mode = EditMode::NONE;

	sf::Vector2<bool> m_draw_axis;

	// Stores initial transform to get back in case user wants to discard changes.
	sf::Transformable m_initial_active_figure_transform;
	sf::Color m_initial_active_figure_color;
	sf::Vector2f m_initial_rotation_vector;
	// Stores temp changes that wasn't confirmed.
	sf::Transformable m_temp_scale_transform;
	sf::Color m_temp_color;

	sf::Vector2f m_initial_mouse_position;
	sf::Vector2f m_last_mouse_position;
	sf::Vector2f m_mouse_click_position;

	const char* m_color_to_change = "";

	// To scale by corresponding axis.
	bool m_xy_pressed[2] = { false, false };
};

