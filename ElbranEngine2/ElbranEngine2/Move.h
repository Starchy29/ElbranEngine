#pragma once
#include "Common.h"

struct TargetArea {
	enum class Shape {
		Self,
		Melee,
		Radius,
		Line,
		SquareZone,
		DiamondZone
	} shape;
	uint8_t range;
	uint8_t zoneWidth;

	static TargetArea Self() { return { Shape::Self, 0 }; }
	static TargetArea Melee() { return { Shape::Melee, 1 }; }
	static TargetArea Radius(uint8_t range) { return { Shape::Radius, range }; }
};

struct MoveEffect {
	enum class Type {
		Movement,
		Damage,
		Heal,
		Shield,
		Zone
	} type;

	union {
		int16_t damage;
		int16_t healing;

		struct {
			uint8_t strength;
			uint8_t duration;
			bool fragile;
		} shieldType;
	};

	static MoveEffect Damage(int16_t amount) { return { Type::Damage, amount}; }
	static MoveEffect Heal(int16_t amount) { return { Type::Heal, amount}; }
};

struct Move {
	TargetArea targetArea;
	MoveEffect effect;
	uint8_t cooldown;
};