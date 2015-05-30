#include "DisplayableObject.hpp"

unsigned int DisplayableObject::id = 1;

DisplayableObject::DisplayableObject(std::string _name, sf::Vector2f _coord, State _state) : DisplayableObject(_name, _coord.x, _coord.y, _state)
{

}

DisplayableObject::DisplayableObject(std::string _name, float _x, float _y, State _state)
{
	m_id = DisplayableObject::id;
	DisplayableObject::id++;

	m_name = _name;
	m_coord.x = _x;
	m_coord.y = _y;

	m_state = _state;
}

DisplayableObject::~DisplayableObject()
{

}

InfoForDisplay DisplayableObject::GetInfoForDisplay()
{
	InfoForDisplay info;
	info.id = m_id;
	info.name = m_name;
	info.state = m_state;
	info.coordinates = m_coord;
	info.reverse = m_reverseSprite;
	return info;
}