#include "ParticleRenderer.h"
#include "Application.h"
#include "Mesh.h"
#include "ComputeShader.h"
using namespace DirectX;

#define CONTINUOUS_SPAWNS -1

ParticleRenderer::ParticleRenderer(unsigned int maxParticles) {
	const AssetManager* assets = APP->Assets();
	mesh = nullptr;
	vertexShader = assets->particleVS;
	pixelShader = assets->colorPS;
	geometryShader = assets->particleGS;
	spawnShader = assets->particleSpawnCS;
	moveShader = assets->particleMoveCS;

	this->maxParticles = maxParticles;
	particleCount = 0;
	lastIndex = maxParticles - 1; // causes the first spawn to go in index 0

	spawnsLeft = 0;
	spawnInterval = 0.0f;
	timer = 0.f;
	
	DXCore* directX = APP->Graphics();
	dxDevice = directX->GetDevice();
	dxContext = directX->GetContext();

	// create a vertex buffer for the particles
	D3D11_BUFFER_DESC bufferDescription = {};
	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth = sizeof(Particle) * maxParticles;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;
	bufferDescription.MiscFlags = 0; // D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDescription.StructureByteStride = 0;

	dxDevice->CreateBuffer(&bufferDescription, 0, vertexBuffer.GetAddressOf());

	// create a read/write buffer for the particles that the compute shader can use
	bufferDescription.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDescription.StructureByteStride = sizeof(Particle);

	dxDevice->CreateBuffer(&bufferDescription, 0, computeParticleBuffer.GetAddressOf());

	// create the view for the read/write buffer
	D3D11_UNORDERED_ACCESS_VIEW_DESC viewDescription = {};
	viewDescription.Format = DXGI_FORMAT_UNKNOWN;
	viewDescription.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	viewDescription.Buffer.FirstElement = 0;
	viewDescription.Buffer.NumElements = maxParticles;
	viewDescription.Buffer.Flags = 0;

	dxDevice->CreateUnorderedAccessView(computeParticleBuffer.Get(), &viewDescription, computeParticleView.GetAddressOf());

	// create the buffer for setting initial positions
	initalPosBuffer = Shader::CreateArrayBuffer(maxParticles, sizeof(Vector2), ShaderDataType::Float2);
}

void ParticleRenderer::Update(float deltaTime) {
	// update particles

	
	
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
	dxContext->CopyResource(vertexBuffer.Get(), computeParticleBuffer.Get());
	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	vertexShader->SetShader();

	// geometry shader
	XMFLOAT4X4 matrix = camera->GetView();
	XMMATRIX product = XMLoadFloat4x4(&matrix);
	matrix = camera->GetProjection();
	product = XMMatrixMultiply(product, XMLoadFloat4x4(&matrix));
	XMStoreFloat4x4(&matrix, product);

	float z = transform.GetGlobalZ();
	float aspectRatio = 1.0f;

	geometryShader->SetConstantVariable("viewProjection", &matrix);
	geometryShader->SetConstantVariable("z", &z);
	geometryShader->SetConstantVariable("spriteAspectRatio", &aspectRatio);
	geometryShader->SetShader();

	// pixel shader
	Color red = Color::Red;
	pixelShader->SetConstantVariable("color", &red);
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

void ParticleRenderer::Emit(int newParticles) {
	// don't spawn too many
	if(particleCount >= maxParticles) {
		return;
	}

	if(particleCount + newParticles > maxParticles) {
		newParticles = maxParticles - particleCount;
	}

	// determine the random start positions
	const Random* rng = APP->RNG();
	Vector2* positions = new Vector2[newParticles];
	Vector2 objectPosition = owner->GetTransform()->GetPosition(true);
	for(int i = 0; i < newParticles; i++) {
		positions[i] = objectPosition;
		if(spawnRadius > 0) {
			float angle = rng->Generate(0.f, DirectX::XM_2PI);
			positions[i] += rng->Generate(0.f, spawnRadius) * Vector2(cos(angle), sin(angle));
		}
	}

	Shader::WriteArray(positions, &initalPosBuffer);

	// dispatch the compute shader
	spawnShader->SetReadWriteBuffer(computeParticleView, 0);
	spawnShader->SetArrayBuffer(initalPosBuffer.view, 1);

	spawnShader->SetConstantVariable("spawnCount", &newParticles);
	spawnShader->SetConstantVariable("lastParticle", &lastIndex);
	spawnShader->SetConstantVariable("maxParticles", &maxParticles);
	spawnShader->SetConstantVariable("lifespan", &lifespan);

	spawnShader->Dispatch(newParticles, 1);

	// update trackers
	particleCount += newParticles;
	lastIndex += newParticles;
	if(lastIndex >= maxParticles) {
		lastIndex -= maxParticles;
	}

	delete[] positions;
}
