#pragma once
#include "Unit.h"
#include "Tilemap.h"

#define MAX_UNITS_PER_TEAM 10
#define MAX_BOARD_WIDTH 20

// 0 to (targetCount-1) are tiles with valid targets, targetCount to areaSize are tiles in the target group with no targets
struct TargetGroup {
	Int2* tiles;
	uint8_t targetCount;
	uint8_t areaSize;
};

struct TargetOptions {
	Int2 tiles[200];
	TargetGroup groups[200];
	uint16_t groupCount;
	uint16_t usedTiles;

	void PushTile(Int2 tile, bool hasValidTarget);
	void StartGroup();
};

struct TileOccupants {
	UnitState* unit;
};

struct BattleState {
	UnitState team1[MAX_UNITS_PER_TEAM];
	UnitState team2[MAX_UNITS_PER_TEAM];
	uint8_t numAllies;
	uint8_t numEnemies;

	uint8_t playerTurn;

	// zones
	TileOccupants board[MAX_BOARD_WIDTH * MAX_BOARD_WIDTH];

	inline TileOccupants* GetOccupants(Int2 tile)  { return &board[tile.x + MAX_BOARD_WIDTH * tile.y]; }
};

struct Command {
	uint8_t unitIndex;
	uint8_t moveIndex;
	uint8_t targetIndex;
};

class Battle {
public:
	Tilemap tiles;
	UnitType team1[MAX_UNITS_PER_TEAM];
	UnitType team2[MAX_UNITS_PER_TEAM];

	BattleState state;

	void GetCoveredArea(Int2* outTiles, Int2 userTile, const TargetArea* moveArea) const;
	const TargetOptions* DetermineTargetOptions(const UnitState* user, const Move* move);
	bool HasValidTarget(Int2 tile, const UnitState* user, const Move* move) const;

	BattleState ProcessCommand(BattleState state, Command command, void* outAnimationSequence);
	void EndTurn();

private:
	TargetOptions foundTargets; // one data batch for target groups, only one target group can be evaluated at a time
};

