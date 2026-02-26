#include "Menu.h"

Menu::Menu(sf::RenderWindow& window, Input& input, GameState& state, AudioManager& audio) :
	BaseLevel(window, input, state, audio), m_prompt(m_font)
{
	reset();
	m_prompt.setString("Press space to start");
	m_prompt.setCharacterSize(24);
	m_prompt.setPosition({ 100,150 });
	if (!m_font.openFromFile("font/arial.ttf")) std::cerr << "failed to get font in menu";
}

void Menu::handleInput(float dt)
{
	if (m_input.isPressed(sf::Keyboard::Scancode::Space))
		m_gameState.setCurrentState(State::LEVEL);
}

// Resets the camera to original size and position.
void Menu::reset()
{
	m_window.setView(m_window.getDefaultView());

	m_audio.stopAllMusic();
	m_audio.playMusicbyName("corporate");

}

void Menu::render()
{
	beginDraw();
	m_window.draw(m_prompt);
	endDraw();
}


