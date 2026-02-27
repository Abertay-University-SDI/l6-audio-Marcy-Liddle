#include "Level.h"

Level::Level(sf::RenderWindow& hwnd, Input& in, GameState& gs, AudioManager& audio) :
    BaseLevel(hwnd, in, gs, audio), m_timerText(m_font), m_winText(m_font), m_scoreboardText(m_font)//, m_corpoSound(m_corpoBuffer), m_natureSound(m_natureBuffer), m_yaySound(m_yayBuffer), m_bahSound(m_bahBuffer)
{
    if (!m_font.openFromFile("font/arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }

    // expensive file i/o we will do only once
    if (!m_rabbitTexture.loadFromFile("gfx/rabbit_sheet.png")) std::cerr << "no rabbit texture";
    if (!m_sheepTexture.loadFromFile("gfx/sheep_sheet.png")) std::cerr << "no sheep texture";

    // everything else we can chuck into reset()
    m_playerRabbit = nullptr;    // ensures nothing is deleted inside reset();
    reset();   

}

Level::~Level()
{
    // We made a lot of **new** things, allocating them on the heap
    // So now we have to clean them up
    delete m_playerRabbit;
    for (Sheep* s : m_sheepList)
    {
        delete s;
    }
    m_sheepList.clear();
}

void Level::reset()
{
    m_audio.stopAllMusic();

    m_audio.playMusicbyName("nature");

    levelSize = { 800.f, 800.f };
    m_levelBounds = { {0.f, 0.f}, {levelSize } };
    m_isGameOver = false;
    if (m_playerRabbit) delete m_playerRabbit;
    m_playerRabbit = new Rabbit();

    
    m_playerRabbit->setSize({ 32,32 });
    m_playerRabbit->setInput(&m_input);
    m_playerRabbit->setTexture(&m_rabbitTexture);
    m_playerRabbit->setWorldSize(levelSize.x, levelSize.y);

    m_timerText.setFont(m_font);
    m_timerText.setCharacterSize(24);
    m_timerText.setFillColor(sf::Color::White);

    // "Game Over" text setup
    m_winText.setFont(m_font);
    m_winText.setString("ROUND COMPLETE!");
    m_winText.setCharacterSize(50);
    m_winText.setFillColor(sf::Color::Blue);
    m_winText.setPosition({ -1000.f, 100.f });

    m_bgFarm.setFillColor(sf::Color::Green);
    m_bgFarm.setSize(levelSize);
    m_scoreboardText.setFont(m_font);

    m_walls.clear();
    for (Sheep* s : m_sheepList)
    {
        delete s;
    }
    m_sheepList.clear();

    loadLevel("data/level1.txt", levelSize);

    m_timeSpent = 0.f;
    m_sheepTimer = 0.f;

}

void Level::UpdateCamera()
{
    sf::View view = m_window.getView();
    sf::Vector2f targetPos = m_playerRabbit->getPosition();

    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f halfSize = { viewSize.x / 2.f, viewSize.y / 2.f };

    sf::Vector2f newCenter;

    newCenter.x = std::clamp(targetPos.x,
        m_levelBounds.position.x + halfSize.x,
        m_levelBounds.size.x - halfSize.x);

    newCenter.y = std::clamp(targetPos.y,
        m_levelBounds.position.y + halfSize.y,
        m_levelBounds.size.y - halfSize.y);

    view.setCenter(newCenter);

    // top-left corner of the current view in world space
    sf::Vector2f viewTopLeft = newCenter - halfSize;
    // Position text at the bottom-left of the current view with a small margin (e.g., 30px)
    float margin = 30.f;
    m_timerText.setPosition({ viewTopLeft.x + margin, viewTopLeft.y + viewSize.y - margin });

    m_window.setView(view);
}

// handle user input
void Level::handleInput(float dt)
{
    if (m_input.isPressed(sf::Keyboard::Scancode::Escape))
        m_gameState.setCurrentState(State::MENU);
    m_playerRabbit->handleInput(dt);
}

// checks and manages sheep-sheep, sheep-wall, sheep-goal, player-wall
void Level::manageCollisions()
{
    for (int i = 0; i < m_sheepList.size(); i++)
    {
        if (!m_sheepList[i]->isAlive()) continue;   // ignore scored sheep.

        // sheep collide with walls, with other sheep and with the goal.
        for (auto wall : m_walls)
        {
            if (Collision::checkBoundingBox(wall, *m_sheepList[i]))
            {
                m_sheepList[i]->collisionResponse(wall);
            }
        }
        for (int j = i + 1; j < m_sheepList.size(); j++)
        {
            if (!m_sheepList[j]->isAlive()) continue; // ignore scored sheep here too
            if (Collision::checkBoundingBox(*m_sheepList[i], *m_sheepList[j]))
            {
                // DANGER check i and j carefully here team.
                m_sheepList[i]->collisionResponse(*m_sheepList[j]);
                m_sheepList[j]->collisionResponse(*m_sheepList[i]);
            }
        }
        if (Collision::checkBoundingBox(*m_sheepList[i], m_goal))
        {
            m_audio.playSoundbyName("yay");
            m_sheepList[i]->collideWithGoal(m_goal); 
            spawnSheep(levelSize);
            m_maxTime += m_additionalTime;
        }
            
    }
    for (auto wall : m_walls)
    {
        if (Collision::checkBoundingBox(wall, *m_playerRabbit))
        {
            m_playerRabbit->collisionResponse(wall);
        }
    }
}

// Update game objects
void Level::update(float dt)
{
    if (m_isGameOver) return;   // if the game is over, don't continue trying to process game logic

    m_playerRabbit->update(dt);
    for (Sheep* s : m_sheepList)
    {
        if (s->isAlive()) s->update(dt);
    }

    // Timer 
    m_timeSpent += dt;
    float timeElapsed = m_maxTime - m_timeSpent;
    m_timerText.setString("Time: " + std::to_string(static_cast<int>(timeElapsed)));


    manageCollisions();
    UpdateCamera();
    m_isGameOver = timeElapsed > 99;    // temporary

    m_sheepTimer += dt;
    if (m_sheepTimer >= SHEEP_INTERVAL)
    {
        spawnSheep(levelSize);
    }

    if (m_isGameOver)
    {
        // will happen ONCE in the frame that game over is triggered.
        writeHighScore(timeElapsed);
        displayScoreboard();
    }
}

// Render level
void Level::render()
{
    beginDraw();
    m_window.draw(m_bgFarm);
    m_window.draw(m_goal);
    for (auto w : m_walls) m_window.draw(w);
    for (auto s : m_sheepList)
    {
        if (s->isAlive()) m_window.draw(*s);
    }
    m_window.draw(*m_playerRabbit);
    m_window.draw(m_timerText);

    if (m_isGameOver)
    {
        m_window.draw(m_winText);
        m_window.draw(m_scoreboardText);
    }

    displayHUD();

    endDraw();
}

void Level::writeHighScore(float timeTaken)
{
    std::ofstream outputFile("data/scoreboard.txt", std::ios::app);

    if (outputFile.is_open())
    {
        outputFile << std::fixed << std::setprecision(2) << timeTaken << "\n";
        outputFile.close();
    }
}

void Level::displayScoreboard()
{
    std::ifstream inputFile("data/scoreboard.txt");
    std::string line;
    std::string fullText = "Highscores:\n";
    while (std::getline(inputFile, line))
    {
        fullText += line + "\n";
    }
    inputFile.close();

    m_scoreboardText.setString(fullText);
    m_scoreboardText.setCharacterSize(24);
    m_scoreboardText.setFillColor(sf::Color::Black);
    m_scoreboardText.setPosition({ 400,200 });
}

void Level::loadLevel(std::string fileName, sf::Vector2f worldSize)
{
    std::ifstream inputFile(fileName);
    std::string object;
    float x;
    float y;
    float w;
    float h;

    // while I can read a line which is "string float float"
    while (inputFile >> object)
    {
        if (object == "SHEEP")
        {
            inputFile >> x >> y;
            Sheep* newSheep = new Sheep(sf::Vector2f(x, y), m_playerRabbit);
            newSheep->setTexture(&m_sheepTexture);
            newSheep->setSize({ 32,32 });
            newSheep->setWorldSize(worldSize.x, worldSize.y);
            newSheep->setAudioPointer(&m_audio);
            m_sheepList.push_back(newSheep);

        }
        else if (object == "RABBIT")
        {
            inputFile >> x >> y;
            m_playerRabbit->setPosition({ x, y });
        }
        else if (object == "GOAL")
        {
            inputFile >> x >> y;
            m_goal.setSize({ 50, 50 });
            m_goal.setFillColor(sf::Color::Blue);
            m_goal.setPosition({ x, y });
            m_goal.setCollisionBox({ { 0,0 }, { 50,50 } });

        }
        else if (object == "WALL")
        {
            inputFile >> x >> y >> w >> h;
            GameObject wall;
            wall.setPosition({ x, y });
            wall.setSize({ w,h });
            wall.setFillColor(sf::Color::Black);
            wall.setCollisionBox({ { 0,0 }, { w,h } });
            m_walls.push_back(wall);
        }
        else
        {
            std::cout << "Type " << object << " found, but not recognised\n";
        }
    }
}


void Level::spawnSheep(sf::Vector2f worldSize)
{
    float x = rand() % (static_cast<int>(worldSize.x));
    float y = rand() % (static_cast<int>(worldSize.y));

    while (!checkPositionOutsideWalls({ x,y }))
    {
        x = rand() % (static_cast<int>(worldSize.x));
        y = rand() % (static_cast<int>(worldSize.y));
    }

    Sheep* newSheep = new Sheep(sf::Vector2f(x, y), m_playerRabbit);
    newSheep->setTexture(&m_sheepTexture);
    newSheep->setSize({ 32,32 });
    newSheep->setWorldSize(worldSize.x, worldSize.y);
    newSheep->setAudioPointer(&m_audio);
    m_sheepList.push_back(newSheep);

    m_sheepTimer = 0.f;
}

bool Level::checkPositionOutsideWalls(sf::Vector2f pos)
{
    sf::Vector2i checkPos
    ({
         static_cast<int>(pos.x),
         static_cast<int>(pos.y)
      });

    for (auto& wall : m_walls)
        if (Collision::checkBoundingBox(wall, checkPos)) return false;
    return true;

}


void Level::displayHUD()
{

    // step one: get the references
    sf::View world_view = m_window.getView();
    sf::Vector2f middle = world_view.getCenter();
    sf::Vector2f vSize = world_view.getSize();

    // step two: set to default view
    m_window.setView(m_window.getDefaultView());

    // step three: position markers
    float half_w = vSize.x / 2.0f;
    float half_h = vSize.y / 2.0f;

    for (auto& sheep : m_sheepList)
    {
        if (!sheep->isAlive()) continue;

        // by default markers should be in middle of x and y
        float marker_x = half_w;
        float marker_y = half_h;

        sf::Vector2f sPos = sheep->getPosition();

        if (sPos.x > middle.x + half_w) {
            marker_x = vSize.x - 10.f;
        }
        else if (sPos.x < middle.x - half_w) {
            marker_x = 10.f;
        }

        if (sPos.y > middle.y + half_h) {
            marker_y = vSize.y - 10.f;
        }
        else if (sPos.y < middle.y - half_h) {
            marker_y = 10.f;
        }

        if (marker_x == half_w && marker_y == half_h) {
            continue;
        }


        // still step three: display the markers
        sf::CircleShape marker(5.f);
        marker.setOrigin({ 5.f, 5.f });
        marker.setFillColor(sf::Color::Red);
        marker.setPosition({ marker_x, marker_y });
        m_window.draw(marker);
    }

    // step four: Restore world view 
    m_window.setView(world_view);

}