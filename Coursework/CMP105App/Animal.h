#pragma once
#include "Framework/GameObject.h" 
#include "Framework/Animation.h" 
#include "Framework/AudioManager.h"

class Animal : public GameObject
{
public:
    Animal();
    virtual ~Animal();

    virtual void update(float dt) override;
    void setWorldSize(float x, float y);
    void collisionResponse(GameObject& collider) override;
    void setAudioPointer(AudioManager* audio);

protected:  // RECAP: protected means "private but too all except my kids", so Sheep/Rabbit can change them.
    void setDirection();
    void checkWallAndBounce();

    // Shared Properties
    sf::Vector2f m_worldSize;
    sf::Vector2f m_acceleration; // Controlled by Input or AI

    // Tuning variables 
    float m_dragFactor;
    float m_restitution;

    // Animations 
    Animation m_walkDown;
    Animation m_walkUp;
    Animation m_walkRight;
    Animation m_walkUpRight;
    Animation m_walkDownRight;

    Animation* m_currentAnimation;

    AudioManager* m_audio;
};