#include <cstring>
#include "LevelImporter.hpp"
#include "../Engines/GameEngine.hpp"

using namespace irr;
using namespace io;

const std::string LevelImporter::levelsPath = "../assets/levels/";

LevelImporter::LevelImporter(GameEngine *_parent)
{
	m_gameEngine = _parent;
}

bool LevelImporter::LoadLevel(std::string _lvlName)
{
	bool fileNotEmpty = false;
	std::string lvlFullName = LevelImporter::levelsPath + _lvlName + ".xml";
	m_lvlFile = createIrrXMLReader(lvlFullName.c_str());

	while (m_lvlFile && m_lvlFile->read())
	{
		fileNotEmpty = true;
		switch (m_lvlFile->getNodeType())
		{
			case EXN_ELEMENT:
				if (!strcmp("level", m_lvlFile->getNodeName()))
				{
					LevelInfo info;
					info.backgroundName = GetAttributeValue("background");
					info.size.x = GetAttributeValueAsFloat("width");
					info.size.y = GetAttributeValueAsFloat("height");

					// And that's exactly why you need listeners
					EngineEvent tmpEvent(INFO_LVL, info);
					m_gameEngine->PushEvent(tmpEvent);
					m_gameEngine->TransmitInfoToGFX(tmpEvent);
				}
				if (!strcmp("characters", m_lvlFile->getNodeName()))
					StoreCharactersInitialPositions();
				if (!strcmp("foreground", m_lvlFile->getNodeName()))
					StoreListForegroundTileNames();
				break;
			default:
				break;
		}
	}

	if (!fileNotEmpty)
	{
		std::cerr << "Can't read level file " << lvlFullName << std::endl;
		return false;
	}

	delete m_lvlFile;
	m_lvlFile = NULL;
	return true;
}

void LevelImporter::StoreCharactersInitialPositions()
{
	bool foundOneCharacter = false;
	const char* nodeName;

	while (m_lvlFile && m_lvlFile->read())
	{
		switch (m_lvlFile->getNodeType())
		{
			case EXN_ELEMENT:
				foundOneCharacter = true;
				nodeName = m_lvlFile->getNodeName();
				if (!strcmp("mario", nodeName))
				{
					sf::Vector2f initPosMario;
					initPosMario.x = GetAttributeValueAsFloat("x");
					initPosMario.y = GetAttributeValueAsFloat("y");
					m_gameEngine->SetMarioInitialPosition(initPosMario);

					Player *mario = new Player("mario", initPosMario);
					EngineEvent newMario(NEW_CHARACTER, mario);
					m_gameEngine->PushEvent(newMario);
				}
				if (!strcmp("goomba", nodeName))
				{
					Direction tmpDir = GetAttributeValue("direction", true) == "left" ? DLEFT : DRIGHT; // direction = right if attribute not here
					Goomba *goomba = new Goomba("goomba", GetAttributeValueAsFloat("x"), GetAttributeValueAsFloat("y"), tmpDir);
					EngineEvent newGoomba(NEW_CHARACTER, goomba);
					m_gameEngine->PushEvent(newGoomba);
				}
				break;
			case EXN_ELEMENT_END:
				if (!foundOneCharacter)
					std::cerr << "No character in level file." << std::endl;
				return;
			default:
				break;
		}
	}
}

void LevelImporter::StoreListForegroundTileNames()
{
	bool foundTiles = false;
	const char* nodeName;

	while (m_lvlFile && m_lvlFile->read())
	{
		switch (m_lvlFile->getNodeType())
		{
			case EXN_ELEMENT:
				foundTiles = true;
				nodeName = m_lvlFile->getNodeName();
				if (!strcmp("box", nodeName))
					StoreBox();
				if (!strcmp("pipe", nodeName))
					StorePipe();
				if (!strcmp("floor_tile", nodeName))
					StoreFloor();
				break;
			case EXN_ELEMENT_END:
				if (!foundTiles)
					std::cerr << "No foreground items in level file." << std::endl;
				return;
			default:
				break;
		}
	}
}

void LevelImporter::StoreBox()
{
	sf::Vector2f tmpCoords;
	std::string tmpTileName;
	GetCoordinatesAndTileName(&tmpCoords, &tmpTileName);

	State tmpState = GetAttributeValue("state", true) == "empty" ? EMPTY : NORMAL;

	Box *tmpBox = new Box("item_" + tmpTileName, tmpCoords, tmpState);
	EngineEvent newBox(NEW_FOREGROUND_ITEM, tmpBox);
	m_gameEngine->PushEvent(newBox);

	SendInfoPosLvlToGFX(tmpBox->GetInfoForDisplay());
}

void LevelImporter::StorePipe()
{
	sf::Vector2f tmpCoords;
	std::string tmpTileName;
	GetCoordinatesAndTileName(&tmpCoords, &tmpTileName);

	PipeType type = GetPipeTypeFromXML();
	int id = GetAttributeValueAsInt("id");

	if (std::find(m_pipeIds.begin(), m_pipeIds.end(), id) == m_pipeIds.end())
	{
		Pipe *tmpPipe = new Pipe("item_" + tmpTileName, tmpCoords, id, type, m_gameEngine);
		EngineEvent newPipe(NEW_PIPE, tmpPipe);
		m_gameEngine->PushEvent(newPipe);

		SendInfoPosLvlToGFX(tmpPipe->GetInfoForDisplay());

		m_pipeIds.push_back(id);
	}
	else
		std::cerr << "Another pipe with id " << id << " already exists. New pipe not created." << std::endl;
}

PipeType LevelImporter::GetPipeTypeFromXML()
{
	std::string type_str = GetAttributeValue("type", false);
	if (type_str == "travel")
		return TRAVEL;
	if (type_str == "spawn")
		return SPAWN;
	if (type_str == "flower")
		return FLOWER;
	assert(false); // TODO Should throw some sort of NotImplementedException ?
}

void LevelImporter::StoreFloor()
{
	sf::Vector2f tmpCoords;
	std::string tmpTileName;
	GetCoordinatesAndTileName(&tmpCoords, &tmpTileName);

	DisplayableObject *tmpFloor = new DisplayableObject("floor_" + tmpTileName, tmpCoords, NORMAL);
	EngineEvent newFloor(NEW_FOREGROUND_ITEM, tmpFloor);
	m_gameEngine->PushEvent(newFloor);

	SendInfoPosLvlToGFX(tmpFloor->GetInfoForDisplay());
}

std::string LevelImporter::GetAttributeValue(const char* _name, bool _optionalAttribute)
{
	std::string str(m_lvlFile->getAttributeValueSafe(_name));
	if (str == "" && !_optionalAttribute)
		std::cerr << "Can't read attribute " << _name << std::endl;
	return str;
}

float LevelImporter::GetAttributeValueAsFloat(const char* _name)
{
	float ret = m_lvlFile->getAttributeValueAsFloat(_name);
	if (ret == -1)
		std::cerr << "Can't read attribute " << _name << std::endl;
	return ret;
}

int LevelImporter::GetAttributeValueAsInt(const char* _name)
{
	int ret = m_lvlFile->getAttributeValueAsInt(_name);
	if (ret == -1)
		std::cerr << "Can't read attribute " << _name << std::endl;
	return ret;
}

void LevelImporter::SendInfoPosLvlToGFX(InfoForDisplay _info)
{
	EngineEvent tmpEvent(INFO_POS_LVL, _info);
	m_gameEngine->TransmitInfoToGFX(tmpEvent);
}

void LevelImporter::GetCoordinatesAndTileName(sf::Vector2f *_coords, std::string *_tileName)
{
	_coords->x = GetAttributeValueAsFloat("x");
	_coords->y = GetAttributeValueAsFloat("y");

	*_tileName = GetAttributeValue("sprite");
}
