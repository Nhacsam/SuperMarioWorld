#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "Engine.hpp"
#include "Player.hpp"

/*
    Game engine: Handles the movements of the player, collisions, etc.
*/
class GameEngine : public Engine
{
    public:
        GameEngine(Game *_g);
        ~GameEngine();

		void Frame();
		void Frame(float _dt);

    private:
		bool m_levelStarted;
		Player *m_mario;

		std::map<unsigned int, sf::Rect<float>> m_foregroundObjectCoords; // Part of the level the characters can be in collision with

		void ProcessEvent(EngineEvent& _event);
		void HandlePressedKey(sf::Keyboard::Key _key);
		void HandleReleasedKey(sf::Keyboard::Key _key);

		void CheckMarioDeath();
		void KillMario();
		void SendMarioPosition(float _dt);

		void UpdateMarioPosition(float _dt);
		void HandleCollisionsWithMapEdges(MovingObject& _obj);
		void HandleCollisionsWithLevel(MovingObject& _obj);
		bool GameEngine::HandleCollisionWithRect(unsigned int _objId, sf::Rect<float>_ref);

		void StartLevel();
};

#endif // GAMEENGINE_H
