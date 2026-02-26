#include "Animal.h"
#include <cmath>

Animal::Animal()
{
    // Set default physics values
    m_dragFactor = 0.9f;    // Example default
    m_restitution = 0.8f;    // Example default
    m_velocity = { 0, 0 };
    m_acceleration = { 0, 0 };

    
}

Animal::~Animal() {}

void Animal::update(float dt)
{
    // Lab 3 work used here, just apply the physics
    m_velocity += m_acceleration * dt;
    m_velocity *= m_dragFactor;
    
    // handle animation (lab 2 work + new setDirection method)
    setDirection();
    m_currentAnimation->animate(dt); 
    setTextureRect(m_currentAnimation->getCurrentFrame());

    checkWallAndBounce();
    move(m_velocity);
}

void Animal::setWorldSize(float x, float y)
{
    m_worldSize = { x, y };
}

// default collision response is to bounce. Likely only used for Sheep-Sheep collision and magnetism should prevent Sheep-Rabbit
void Animal::collisionResponse(GameObject& collider)
{
    m_velocity *= -m_restitution;

    if (m_velocity.lengthSquared() < 150)
        if (m_velocity.length() > 0)
           move(m_velocity.normalized() * 25.f);    
    else
        if (m_velocity.length() > 0)
            move(m_velocity.normalized() * 5.f);
    setDirection();       // handle animation check AGAIN (update usually comes before collisions in level::update)
}

// don't be hitting walls
void Animal::checkWallAndBounce()
{
    sf::Vector2f pos = getPosition();
    sf::Vector2f size = getSize();

    if ((pos.x < 0 && m_velocity.x < 0) || (pos.x + size.x > m_worldSize.x && m_velocity.x > 0))
        m_velocity.x *= -m_restitution;

    if ((pos.y < 0 && m_velocity.y < 0) || (pos.y + size.y > m_worldSize.y && m_velocity.y > 0))
        m_velocity.y *= -m_restitution;
}

// sets animation pointer based on direction
void Animal::setDirection()
{
    if (m_velocity.lengthSquared() < 1.0f) return;

    float angle = std::atan2(m_velocity.y, m_velocity.x) * (180.0f / 3.14159f);

    if (angle > -22.5f && angle <= 22.5f) {
        m_currentAnimation = &m_walkRight;
        m_currentAnimation->setFlipped(false);
    }
    else if (angle > 22.5f && angle <= 67.5f) {
        m_currentAnimation = &m_walkDownRight;
        m_currentAnimation->setFlipped(false);
    }
    else if (angle > 67.5f && angle <= 112.5f) {
        m_currentAnimation = &m_walkDown;
        m_currentAnimation->setFlipped(false);
    }
    else if (angle > 112.5f && angle <= 157.5f) {
        m_currentAnimation = &m_walkDownRight;
        m_currentAnimation->setFlipped(true);
    }
    else if (angle > 157.5f || angle <= -157.5f) {
        m_currentAnimation = &m_walkRight;
        m_currentAnimation->setFlipped(true);
    }
    else if (angle > -157.5f && angle <= -112.5f) {
        m_currentAnimation = &m_walkUpRight;
        m_currentAnimation->setFlipped(true);
    }
    else if (angle > -112.5f && angle <= -67.5f) {
        m_currentAnimation = &m_walkUp;
        m_currentAnimation->setFlipped(false);
    }
    else if (angle > -67.5f && angle <= -22.5f) {
        m_currentAnimation = &m_walkUpRight;
        m_currentAnimation->setFlipped(false);
    }
}

void Animal::setAudioPointer(AudioManager* audio)
{
    m_audio = audio;
}