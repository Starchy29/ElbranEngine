#include "AssetContainer.h"
#include "GraphicsAPI.h"
#include "Application.h"
#include "SoundMixer.h"
#include <stdint.h>
#include "lodepng.h"

#define LoadSpriteSheetPNG(fileName, rows, cols, numSprites) SpriteSheet{ LoadPNG(fileName), rows, cols, numSprites }

struct ByteColor {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

void AssetContainer::Initialize(std::wstring filePath, GraphicsAPI* graphics, SoundMixer* audio) {
	defaultSampler = graphics->CreateDefaultSampler();
	graphics->SetSampler(ShaderStage::Vertex, &defaultSampler, 0);
	graphics->SetSampler(ShaderStage::Geometry, &defaultSampler, 0);
	graphics->SetSampler(ShaderStage::Pixel, &defaultSampler, 0);
	graphics->SetSampler(ShaderStage::Compute, &defaultSampler, 0);

	// create unit square
	Vertex vertices[] = {
		{ Vector2(-0.5f, -0.5f), Vector2(0.0f, 1.0f) },
		{ Vector2(-0.5f, 0.5f), Vector2(0.0f, 0.0f) },
		{ Vector2(0.5f, 0.5f), Vector2(1.0f, 0.0f) },
		{ Vector2(0.5f, -0.5f), Vector2(1.0f, 1.0f) }
	};

	uint32_t indices[] = {
		0, 1, 3, // clockwise winding order
		1, 2, 3
	};

	unitSquare = graphics->CreateMesh(vertices, 4, indices, 6, false);

	// create unit triangle
	Vertex triVerts[] = {
		{ Vector2(-0.5f, 0.5f), Vector2(0.0f, 0.0f) },
		{ Vector2(0.5f, 0.f), Vector2(1.0f, 0.5f) },
		{ Vector2(-0.5f, -0.5f), Vector2(0.0f, 1.0f) }
	};

	uint32_t triIndices[] = { 0, 1, 2 };
	unitTriangle = graphics->CreateMesh(triVerts, 3, triIndices, 3, false);

	// load shaders
	fullscreenVS = graphics->LoadVertexShader(L"FullscreenVS.cso");
	cameraVS = graphics->LoadVertexShader(L"CameraVS.cso");
	particlePassPS = graphics->LoadVertexShader(L"ParticlePassVS.cso");

	particleQuadGS = graphics->LoadGeometryShader(L"ParticleQuadGS.cso");

	solidColorPS = graphics->LoadPixelShader(L"SolidColorPS.cso");
	texturePS = graphics->LoadPixelShader(L"TexturePS.cso");
	circleFillPS = graphics->LoadPixelShader(L"CircleFillPS.cso");
	textRasterizePS = graphics->LoadPixelShader(L"TextRasterizePS.cso");

	conSatValPP = graphics->LoadPixelShader(L"ConSatValPP.cso");
	blurPP = graphics->LoadPixelShader(L"BlurPP.cso");
	bloomFilterPP = graphics->LoadPixelShader(L"BloomFilterPP.cso");
	screenSumPP = graphics->LoadPixelShader(L"ScreenSumPP.cso");

	brightnessSumCS = graphics->LoadComputeShader(L"BrightnessSumCS.cso");
	particleSpawnCS = graphics->LoadComputeShader(L"ParticleSpawnCS.cso");
	particleMoveCS = graphics->LoadComputeShader(L"ParticleMoveCS.cso");

	testSprite = LoadPNG(L"elbran.png");
	testBMP = LoadBMP(L"testbmp.bmp");
	arial = Font::Load(L"arial.ttf");
}

void AssetContainer::Release() {
	GraphicsAPI* graphics = app.graphics;
	SoundMixer* audio = app.audio;

	graphics->ReleaseSampler(&defaultSampler);
	graphics->ReleaseMesh(&unitSquare);
	graphics->ReleaseMesh(&unitTriangle);

	graphics->ReleaseShader(&fullscreenVS);
	graphics->ReleaseShader(&cameraVS);
	graphics->ReleaseShader(&particlePassPS);

	graphics->ReleaseShader(&particleQuadGS);

	graphics->ReleaseShader(&solidColorPS);
	graphics->ReleaseShader(&texturePS);
	graphics->ReleaseShader(&circleFillPS);
	graphics->ReleaseShader(&textRasterizePS);

	graphics->ReleaseShader(&conSatValPP);
	graphics->ReleaseShader(&blurPP);
	graphics->ReleaseShader(&bloomFilterPP);
	graphics->ReleaseShader(&screenSumPP);

	graphics->ReleaseShader(&brightnessSumCS);
	graphics->ReleaseShader(&particleSpawnCS);
	graphics->ReleaseShader(&particleMoveCS);

	graphics->ReleaseTexture(&testSprite);
	graphics->ReleaseTexture(&testBMP);

	arial.Release();
}

Texture2D AssetContainer::LoadBMP(std::wstring fileName) {
	LoadedFile file = app.LoadFile(app.filePath + L"assets\\" + fileName);
	file.littleEndian = true;
	assert(file.ReadUInt16() == 0x4D42); // file type must be "BM"
	file.readLocation = 10;
	uint32_t bitmapOffset = file.ReadUInt32();

	uint32_t headerSize = file.ReadUInt32();
	uint8_t version;
	switch(headerSize) {
	case 12: version = 2; break;
	case 40: version = 3; break;
	case 108: version = 4; break;
	default: assert(false); break;
	}

	int32_t width;
	int32_t height;
	if(version == 2) {
		width = file.ReadInt16();
		height = file.ReadInt16();
	} else {
		width = file.ReadInt32();
		height = file.ReadInt32();
	}

	file.readLocation += 2;
	uint16_t bitsPerPixel = file.ReadUInt16();

	uint32_t compressionMethod = 0;
	uint32_t colorsUsed = 0;
	uint32_t redMask = 0;
	uint32_t greenMask = 0;
	uint32_t blueMask = 0;
	uint32_t alphaMask = 0;
	if(version > 2) {
		compressionMethod = file.ReadUInt32();
		file.readLocation += 12;
		colorsUsed = file.ReadUInt32();
		file.readLocation += 4;

		if(version == 4) {
			redMask = file.ReadUInt32();
			greenMask = file.ReadUInt32();
			blueMask = file.ReadUInt32();
			alphaMask = file.ReadUInt32();
			file.readLocation += 52;
		}
	}

	// read color palette
	ByteColor palette[256];
	if(bitsPerPixel <= 8) {
		if(colorsUsed == 0) colorsUsed = 1 << bitsPerPixel;
		for(uint32_t i = 0; i < colorsUsed; i++) {
			uint8_t blue = file.ReadByte();
			uint8_t green = file.ReadByte();
			uint8_t red = file.ReadByte();
			if(version != 2) file.readLocation++;
			palette[i] = ByteColor(red, green, blue, 255);
		}
	}

	// read bit mask instead
	if(version == 3 && compressionMethod == 3) {
		redMask = file.ReadUInt32();
		greenMask = file.ReadUInt32();
		blueMask = file.ReadUInt32();
	}

	uint8_t redMaskShift = 0;
	uint8_t greenMaskShift = 0;
	uint8_t blueMaskShift = 0;
	if(compressionMethod == 3) {
		uint32_t testMask;
		if(redMask > 0) {
			testMask = redMask;
			while(!(testMask & 0b00000001)) {
				redMaskShift++;
				testMask = testMask << 1;
			}
		}
		if(greenMask > 0) {
			testMask = greenMask;
			while(!(testMask & 0b00000001)) {
				greenMaskShift++;
				testMask = testMask << 1;
			}
		}
		if(blueMask > 0) {
			testMask = blueMask;
			while(!(testMask & 0b00000001)) {
				blueMaskShift++;
				testMask = testMask << 1;
			}
		}
	}

	// load bits
	ByteColor* loadedBits = new ByteColor[width*height];
	file.readLocation = bitmapOffset;
	bool topDown = height < 0;

	if(version == 3 && bitsPerPixel == 16) {
		file.littleEndian = false;
	}
	
	for(int32_t i = 0; i < height; i++) {
		// read each scan line
		int32_t y = topDown ? i : height - 1 - i;
		uint8_t colorByte;
		uint8_t encodeByte;
		uint16_t colorWord;
		uint32_t colorDWord;
		uint8_t red;
		uint8_t green;
		uint8_t blue;

		int32_t x = 0;
		while(x < width) {
			switch(bitsPerPixel) {
			case 1:
				colorByte = file.ReadByte();
				for(int32_t i = 0; i < 8 && x < width; i++) {
					loadedBits[x + y * width] = palette[colorByte & 0b00000001];
					colorByte = colorByte >> 1;
					x++;
				}
				break;
			case 4:
				if(compressionMethod == 2) {
					encodeByte = file.ReadByte();
					if(encodeByte == 0) {
						encodeByte = file.ReadByte();
						if(encodeByte == 0) {
							// end scan line
							x = width;
							break;
						}
						else if(encodeByte == 1) {
							// end RLE
							i = height;
							break;
						}
						else if(encodeByte == 2) {
							// jump to another part of the image
							x = file.ReadByte();
							i = file.ReadByte();
							break;
						}

						// unencoded run
						for(uint8_t j = 0; j < encodeByte; j++) {
							if(j % 2 == 0) colorByte = file.ReadByte();
							loadedBits[x + y * width] = palette[j % 2 == 0? colorByte & 0b11110000 : colorByte & 0b00001111];
							x++;
						}
						if(encodeByte % 2 == 1) file.readLocation++;
					} else {
						// encoded run
						colorByte = file.ReadByte();;
						for(uint8_t j = 0; j < encodeByte; j++) {
							loadedBits[x + y * width] = palette[j % 2 == 0? colorByte & 0b11110000 : colorByte & 0b00001111];
							x++;
						}
					}
				} else {
					colorByte = file.ReadByte();
					loadedBits[x + y * width] = palette[colorByte & 0b00001111];
					x++;
					if(x >= width) break;
					colorByte = colorByte >> 4;
					loadedBits[x + y * width] = palette[colorByte];
				}
				break;
			case 8:
				if(compressionMethod == 1) {
					encodeByte = file.ReadByte();
					if(encodeByte == 0) {
						encodeByte = file.ReadByte();
						if(encodeByte == 0) {
							// end scan line
							x = width;
							break;
						}
						else if(encodeByte == 1) {
							// end RLE
							i = height;
							break;
						}
						else if(encodeByte == 2) {
							// jump to another part of the image
							x = file.ReadByte();
							i = file.ReadByte();
							break;
						}

						// unencoded run
						for(uint8_t j = 0; j < encodeByte; j++) {
							loadedBits[x + y * width] = palette[file.ReadByte()];
							x++;
						}
						if(encodeByte % 2 == 1) file.readLocation++;
					} else {
						// encoded run
						colorByte = file.ReadByte();;
						for(uint8_t j = 0; j < encodeByte; j++) {
							loadedBits[x + y * width] = palette[colorByte];
							x++;
						}
					}
				} else {
					loadedBits[x + y * width] = palette[file.ReadByte()];
					x++;
				}
				break;
			case 16:
				colorWord = file.ReadUInt16();
				colorDWord = colorWord << 16;
				loadedBits[x + y * width] = ByteColor(
					(colorDWord & redMask) >> redMaskShift, 
					(colorDWord & greenMask) >> greenMaskShift, 
					(colorDWord & blueMask) >> blueMaskShift, 
					255
				);
				x++;
				break;
			case 24:
				blue = file.ReadByte();
				green = file.ReadByte();
				red = file.ReadByte();
				loadedBits[x + y * width] = ByteColor(red, green, blue, 255);
				x++;
				break;
			case 32:
				colorDWord = file.ReadUInt32();
				loadedBits[x + y * width] = ByteColor(
					(colorDWord & redMask) >> redMaskShift, 
					(colorDWord & greenMask) >> greenMaskShift, 
					(colorDWord & blueMask) >> blueMaskShift, 
					255
				);
				x++;
				break;
			default:
				assert(false);
				break;
			}
		}

		if(compressionMethod == 0) {
			// end on a 4-byte boundary
			file.readLocation += width % 4;
		}
	}

	file.Release();
	Texture2D result = app.graphics->CreateConstantTexture(width, height, (uint8_t*)loadedBits);
	delete[] loadedBits;
	return result;
}

Texture2D AssetContainer::LoadPNG(std::wstring fileName) {
	LoadedFile file = app.LoadFile(app.filePath + L"assets\\" + fileName);
	std::vector<uint8_t> lodeFile(file.bytes, file.bytes + file.fileSize);
	std::vector<uint8_t> loadedImage;
	uint32_t width;
	uint32_t height;
	lodepng::decode(loadedImage, width, height, lodeFile); // thank you Lode Vandevenne
	Texture2D result = app.graphics->CreateConstantTexture(width, height, loadedImage.begin()._Ptr);
	file.Release();
	return result;
}
