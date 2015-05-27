#include "GameEngine.hpp"
#include "Game.hpp"


GameEngine::GameEngine(Game *_g) : Engine(_g), m_levelStarted(false)
{
	m_mario = new Player("mario", SIZE_BLOCK * 5, 150);
}

GameEngine::~GameEngine()
{
	delete m_mario;
}

void GameEngine::Frame()
{
	assert(false); // The overloaded Frame function should be called instead
}

void GameEngine::Frame(float _dt)
{
	if (!m_levelStarted)
		StartLevel(); // In the future there will be some sort of level selection so this call will be moved

	ProcessQueue();

	if (m_mario != NULL)
	{
		UpdateMarioPosition(_dt);
		HandleCollisionsWithMapEdges(*m_mario);
		HandleCollisionsWithLevel(*m_mario);
		CheckMarioDeath();
		SendMarioPosition(_dt);
	}
}

void GameEngine::ProcessEvent(EngineEvent& _event)
{
	switch (_event.m_type)
	{
		case KEY_PRESSED:
			HandlePressedKey(_event.data.m_key);
			break;
		case KEY_RELEASED:
			HandleReleasedKey(_event.data.m_key);
			break;
		case INFO_POS_LVL:
			m_foregroundObjectCoords[_event.data.m_id] = _event.m_rect;
			break;
		case GAME_STOPPED:
			m_parent->Stop();
			break;
		default:
			break;
	}
}

void GameEngine::HandlePressedKey(sf::Keyboard::Key _key)
{
	EngineEvent tmpEvent;
	switch (_key)
	{
		case sf::Keyboard::Left:
			if (m_mario != NULL)
				m_mario->Move(-1);
			break;
		case sf::Keyboard::Right:
			if (m_mario != NULL)
				m_mario->Move(1);
			break;
		case sf::Keyboard::C:
			if (m_mario != NULL)
				m_mario->ToggleRun(true);
			break;
		case sf::Keyboard::Space:
			if (m_mario != NULL && m_mario->Jump())
			{
				tmpEvent.set(PLAY_SOUND, JUMP_SND);
				m_engines["s"]->PushEvent(tmpEvent);
			}
			break;
		case sf::Keyboard::Escape:
			if (m_mario == NULL)
				m_mario = new Player("mario", SIZE_BLOCK * 5, 150);
			break;
		default:
			break;
	}
}

void GameEngine::HandleReleasedKey(sf::Keyboard::Key _key)
{
	switch (_key)
	{
		case sf::Keyboard::Left:
		case sf::Keyboard::Right:
			if (m_mario != NULL)
				m_mario->Move(0);
			break;
		case sf::Keyboard::C:
			if (m_mario != NULL)
				m_mario->ToggleRun(false);
			break;
		case sf::Keyboard::Space:
			if (m_mario != NULL)
				m_mario->EndJump();
			break;
		default:
			break;
	}
}

// This will probably need an argument (like the name of the level) at some point
void GameEngine::StartLevel()
{
	EngineEvent startLevel;
	startLevel.set(LEVEL_START, "");
	m_engines["s"]->PushEvent(startLevel);
	m_levelStarted = true;
}

void GameEngine::UpdateMarioPosition(float _dt)
{
	unsigned int id = m_mario->GetID();

	m_mario->UpdatePosition(_dt);

	sf::Vector2f pos = m_mario->GetPosition();
	m_foregroundObjectCoords[id].left = pos.x;
	m_foregroundObjectCoords[id].top = pos.y;
}

void GameEngine::CheckMarioDeath()
{
	EngineEvent tmpEvent;

	if (m_mario->IsDead())
	{
		tmpEvent.set(PLAY_SOUND, DEATH_SND);
		m_engines["s"]->PushEvent(tmpEvent);
		KillMario();
	}
}

void GameEngine::KillMario()
{
	delete m_mario;
	m_mario = NULL;
}

// Send Mario's position to gfx
void GameEngine::SendMarioPosition(float _dt)
{
	EngineEvent tmpEvent;

	if (m_mario == NULL)
		return;

	tmpEvent.set(INFO_POS_CHAR, m_mario->GetInfoForDisplay());
	m_engines["gfx"]->PushEvent(tmpEvent);
#ifdef DEBUG_MODE
	tmpEvent.set(INFO_DEBUG, m_mario->GetDebugInfo());
	m_engines["gfx"]->PushEvent(tmpEvent);
#endif
}