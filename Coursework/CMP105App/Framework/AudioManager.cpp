#include "AudioManager.h"

AudioManager::AudioManager()
{
	addMusic("nature", "sfx/nature.mp3");
	addMusic("corporate", "sfx/corporate.mp3");


	addSound("yay", "sfx/yay.mp3");
	addSound("bah", "sfx/bah.mp3");
}

AudioManager::~AudioManager()
{
}

void AudioManager::addSound(const std::string& tag, const std::string& filename)
{
	if (!m_sounds[tag].loadSound(filename))
	{
		std::cerr << "AudioManager failed to load sound: " << filename << " with tag: " << tag << "\n";
	}
}

void AudioManager::playSoundbyName(const std::string& tag)
{
	// Find the sound in the map.
	auto it = m_sounds.find(tag);
	if (it != m_sounds.end())
	{
		// If found (iterator is not at the end), play the sound.
		// it->first is the tag, it->second is the SoundObject.
		it->second.getSound().play();
	}
}

void AudioManager::stopAllSounds()
{
	// Iterate through all the sounds in the map and stop them.
	for (auto& pair : m_sounds)
	{
		pair.second.getSound().stop();
	}
}

sf::Sound* AudioManager::getSound(const std::string& tag)
{
	auto it = m_sounds.find(tag);
	if (it != m_sounds.end())
	{
		// Return a pointer to the sf::Sound instance.
		return &it->second.getSound();
	}

	// Return nullptr if the tag doesn't exist.
	return nullptr;
}

void AudioManager::addMusic(const std::string& tag, const std::string& filename)
{
	// Create a new sf::Music object on the heap, managed by a unique_ptr.
	auto music = std::make_unique<sf::Music>();

	// Try to open the music file.
	if (!music->openFromFile(filename))
	{
		std::cerr << "Error: Could not load music file: " << filename << std::endl;
		return; // Don't add it to the map if it fails to load.
	}

	// Move the unique_ptr into the map. This transfers ownership to the map.
	m_musicStreams[tag] = std::move(music);
}

void AudioManager::playMusicbyName(const std::string& tag)
{
	auto it = m_musicStreams.find(tag);
	if (it != m_musicStreams.end())
	{
		// it->second is the std::unique_ptr. ->get() gives the raw pointer.
		it->second->play();
	}
}

void AudioManager::stopAllMusic()
{
	// Iterate through the music map and stop each stream.
	for (auto& pair : m_musicStreams)
	{
		pair.second->stop();
	}
}

sf::Music* AudioManager::getMusic(const std::string& tag)
{
	auto it = m_musicStreams.find(tag);
	if (it != m_musicStreams.end())
	{
		// Return the raw pointer from the unique_ptr.
		return it->second.get();
	}

	return nullptr;
}
