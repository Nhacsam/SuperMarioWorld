#ifndef DISPLAYABLEOBJECT_H
#define DISPLAYABLEOBJECT_H

#include <SFML/System/Vector2.hpp>
#include "Debug.hpp"
#include "PhysicsConstants.hpp"

typedef enum {
	STATIC,
	WALK,
	RUN,
	JUMP,		// Only used for transmission to GameEngine
	FALL,		// Idem
	NORMAL,		// Items with only one state e.g. the animated foreground items
	EMPTY,		// ? Boxes
	UNKNOWN
} State;

/*
*	Information sent from g to gfx for display. The name and state will be used to fetch the correct sprite.
*/
struct InfoForDisplay
{
	unsigned int id;
	std::string name;	// to find sprite
	State state;		// Idem
	sf::Vector2f coordinates;
	bool reverse; // Reverse sprite display (left/right) ?
};

/*
*	A DisplayableObject can be the player itself, an enemy, or stuff like pipes, '?' blocks, the floor etc. Anything you can display.
*/
class DisplayableObject
{
	public:
		DisplayableObject(std::string _name, sf::Vector2f _coord, State _state = UNKNOWN);
		DisplayableObject(std::string _name, float _x, float _y, State _state = UNKNOWN);
		virtual ~DisplayableObject();

		virtual InfoForDisplay GetInfoForDisplay();
		sf::Vector2f GetPosition() const { return m_coord; };
		void SetPosition(const sf::Vector2f _pos) { m_coord = _pos; };
		State GetState() const { return m_state; }
		unsigned int GetID() const { return m_id; };
		void SetX(const float _x) { m_coord.x = _x; };
		void SetY(const float _y) { m_coord.y = _y; };

	protected:
		int m_id; // Unique identifier for each object
		std::string m_name;
		State m_state;

		sf::Vector2f m_coord; // Coordinates of the top left corner, in pixels, with respect to the top left corner of the window

		bool m_reverseSprite;

	private:
		static unsigned int id;
};

#endif
