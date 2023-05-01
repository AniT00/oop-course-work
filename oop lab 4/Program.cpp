#include "Program.h"

Program::Program()
	: scene_window(sf::VideoMode(SCENE_WINDOW_WIDTH, SCENE_WINDOW_HEIGHT), "Scene")
{
	scene_window.setFramerateLimit(FRAME_LIMIT);
	sceneController = SceneController::GetInstance(&scene_window);
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

		sceneController->update();

		sceneController->draw();

		//hierarchy_window.clear(Color::White);

		//hierarchy_window.display();
	}
}

Program::~Program()
{
	delete sceneController;
}

void Program::handleObjectManipulation()
{
	if (mode == InputMode::OBJECT_EDITING &&
		active_manipulating_object != ObjectManipulation::NONE
		&& sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		sf::Vector2i cur = sf::Mouse::getPosition(scene_window);
		sf::Vector2f change(cur - mouse_position);
		auto m_active = sceneController->getActiveFigure();
		switch (active_manipulating_object)
		{
		case ObjectManipulation::MOVING:
			m_active->move(change);
			break;
		case ObjectManipulation::ROTATING:
			m_active->rotate(change.x);
			break;
		case ObjectManipulation::SCALING:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
			{
				m_active->scale(sf::Vector2f(change.x, 0.f));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
			{
				m_active->scale(sf::Vector2f(0.f, change.y));
			}
			break;
		case ObjectManipulation::COLORING:
		{
			if (change.x == 0) break;
			bool changed = false;
			auto& tmp = sceneController->getActiveFigure()->getShape();
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
				sceneController->getActiveFigure()->changeColor(color);
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
		{
			std::pair<Figure*, Figure*> clicked_figure = sceneController->getIntersection(sf::Vector2f(sf::Mouse::getPosition(scene_window)));
			if (clicked_figure.first != nullptr)
			{
				sceneController->setCurrentFigure(clicked_figure.first);
				active_composite = clicked_figure.first;

				changeMode(InputMode::OBJECT_EDITING);
			}
		}
		else if (mode == InputMode::EXPAND_COMPOSITE)
		{
			std::pair<Figure*, Figure*> clicked_figure = sceneController->getIntersection(sf::Vector2f(sf::Mouse::getPosition(scene_window)));
			if (clicked_figure.first != nullptr &&
				clicked_figure.first != sceneController->getCurrentComposite()) // Trying to add yourself in.
			{
				sceneController->giveAway(clicked_figure.first);
				sceneController->add(clicked_figure.first);
				changeMode(InputMode::OBJECT_EDITING);
			}
		}
		else if (mode == InputMode::UNITE_COMPOSITES)
		{
			std::pair<Figure*, Figure*> clicked_figure = sceneController->getIntersection(sf::Vector2f(sf::Mouse::getPosition(scene_window)));
			if (clicked_figure.first != nullptr)
			{
				if (first_of_unity == nullptr)
				{
					first_of_unity = clicked_figure.first;
				}
				else
				{
					sceneController->giveAway(first_of_unity);
					sceneController->giveAway(clicked_figure.first);

					sceneController->endNewComposite();
					sceneController->createNewComposite();
					sceneController->add(first_of_unity);
					sceneController->add(clicked_figure.first);
					first_of_unity = nullptr;
					changeMode(InputMode::OBJECT_EDITING);
				}
			}
		}
		else
		{
			mouse_position = sf::Mouse::getPosition(scene_window);
			active_manipulating_object = object_manipulation_type;
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
			sceneController->endNewComposite();
			sceneController->createNewComposite();
			changeMode(InputMode::ADD_COMPOSITE);
			break;
		case sf::Keyboard::U:
			changeMode(InputMode::UNITE_COMPOSITES);
			break;
		default:
			break;
		}
		break;
	case InputMode::ADD_COMPOSITE:
	{
		Figure* new_object = nullptr;
		switch (key.code)
		{
		case sf::Keyboard::E:
			changeMode(InputMode::EXPAND_COMPOSITE);
			break;
			// Circle
		case sf::Keyboard::C:
		{
			new_object = new Circle();
			break;
		}/////////////////////////////////////////////////////////////////////
		 // Triangle
		case sf::Keyboard::T:
			new_object = new Triangle();
			break;
			// Rectangle
		case sf::Keyboard::R:
		{
			new_object = new Rectangle();
			break;
		}
		// Finish creating composition
		case sf::Keyboard::Escape:
			sceneController->endNewComposite();
			changeMode(InputMode::MAIN);
			break;
		default:
			break;
		}
		if (new_object != nullptr)
		{
			sceneController->add(new_object);
			if (active_composite == nullptr)
			{
				active_composite = new_object;
			}

			changeMode(InputMode::OBJECT_EDITING);
			object_manipulation_type = ObjectManipulation::MOVING;
		}
		break;
	}
	case InputMode::OBJECT_EDITING:
	{
		switch (key.code)
		{
		case sf::Keyboard::Delete:
		{
			sceneController->deleteNewComposite();
			changeMode(InputMode::MAIN);
		}
		break;
		case sf::Keyboard::P:
			sceneController->getActiveFigure()->changeMovement();
			break;
		case sf::Keyboard::T:
			sceneController->getActiveFigure()->changeTail();
			break;
		case sf::Keyboard::V:
			sceneController->getActiveFigure()->changeVisibility();
			break;
		case sf::Keyboard::C:
			sceneController->resetActiveToDefault();
			break;
			// Finish this and add another object to composition
		case sf::Keyboard::A:
			changeMode(InputMode::ADD_COMPOSITE);
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
			object_manipulation_type = ObjectManipulation::COLORING;
			system("cls");
			sf::Color color = sceneController->getActiveFigure()->getShape().getFillColor();
			std::cout << "(F) Red:\t" << (int)color.r << '\n';
			std::cout << "(G) Green\t" << (int)color.g << '\n';
			std::cout << "(B) Blue:\t" << (int)color.b << '\n';
			sceneController->getActiveFigure()->setActive(false);
		}	break;
		case sf::Keyboard::O:
			sceneController->getActiveFigure()->changeMovement();
			break;
		case sf::Keyboard::Escape:
			if (object_manipulation_type == ObjectManipulation::COLORING)
				sceneController->getActiveFigure()->setActive(true);
			sceneController->endNewComposite();
			sceneController->setCurrentComposite(nullptr);
			sceneController->setCurrentFigure(nullptr);
			active_composite = nullptr;
			changeMode(InputMode::MAIN);

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
			changeMode(InputMode::OBJECT_EDITING);
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
	case InputMode::ADD_COMPOSITE:
		std::cout << "(E) Existing (choose on the screen)" << '\n';
		std::cout << "(C) Circle" << '\n';
		std::cout << "(T) Triangle" << '\n';
		std::cout << "(R) Rectangle" << '\n';
		break;
	case InputMode::OBJECT_EDITING:
		system("cls");
		std::cout << "(Esc) Finish composition creation" << '\n';
		std::cout << "(Delete) Delete composite" << '\n';
		std::cout << "(A) Finish this and add another figure" << '\n';
		std::cout << "(M) Move" << '\n';
		std::cout << "(R) Rotate" << '\n';
		std::cout << "(S) Scale" << '\n';
		std::cout << "(V) Change visibility" << '\n';
		std::cout << "(H) Change color" << '\n';
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
