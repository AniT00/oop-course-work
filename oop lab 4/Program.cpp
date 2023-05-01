#include "Program.h"

Program::Program()
	: scene_window(sf::VideoMode(SCENE_WINDOW_WIDTH, SCENE_WINDOW_HEIGHT), "Scene")
{
	scene_window.setFramerateLimit(FRAME_LIMIT);
	m_sceneController = SceneController::GetInstance(&scene_window);
	changeMode(InputMode::MAIN);
}

void Program::run()
{

	/*RenderWindow hierarchy_window
	(
	VideoMode
	(
	HIERARCHY_WINDOW_WIDTH,
	HIERARCHY_WINDOW_HEIGHT
	),
	"Object hierarchy");*/

	while (scene_window.isOpen())
	{
		handleEvents();

		handleObjectManipulation();

		m_sceneController->update();

		m_sceneController->draw();

		//hierarchy_window.clear(Color::White);

		//hierarchy_window.display();
	}
}

Program::~Program()
{
	delete m_sceneController;
}

void Program::handleObjectManipulation()
{
	if (mode == InputMode::EDIT_OBJECT &&
		object_manipulation_type != ObjectManipulation::NONE
		&& sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		sf::Vector2i cur = sf::Mouse::getPosition(scene_window);
		sf::Vector2f change(cur - mouse_position);
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
		if (mode == InputMode::MAIN)
		{	// Select object.
			std::pair<Figure*, Figure*> clicked_figure = m_sceneController->getIntersection(sf::Vector2f(sf::Mouse::getPosition(scene_window)));
			Figure* composite = clicked_figure.first;
			Figure* primitive = clicked_figure.second;
			if (composite != nullptr)
			{
				setActive(m_active_figure == composite ? primitive : composite);
				changeMode(InputMode::EDIT_OBJECT);
			}
		}
		else if (mode == InputMode::EXPAND_COMPOSITE)
		{	// add existing composite.
			std::pair<Figure*, Figure*> clicked_figure = m_sceneController->getIntersection(sf::Vector2f(sf::Mouse::getPosition(scene_window)));
			if (clicked_figure.first != nullptr &&
				clicked_figure.first != m_construct_composite) // Trying to add yourself in.
			{
				m_sceneController->remove(clicked_figure.first);
				m_construct_composite->add(clicked_figure.first);
				changeMode(InputMode::EDIT_OBJECT);
			}
		}
		else if (mode == InputMode::UNITE_COMPOSITES)
		{	// unite two composites.
			std::pair<Figure*, Figure*> clicked_figure = m_sceneController->getIntersection(sf::Vector2f(sf::Mouse::getPosition(scene_window)));
			Figure* second_of_union = clicked_figure.first;
			if (second_of_union != nullptr)
			{
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
			}
		}
		else
		{
			mouse_position = sf::Mouse::getPosition(scene_window);
		}
		break;
	case MouseButtonReleased:
		handleObjectManipulation();
		active_manipulating_object = ObjectManipulation::NONE;
		break;
	default:
		break;
	}
}

void Program::OnKeyboardPress(sf::Event::KeyEvent key)
{
	switch (mode)
	{
	case InputMode::MAIN:
		switch (key.code)
		{
			// Construct new composite
		case sf::Keyboard::A:
			changeMode(InputMode::ADD_FIGURE);
			break;
		case sf::Keyboard::U:
			changeMode(InputMode::UNITE_COMPOSITES);
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
		}/////////////////////////////////////////////////////////////////////
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
			changeMode(InputMode::EDIT_OBJECT);
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
			changeMode(InputMode::ADD_FIGURE);
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
			// TODO
			object_manipulation_type = ObjectManipulation::COLORING;
			system("cls");
			sf::Color color = m_active_figure->getShape().getFillColor();
			std::cout << "(F) Red:\t" << (int)color.r << '\n';
			std::cout << "(G) Green\t" << (int)color.g << '\n';
			std::cout << "(B) Blue:\t" << (int)color.b << '\n';
			m_active_figure->setActive(false);
		}	break;
		case sf::Keyboard::O:
			m_active_figure->changeMovement();
			break;
		case sf::Keyboard::Escape:
			if (object_manipulation_type == ObjectManipulation::COLORING)
				m_active_figure->setActive(true);
			if (m_construct_composite == m_active_figure)
			{
				m_construct_composite = nullptr;
				setActive(nullptr);
			}
			else
			{
				m_active_figure = m_construct_composite;
			}
			changeMode(InputMode::EDIT_OBJECT);
			break;
		default:
			break;
		}
	}
	break;
	case InputMode::UNITE_COMPOSITES:
		if (key.code == sf::Keyboard::Escape)
			changeMode(InputMode::MAIN);
		break;
	case InputMode::EXPAND_COMPOSITE:
		if (key.code == sf::Keyboard::Escape)
			changeMode(InputMode::EDIT_OBJECT);
		break;
	default:
		break;
	}
}

void Program::changeMode(InputMode _mode)
{
	system("cls");
	switch (_mode)
	{
	case InputMode::MAIN:
		std::cout << "(A) Add composite" << '\n';
		std::cout << "(U) Unity with another composite" << '\n';
		break;
	case InputMode::ADD_FIGURE:
		std::cout << "(E) Existing (choose on the screen)" << '\n';
		std::cout << "(C) Circle" << '\n';
		std::cout << "(T) Triangle" << '\n';
		std::cout << "(R) Rectangle" << '\n';
		break;
	case InputMode::EDIT_OBJECT:
		system("cls");
		std::cout << "(Esc) Finish composition creation" << '\n';
		std::cout << "(Delete) Delete composite" << '\n';
		std::cout << "(A) Add another primitive" << '\n';
		std::cout << "(M) Move" << '\n';
		std::cout << "(R) Rotate" << '\n';
		std::cout << "(S) Scale" << '\n';
		std::cout << "(V) Change visibility" << '\n';
		if (m_active_figure != m_construct_composite)
		{
			std::cout << "(H) Change color" << '\n';
		}
		std::cout << "(T) Change tail visibility" << '\n';
		std::cout << "(O) Move automatically" << '\n';
		std::cout << "(C) Change to default" << '\n';
		std::cout << "(P) Move object automatically" << '\n';
		break;
	case InputMode::EXPAND_COMPOSITE:
		system("cls");
		std::cout << "(Esc) Cancle" << '\n';
		std::cout << "Click on the composite you want to add to new composite." << '\n';
		break;
	case InputMode::UNITE_COMPOSITES:
		system("cls");
		std::cout << "(Esc) Cancle" << '\n';
		std::cout << "Click on two composites you want to unite." << '\n';
		break;
	default:
		break;
	}
	mode = _mode;
}

void Program::setActive(Figure* figure)
{
	if (m_active_figure != nullptr)
		m_active_figure->setActive(false);
	m_active_figure = figure;
	if (m_active_figure != nullptr)
		m_active_figure->setActive(true);
}

