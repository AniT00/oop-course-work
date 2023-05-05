#include "Program.h"

Program::Program()
	: scene_window(sf::VideoMode(SCENE_WINDOW_WIDTH, SCENE_WINDOW_HEIGHT), "Scene")
{
	scene_window.setFramerateLimit(FRAME_LIMIT);

	sf::View view(scene_window.getView());
	view.setCenter(0.f, 0.f);
	scene_window.setView(view);

	m_sceneController = SceneController::GetInstance(&scene_window);

	m_menu_stack = new std::stack<Menu*>();

	Menu* edit_menu = new Menu(m_menu_stack,
		{
			{ sf::Keyboard::Delete, new MenuOption("Delete composite", [this]() { deleteActive(); }) },
			{ sf::Keyboard::A, new MenuOption("Add figure", [this]() { addFigure(); }) },
			{ sf::Keyboard::M, new MenuOption("Move", [this]() { object_manipulation_type = ObjectManipulation::MOVING; }) },
			{ sf::Keyboard::R, new MenuOption("Rotate", [this]() { object_manipulation_type = ObjectManipulation::SCALING; }) },
			{ sf::Keyboard::S, new MenuOption("Scale", [this]() { object_manipulation_type = ObjectManipulation::ROTATING; }) },
			{ sf::Keyboard::L, new MenuOption("Move object automatically", [this]() { m_active_figure->changeMovement(); }) },
			{ sf::Keyboard::T, new MenuOption("Change tail visibility", [this]() { m_active_figure->changeTail(); }) },
			{ sf::Keyboard::V, new MenuOption("Change visibility", [this]() { m_active_figure->changeVisibility(); }) },
			{ sf::Keyboard::C, new MenuOption("Change to default", [this]() { m_active_figure->reset(); }) },
			{ sf::Keyboard::H, new MenuOption("Change color", [this]() { changeActiveFigureColor(); }) },
		});

	m_root_menu = new Menu(m_menu_stack,
		{
			{ sf::Keyboard::A, new MenuOption("Add composite",[this]() { addFigure(); }) },
			{ sf::Keyboard::U, new MenuOption("Unity with another composite",[this]() { addFigure(); }) },
			{ sf::Keyboard::S, new MenuOption("Save scene",[this]() { saveScene(); }) },
			{ sf::Keyboard::L, new MenuOption("Load scene",[this] () { loadScene(); }) }
		});

	changeMode(InputMode::MAIN);

	std::vector<Figure*> default_prototypes({
		new Circle(),
		new Triangle(),
		new Rectangle()
		});

	m_prototype_names = std::list<std::string>({
		"Cirlce",
		"Triangle",
		"Rectangle"
		});

	m_prototypes.insert({
		{ "Cirlce",		default_prototypes[0]->clone() },
		{ "Triangle",	default_prototypes[1]->clone() },
		{ "Rectangle",	default_prototypes[2]->clone() }
		});

	m_primitiveFactory = new FigureFactory();
	m_primitiveFactory->registerType("Circle", Circle::create);
	m_primitiveFactory->registerType("Triangle", Triangle::create);
	m_primitiveFactory->registerType("Rectangle", Rectangle::create);
	m_primitiveFactory->registerType("Composite", Composite::create);
	Composite::setPrimitiveFactory(m_primitiveFactory);
	
}

void Program::run()
{
	while (scene_window.isOpen())
	{
		handleEvents();

		if (m_mode == InputMode::EDIT_OBJECT &&
			object_manipulation_type != ObjectManipulation::NONE)
		{
			handleObjectManipulation();
		}

		m_sceneController->update();

		m_sceneController->draw();
	}
}

Program::~Program()
{
	delete m_sceneController;
	for (auto& elem : m_prototypes)
	{
		delete elem.second;
	}
}

void Program::handleObjectManipulation()
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
		m_mouse_pressed_in_window &&
		m_active_figure != nullptr)
	{
		
		sf::Vector2f cur = getMouseWorldPosition();
		sf::Vector2f change(cur - mouse_position);
		if (change.x != 0 || change.y != 0)
		{
			m_active_composite_modified = true;
		}
		switch (object_manipulation_type)
		{
		case ObjectManipulation::MOVING:
			m_active_figure->move(change);
			break;
		case ObjectManipulation::ROTATING:
			m_active_figure->rotate(change.x);
			break;
		case ObjectManipulation::SCALING:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
			{
				m_active_figure->scale(sf::Vector2f(change.x, 0.f));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
			{
				m_active_figure->scale(sf::Vector2f(0.f, change.y));
			}
			break;
		case ObjectManipulation::COLORING:
		{
			if (change.x == 0) break;
			bool changed = false;
			auto& tmp = m_active_figure->getShape();
			sf::Color color;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			{
				color.r += change.x;
				changed = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
			{
				color.g += change.x;
				changed = true;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
			{
				color.b += change.x;
				changed = true;
			}
			if (changed)
			{
				m_active_figure->changeColor(color);
				system("cls");
				color = tmp.getFillColor();
				std::cout << "(F) Red:\t" << (int)color.r << '\n';
				std::cout << "(G) Green\t" << (int)color.g << '\n';
				std::cout << "(B) Blue:\t" << (int)color.b << '\n';
			}
		}
		break;
		default:
			break;
		}

		mouse_position = cur;
	}
}

void Program::handleEvents()
{
	sf::Event event;
	while (scene_window.pollEvent(event))
	{
		using enum sf::Event::EventType;
		switch (event.type)
		{
		case Closed:
			scene_window.close();
			break;
		case KeyPressed:
		case MouseButtonPressed:
		case MouseButtonReleased:
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
		OnKeyboardPress(event.key);
		break;
	case MouseButtonPressed:
		switch (m_mode)
		{
		case InputMode::EDIT_OBJECT:
			// Register changes in mouse position.
			m_mouse_pressed_in_window = true;
			mouse_position = getMouseWorldPosition();
			break;
		case InputMode::EXPAND_COMPOSITE:
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
		case InputMode::UNITE_COMPOSITES:
		{
			// unite two composites.
			std::pair<Figure*, Figure*> clicked_figure = m_sceneController->getIntersection(getMouseWorldPosition());
			Figure* second_of_union = clicked_figure.first;
			if (second_of_union == nullptr)
			{
				break;
			}
			if (first_of_union == nullptr)
			{
				first_of_union = second_of_union;
			}
			else
			{
				m_sceneController->remove(first_of_union);
				m_sceneController->remove(second_of_union);

				Composite* composite = new Composite();
				composite->add(first_of_union);
				composite->add(second_of_union);
				m_sceneController->add(composite);
				first_of_union = nullptr;
				changeMode(InputMode::EDIT_OBJECT);
			}
			break;
		}
		default:
			break;
		}
		break;
	case MouseButtonReleased:
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
			changeMode(InputMode::EDIT_OBJECT);
			break;
		}
		default:
			break;
		}
		handleObjectManipulation();
		active_manipulating_object = ObjectManipulation::NONE;
		m_mouse_pressed_in_window = false;
		m_active_composite_modified = false;
		break;
	default:
		break;
	}
}

void Program::OnKeyboardPress(sf::Event::KeyEvent key)
{
	switch (m_mode)
	{
	case InputMode::MAIN:
		switch (key.code)
		{
			// Construct new composite
		case sf::Keyboard::A:
			addFigure(InputMode::MAIN);
			break;
		/*case sf::Keyboard::P:
			changeMode(InputMode::VIEW_PROTOTYPES);
			selectPrototype();
			changeMode(InputMode::MAIN);
			break;*/
		case sf::Keyboard::U:
			changeMode(InputMode::UNITE_COMPOSITES);
			break;
		case sf::Keyboard::S:
			saveScene();
			updateModeHint(m_mode);
			break;
		case sf::Keyboard::L:
			loadScene();
			updateModeHint(m_mode);
			break;
		default:
			break;
		}
		break;
	case InputMode::ADD_FIGURE:
	{
		Figure* new_figure = nullptr;
		switch (key.code)
		{
		case sf::Keyboard::E:
			changeMode(InputMode::EXPAND_COMPOSITE);
			break;
			// Circle
		case sf::Keyboard::C:
		{
			new_figure = new Circle();
			break;
		}
		 // Triangle
		case sf::Keyboard::T:
			new_figure = new Triangle();
			break;
			// Rectangle
		case sf::Keyboard::R:
		{
			new_figure = new Rectangle();
			break;
		}
		// Cancel adding primitive
		case sf::Keyboard::Escape:
			changeMode(m_construct_composite == nullptr ? InputMode::MAIN : InputMode::EDIT_OBJECT);
			break;
		default:
			break;
		}
		if (new_figure != nullptr)
		{
			if (m_construct_composite == nullptr)
			{
				m_construct_composite = new Composite(new_figure);
				m_sceneController->add(m_construct_composite);
				setActive(new_figure);
			}
			else
			{
				m_construct_composite->add(new_figure);
				setActive(new_figure);
			}
			changeMode(InputMode::EDIT_OBJECT);
			object_manipulation_type = ObjectManipulation::MOVING;
		}
		break;
	}
	//case InputMode::EDIT_COMPOSITE:

	case InputMode::EDIT_OBJECT:
	{
		switch (key.code)
		{
		case sf::Keyboard::Delete:
		{
			// If editing composite, remove it from scene.
			if (m_construct_composite == m_active_figure)
			{
				m_sceneController->remove(m_construct_composite);
				delete m_construct_composite;
			}
			else // remove figure from composite.
			{
				m_construct_composite->remove(m_active_figure);
				setActive(nullptr);
				delete m_active_figure;
			}
			changeMode(InputMode::MAIN);
		}
		break;
		case sf::Keyboard::P:
		{
			createPrototype();
			break;
		}
		case sf::Keyboard::L:
			m_active_figure->changeMovement();
			break;
		case sf::Keyboard::T:
			m_active_figure->changeTail();
			break;
		case sf::Keyboard::V:
			m_active_figure->changeVisibility();
			break;
		case sf::Keyboard::C:
			m_active_figure->reset();
			break;
		case sf::Keyboard::A:
			m_active_figure == m_construct_composite;
			changeMode(InputMode::VIEW_PROTOTYPES);
			selectPrototype();
			break;
			// Scale object
		case sf::Keyboard::S:
			object_manipulation_type = ObjectManipulation::SCALING;
			break;
			// Rotate object
		case sf::Keyboard::R:
			object_manipulation_type = ObjectManipulation::ROTATING;
			break;
			// Move object
		case sf::Keyboard::M:
			object_manipulation_type = ObjectManipulation::MOVING;
			break;
		case sf::Keyboard::H:
		{
			if(m_active_figure == m_construct_composite)
			{
				break;
			}
			// TODO
			object_manipulation_type = ObjectManipulation::COLORING;
			system("cls");
			sf::Color color = m_active_figure->getShape().getFillColor();
			std::cout << "(F) Red:\t" << (int)color.r << '\n';
			std::cout << "(G) Green\t" << (int)color.g << '\n';
			std::cout << "(B) Blue:\t" << (int)color.b << '\n';
			m_active_figure->setActive(false);
		}	break;
		case sf::Keyboard::Escape:
			object_manipulation_type = ObjectManipulation::NONE;
			if (object_manipulation_type == ObjectManipulation::COLORING)
				m_active_figure->setActive(true);
			if (m_construct_composite == m_active_figure)
			{
				m_construct_composite = nullptr;
				setActive(nullptr);
				changeMode(InputMode::MAIN);
			}
			else
			{
				setActive(m_construct_composite);
				updateModeHint(m_mode);
			}
			break;
		default:
			break;
		}
		break;
	}
	case InputMode::UNITE_COMPOSITES:
		if (key.code == sf::Keyboard::Escape)
			changeMode(InputMode::MAIN);
		break;
	case InputMode::EXPAND_COMPOSITE:
		if (key.code == sf::Keyboard::Escape)
			changeMode(InputMode::EDIT_OBJECT);
		break;
	case InputMode::VIEW_PROTOTYPES:
		//if(key.code >= sf::Keyboard::Num1)
		switch (key.code)
		{
		case sf::Keyboard::Escape:
			changeMode(InputMode::MAIN);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Program::changeMode(InputMode _mode)
{
	m_mode = _mode;
	updateModeHint(m_mode);
}

void Program::setActive(Figure* figure)
{
	if (m_active_figure != nullptr)
		m_active_figure->setActive(false);
	m_active_figure = figure;
	if (m_active_figure != nullptr)
		m_active_figure->setActive(true);
}

void Program::updateModeHint(InputMode m_mode)
{
	system("cls");
	switch (m_mode)
	{
	case InputMode::MAIN:
		std::cout << "(A) Add composite" << '\n';
		std::cout << "(P) View prototype collection" << '\n';
		std::cout << "(U) Unity with another composite" << '\n';
		std::cout << "(S) Save scene" << '\n';
		std::cout << "(L) Load scene" << '\n';
		break;
	case InputMode::ADD_FIGURE:
		std::cout << "(E) Existing (choose on the screen)" << '\n';
		std::cout << "(C) Circle" << '\n';
		std::cout << "(T) Triangle" << '\n';
		std::cout << "(R) Rectangle" << '\n';
		break;
	case InputMode::EDIT_OBJECT:
		std::cout << (m_active_figure == m_construct_composite ?
			"Composite " : "Primitive") << " selected.\n";
		//system("cls");
		std::cout << "(Esc) Finish composition creation" << '\n';
		std::cout << "(Delete) Delete composite" << '\n';
		std::cout << "(A) Add another primitive" << '\n';
		std::cout << "(M) Move" << '\n';
		std::cout << "(R) Rotate" << '\n';
		std::cout << "(S) Scale" << '\n';
		std::cout << "(V) Change visibility" << '\n';
		std::cout << "(T) Change tail visibility" << '\n';
		std::cout << "(C) Change to default" << '\n';
		std::cout << "(L) Move object automatically" << '\n';
		if (m_active_figure != m_construct_composite)
		{
			std::cout << "(H) Change color" << '\n';
		}
		else
		{
			std::cout << "(P) Add to prototype collection" << '\n';
		}
		break;
	case InputMode::EXPAND_COMPOSITE:
		//system("cls");
		std::cout << "(Esc) Cancle" << '\n';
		std::cout << "Click on the composite you want to add to new composite." << '\n';
		break;
	case InputMode::UNITE_COMPOSITES:
		//system("cls");
		std::cout << "(Esc) Cancle" << '\n';
		std::cout << "Click on two composites you want to unite." << '\n';
		break;
	case InputMode::VIEW_PROTOTYPES:
	{
		int i = 1;
		for (auto& m_name : m_prototype_names)
		{
			std::cout << i++ << ". " << m_name << '\n';
		}
		break;
	}
	default:
		break;
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
	updateModeHint(m_mode);
}

Figure* Program::selectPrototype()
{
	std::cout << "Select prototype number (leave empty to cancel): ";
	int index = 0;
	std::string input;
	std::getline(std::cin, input);
	if (!input.empty())
	{
		index = atoi(input.c_str());
		if (index > 0 && index <= m_prototype_names.size())
		{
			std::string prototype_name = *std::next(m_prototype_names.begin(), index - 1);
			return (Figure*)m_prototypes[prototype_name]->clone();
			/*if (m_construct_composite == nullptr)
			{
				m_sceneController->add(new_figure);
			}
			else
			{
				m_construct_composite->add(new_figure);
			}*/
		}
	}
	return nullptr;
}

void Program::addFigure(InputMode back_to)
{
	changeMode(InputMode::VIEW_PROTOTYPES);
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

void Program::addFigure()
{
	changeMode(InputMode::VIEW_PROTOTYPES);
	// Primitive can not exist by himself on the scene.
	if (m_construct_composite == nullptr)
	{
		m_construct_composite = new Composite();
	}
	Figure* new_figure = selectPrototype();
	if (new_figure != nullptr)
	{
		m_construct_composite->add(new_figure);
	}
	setActive(m_construct_composite);
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
	m_menu_stack->pop();
}

sf::Vector2f Program::getMouseWorldPosition()
{
    sf::Vector2i pixel_coord = sf::Mouse::getPosition(scene_window);
    return scene_window.mapPixelToCoords(pixel_coord);
}
