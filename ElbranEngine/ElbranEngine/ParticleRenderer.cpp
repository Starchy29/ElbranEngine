#include "ParticleRenderer.h"
#include "Application.h"
#include "AssetManager.h"
#include "DXCore.h"
#include "Random.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Camera.h"
#include "Mesh.h"
#include "ComputeShader.h"
#include "GeometryShader.h"
#include "SpriteAtlas.h"
#include "GameObject.h"
using namespace DirectX;

#define PARTICLE_BYTES 36 // based on struct in ShaderStructs.hlsli
#define CONTINUOUS_SPAWNS -1

ParticleRenderer::ParticleRenderer(unsigned int maxParticles, float lifespan, bool blendAdditive) {
	const AssetManager* assets = APP->Assets();
	vertexShader = assets->particleVS;
	geometryShader = assets->particleGS;
	pixelShader = assets->imagePS;
	spawnShader = assets->particleSpawnCS;
	moveShader = assets->particleMoveCS;

	this->lifespan = lifespan;
	this->maxParticles = maxParticles;
	lastIndex = maxParticles - 1; // causes the first spawn to go in index 0
	this->blendAdditive = blendAdditive;

	applyLights = false;
	tint = Color::White;
	width = 1.0f;
	totalGrowth = 0.0f;
	spinRate = 0.f;
	secondsPerFrame = 0.f;
	fadeDuration = 0.f;
	speed = 0.f;
	moveStyle = ParticleMovement::None;
	faceMoveDirection = false;
	spawnCircular = true;
	moveDirection = Vector2::Zero;
	scaleWithParent = false;

	spawnsLeft = 0;
	spawnInterval = 0.0f;
	timer = 0.f;
	spawnsPerInterval = 1;
	
	directX = APP->Graphics();
	dxDevice = directX->GetDevice();
	dxContext = directX->GetContext();

	// create a read/write buffer for the particles
	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth = PARTICLE_BYTES * maxParticles;
	bufferDescription.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDescription.StructureByteStride = PARTICLE_BYTES;

	dxDevice->CreateBuffer(&bufferDescription, 0, particleBuffer.GetAddressOf());

	// create the read/write view for the  buffer
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDescription = {};
	uavDescription.Format = DXGI_FORMAT_UNKNOWN;
	uavDescription.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDescription.Buffer.FirstElement = 0;
	uavDescription.Buffer.NumElements = maxParticles;
	uavDescription.Buffer.Flags = 0;

	dxDevice->CreateUnorderedAccessView(particleBuffer.Get(), &uavDescription, readWriteView.GetAddressOf());

	// create the read only view for the buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDescription = {};
	srvDescription.Format = DXGI_FORMAT_UNKNOWN;
	srvDescription.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDescription.Buffer.FirstElement = 0;
	srvDescription.Buffer.NumElements = maxParticles;

	dxDevice->CreateShaderResourceView(particleBuffer.Get(), &srvDescription, readOnlyView.GetAddressOf());

	// create the buffers for setting initial positions and velocities
	initalPosBuffer = Shader::CreateArrayBuffer(maxParticles, sizeof(Vector2), ShaderDataType::Float2);
	initialVelBuffer = Shader::CreateArrayBuffer(maxParticles, sizeof(Vector2), ShaderDataType::Float2);
}

ParticleRenderer::ParticleRenderer(unsigned int maxParticles, float lifespan, bool blendAdditive, std::shared_ptr<Sprite> sprite)
	: ParticleRenderer(maxParticles, lifespan, blendAdditive)
{
	this->sprite = sprite;
	animated = false;
}

ParticleRenderer::ParticleRenderer(unsigned int maxParticles, float lifespan, bool blendAdditive, std::shared_ptr<SpriteAtlas> animation)
	: ParticleRenderer(maxParticles, lifespan, blendAdditive)
{
	sprite = std::static_pointer_cast<Sprite>(animation);
	animated = true;
}

void ParticleRenderer::Update(float deltaTime) {
	// update particles
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	dxContext->VSSetShaderResources(0, 1, nullSRV); // unbind from vertex shader
	moveShader->SetReadWriteBuffer(readWriteView, 0);

	float growthRate = totalGrowth / lifespan;
	if(scaleWithParent) {
		Vector2 size = owner->GetTransform()->GetScale(true);
		growthRate *= (size.x + size.y) / 2.f;
	}
	moveShader->SetConstantVariable("maxParticles", &maxParticles);
	moveShader->SetConstantVariable("deltaTime", &deltaTime);
	moveShader->SetConstantVariable("growthRate", &growthRate);
	moveShader->SetConstantVariable("spinRate", &spinRate);
	moveShader->SetConstantVariable("fadeDuration", &fadeDuration);
	moveShader->Dispatch(maxParticles, 1);
	
	// update spawning
	if(spawnsLeft == 0) {
		return;
	}

	timer -= deltaTime;
	if(timer < 0.f) {
		timer += spawnInterval;
		if(spawnsLeft != CONTINUOUS_SPAWNS) {
			spawnsLeft--;
		}
		Emit(spawnsPerInterval);
	}
}

void ParticleRenderer::Draw(Camera* camera, const Transform& transform) {
	if(blendAdditive) {
		directX->SetBlendMode(BlendState::Additive);
	}

	// vertex shader
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	dxContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0); // unbind from compute shader
	ID3D11Buffer* nullVertices[1] = { nullptr };
	UINT zero = 0;
	dxContext->IASetVertexBuffers(0, 1, nullVertices, &zero, &zero);
	vertexShader->SetArrayBuffer(readOnlyView, 0);
	vertexShader->SetShader();

	// geometry shader
	float z = transform.GetGlobalZ();
	float aspectRatio = sprite->GetAspectRatio();

	Vector2 spriteDims = Vector2(1.f, 1.f);
	int columns = 1;
	int frameCount = 1;
	if(animated) {
		SpriteAtlas* atlas = (SpriteAtlas*)sprite.get();
		columns = atlas->NumCols();
		spriteDims = Vector2(1.0f / columns, 1.0f / atlas->NumRows());
		frameCount = atlas->SpriteCount();
	}

	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	geometryShader->SetConstantVariable("viewProjection", screenSpace ? camera->GetProjection() : camera->GetViewProjection());
	geometryShader->SetConstantVariable("z", &z);
	geometryShader->SetConstantVariable("spriteAspectRatio", &aspectRatio);
	geometryShader->SetConstantVariable("spriteDims", &spriteDims);
	geometryShader->SetConstantVariable("atlasCols", &columns);
	geometryShader->SetConstantVariable("animationFrames", &frameCount);
	geometryShader->SetConstantVariable("frameDuration", &secondsPerFrame);
	geometryShader->SetShader();

	// pixel shader
	pixelShader->SetConstantVariable("tint", &tint);
	pixelShader->SetConstantVariable("lit", &applyLights);
	pixelShader->SetSampler(APP->Assets()->defaultSampler);
	pixelShader->SetTexture(sprite->GetResourceView());
	pixelShader->SetShader();

	// draw
	dxContext->Draw(maxParticles, 0);

	// clean up
	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	GeometryShader::ClearShader();
	Mesh::ClearLastDrawn();

	if(blendAdditive) {
		directX->SetBlendMode(BlendState::AlphaBlend);
	}
}

IBehavior* ParticleRenderer::Clone() {
	return new ParticleRenderer(*this);
}

void ParticleRenderer::Spawn(int amount, float duration) {
	spawnsLeft += max(amount, 0);
	spawnInterval = max(duration / amount, 0);
	timer = spawnInterval;

	if(spawnInterval == 0.f) {
		// spawn all immediately
		Emit(spawnsLeft);
		spawnsLeft = 0;
	}
}

void ParticleRenderer::SetSpawnRate(float perSec, int groupSize) {
	spawnsLeft = CONTINUOUS_SPAWNS;
	spawnInterval = max(1.0f / perSec, 0);
	timer = spawnInterval;
	spawnsPerInterval = groupSize;
}

bool ParticleRenderer::BlendsAdditive() const {
	return blendAdditive;
}

// if there are too many particles, the oldest will be replaced
void ParticleRenderer::Emit(unsigned int newParticles) {
	SetSpawnData(newParticles);

	// dispatch the compute shader
	float trueWidth = width;
	if(scaleWithParent) {
		Vector2 size = owner->GetTransform()->GetScale(true);
		trueWidth *= (size.x + size.y) / 2.f;
	}
	spawnShader->SetReadWriteBuffer(readWriteView, 0);
	spawnShader->SetConstantVariable("spawnCount", &newParticles);
	spawnShader->SetConstantVariable("lastParticle", &lastIndex);
	spawnShader->SetConstantVariable("maxParticles", &maxParticles);
	spawnShader->SetConstantVariable("lifespan", &lifespan);
	spawnShader->SetConstantVariable("width", &trueWidth);
	bool moving = moveStyle != ParticleMovement::None && (moveStyle == ParticleMovement::Directional || moveDirection != Vector2::Zero);
	spawnShader->SetBool("faceMoveDirection", moving && faceMoveDirection);

	spawnShader->Dispatch(newParticles, 1);

	// update trackers
	lastIndex += newParticles;
	if(lastIndex >= maxParticles) {
		lastIndex -= maxParticles;
	}
}

void ParticleRenderer::SetSpawnData(unsigned int newParticles) {
	const Random* rng = APP->RNG();

	// determine the random start positions
	Vector2* positions = new Vector2[newParticles];
	const DirectX::XMFLOAT4X4* transform = owner->GetTransform()->GetWorldMatrix();
	for(unsigned int i = 0; i < newParticles; i++) {
		Vector2 randPos = spawnCircular ? rng->GenerateInCircle(0.5f) : Vector2(rng->GenerateFloat(-0.5f, 0.5f), rng->GenerateFloat(-0.5f, 0.5f));
		positions[i] = randPos.Transform(transform);
	}

	Shader::WriteArray(positions, &initalPosBuffer);
	spawnShader->SetArrayBuffer(initalPosBuffer.view, 0);

	// set the initial velocities
	float rotation;
	Vector2 vector;
	Vector2* velocities = new Vector2[newParticles];
	float trueSpeed = speed;
	if(scaleWithParent) {
		Vector2 size = owner->GetTransform()->GetScale(true);
		trueSpeed *= (size.x + size.y) / 2.f;
	}

	switch(moveStyle) {
	case ParticleMovement::None:
		for(unsigned int i = 0; i < newParticles; i++) {
			velocities[i] = Vector2::Zero;
		}
		break;
	case ParticleMovement::Forward:
		rotation = owner->GetTransform()->GetRotation(true);
		vector = Vector2(trueSpeed * cos(rotation), trueSpeed * sin(rotation));
		for(unsigned int i = 0; i < newParticles; i++) {
			velocities[i] = vector;
		}
		break;
	case ParticleMovement::Directional:
		vector = trueSpeed * moveDirection;
		for(unsigned int i = 0; i < newParticles; i++) {
			velocities[i] = vector;
		}
		break;
	case ParticleMovement::Outward:
		vector = owner->GetTransform()->GetPosition(true);
		for(unsigned int i = 0; i < newParticles; i++) {
			velocities[i] = trueSpeed * (positions[i] - vector).Normalize();
		}
		break;
	case ParticleMovement::Random:
		for(unsigned int i = 0; i < newParticles; i++) {
			velocities[i] = trueSpeed * rng->GenerateOnCircle();
		}
		break;
	}

	Shader::WriteArray(velocities, &initialVelBuffer);
	spawnShader->SetArrayBuffer(initialVelBuffer.view, 1);

	delete[] positions;
	delete[] velocities;
}
