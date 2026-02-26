#pragma once

#include "Framework/BaseLevel.h"
#include "Framework/GameObject.h"
#include "Framework/Collision.h"
#include "Sheep.h"
#include "Rabbit.h"
#include "Framework/AudioManager.h"
#include <SFML/Audio.hpp>

#include <iomanip>      // Phase one includes
#include <fstream>

class Level : BaseLevel {
public:
	Level(sf::RenderWindow& window, Input& input, GameState& state, AudioManager& audio);
	~Level() override;

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

    void spawnSheep(sf::Vector2f worldSize);
    bool checkPositionOutsideWalls(sf::Vector2f pos)

    void reset();

private:
    void UpdateCamera();
    void manageCollisions();

    void writeHighScore(float timeTaken);  
    void displayScoreboard();

    void loadLevel(std::string fileName, sf::Vector2f worldSize); 

    sf::View m_cameraView;

    // Entities
    Rabbit* m_playerRabbit;
    std::vector<Sheep*> m_sheepList;
    std::vector<GameObject> m_walls; 
    GameObject m_goal;
    sf::Texture m_sheepTexture;
    sf::Texture m_rabbitTexture;
    GameObject m_bgFarm;

    // Level Data
    sf::FloatRect m_levelBounds;
    bool m_isGameOver;

    // UI & Timer
    float m_timeSpent;
    float m_sheepTimer;
    float m_maxTime = 30.f;
    const float SHEEP_INTERVAL = 10.f;
    float m_additionalTime = 5.f;

    sf::Font m_font;
    sf::Text m_timerText;
    sf::Text m_winText;
	
    sf::Text m_scoreboardText;  
    std::string m_levelName;


};