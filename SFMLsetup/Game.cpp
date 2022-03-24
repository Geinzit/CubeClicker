#include "Game.h"


// Private Functions
void Game::initVariables()
{
	this->videoMode.height = 1080;
	this->videoMode.width = 1920;
	this->window = nullptr;

	/*
	int points;
	float enemySpawnTimer;
	float enemySpawnTimerMax;
	int maxEnemies;
	*/
	this->health = 5;
	this->points = 0;
	this->enemySpawnTimer = 0.f;
	this->enemySpawnTimerMax = 10.f;
	this->maxEnemies = 10;
	this->mouseHeld = false;
	this->endGame = false;

}

void Game::initWindow()
{
	this->window = new sf::RenderWindow(this->videoMode, "Game 1", sf::Style::Titlebar | sf::Style::Close);
	this->window->setFramerateLimit(60);
}

void Game::initFont()
{
	if (this->font.loadFromFile("Fonts/Dosis-Light.ttf"))
	{
		std::cout << "ERROR::GAME::INITFONTS::Failed to load from files" << "\n";
	}
}

void Game::initText()
{
	this->uiText.setFont(this->font);
	this->uiText.setCharacterSize(30);
	this->uiText.setFillColor(sf::Color::White);
	this->uiText.setString("None");
}

void Game::initEnemies()
{
	this->enemy.setPosition(10.f, 10.f);
	this->enemy.setSize(sf::Vector2f(100.f,100.f));
	this->enemy.setScale(sf::Vector2f(0.5f, 0.5f));
	this->enemy.setFillColor(sf::Color::Cyan);
	
	/*
	this->enemy.setOutlineColor(sf::Color::Green);
	this->enemy.setOutlineThickness(1.f);
	*/


}

//COnstructors / Destructors
Game::Game()
{
	this->initVariables();
	this->initWindow();
	this->initEnemies();
	this->initFont();
	this->initText();
}

Game::~Game() 
{
	delete this->window;
}

const bool Game::running() const
{
	return this->window->isOpen();
}
const bool Game::getEndGame() const
{
	return this->endGame;
}
//functions

void Game::spawnEnemies()
{
	/*
		@return void

		Spawns enemies and sets their colors and positions.
		-Sets a random position
		-Sets a random color

	*/
	this->enemy.setPosition(
		static_cast<float>(rand() % static_cast<int>(this->window->getSize().x - this->enemy.getSize().x)),
		0.f
		);

	//randomize enemy type
	int typ = rand() % 5;
	
	switch (typ)
	{
	case 0:
		this->enemy.setSize(sf::Vector2f(50.f, 50.f));
		this->enemy.setFillColor(sf::Color::Red);
		break;
	case 1:
		this->enemy.setSize(sf::Vector2f(75.f, 75.f));
		this->enemy.setFillColor(sf::Color::Blue);
		break;
	case 2:
		this->enemy.setSize(sf::Vector2f(100.f, 100.f));
		this->enemy.setFillColor(sf::Color::Green);
		break;
	case 3:
		this->enemy.setSize(sf::Vector2f(125.f, 125.f));
		this->enemy.setFillColor(sf::Color::Cyan);
		break;
	case 4:
		this->enemy.setSize(sf::Vector2f(150.f, 150.f));
		this->enemy.setFillColor(sf::Color::White);
		break;
	default:
		this->enemy.setSize(sf::Vector2f(50.f, 50.f));
		this->enemy.setFillColor(sf::Color::Yellow);
		break;
	}

	this->enemies.push_back(enemy);
}

void Game::pollEvents()
{
	while (this->window->pollEvent(this->ev))
	{
		switch (this->ev.type)
		{
		case sf::Event::Closed:
			this->window->close();
			break;
		case sf::Event::KeyPressed:
			if (this->ev.key.code == sf::Keyboard::Escape)
				this->window->close();
			break;
		}
	}
}

void Game::updateText()
{
	std::stringstream ss;

	ss << "Health: " << this->health << '\n'
		<< "Points: " << this->points;
	
	this->uiText.setString(ss.str());
}

void Game::updateMousePosition()
{
	/*
		@ return void
		;
		updates the mouse positions:
			- Mouse position relative to window (Vector2i)

	*/
	this->mousePosWindow = sf::Mouse::getPosition(*this->window);
	this->mousePosView = this->window->mapPixelToCoords(this->mousePosWindow);
}

void Game::updateEnemies()
{
	/*
		@return void

		-Removes the enemies at the edge of the screen
	*/
	if (this->enemies.size() < this->maxEnemies)
	{
		if (this->enemySpawnTimer >= this->enemySpawnTimerMax)
		{
			this->spawnEnemies();
			this->enemySpawnTimer = 0.f;
		}
		else
			this->enemySpawnTimer += 1.f;
	}

	//Moving and updating the enemies
	for (int i = 0; i < this->enemies.size(); i++) 
	{
		this->enemies[i].move(0.f, 2.f);
		
		//If the enemy is past the bottom of the screen
		if (this->enemies[i].getPosition().y > this->window->getSize().y)
		{
			this->enemies.erase(this->enemies.begin() + i);
			this->health -= 1;
		}
	}
	//Check if clicked upon
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if(this->mouseHeld==false)
		{
			this->mouseHeld = true;
			for (size_t i = 0; i < this->enemies.size(); i++)
			{
				if (this->enemies[i].getGlobalBounds().contains(this->mousePosView))
				{
					//Gain points
					if (this->enemies[i].getFillColor() == sf::Color::Red)
						this->points += 50;
					else if(this->enemies[i].getFillColor() == sf::Color::Blue)
						this->points += 40;
					else if (this->enemies[i].getFillColor() == sf::Color::Green)
						this->points += 30;
					else if (this->enemies[i].getFillColor() == sf::Color::Cyan)
						this->points += 20;
					else if (this->enemies[i].getFillColor() == sf::Color::White)
						this->points += 10;

					//Delete the enemy
					this->enemies.erase(this->enemies.begin() + i);
					break;
				}
			}
		}
	}
	else
	{
		this->mouseHeld = false;
	}
}

void Game::update()
{
	//Event polling
	this->pollEvents();

	if (this->endGame == false)
	{
		this->updateMousePosition();

		this->updateText();

		this->updateEnemies();	
	}


	//endGame=true
	if (this->health <= 0)
		this->endGame = true;
}


void Game::renderText(sf::RenderTarget& target)
{
	target.draw(this->uiText);
}

void Game::renderEnemies(sf::RenderTarget& target)
{
	for (auto& e : this->enemies)
	{
		target.draw(e);
	}
}

void Game::render() 
{
	/*
		@return void

		- clear old frame
		- render objects
		- display frame in window

		Renders the game objects
	*/
	this->window->clear();

	//Draw game objects
	this->renderEnemies(*this->window);
	
	this->renderText(*this->window);

	this->window->display();

}

