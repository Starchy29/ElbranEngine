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
	LoadedFile file = app.LoadFile(app.filePath + L"Assets\\" + fileName);
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
	lodepng::decode();
	

	LoadedFile file = app.LoadFile(app.filePath + L"Assets\\" + fileName);
	std::vector<uint8_t> lodeFile(file.bytes, file.bytes + file.fileSize);

	file.readLocation = 16;
	uint32_t width = file.ReadUInt32();
	uint32_t height = file.ReadUInt32();
	uint8_t bitDepth = file.ReadByte();
	uint8_t colorType = file.ReadByte();
	uint8_t compressionMethod = file.ReadByte();
	uint8_t filterMethod = file.ReadByte();
	uint8_t interlaceMethod = file.ReadByte();
	ASSERT(compressionMethod == 0 && filterMethod == 0 && interlaceMethod == 0);
	file.readLocation += 4; // skip chunk length

	uint32_t chunkSize;
	uint8_t chunkName[4] {};
	ByteColor palette[256];
	while(chunkName[0] != 'I' || chunkName[1] != 'D' || chunkName[2] != 'A' || chunkName[3] != 'T') {
		chunkSize = file.ReadUInt32();
		chunkName[0] = file.ReadByte();
		chunkName[1] = file.ReadByte();
		chunkName[2] = file.ReadByte();
		chunkName[3] = file.ReadByte();

		if(chunkName[0] == 'P' && chunkName[1] == 'L' && chunkName[2] == 'T' && chunkName[3] == 'E') {
			for(uint32_t i = 0; i < chunkSize / 3; i++) {
				palette[i].red = file.ReadByte();
				palette[i].green = file.ReadByte();
				palette[i].blue = file.ReadByte();
			}
		} else {
			file.readLocation += chunkSize;
		}

		file.readLocation += 4; // skip chunk length
	}

	// read IDAT chunks
	bool hasPalette = colorType & 0b00000001;
	bool greyScale = !(colorType & 0b00000010);
	bool hasAlpha = colorType & 0b00000100;

	uint8_t bitsPerPixelSample = (hasPalette ? 8 : bitDepth) * (greyScale ? 1 : 3);
	if(hasAlpha) bitsPerPixelSample += bitDepth;
	uint8_t* decompressedImage = new uint8_t[height * (width * bitsPerPixelSample + 1)]; // each scanline is preceded by a byte for filter type
	uint32_t outputLocation = 0;

	while(chunkName[0] == 'I' && chunkName[1] == 'D' && chunkName[2] == 'A' && chunkName[3] == 'T') {
		file.readLocation++;
		uint8_t flags = file.ReadByte();
		if(flags & 0b00010000) file.readLocation += 4; // skip preset dictionary
		
		// decompress data that was compressed with deflate algorithm
		file.littleEndian = true;
		bool lastBlock = false;
		while(!lastBlock) {
			bool lastBlock = file.ReadBits(1);
			uint8_t compressionType = file.ReadBits(2);

			if(compressionType == 0) {
				uint8_t uncompressedLength = file.ReadUInt16();
				file.readLocation += 2;
				file.ReadBytes(uncompressedLength, decompressedImage + outputLocation);
				outputLocation += uncompressedLength;
			} else {
				if(compressionType == 2) {
					// read representation of code trees
				}

				uint16_t literalOrLengthCode = 0;
				while(literalOrLengthCode != 256) {
					// decode literal/length value from input stream
					literalOrLengthCode = 0;

					if(literalOrLengthCode < 256) {
						// copy value (literal byte) to output stream
						decompressedImage[outputLocation] = (uint8_t)literalOrLengthCode;
						outputLocation++;
					}
					else if(literalOrLengthCode > 256) {
						uint8_t numExtraBits;
						uint8_t currentExtraBits;

						// decode length value
						uint16_t actualLength = 0;
						if(literalOrLengthCode == 285) {
							actualLength = 258;
						}
						else if(literalOrLengthCode < 265) {
							actualLength = literalOrLengthCode - 254;
						}
						else {
							actualLength = 10;
							currentExtraBits = 0;
							for(uint16_t i = 265; i < literalOrLengthCode; i++) {
								if(i % 4 == 1) currentExtraBits++;
								actualLength += 1 << currentExtraBits; // 2^extra bits
							}

							numExtraBits = (literalOrLengthCode - 261) / 4;
							actualLength = actualLength + 1 + file.ReadBits(numExtraBits);
						}

						// decode distance from input stream
						uint8_t distCode = file.ReadBits(5);
						uint16_t backDistance = distCode + 1; // default case for distCode < 4

						numExtraBits = 0;
						if(distCode > 3) {
							currentExtraBits = 0;
							backDistance = 4;
							for(uint8_t i = 4; i < distCode; i++) {
								if(i % 2 == 0) currentExtraBits++;
								backDistance += 1 << currentExtraBits; // 2^extra bits
							}

							numExtraBits = (distCode - 2) / 2;
							backDistance = backDistance + 1 + file.ReadBits(numExtraBits);
						}
						
						// duplicate length bytes starting from backward distance from output stream
						for(uint16_t i = 0; i < actualLength; i++) {
							decompressedImage[outputLocation + i] = decompressedImage[outputLocation - backDistance + i];
						}
						outputLocation += actualLength;
					}
				}
			}
		}
		file.littleEndian = false;

		// read header of next chunk
		chunkSize = file.ReadUInt32();
		chunkName[0] = file.ReadByte();
		chunkName[1] = file.ReadByte();
		chunkName[2] = file.ReadByte();
		chunkName[3] = file.ReadByte();
		file.readLocation += 4;
	}

	// filter data
	for(uint32_t y = 0; y < height; y++) {
		uint8_t filterType = decompressedImage[y * width];
		for(uint32_t x = 1; x <= width; x++) {

		}
	}

	// convert data to pixels
	ByteColor* loadedBits = new ByteColor[width*height];
	switch(bitDepth) {
	case 1:
		break;
	case 2:
		break;
	case 4:
		break;
	case 8:
		break;
	case 16:
		break;
	}

	Texture2D result = app.graphics->CreateConstantTexture(width, height, (uint8_t*)loadedBits);
	file.Release();
	delete[] decompressedImage;
	delete[] loadedBits;
	return result;
}
