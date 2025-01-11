#include "ParticleRenderer.h"
#include "Application.h"
#include "Mesh.h"
#include "ComputeShader.h"
#include "SpriteAtlas.h"
using namespace DirectX;

#define PARTICLE_BYTES 36 // based on struct in ShaderStructs.hlsli
#define CONTINUOUS_SPAWNS -1

ParticleRenderer::ParticleRenderer(unsigned int maxParticles, float lifespan) {
	const AssetManager* assets = APP->Assets();
	mesh = nullptr;
	vertexShader = assets->particleVS;
	geometryShader = assets->particleGS;
	pixelShader = assets->imagePS;
	spawnShader = assets->particleSpawnCS;
	moveShader = assets->particleMoveCS;

	this->lifespan = lifespan;
	this->maxParticles = maxParticles;
	lastIndex = maxParticles - 1; // causes the first spawn to go in index 0

	applyLights = false;
	tint = Color::White;
	width = 1.0f;
	growthRate = 0.f;
	spinRate = 0.f;
	secondsPerFrame = 0.f;
	fadeDuration = 0.f;
	speed = 0.f;
	moveStyle = ParticleMovement::None;
	faceMoveDirection = false;

	spawnsLeft = 0;
	spawnInterval = 0.0f;
	timer = 0.f;
	
	DXCore* directX = APP->Graphics();
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

ParticleRenderer::ParticleRenderer(unsigned int maxParticles, float lifespan, std::shared_ptr<Sprite> sprite)
	: ParticleRenderer(maxParticles, lifespan)
{
	this->sprite = sprite;
	animated = false;
}

ParticleRenderer::ParticleRenderer(unsigned int maxParticles, float lifespan, std::shared_ptr<SpriteAtlas> animation)
	: ParticleRenderer(maxParticles, lifespan)
{
	sprite = std::static_pointer_cast<Sprite>(animation);
	animated = true;
}

void ParticleRenderer::Update(float deltaTime) {
	// update particles
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	dxContext->VSSetShaderResources(0, 1, nullSRV); // unbind from vertex shader
	moveShader->SetReadWriteBuffer(readWriteView, 0);

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
		Emit(1);
	}
}

void ParticleRenderer::Draw(Camera* camera, const Transform& transform) {
	// vertex shader
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	dxContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0); // unbind from compute shader
	ID3D11Buffer* nullVertices[1] = { nullptr };
	UINT zero = 0;
	dxContext->IASetVertexBuffers(0, 1, nullVertices, &zero, &zero);
	vertexShader->SetArrayBuffer(readOnlyView, 0);
	vertexShader->SetShader();

	// geometry shader
	XMFLOAT4X4 matrix = camera->GetView();
	XMMATRIX product = XMLoadFloat4x4(&matrix);
	matrix = camera->GetProjection();
	product = XMMatrixMultiply(product, XMLoadFloat4x4(&matrix));
	XMStoreFloat4x4(&matrix, product);

	float z = transform.GetGlobalZ();
	float aspectRatio = sprite->GetAspectRatio();

	Vector2 spriteDims = Vector2(1.f, 1.f);
	int columns = 1;
	int frameCount = 1;
	if(animated) {
		SpriteAtlas* atlas = (SpriteAtlas*)sprite.get();
		columns = atlas->NumCols();
		spriteDims = Vector2(1.0 / columns, 1.0f / atlas->NumRows());
		frameCount = atlas->SpriteCount();
	}

	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	geometryShader->SetConstantVariable("viewProjection", &matrix);
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

void ParticleRenderer::SetSpawnRate(float perSec) {
	spawnsLeft = CONTINUOUS_SPAWNS;
	spawnInterval = max(1.0f / perSec, 0);
	timer = spawnInterval;
}

// sets the growth rate based on the current start width and duration
void ParticleRenderer::SetEndWidth(float endWidth) {
	growthRate = (endWidth - width) / lifespan;
}

// if there are too many particles, the oldest will be replaced
void ParticleRenderer::Emit(int newParticles) {
	SetSpawnData(newParticles);

	// dispatch the compute shader
	spawnShader->SetReadWriteBuffer(readWriteView, 0);
	spawnShader->SetConstantVariable("spawnCount", &newParticles);
	spawnShader->SetConstantVariable("lastParticle", &lastIndex);
	spawnShader->SetConstantVariable("maxParticles", &maxParticles);
	spawnShader->SetConstantVariable("lifespan", &lifespan);
	spawnShader->SetConstantVariable("width", &width);
	spawnShader->SetBool("faceMoveDirection", faceMoveDirection);

	spawnShader->Dispatch(newParticles, 1);

	// update trackers
	lastIndex += newParticles;
	if(lastIndex >= maxParticles) {
		lastIndex -= maxParticles;
	}
}

void ParticleRenderer::SetSpawnData(int newParticles) {
	const Random* rng = APP->RNG();

	// determine the random start positions
	Vector2* positions = new Vector2[newParticles];
	Vector2 objectPosition = owner->GetTransform()->GetPosition(true);
	for(int i = 0; i < newParticles; i++) {
		positions[i] = objectPosition;
		if(spawnRadius > 0) {
			positions[i] += rng->GenerateInCircle(spawnRadius);
		}
	}

	Shader::WriteArray(positions, &initalPosBuffer);
	spawnShader->SetArrayBuffer(initalPosBuffer.view, 0);

	// set the initial velocities
	Vector2 forward = speed * Vector2::Right;
	Vector2* velocities = new Vector2[newParticles];
	switch(moveStyle) {
	case ParticleMovement::None:
		for(int i = 0; i < newParticles; i++) {
			velocities[i] = Vector2::Zero;
		}
		break;
	case ParticleMovement::Forward:
		for(int i = 0; i < newParticles; i++) {
			velocities[i] = forward;
		}
		break;
	case ParticleMovement::Outward:
		for(int i = 0; i < newParticles; i++) {
			velocities[i] = speed * (positions[i] - objectPosition).Normalize();
		}
		break;
	case ParticleMovement::Random:
		for(int i = 0; i < newParticles; i++) {
			velocities[i] = rng->GenerateOnCircle();
		}
		break;
	}

	Shader::WriteArray(velocities, &initialVelBuffer);
	spawnShader->SetArrayBuffer(initialVelBuffer.view, 1);

	delete[] positions;
	delete[] velocities;
}
