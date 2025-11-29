#include "RenderGroup.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "ShaderConstants.h"

// these match Lighting.hlsli
#define LIGHTS_REGISTER 127
#define LIGHT_INFO_REGISTER 1

void RenderGroup::Initialize(uint32_t numTransforms, uint32_t numRenderers) {
	ASSERT(numRenderers >= numTransforms);
	ambientLight = Color::White;
	backgroundColor = Color::White;
	backgroundImage = nullptr;
	camera = {};

	transformCount = 0;
	transformCapacity = numTransforms;
	rendererCount = 0;
	rendererCapacity = numRenderers;

	uint8_t* dataBlock = new uint8_t[numTransforms * (sizeof(Transform) + sizeof(Matrix)) + numRenderers * sizeof(Renderer)] {};
	transforms = (Transform*)dataBlock;
	dataBlock += numTransforms * sizeof(Transform);
	worldMatrices = (Matrix*)dataBlock;
	dataBlock += numTransforms * sizeof(Matrix);
	renderers = (Renderer*)dataBlock;

	camera.viewWidth = 1.f;
	camera.transform = ReserveTransform(&camera.worldMatrix);
}

void RenderGroup::Release(GraphicsAPI* graphics) {
	for(uint32_t i = 0; i < rendererCount; i++) {
		renderers[i].Release(graphics);
	}
	delete[] transforms; // all three arrays were allocated in one allocation
}

struct OrderedRenderer {
	Renderer* renderer;
	float globalZ;
};

void RenderGroup::Draw() const {
	GraphicsAPI* graphics = &app.graphics;

	// convert all transforms into local matrices
	Matrix* localMatrices = (Matrix*)app.frameBuffer.Reserve(sizeof(Matrix) * transformCount);
	for(uint32_t i = 0; i < transformCount; i++) {
		localMatrices[i] =
			Matrix::Translation(transforms[i].position.x, transforms[i].position.y, transforms[i].zOrder) *
			Matrix::Rotation(transforms[i].rotation) *
			Matrix::Scale(transforms[i].scale.x, transforms[i].scale.y);

		worldMatrices[i] = localMatrices[i];
	}

	// convert all local matrices into global matrices
	for(uint32_t i = 0; i < transformCount; i++) {
		Transform* parent = transforms[i].parent;
		while(parent) {
			worldMatrices[i] = localMatrices[parent - transforms] * worldMatrices[i];
			parent = parent->parent;
		}
	}

	// create projection matrix
	Vector2 cameraPosition = *camera.worldMatrix * Vector2::Zero;
	Matrix viewProjection =
		Matrix::ProjectOrthographic(camera.viewWidth, camera.GetViewHeight(), CAMERA_DEPTH) *
		Matrix::View(cameraPosition, camera.transform->rotation);

	// sort the renderers into the correct drawing order and compile the light data together
	LightData lightData[MAX_LIGHTS_ONSCREEN];
	LightConstants lightConstants;
	lightConstants.ambientLight = ambientLight;
	lightConstants.lightCount = 0;
	const AlignedRect screenArea = AlignedRect(-1.f, 1.f, 1.f, -1.f);

	uint32_t numOpaque = 0;
	uint32_t numTranslucent = 0;
	OrderedRenderer* opaques = (OrderedRenderer*)app.frameBuffer.Reserve(sizeof(OrderedRenderer) * rendererCount);
	OrderedRenderer* translucents = (OrderedRenderer*)app.frameBuffer.Reserve(sizeof(OrderedRenderer) * rendererCount);

	for(uint32_t i = 0; i < rendererCount; i++) {
		if(renderers[i].hidden) continue;
		switch(renderers[i].type) {
		case Renderer::Type::None: break;
		case Renderer::Type::Light: {
			if(lightConstants.lightCount >= MAX_LIGHTS_ONSCREEN) continue;
			
			// omit offscreen lights
			Vector2 closestPoint = renderers[i].transform->position + (cameraPosition - renderers[i].transform->position).SetLength(renderers[i].lightData.radius);
			closestPoint = viewProjection * closestPoint; // project into normalized screen coordinates, where values between -1 and 1 are on screen
			if(!(Circle(renderers[i].transform->position, renderers[i].lightData.radius).Contains(cameraPosition) || screenArea.Contains(closestPoint))) continue;

			// copy light data to gpu
			lightData[lightConstants.lightCount].color = renderers[i].lightData.color;
			lightData[lightConstants.lightCount].radius = renderers[i].lightData.radius;
			lightData[lightConstants.lightCount].brightness = renderers[i].lightData.brightness;

			lightData[lightConstants.lightCount].worldPosition = *(renderers[i].worldMatrix) * Vector2::Zero;
			lightData[lightConstants.lightCount].direction = (*renderers[i].worldMatrix * Vector2::Right - lightData[lightConstants.lightCount].worldPosition).Normalize();
			lightData[lightConstants.lightCount].coneEdge = *(renderers[i].worldMatrix) * Vector2::FromAngle(renderers[i].lightData.coneSize * 0.5f);

			lightConstants.lightCount++;
		} break;
		default: {
			OrderedRenderer addition = { &renderers[i], renderers[i].worldMatrix->values[2][3]};
			if(renderers[i].translucent) {
				// sort translucents back to front
				uint32_t insertIndex = numTranslucent;
				while(insertIndex > 0 && addition.globalZ > translucents[insertIndex - 1].globalZ) {
					translucents[insertIndex] = translucents[insertIndex - 1];
					insertIndex--;
				}
				translucents[insertIndex] = addition;
				numTranslucent++;
			} else {
				// sort opaques front to back
				uint32_t insertIndex = numOpaque;
				while(insertIndex > 0 && addition.globalZ < opaques[insertIndex - 1].globalZ) {
					opaques[insertIndex] = opaques[insertIndex - 1];
					insertIndex--;
				}
				opaques[insertIndex] = addition;
				numOpaque++;
			}
		} break;
		}
	}

	// send the light data to the gpu for pixel shaders
	graphics->WriteBuffer(&lightData, sizeof(LightData) * MAX_LIGHTS_ONSCREEN, graphics->lightsBuffer.buffer);
	graphics->SetArray(ShaderStage::Pixel, &graphics->lightsBuffer, LIGHTS_REGISTER);

	graphics->WriteBuffer(&lightConstants, sizeof(LightConstants), graphics->lightInfoBuffer.data);
	graphics->SetConstants(ShaderStage::Pixel, &graphics->lightInfoBuffer, LIGHT_INFO_REGISTER);

	// send the projection matrix to the gpu for vertex shaders
	viewProjection = viewProjection.Transpose();
	graphics->WriteBuffer(&viewProjection, sizeof(Matrix), graphics->projectionBuffer.data);
	graphics->SetConstants(ShaderStage::Vertex, &graphics->projectionBuffer, 1);
	graphics->SetConstants(ShaderStage::Geometry, &graphics->projectionBuffer, 1);

	// draw opaques front to back
	for(uint32_t i = 0; i < numOpaque; i++) {
		opaques[i].renderer->Draw(graphics, &app.assets);
	}

	// draw the background
	if(backgroundImage) {
		TexturePSConstants psInput;
		psInput.tint = backgroundColor;
		psInput.lit = false;

		graphics->SetTexture(ShaderStage::Pixel, backgroundImage, 0);
		graphics->SetPixelShader(&app.assets.texturePS, &psInput, sizeof(TexturePSConstants));
	} else {
		graphics->SetPixelShader(&app.assets.solidColorPS, &backgroundColor, sizeof(Color));
	}
	graphics->DrawFullscreen();

	// draw translucents back to front
	graphics->SetBlendMode(BlendState::AlphaBlend);
	for(int i = 0; i < numTranslucent; i++) {
		translucents[i].renderer->Draw(graphics, &app.assets);
	}
	graphics->SetBlendMode(BlendState::None);
}

Transform* RenderGroup::ReserveTransform(const Matrix** outMatrix) {
	ASSERT(transformCount < transformCapacity);

	transforms[transformCount] = {};
	transforms[transformCount].scale = Vector2(1.f, 1.f);
	worldMatrices[transformCount] = Matrix::Identity;

	transformCount++;
	if(outMatrix) *outMatrix = &worldMatrices[transformCount - 1];
	return &transforms[transformCount - 1];
}

Renderer* RenderGroup::ReserveRenderer() {
	ASSERT(rendererCount < rendererCapacity);
	renderers[rendererCount] = {};
	Renderer* reserved = &renderers[rendererCount];
	reserved->transform = ReserveTransform(&reserved->worldMatrix);
	rendererCount++;
	return reserved;
}
