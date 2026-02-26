#include "Sheep.h"

Sheep::Sheep(sf::Vector2f initial_position, GameObject* pRabbit)
{
	setPosition(initial_position);
	m_rabbitPointer = pRabbit;


	for (int i = 0; i < 4; i++)
		m_walkDown.addFrame({ { 64 * i, 0 }, { 64, 64 } });
	m_walkDown.setLooping(true);
	m_walkDown.setFrameSpeed(0.25f);

	for (int i = 0; i < 4; i++)
		m_walkUp.addFrame({ { (64 * (i + 4)), 0 }, { 64, 64 } });
	m_walkUp.setLooping(true);
	m_walkUp.setFrameSpeed(0.25f);

	for (int i = 0; i < 4; i++)
		m_walkUpRight.addFrame({ { 64 * i, 64 }, { 64, 64 } });
	m_walkUpRight.setLooping(true);
	m_walkUpRight.setFrameSpeed(0.25f);

	for (int i = 0; i < 4; i++)
		m_walkRight.addFrame({ { (64 * (i + 4)), 64 }, { 64, 64 } });
	m_walkRight.setLooping(true);
	m_walkRight.setFrameSpeed(0.25f);

	for (int i = 0; i < 4; i++)
		m_walkDownRight.addFrame({ { 64 * i, 128 }, { 64, 64 } });
	m_walkDownRight.setLooping(true);
	m_walkDownRight.setFrameSpeed(0.25f);

	m_currentAnimation = &m_walkDown;
	setTextureRect(m_currentAnimation->getCurrentFrame());
	setCollisionBox({ { 4,4 }, { 24,24} });
}

Sheep::~Sheep()
{
}


void Sheep::update(float dt)
{
	// get vector from position to rabbit
	sf::Vector2f to_rabbit = m_rabbitPointer->getPosition() - getPosition();
	
	m_acceleration = { 0.f, 0.f };	// no new acceleration by default

	// check if rabbit is close enough to effect acceleration
	if (to_rabbit.length() <= ALERT_DISTANCE && to_rabbit.length() > 0)
	{
		// head away from them
		m_acceleration = -to_rabbit.normalized() * ACCELERATION;
		m_audio->playSoundbyName("bah");
	}

	Animal::update(dt);
}

void Sheep::collideWithGoal(GameObject& goal)
{
	
	setAlive(false);
}