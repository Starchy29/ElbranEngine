#pragma once
#include "Common.h"

class GraphicsAPI;
class AssetContainer;
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
			// all range [-1,1]
			float contrast;
			float saturation;
			float brightness;
		} hsvData;

		struct {
			uint16_t blurRadius;
			float brightnessThreshold;
		} bloomData;
	};

	void Render(const RenderTarget* input, RenderTarget* output, GraphicsAPI*, const AssetContainer*) const;
	bool IsActive() const;

	void Blur(uint16_t blurRadius);
	void HSV(float contrast, float saturation, float brightness);
	void Bloom(uint16_t blurRadius, float brightnessThreshold);
};