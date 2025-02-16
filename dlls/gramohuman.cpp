#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"

#pragma once

class Gramohuman : public CBaseMonster
{
private:
    /* data */
    public:
	void Spawn() override;
};

void Gramohuman::Spawn()
{
    ALERT( at_console, "I live!\n" );
    PRECACHE_MODEL("models/monsters/gramohuman.mdl");
	SET_MODEL(ENT(pev), "models/monsters/gramohuman.mdl");

	pev->health = 10;
}

LINK_ENTITY_TO_CLASS(monster_gramohuman, Gramohuman );