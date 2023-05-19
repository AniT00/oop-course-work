#include "common_functions.h"
#include "Program.h"

#include <algorithm>
#include <utility>
#include <iterator>
#include <numeric>
#include <list>
#include <chrono>

Program* Program::_instance = nullptr;

Program::Program()
	: m_scene_window(sf::VideoMode(SCENE_WINDOW_WIDTH, SCENE_WINDOW_HEIGHT), "Scene"),
	m_figureEditor(&m_figureEditor_context),
	m_figureEditor_context(this, std::bind(&Program::finishModifyingActiveFigure, this), &m_draw_axis)
{
	m_scene_window.setFramerateLimit(FRAME_LIMIT);

	sf::View view(m_scene_window.getView());
	view.setCenter(0.f, 0.f);
	m_scene_window.setView(view);

	m_sceneController = SceneController::GetInstance(&m_scene_window);

	std::list<key_option_pair_t> common_options
	{
		{ sf::Keyboard::Delete, std::make_shared<MenuOption>("Delete composite", [this]() { deleteActive(); }) },
		{ sf::Keyboard::A, std::make_shared<MenuOption>("Add figure", [this]() 
			{
				addFigure();
				if (m_menu_stack.top() != m_menus["Edit primitive"])
				{
					openMenu(m_menus["Edit primitive"]);
				}
			}) },
		{ sf::Keyboard::M, std::make_shared<MenuOption>("Move", std::bind(&Program::startMovingFigure, this)) },
		{ sf::Keyboard::R, std::make_shared<MenuOption>("Rotate", std::bind(&Program::startRotatingFigure, this)) },
		{ sf::Keyboard::S, std::make_shared<MenuOption>("Scale", std::bind(&Program::startScalingFigure, this)) },
		{ sf::Keyboard::L, std::make_shared<MenuOption>("Move object automatically", [this]() { m_active_figure->changeMovement(); }) },
		{ sf::Keyboard::T, std::make_shared<MenuOption>("Change tail visibility", [this]() { m_active_figure->changeTail(); }) },
		{ sf::Keyboard::V, std::make_shared<MenuOption>("Change visibility", [this]() { m_active_figure->changeVisibility(); }) },
		{ sf::Keyboard::C, std::make_shared<MenuOption>("Change to default", [this]() { m_active_figure->reset(); }) },
		{ sf::Keyboard::Z, std::make_shared<MenuOption>("Undo", [this]() { undo(); }) },
		{ sf::Keyboard::Y, std::make_shared<MenuOption>("Redo", [this]() { redo(); }) },
	};


	Menu* edit_composite_menu = new Menu(common_options);
	edit_composite_menu->setTitle("Composite selected.");
	edit_composite_menu->add(0, {
		{ sf::Keyboard::Escape, std::make_shared<MenuOption>("Finish composition creation", std::bind(&Program::finishComposite, this))}
		});
	edit_composite_menu->add({
		{ sf::Keyboard::P, std::make_shared<MenuOption>("Add to prototype collection", std::bind(&Program::createPrototype, this)) }
		});

	Menu* edit_primitive_menu = new Menu(common_options);
	edit_primitive_menu->add(0, {
		{ sf::Keyboard::Escape, std::make_shared<MenuOption>("Finish editing primitive",
		[this]() { 
				closeMenu();
				setActive(m_construct_composite);
			}) }
		});
	edit_primitive_menu->add({ 
		{ sf::Keyboard::H, std::make_shared<MenuOption>("Change color", std::bind(&Program::colorize_figure, this)) } 
		});
	edit_primitive_menu->setTitle("Primitive selected");

	Menu* root_menu = new Menu(
		{
			{ sf::Keyboard::A, std::make_shared<MenuOption>("Add composite", std::bind(&Program::tryAddFigure, this)) },
			{ sf::Keyboard::U, std::make_shared<MenuOption>("Unity with another composite",[this]() { changeMode(InputMode::UNITE_COMPOSITES); })},
			{ sf::Keyboard::S, std::make_shared<MenuOption>("Save scene",[this]() { saveScene(); printMenu(); })},
			{ sf::Keyboard::L, std::make_shared<MenuOption>("Load scene",[this]() { loadScene(); printMenu(); }) }
		});
	
	m_menus.insert(
		{
			{ "Edit composite",	edit_composite_menu },
			{ "Edit primitive",	edit_primitive_menu },
			{ "Root menu",		root_menu }
		});

	openMenu(m_menus["Root menu"]);

	addPrimitivePrototype("Circle",		Circle::create);
	addPrimitivePrototype("Triangle",	Triangle::create);
	addPrimitivePrototype("Rectangle",	Rectangle::create);
	
	Composite::setPrimitiveFactory(&m_primitiveFigureFactory);	
}

void Program::colorize_figure()
{
	startModifyingFigure("", FigureEditor::EditMode::COLORING);
	printColorHint();
}

void Program::finishComposite()
{
	closeMenu();
	m_construct_composite = nullptr;
	setActive(nullptr);
	changeMode(InputMode::MAIN);
}

void Program::startScalingFigure()
{
	std::stringstream s;
	s << "Press Escape to discard changes.\n";
	s << "Press left mouse button to save changes.\n";
	s << "Press X to switch scaling horizontally.\n";
	s << "Press Y to switch scaling horizontally.";
	startModifyingFigure(s.str(), FigureEditor::EditMode::SCALING);
}

void Program::startRotatingFigure()
{
	std::stringstream s;
	s << "Press Escape to discard changes.\n";
	s << "Press left mouse button to save changes.";
	startModifyingFigure(s.str(), FigureEditor::EditMode::ROTATING);
}

void Program::startMovingFigure()
{
	std::stringstream s;
	s << "Press Escape to discard changes.\n";
	s << "Press left mouse button to save changes.";
	startModifyingFigure(s.str(), FigureEditor::EditMode::MOVING);
}

void Program::startModifyingFigure(std::string hint, FigureEditor::EditMode mode)
{
	std::cout << hint;
	m_figureEditor_context.m_mouse_world_position = getMouseWorldPosition();
	const Figure::Memento* memento = m_active_figure->save();
	m_figureEditor.activate(mode, memento);
	//m_undo.push(memento);
	while (!m_redo.empty())
	{
		delete m_redo.top();
		m_redo.pop();
	}
	changeMode(InputMode::EDIT_FIGURE);
}

void Program::tryAddFigure()
{
	if (addFigure())
	{
		openMenu(m_menus["Edit composite"]);
		changeMode(InputMode::FIGURE_SELECTED);
	}
	else
	{
		printMenu();
	}
}

void Program::undo()
{
	//if (m_first_undo) { m_redo.push(m_active_figure->save()); }
	if (m_undo.size() > 1)
	{
		m_redo.push(m_undo.top());
		m_undo.pop();
		m_active_figure->restore(m_undo.top());
	}
}

void Program::redo()
{
	if (!m_redo.empty())
	{
		const Figure::Memento* state = m_redo.top();
		m_active_figure->restore(state);
		m_undo.push(state);
		m_redo.pop();
	}
}

Program* Program::GetInstance()
{
	if (_instance == nullptr)
	{
		_instance = new Program();
	}
	return _instance;
}

void Program::run()
{
	while (m_scene_window.isOpen())
	{
		auto start = std::chrono::high_resolution_clock::now();
		handleEvents();

		m_sceneController->update();

		m_sceneController->clear();

		m_sceneController->drawAxis(m_draw_axis);

		m_sceneController->draw();

		m_sceneController->display();

		auto end = std::chrono::high_resolution_clock::now();

		long long time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (time != 0)
		{
			std::string fps = std::to_string(1000 / time);
			m_scene_window.setTitle(fps);
		}
	}
}

Program::~Program()
{
	delete m_sceneController;
	for (auto& elem : m_prototypes)
	{
		delete elem.second;
	}
	for (auto& elem : m_menus)
	{
		delete elem.second;
	}
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
			break;
		case Closed:
			m_scene_window.close();
			break;
		case KeyPressed:
		case KeyReleased:
		case MouseButtonPressed:
		case MouseButtonReleased:
		case MouseMoved:
			m_figureEditor_context.m_mouse_world_position = getMouseWorldPosition();
			if (m_mode == InputMode::EDIT_FIGURE)
			{
				if (!m_figureEditor.handleInput(event))
				{
					handleInput(event);
				}
			}
			else
			{
				handleInput(event);
			}
			break;
		default:
			break;
		}
	}
}

void Program::handleInput(const sf::Event& event)
{
	using enum sf::Event::EventType;
	switch (event.type)
	{
	case KeyReleased:
		m_menu_stack.top()->handleKey(event.key.code);
		break;
	case MouseButtonPressed:
		OnMouseButtonPressed();
		break;
	case MouseButtonReleased:
		OnMouseButtonReleased();
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
		openMenu(m_menus["Edit composite"]);
		changeMode(InputMode::FIGURE_SELECTED);
		break;
	}
	case InputMode::FIGURE_SELECTED:
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
		openMenu(m_menus["Edit primitive"]);
		//changeMode(InputMode::FIGURE_SELECTED);
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
	using enum InputMode;
	switch (m_mode)
	{
	case FIGURE_SELECTED:
		// Register changes in mouse position.
		m_mouse_pressed_in_window = true;
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
			changeMode(InputMode::FIGURE_SELECTED);
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
			changeMode(InputMode::FIGURE_SELECTED);
			openMenu(m_menus["Edit composite"]);
			setActive(composite);
		}
		break;
	}
	default:
		break;
	}
}


void Program::changeMode(InputMode _mode)
{
	m_mode = _mode;
}

void Program::setActive(Figure* figure)
{
	if (m_active_figure == figure) { return; }
	m_first_undo = true;
	while (!m_undo.empty())
	{
		m_undo.pop();
	}
	while (!m_redo.empty())
	{
		m_redo.pop();
	}
	if (m_active_figure != nullptr)
		m_active_figure->setActive(false);
	m_active_figure = figure;
	if (m_active_figure != nullptr)
	{
		m_active_figure->setActive(true);
		m_figureEditor.setFigure(m_active_figure);
		m_undo.push(m_active_figure->save());
	}
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

void Program::printColorHint(bool shortened)
{
	system("cls");
	if (!shortened)
	{
		std::cout << "Press Escape to discard changes.\n";
		std::cout << "Press left mouse button to save changes.\n";
		std::cout << "Press specified button below to begin changing corresponding color.\n";
		std::cout << "Move mouse cursor left/right to decrease/increase color value.\n";

	}
	sf::Color color = m_active_figure->getColor();
	std::cout << "(R) Red:\t" << (int)color.r << '\n';
	std::cout << "(G) Green\t" << (int)color.g << '\n';
	std::cout << "(B) Blue:\t" << (int)color.b << '\n';
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
	Figure* new_figure = selectPrototype();
	Figure* new_active = new_figure;
	if (m_construct_composite == nullptr)
	{
		m_construct_composite = new Composite();
		new_active = m_construct_composite;
		m_sceneController->add(m_construct_composite);
	}
	//printMenu();
	if (new_figure == nullptr)
	{
		return false;
	}
	m_construct_composite->add(new_figure);
	setActive(new_active);
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
	std::ofstream scene_file(file_name, std::ios_base::trunc);
	SceneController::Memento snapshot = m_sceneController->save();
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
		if (file_name.empty())
		{
			return;
		}
		file_name += ".scene";
		
		std::fstream file(file_name, std::ios_base::in);
		if (file.good())
		{
			break;
		}
		system("cls");
		std::cout << "\nFile does not exists.\n";
	}
	std::ifstream scene_file(file_name, std::ios_base::binary);
	SceneController::Memento snapshot(scene_file);
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
	setActive(nullptr);
	delete m_active_figure;
	//We're in edit mode, go back.
	m_menu_stack.pop();
}

void Program::finishModifyingActiveFigure()
{
	m_undo.push(m_active_figure->save());
	printMenu();
	changeMode(InputMode::FIGURE_SELECTED);
}

sf::Vector2f Program::getMouseWorldPosition()
{
    sf::Vector2i pixel_coord = sf::Mouse::getPosition(m_scene_window);
    return m_scene_window.mapPixelToCoords(pixel_coord);
}
