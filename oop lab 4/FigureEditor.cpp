#include "FigureEditor.h"

FigureEditor::Context::Context(Program* program_instance, 
	std::function<void()> quit_callback, sf::Vector2<bool>* draw_axis)
{
    m_program_instance = program_instance;
    m_quit_callback = quit_callback;
	m_draw_axis = draw_axis;
}

FigureEditor::FigureEditor(Context* context)
	: m_context(context)
{
}

void FigureEditor::setInitialTransform(sf::Transformable transform)
{
	m_initial_active_figure_transform = transform;
}

void FigureEditor::setInitialMousePosition(const sf::Vector2f& pos)
{
	m_initial_mouse_position = pos;
}

void FigureEditor::activate(EditMode mode)
{
	m_mode = mode;
	switch (m_mode)
	{
	case FigureEditor::ROTATING:
		m_initial_active_figure_transform.setRotation(m_edit_figure->getRotation());
		//break;
	case FigureEditor::MOVING:
		m_initial_active_figure_transform.setPosition(m_edit_figure->getWorldPosition());
		break;
	case FigureEditor::SCALING:
		m_initial_active_figure_transform.setScale(m_edit_figure->getScale());
		break;
	case FigureEditor::COLORING:
		m_initial_active_figure_color = m_edit_figure->getColor();
		break;
	default:
		break;
	}
}

FigureEditor::EditMode FigureEditor::getMode()
{
	return m_mode;
}

void FigureEditor::setFigure(Figure* figure)
{
	m_edit_figure = figure;
}

const Figure* FigureEditor::getFigure() const
{
	return m_edit_figure;
}

bool FigureEditor::handleInput(const sf::Event& event)
{
	using enum sf::Event::EventType;
	bool handled = false;
	switch (event.type)
	{
	case KeyPressed:
		if (m_mode == SCALING)
		{
			if (event.key.code == sf::Keyboard::X)
			{
				m_xy_pressed[0] = !m_xy_pressed[0];
				m_context->m_draw_axis->x = m_xy_pressed[0];
				m_initial_mouse_position = m_context->m_mouse_world_position;
				m_temp_scale_transform.setScale(m_edit_figure->getScale());
				handled = true;
			}
			else if (event.key.code == sf::Keyboard::Y)
			{
				m_xy_pressed[1] = !m_xy_pressed[1];
				m_context->m_draw_axis->y = m_xy_pressed[1];
				m_initial_mouse_position = m_context->m_mouse_world_position;
				m_temp_scale_transform.setScale(m_edit_figure->getScale());
				handled = true;
			}
		}
		else if (m_mode == COLORING)
		{
			if (is_in(event.key.code, sf::Keyboard::R, sf::Keyboard::G, sf::Keyboard::B))
			{
				m_color_to_change = menu_tools::key_names.at(event.key.code);
				handled = true;
			}
		}
		if (event.key.code == sf::Keyboard::Escape)
		{
			discardlActiveFigureChanges();
			handled = true;
		}
		break;
	case MouseButtonPressed:
		m_context->m_quit_callback();
		handled = true;
		break;
	case MouseMoved:
		OnMouseMoved();
		handled = true;
		break;
	default:
		break;
	}
	return handled;
}

void FigureEditor::OnMouseMoved()
{
	using enum EditMode;
	switch (m_mode)
	{
	case MOVING:
	{
		sf::Vector2f change(m_context->m_mouse_world_position - m_initial_mouse_position);
		m_edit_figure->setPosition(m_initial_active_figure_transform.getPosition() + change);
		break;
	}
	case ROTATING:
	{
		sf::Vector2f cur = m_context->m_mouse_world_position;
		cur -= m_initial_active_figure_transform.getPosition();
		sf::Vector2f initial = m_initial_mouse_position;
		initial -= m_initial_active_figure_transform.getPosition();
		float change = get_clockwise_angle(initial, cur);
		float length = vector_length(m_context->m_mouse_world_position - m_initial_active_figure_transform.getPosition());
		float c = get_clockwise_angle(sf::Vector2f(0.f, 1.f), cur);
		m_edit_figure->setRotation(m_initial_active_figure_transform.getRotation() + change);
		break;
	}
	case SCALING:
	{
		sf::Vector2f cur = m_context->m_mouse_world_position;
		cur -= m_initial_active_figure_transform.getPosition();
		sf::Vector2f initial = m_initial_mouse_position;
		initial -= m_initial_active_figure_transform.getPosition();
		float cur_length = vector_length(cur);
		float initial_length = vector_length(initial);
		float factor = cur_length / initial_length;
		//TODO class member
		m_edit_figure->setScale(
			m_xy_pressed[0] ? m_temp_scale_transform.getScale().x * factor : m_edit_figure->getScale().x,
			m_xy_pressed[1] ? m_temp_scale_transform.getScale().y * factor : m_edit_figure->getScale().y);
		break;
	}
	case COLORING:
	{
		float change = m_context->m_mouse_world_position.x - m_last_mouse_position.x;
		if (change)
		{
			sf::Color new_change = m_edit_figure->getColor();
			bool updateMenu = false;
			// Get first symbol. It should be 'R', 'G' or 'B'.
			switch (m_color_to_change[0])
			{
			case 'R':
				new_change.r += change;
				updateMenu = true;
				break;
			case 'G':
				new_change.g += change;
				updateMenu = true;
				break;
			case 'B':
				new_change.b += change;
				updateMenu = true;
				break;
			default:
				printColorHint(false);
				break;
			}
			if (updateMenu)
			{
				printColorHint(true);
			}
			m_edit_figure->setColor(new_change);

		}
		break;
	}
	default:
		break;
	}
	m_last_mouse_position = m_context->m_mouse_world_position;
}

void FigureEditor::discardlActiveFigureChanges()
{
	switch (m_mode)
	{
	case MOVING:
		m_edit_figure->setPosition(m_initial_active_figure_transform.getPosition());
		break;
	case ROTATING:
		m_edit_figure->setRotation(m_initial_active_figure_transform.getRotation());
		break;
	case SCALING:
		m_edit_figure->setScale(m_initial_active_figure_transform.getScale());
		break;
	default:
		break;
	}
	m_context->m_quit_callback();
}

void FigureEditor::printColorHint(bool shortened)
{
	system("cls");
	if (!shortened)
	{
		std::cout << "Press Escape to discard changes.\n";
		std::cout << "Press left mouse button to save changes.\n";
		std::cout << "Press specified button below to begin changing corresponding color.\n";
		std::cout << "Move mouse cursor left/right to decrease/increase color value.\n";

	}
	sf::Color color = m_edit_figure->getColor();
	std::cout << "(R) Red:\t" << (int)color.r << '\n';
	std::cout << "(G) Green\t" << (int)color.g << '\n';
	std::cout << "(B) Blue:\t" << (int)color.b << '\n';
}
