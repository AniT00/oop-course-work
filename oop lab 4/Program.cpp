#include "Program.h"

#include <algorithm>
#include <utility>
#include <iterator>
#include <numeric>
#include <list>

template<typename First, typename ... T>
inline bool is_in(First&& first, T && ... t)
{
	// Use of fold expression
	return ((first == t) || ...);
}

inline float vector_length(const sf::Vector2f& vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}

float get_clockwise_angle(sf::Vector2f first, sf::Vector2f second)
{
	float dot = first.x * second.x + first.y * second.y;
	float det = first.x * second.y - first.y * second.x;
	return atan2f(det, dot) * 180 / M_PI;
}

Program::Program()
	: m_scene_window(sf::VideoMode(SCENE_WINDOW_WIDTH, SCENE_WINDOW_HEIGHT), "Scene")
{
	m_scene_window.setFramerateLimit(FRAME_LIMIT);

	sf::View view(m_scene_window.getView());
	view.setCenter(0.f, 0.f);
	m_scene_window.setView(view);

	m_sceneController = SceneController::GetInstance(&m_scene_window);

	std::list<key_option_pair_t> common_options
	{
		{ sf::Keyboard::Delete, new MenuOption("Delete composite", [this]() { deleteActive(); }) },
		{ sf::Keyboard::A, new MenuOption("Add figure", [this]() {
			addFigure();
			openMenu(m_edit_primitive_menu); 
			})},
		{ sf::Keyboard::M, new MenuOption("Move", [this]() 
			{ 
				std::cout << "Press Escape to discard changes.\n";
				std::cout << "Press left mouse button to save changes.";
				m_initial_active_figure_transform.setPosition(m_active_figure->getPosition());
				m_initial_mouse_position = getMouseWorldPosition();
				changeMode(InputMode::MOVING);
			}) },
		{ sf::Keyboard::R, new MenuOption("Rotate", [this]() 
			{ 
				std::cout << "Press Escape to discard changes.\n";
				std::cout << "Press left mouse button to save changes.";
				m_initial_active_figure_transform.setRotation(m_active_figure->getRotation());
				m_initial_mouse_position = getMouseWorldPosition();
				changeMode(InputMode::ROTATING);
			}) },
		{ sf::Keyboard::S, new MenuOption("Scale", [this]() 
			{
				std::cout << "Press Escape to discard changes.\n";
				std::cout << "Press left mouse button to save changes.\n";
				std::cout << "Press X to switch scaling horizontally.\n";
				std::cout << "Press Y to switch scaling horizontally.";
				m_initial_active_figure_transform.setScale(m_active_figure->getScale());
				m_initial_mouse_position = getMouseWorldPosition();
				changeMode(InputMode::SCALING);
			}) },
		{ sf::Keyboard::L, new MenuOption("Move object automatically", [this]() { m_active_figure->changeMovement(); }) },
		{ sf::Keyboard::T, new MenuOption("Change tail visibility", [this]() { m_active_figure->changeTail(); }) },
		{ sf::Keyboard::V, new MenuOption("Change visibility", [this]() { m_active_figure->changeVisibility(); }) },
		{ sf::Keyboard::C, new MenuOption("Change to default", [this]() { m_active_figure->reset(); }) },
		{ sf::Keyboard::H, new MenuOption("Change color", 
		[this]() { /*object_manipulation_type = ObjectManipulation::COLORING;*/ printColorHint(); })}
	};

	m_edit_composite_menu = new Menu(common_options);
	m_edit_composite_menu->setTitle("Composite selected.");
	m_edit_composite_menu->add(0, {
		{ sf::Keyboard::Escape, new MenuOption("Finish composition creation", 
		[this]() {
				closeMenu(); 
				m_construct_composite = nullptr;
				setActive(nullptr);
				changeMode(InputMode::MAIN);
			})}
		});
	m_edit_composite_menu->add({
		{ sf::Keyboard::P, new MenuOption("Add to prototype collection", [this]() { createPrototype(); }) }
		});

	m_edit_primitive_menu = new Menu(common_options);
	m_edit_primitive_menu->add(0, {
		{ sf::Keyboard::Escape, new MenuOption("Finish editing primitive", 
		[this]() { 
				closeMenu();
				setActive(m_construct_composite);
			}) }
		});
	m_edit_primitive_menu->setTitle("Primitive selected");

	m_root_menu = new Menu(
		{
			{ sf::Keyboard::A, new MenuOption("Add composite",
			[this]() {
					if (addFigure())
					{
						openMenu(m_edit_composite_menu);
						changeMode(InputMode::EDIT_OBJECT);
					}
				})},
			{ sf::Keyboard::U, new MenuOption("Unity with another composite",[this]() { addFigure(); }) },
			{ sf::Keyboard::S, new MenuOption("Save scene",[this]() { saveScene(); printMenu(); })},
			{ sf::Keyboard::L, new MenuOption("Load scene",[this]() { loadScene(); printMenu(); }) }
		});

	openMenu(m_root_menu);

	addPrimitivePrototype("Circle",		Circle::create);
	addPrimitivePrototype("Triangle",	Triangle::create);
	addPrimitivePrototype("Rectangle",	Rectangle::create);
	
	Composite::setPrimitiveFactory(&m_primitiveFigureFactory);	
}

void Program::run()
{
	while (m_scene_window.isOpen())
	{
		handleEvents();

		m_sceneController->update();

		m_sceneController->clear();

		m_sceneController->drawAxis(m_draw_axis);

		m_sceneController->draw();

		m_sceneController->display();
	}
}

Program::~Program()
{
	delete m_sceneController;
	for (auto& elem : m_prototypes)
	{
		delete elem.second;
	}
	delete m_edit_composite_menu;
	delete m_edit_primitive_menu;
}

void Program::handleEvents()
{
	sf::Event event;
	while (m_scene_window.pollEvent(event))
	{
		using enum sf::Event::EventType;
		switch (event.type)
		{
		case Resized:
			m_scene_window.setView(sf::View(sf::Vector2f(0, 0), sf::Vector2f(event.size.width, event.size.height)));
			//sf::View v(sf::FloatRect(0, 0, event.size.width, event.size.height));
			//v.setCenter(0.f, 0.f);
			break;
		case Closed:
			m_scene_window.close();
			break;
		case KeyPressed:
		case MouseButtonPressed:
		case MouseButtonReleased:
		case MouseMoved:
			handleInput(event);
			break;
		default:
			break;
		}
	}
}

void Program::handleInput(sf::Event event)
{
	using enum sf::Event::EventType;
	switch (event.type)
	{
	case KeyPressed:
		using enum InputMode;
		if (is_in(m_mode, MOVING, SCALING, ROTATING)) 
		{
			if (m_mode == SCALING)
			{
				if (event.key.code == sf::Keyboard::X)
				{
					m_xy_pressed[0] = !m_xy_pressed[0];
					m_draw_axis.y = m_xy_pressed[0];
					m_initial_mouse_position = getMouseWorldPosition();
					m_temp_scale_transform.setScale(m_active_figure->getScale());
				}
				else if (event.key.code == sf::Keyboard::Y)
				{
					m_xy_pressed[1] = !m_xy_pressed[1];
					m_draw_axis.y = m_xy_pressed[1];
					m_initial_mouse_position = getMouseWorldPosition();
					m_temp_scale_transform.setScale(m_active_figure->getScale());
				}
			}
			if (event.key.code == sf::Keyboard::Escape)
			{
				discardlActiveFigureChanges();
			}
			break;
		}
		m_menu_stack.top()->handleKey(event.key.code);
		break;
	case MouseButtonPressed:
		OnMouseButtonPressed();
		break;
	case MouseButtonReleased:
		OnMouseButtonReleased();
		break;
	case MouseMoved:
		OnMouseMoved();
		break;
	default:
		break;
	}
}

void Program::OnMouseButtonReleased()
{
	switch (m_mode)
	{
	case InputMode::MAIN:
	{
		// Select composite.
		std::pair<Figure*, Figure*> clicked_figure = m_sceneController->getIntersection(getMouseWorldPosition());
		Figure* composite = clicked_figure.first;
		Figure* primitive = clicked_figure.second;
		if (composite == nullptr)
		{
			break;
		}
		m_construct_composite = composite;
		setActive(composite);
		openMenu(m_edit_composite_menu);
		changeMode(InputMode::EDIT_OBJECT);
		break;
	}
	case InputMode::EDIT_OBJECT:
	{
		// Select primitive.
		if (m_active_figure != m_construct_composite ||
			m_active_composite_modified)
		{
			break;
		}
		std::pair<Figure*, Figure*> clicked_figure = m_sceneController->getIntersection(getMouseWorldPosition());
		Figure* composite = clicked_figure.first;
		Figure* primitive = clicked_figure.second;
		if (primitive == nullptr)
		{
			break;
		}
		setActive(primitive);
		//closeMenu();
		openMenu(m_edit_primitive_menu);
		//changeMode(InputMode::EDIT_OBJECT);
		break;
	}
	default:
		break;
	}
	m_mouse_pressed_in_window = false;
	m_active_composite_modified = false;
}

void Program::OnMouseButtonPressed()
{
	m_mouse_click_position = getMouseWorldPosition();
	using enum InputMode;
	switch (m_mode)
	{
	case MOVING:
	case SCALING:
	case ROTATING:
		endModifyingActiveFigure();
		break;
	case EDIT_OBJECT:
		// Register changes in mouse position.
		m_mouse_pressed_in_window = true;
		m_last_mouse_position = getMouseWorldPosition();
		break;
	case EXPAND_COMPOSITE:
	{
		// add existing composite.
		std::pair<Figure*, Figure*> clicked_figure = m_sceneController->getIntersection(getMouseWorldPosition());
		if (clicked_figure.first != nullptr &&
			clicked_figure.first != m_construct_composite) // Trying to add yourself in.
		{
			m_sceneController->remove(clicked_figure.first);
			m_construct_composite->add(clicked_figure.first);
			changeMode(InputMode::EDIT_OBJECT);
		}
		break;
	}
	case UNITE_COMPOSITES:
	{
		// unite two composites.
		std::pair<Figure*, Figure*> clicked_figure = m_sceneController->getIntersection(getMouseWorldPosition());
		Figure* second_of_union = clicked_figure.first;
		if (second_of_union == nullptr)
		{
			break;
		}
		if (m_first_of_union == nullptr)
		{
			m_first_of_union = second_of_union;
		}
		else
		{
			m_sceneController->remove(m_first_of_union);
			m_sceneController->remove(second_of_union);

			Composite* composite = new Composite();
			composite->add(m_first_of_union);
			composite->add(second_of_union);
			m_sceneController->add(composite);
			m_first_of_union = nullptr;
			changeMode(InputMode::EDIT_OBJECT);
		}
		break;
	}
	default:
		break;
	}
}

void Program::OnMouseMoved()
{
	switch (m_mode)
	{
	case InputMode::MOVING:
	{
		sf::Vector2f change(getMouseWorldPosition() - m_initial_mouse_position);
		m_active_figure->setPosition(m_initial_active_figure_transform.getPosition() + change);
		break;
	}
	case InputMode::ROTATING:
	{
		sf::Vector2f cur = getMouseWorldPosition();
		cur -= m_initial_active_figure_transform.getPosition();
		sf::Vector2f initial = m_initial_mouse_position;
		initial -= m_initial_active_figure_transform.getPosition();
		float change = get_clockwise_angle(initial, cur);
		float length = vector_length(getMouseWorldPosition() - m_initial_active_figure_transform.getPosition());
		float c = get_clockwise_angle(sf::Vector2f(0.f, 1.f), cur);
		m_active_figure->setRotation(m_initial_active_figure_transform.getRotation() + change);
		break;
	}
	case InputMode::SCALING:
	{
		sf::Vector2f cur = getMouseWorldPosition();
		cur -= m_initial_active_figure_transform.getPosition();
		sf::Vector2f initial = m_initial_mouse_position;
		initial -= m_initial_active_figure_transform.getPosition();
		//sf::Vector2f initial_x = m_initial_scale_mouse_position.first;
		//initial_x -= m_initial_active_figure_transform.getPosition();
		//sf::Vector2f initial_y = m_initial_scale_mouse_position.second;
		float cur_length = vector_length(cur);
		float initial_length = vector_length(initial);
		float factor = cur_length / initial_length;
		//TODO class member
		//float initial_x_length = vector_length(initial_x);
		//float initial_y_length = vector_length(initial_y);
		//float factor_x = cur_length / initial_x_length;
		//float factor_y = cur_length / initial_y_length;
		m_active_figure->setScale(
			m_xy_pressed[0] ? m_temp_scale_transform.getScale().x * factor : m_active_figure->getScale().x,
			m_xy_pressed[1] ? m_temp_scale_transform.getScale().y * factor : m_active_figure->getScale().y);
		break;
	}
	default:
		break;
	}
	m_last_mouse_position = getMouseWorldPosition();
}

void Program::changeMode(InputMode _mode)
{
	m_mode = _mode;
}

void Program::setActive(Figure* figure)
{
	if (m_active_figure != nullptr)
		m_active_figure->setActive(false);
	m_active_figure = figure;
	if (m_active_figure != nullptr)
		m_active_figure->setActive(true);
}

void Program::createPrototype()
{
	system("cls");
	std::cout << "Enter prototype name (leave empty to cancel): ";
	std::string prototype_name;
	std::getline(std::cin, prototype_name);
	if (!prototype_name.empty())
	{
		m_prototypes.insert({ prototype_name, m_active_figure->clone() });
		m_prototype_names.push_back(prototype_name);
	}
}

Figure* Program::selectPrototype()
{
	std::cout << "Select prototype number (leave empty to cancel): ";
	int index = 0;
	std::string input;
	std::getline(std::cin, input);
	if (input.empty())
	{
		return nullptr;
	}
	index = atoi(input.c_str());
	if (index > 0 && index <= m_prototype_names.size())
	{
		std::string prototype_name = *std::next(m_prototype_names.begin(), index - 1);
		return (Figure*)m_prototypes[prototype_name]->clone();
	}
	else
	{
		std::cout << "Invalid input. Object wasn't created.";
		getchar();
		return nullptr;
	}
}

void Program::printColorHint()
{
	system("cls");
	sf::Color color = m_active_figure->getColor();
	std::cout << "(F) Red:\t" << (int)color.r << '\n';
	std::cout << "(G) Green\t" << (int)color.g << '\n';
	std::cout << "(B) Blue:\t" << (int)color.b << '\n';
}

void Program::addFigure(InputMode back_to)
{
	printPrototypes();
	//TODO
	//changeMode(InputMode::VIEW_PROTOTYPES);
	// Primitive can not exist by himself on the scene.
	if (m_construct_composite == nullptr)
	{
		m_construct_composite = new Composite();
		setActive(m_construct_composite);
	}
	Figure* new_figure = selectPrototype();
	if (new_figure != nullptr)
	{
		m_construct_composite->add(new_figure);
	}
	changeMode(back_to);
}

void Program::printPrototypes()
{
	system("cls");
	int i = 1;
	for (auto& m_name : m_prototype_names)
	{
		std::cout << i++ << ". " << m_name << '\n';
	}
}

bool Program::addFigure()
{
	printPrototypes();
	//TODO
	//changeMode(InputMode::VIEW_PROTOTYPES);
	// Primitive can not exist by himself on the scene.
	if (m_construct_composite == nullptr)
	{
		m_construct_composite = new Composite();
		m_sceneController->add(m_construct_composite);
	}
	Figure* new_figure = selectPrototype();
	printMenu();
	if (new_figure == nullptr)
	{
		return false;
	}
	m_construct_composite->add(new_figure);
	setActive(m_construct_composite);
	return true;
}

void Program::saveScene()
{
	std::string file_name;
	while (true)
	{
		system("cls");
		std::cout << "Enter file name (witout extension) (leave empty to cancel): ";
		std::getline(std::cin, file_name);
		file_name += ".scene";
		if (file_name.empty())
		{
			return;
		}
		std::fstream file(file_name, std::ios_base::in);
		// If file exists, confirm overwriting.
		if (file.fail())
		{
			break;
		}
		std::string inp;
		bool overwrite = false;
		while (true)
		{
			std::cout << "\nFile already exists. Do you want to overwrite file? (y/n): ";
			std::getline(std::cin, inp);
			if (inp.compare("y") == 0 || inp.compare("Y") == 0)
			{
				overwrite = true;
				break;
			}
			if (inp.compare("n") == 0 || inp.compare("N") == 0)
			{
				break;
			}
		}
		if (overwrite)
		{
			break;
		}
	}
	std::ofstream scene_file(file_name, std::ios_base::binary | std::ios_base::trunc);
	SceneController::Snapshot snapshot = m_sceneController->save();
	scene_file << snapshot;
}

void Program::loadScene()
{
	system("cls");
	std::string file_name;
	while (true)
	{
		std::cout << "Enter file name (witout extension) (leave empty to cancel): ";
		std::getline(std::cin, file_name);
		file_name += ".scene";
		if (file_name.empty())
		{
			return;
		}
		std::fstream file(file_name, std::ios_base::in);
		if (file.good())
		{
			break;
		}
		system("cls");
		std::cout << "\nFile does not exists.\n";
	}
	std::ifstream scene_file(file_name, std::ios_base::binary);
	SceneController::Snapshot snapshot(scene_file);
	m_sceneController->restore(snapshot);
}

void Program::openMenu(Menu* menu)
{
	m_menu_stack.push(menu);
	printMenu();
}

void Program::closeMenu()
{
	m_menu_stack.pop();
	printMenu();
}

void Program::printMenu()
{
	system("cls");
	std::cout << *m_menu_stack.top();
}

void Program::addPrimitivePrototype(const std::string& type_name, std::function<Figure* ()> stat_factory)
{
	m_prototype_names.push_back(type_name);
	m_prototypes.insert({ type_name, stat_factory() });
	m_primitiveFigureFactory.registerType(type_name, stat_factory);
}

void Program::deleteActive()
{
	// If editing composite, remove it from scene.
	if (m_construct_composite == m_active_figure)
	{
		m_sceneController->remove(m_construct_composite);
	}
	else // remove figure from composite.
	{
		// TODO
		m_construct_composite->remove(m_active_figure);
	}
	delete m_active_figure;
	setActive(nullptr);
	//We're in edit mode, go back.
	m_menu_stack.pop();
}

//void Program::changeActiveFigureColor()
//{
//	if (m_active_figure == m_construct_composite)
//	{
//		return;
//	}
//	// TODO
//	system("cls");
//	sf::Color color = m_active_figure->getShape().getFillColor();
//	std::cout << "(F) Red:\t" << (int)color.r << '\n';
//	std::cout << "(G) Green\t" << (int)color.g << '\n';
//	std::cout << "(B) Blue:\t" << (int)color.b << '\n';
//	m_active_figure->setActive(false);
//}

void Program::discardlActiveFigureChanges()
{
	switch (m_mode)
	{
	case InputMode::MOVING:
		m_active_figure->setPosition(m_initial_active_figure_transform.getPosition());
		break;
	case InputMode::ROTATING:
		m_active_figure->setRotation(m_initial_active_figure_transform.getRotation());
		break;
	case InputMode::SCALING:
		m_active_figure->setScale(m_initial_active_figure_transform.getScale());
		break;
	default:
		break;
	}
	endModifyingActiveFigure();
}

void Program::endModifyingActiveFigure()
{
	printMenu();
	changeMode(InputMode::EDIT_OBJECT);
}



sf::Vector2f Program::getMouseWorldPosition()
{
    sf::Vector2i pixel_coord = sf::Mouse::getPosition(m_scene_window);
    return m_scene_window.mapPixelToCoords(pixel_coord);
}
