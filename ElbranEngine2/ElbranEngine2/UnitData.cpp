#include "UnitData.h"

void UnitData::Initialize() {
	testUnit.maxHealth = 3;
	testUnit.moves[0].cooldown = 0;
	testUnit.moves[0].targetArea = TargetArea::Radius(3);
	testUnit.moves[0].effect = MoveEffect::Damage(1);
}
