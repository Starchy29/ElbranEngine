#pragma once
#include "Move.h"
#define MAX_MOVES 6

struct UnitType {
	Move moves[MAX_MOVES];
	uint16_t maxHealth;
};

struct UnitState {
	const UnitType* type;
	Int2 tile;
	uint16_t health;
	uint8_t cooldowns[MAX_MOVES];
	bool canMove;
	bool canAbility;
	// status
	// shield
};