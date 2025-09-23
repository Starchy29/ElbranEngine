#include "Battle.h"

void Battle::GetCoveredArea(Int2* outTiles, Int2 userTile, const TargetArea* moveArea) const {

}

const TargetOptions* Battle::DetermineTargetOptions(const UnitState* user, const Move* move) {
	foundTargets.groupCount = 0;
	uint16_t usedTiles = 0;

	// clamp range to avoid checking tiles out of bounds
	int32_t cappedRangeLeft = Math::Min(user->tile.x, move->targetArea.range);
	int32_t cappedRangeRight = Math::Min(tiles.GetWidth() - user->tile.x, move->targetArea.range);
	int32_t cappedRangeDown = Math::Min(user->tile.y, move->targetArea.range);
	int32_t cappedRangeUp = Math::Min(tiles.GetHeight() - user->tile.y, move->targetArea.range);

	switch(move->targetArea.shape) {
	case TargetArea::Shape::Self:
		foundTargets.groupCount = 1;
		//foundTargets.groups[0] = { user->tile, 1, 1 };
		break;
	case TargetArea::Shape::Radius:
		for(int32_t dx = -cappedRangeLeft; dx <= cappedRangeRight; dx++) {
			int32_t yRange = move->targetArea.range - Math::Abs(dx);
			for(int32_t dy = -Math::Min(user->tile.y, yRange); dy <= Math::Min(tiles.GetHeight() - user->tile.y, yRange); dy++) {
				Int2 testTile = Int2(user->tile.x + dx, user->tile.y + dy);
				foundTargets.StartGroup();
				foundTargets.PushTile(testTile, HasValidTarget(testTile, user, move));
			}
		}
		break;
	}

	if(foundTargets.groupCount > 0 && foundTargets.groups[foundTargets.groupCount-1].targetCount == 0) foundTargets.groupCount--;

	return &foundTargets;
}

bool Battle::HasValidTarget(Int2 tile, const UnitState* user, const Move* move) const {
	switch(move->effect.type) {
	case MoveEffect::Type::Damage:
		break;
	}

	return false;
}

BattleState Battle::ProcessCommand(BattleState state, Command command, void* outAnimationSequence) {
	const UnitState* user = &(state.playerTurn == 0 ? state.team1[command.unitIndex] : state.team2[command.unitIndex]);
	const Move* move = &user->type->moves[command.moveIndex];
	TargetGroup targets = DetermineTargetOptions(user, move)->groups[command.targetIndex];

	switch(move->effect.type) {
	case MoveEffect::Type::Damage:
		for(uint8_t i = 0; i < targets.targetCount; i++) {
			state.GetOccupants(targets.tiles[i])->unit->health -= move->effect.damage;
		}
		break;
	}

	return state;
}

void Battle::EndTurn() {
	// change turn index
	// reset moves
}

void TargetOptions::PushTile(Int2 tile, bool hasValidTarget) {
	if(hasValidTarget) {
		tiles[usedTiles] = groups[groupCount-1].tiles[groups[groupCount-1].targetCount]; // copy first invalid target to end
		tiles[groups[groupCount-1].targetCount] = tile;
		groups[groupCount-1].targetCount++;
	} else {
		tiles[usedTiles] = tile;
	}
	groups[groupCount-1].areaSize++;
	usedTiles++;
}

void TargetOptions::StartGroup() {
	if(groupCount > 0 && groups[groupCount-1].targetCount == 0) {
		// remove empty groups
		groupCount--;
	}

	groups[groupCount].tiles = &tiles[usedTiles];
	groups[groupCount].targetCount = 0;
	groups[groupCount].areaSize = 0;
	groupCount++;
}
