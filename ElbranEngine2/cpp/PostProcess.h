#pragma once
#include "Common.h"

class GraphicsAPI;
struct RenderTarget;

struct PostProcess {
	enum class Type {
		None,
		Blur,
		HSV,
		Bloom
	} type;

	union {
		struct {
			uint16_t blurRadius;
		} blurData;

		struct {
			// all range [0,1]
			float contrast;
			float saturation;
			float brightness;
		} hsvData;

		struct {
			uint16_t blurRadius;
			float threshold;
		} bloomData;
	};

	void Render(GraphicsAPI* graphics, const RenderTarget* input, RenderTarget* output);
	bool IsActive() const; //?
};