#pragma once
#include "PlayerControls.h"
#include "Common.h"

#ifdef WINDOWS
class WindowsInput;
typedef WindowsInput PlatformInput;
#endif

struct Camera;

class InputManager {
public:
	PlayerControls* controllers;
	uint32_t playerCount;

	struct {
		float wheelDelta;
		Vector2 screenPos;
		bool leftClicked;
		bool rightClicked;
		bool middleClicked;
		struct {
			Vector2 screenPos;
			bool leftClicked;
			bool rightClicked;
			bool middleClicked;
		} lastFrame;
	} mouse;

	void Initialize(PlatformInput* platformInput);
	void Release();

	void Update(float deltaTime, UInt2 viewDimensions, UInt2 viewOffset);

	void SetRumble(uint32_t playerIndex, float strength0to1);
	bool WasMouseClicked(bool left) const;
	bool WasMouseReleased(bool left) const;
	Vector2 GetMouseWorldPosition(const Camera* camera) const;
	Vector2 GetMouseWorldDisplacement(const Camera* camera) const;

private:
	PlatformInput* platformInput;
};

