#include "AssetContainer.h"
#include "GraphicsAPI.h"
#include "MemoryArena.h"
#include <stdint.h>
#include "lodepng.h"
#include "FixedList.h"
#include "FileIO.h"

#define LoadSpriteSheetPNG(fileName, rows, cols, numSprites) SpriteSheet{ LoadPNG(fileName), rows, cols, numSprites }

struct ByteColor {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

void AssetContainer::Initialize(GraphicsAPI* graphics, MemoryArena* arena) {
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

	fullscreenVS = LoadVertexShader(graphics, "FullscreenVS.cso");
	cameraVS = LoadVertexShader(graphics, "CameraVS.cso");
	particlePassPS = LoadVertexShader(graphics, "ParticlePassVS.cso");

	particleQuadGS = LoadGeometryShader(graphics, "ParticleQuadGS.cso");

	solidColorPS = LoadPixelShader(graphics, "SolidColorPS.cso");
	texturePS = LoadPixelShader(graphics, "TexturePS.cso");
	circleFillPS = LoadPixelShader(graphics, "CircleFillPS.cso");
	textRasterizePS = LoadPixelShader(graphics, "TextRasterizePS.cso");

	conSatValPP = LoadPixelShader(graphics, "ConSatValPP.cso");
	blurPP = LoadPixelShader(graphics, "BlurPP.cso");
	bloomFilterPP = LoadPixelShader(graphics, "BloomFilterPP.cso");
	screenSumPP = LoadPixelShader(graphics, "ScreenSumPP.cso");

	brightnessSumCS = LoadComputeShader(graphics, "BrightnessSumCS.cso");
	particleSpawnCS = LoadComputeShader(graphics, "ParticleSpawnCS.cso");
	particleMoveCS = LoadComputeShader(graphics, "ParticleMoveCS.cso");
	particleClearCS = LoadComputeShader(graphics, "ParticleClearCS.cso");

	testSprite = LoadPNG(graphics, "elbran.png");
	testBMP = LoadBMP(graphics, "testbmp.bmp");
	apple = LoadPNG(graphics, "apple.png");
	arial = LoadTTF(graphics, arena, "arial.ttf");

	testSheet = SpriteSheet(testSprite);
	testSound = LoadWAV("water plunk.wav");
	testSong = LoadWAV("Menu song.wav");
}

void AssetContainer::Release(GraphicsAPI* graphics) {
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
	graphics->ReleaseShader(&particleClearCS);

	graphics->ReleaseTexture(&testSprite);
	graphics->ReleaseTexture(&testBMP);
	graphics->ReleaseTexture(&apple);

	ReleaseFont(graphics, &arial);

	testSound.Release();
	testSong.Release();
}

void AssetContainer::ReleaseFont(const GraphicsAPI* graphics, Font* font) {
	graphics->ReleaseArrayBuffer(&font->glyphCurves);
	graphics->ReleaseArrayBuffer(&font->firstCurveIndices);
	delete[] font->glyphBaselines;
	delete[] font->glyphDimensions;
	font->charToGlyphIndex.Release();
}

#pragma region File Loaders
VertexShader AssetContainer::LoadVertexShader(const GraphicsAPI* graphics, const char* fileName) {
	char path[128] = "shaders\\";
	AddStrings(path, fileName, path);
	LoadedFile shaderBlob = FileIO::LoadFile(path);
	VertexShader result = graphics->CreateVertexShader(&shaderBlob);
	shaderBlob.Release();
	return result;
}

GeometryShader AssetContainer::LoadGeometryShader(const GraphicsAPI* graphics, const char* fileName) {
	char path[128] = "shaders\\";
	AddStrings(path, fileName, path);
	LoadedFile shaderBlob = FileIO::LoadFile(path);
	GeometryShader result = graphics->CreateGeometryShader(&shaderBlob);
	shaderBlob.Release();
	return result;
}

PixelShader AssetContainer::LoadPixelShader(const GraphicsAPI* graphics, const char* fileName) {
	char path[128] = "shaders\\";
	AddStrings(path, fileName, path);
	LoadedFile shaderBlob = FileIO::LoadFile(path);
	PixelShader result = graphics->CreatePixelShader(&shaderBlob);
	shaderBlob.Release();
	return result;
}

ComputeShader AssetContainer::LoadComputeShader(const GraphicsAPI* graphics, const char* fileName) {
	char path[128] = "shaders\\";
	AddStrings(path, fileName, path);
	LoadedFile shaderBlob = FileIO::LoadFile(path);
	ComputeShader result = graphics->CreateComputeShader(&shaderBlob);
	shaderBlob.Release();
	return result;
}

Texture2D AssetContainer::LoadBMP(const GraphicsAPI* graphics, const char* fileName) {
	char path[128] = "assets\\";
	AddStrings(path, fileName, path);
	LoadedFile file = FileIO::LoadFile(path);
	file.littleEndian = true;
	ASSERT(file.ReadUInt16() == 0x4D42); // file type must be "BM"
	file.readLocation = 10;
	uint32_t bitmapOffset = file.ReadUInt32();

	uint32_t headerSize = file.ReadUInt32();
	uint8_t version;
	switch(headerSize) {
	case 12: version = 2; break;
	case 40: version = 3; break;
	case 108: version = 4; break;
	default: ASSERT(false); break;
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
				ASSERT(false);
				break;
			}
		}

		if(compressionMethod == 0) {
			// end on a 4-byte boundary
			file.readLocation += width % 4;
		}
	}

	file.Release();
	Texture2D result = graphics->CreateConstantTexture(width, height, (uint8_t*)loadedBits);
	result.translucent = false;
	delete[] loadedBits;
	return result;
}

Texture2D AssetContainer::LoadPNG(const GraphicsAPI* graphics, const char* fileName) {
	char path[128] = "assets\\";
	AddStrings(path, fileName, path);
	LoadedFile file = FileIO::LoadFile(path);
	std::vector<uint8_t> lodeFile(file.bytes, file.bytes + file.fileSize);
	std::vector<uint8_t> loadedImage;
	uint32_t width;
	uint32_t height;
	lodepng::decode(loadedImage, width, height, lodeFile); // thank you Lode Vandevenne
	Texture2D result = graphics->CreateConstantTexture(width, height, loadedImage.begin()._Ptr);
	file.Release();
	return result;
}

AudioSample AssetContainer::LoadWAV(const char* fileName) {
	char path[128] = "assets\\";
	AddStrings(path, fileName, path);
	LoadedFile file = FileIO::LoadFile(path);
	file.littleEndian = true;
	uint32_t chunkName = file.ReadUInt32();
	ASSERT(chunkName == 'FFIR');

	file.readLocation += 4;
	chunkName = file.ReadUInt32();
	ASSERT(chunkName == 'EVAW');

	AudioSample loadedSound = {};
	loadedSound.baseVolume = 1.0f;

	while(file.readLocation < file.fileSize) {
		chunkName = file.ReadUInt32();
		uint32_t chunkSize = file.ReadUInt32();
		switch(chunkName) {
		default:
			file.readLocation += chunkSize;
			break;

		case ' tmf':
			file.ReadBytes(chunkSize, (uint8_t*)&loadedSound.format); // WaveFormat struct matches layout from the .wav file
			break;

		case 'atad':
			loadedSound.audioBuffer = new uint8_t[chunkSize];
			loadedSound.bufferLength = chunkSize;
			file.ReadBytes(chunkSize, loadedSound.audioBuffer);
			break;
		}

		if(chunkSize % 2 != 0) file.readLocation++; // padding byte
	}

	file.Release();
	return loadedSound;
}

#pragma region Font Loading
// must match TextRasterizePS
struct BezierCurve {
    Vector2 start;
    Vector2 control;
    Vector2 end;
};

struct Matrix2D {
    Vector2 left;
    Vector2 right;
};

Matrix2D operator*(const Matrix2D& left, const Matrix2D& right) {
    return {
        { left.left.x * right.left.x + left.right.x * right.left.y, left.left.y * right.left.x + left.right.y * right.left.y },
        { left.left.x * right.right.x + left.right.x * right.right.y, left.left.y * right.right.x + left.right.y * right.right.y }
    };
}

struct FontLoader {
    LoadedFile fontFile;
    FixedMap<uint32_t, uint32_t> tableStarts;
    uint32_t locaStart;
    int16_t locFormat;
    uint16_t* contourEndIndices;
    uint8_t* flags;
    Vector2* points;
    int16_t* leftSideBearings;
    uint16_t* advanceWidths;

	MemoryArena* curveArena;
	BezierCurve* curves;
	uint64_t numCurves;

	void AddCurve(BezierCurve curve) {
		BezierCurve* nextSlot = (BezierCurve*)curveArena->Reserve(sizeof(BezierCurve));
		*nextSlot = curve;
		numCurves++;
	}

    void ReadGlyph(uint16_t glyphIndex, AlignedRect* outArea, const Matrix2D* transform, Int2 offset, const AlignedRect* baseArea = nullptr) {
		fontFile.readLocation = locaStart + glyphIndex * (locFormat == 0 ? 2 : 4);
		uint32_t glyphOffset = (locFormat == 0 ? fontFile.ReadUInt16() * 2u : fontFile.ReadUInt32());
		uint32_t nextOffset = (locFormat == 0 ? fontFile.ReadUInt16() * 2u : fontFile.ReadUInt32());
		fontFile.readLocation = tableStarts.Get('glyf') + glyphOffset;

		int16_t contourCount = fontFile.ReadInt16();
		AlignedRect glyphArea;
		glyphArea.left = (float)(fontFile.ReadInt16() - leftSideBearings[glyphIndex]);
		glyphArea.bottom = (float)fontFile.ReadInt16();
		fontFile.readLocation += 2; // skip xMax
		glyphArea.right = glyphArea.left + advanceWidths[glyphIndex];
		glyphArea.top = (float)fontFile.ReadInt16();

		if(outArea) {
			*outArea = glyphArea;
		}

		if(nextOffset == glyphOffset) return; // no curves to load

		if(contourCount >= 0) {
			ReadSimpleGlyph(contourCount, baseArea ? *baseArea : glyphArea, transform, offset);
		} else {
			ReadCompositeGlyph(transform, offset, baseArea ? baseArea : &glyphArea);
		}
	}

#define ON_CURVE_BIT 0b00000001
#define UNSIGNED_X_BIT 0b00000010
#define UNSIGNED_Y_BIT 0b00000100
#define REPEAT_BIT 0b00001000
#define POSITIVE_X_BIT 0b00010000
#define POSITIVE_Y_BIT 0b00100000
#define DUPLICATE_X_BIT 0b00010000
#define DUPLICATE_Y_BIT 0b00100000

    void ReadSimpleGlyph(int16_t contourCount, AlignedRect glyphArea, const Matrix2D* transform, Int2 offset) {
		Vector2 glyphSize = glyphArea.Size();

		for(int16_t i = 0; i < contourCount; i++) {
			contourEndIndices[i] = fontFile.ReadUInt16();
		}
		uint8_t numPoints = contourEndIndices[contourCount - 1] + 1;
		uint16_t instructionLength = fontFile.ReadUInt16();
		fontFile.readLocation += instructionLength;

		// read flags
		for(uint8_t pointIndex = 0; pointIndex < numPoints; pointIndex++) {
			flags[pointIndex] = fontFile.ReadByte();
			if(flags[pointIndex] & REPEAT_BIT) {
				uint8_t repetitions = fontFile.ReadByte();
				for(uint8_t r = 0; r < repetitions; r++) {
					pointIndex++;
					flags[pointIndex] = flags[pointIndex - 1];
				}
			}
		}

		// read x coords
		int16_t currentX = 0;
		for(uint8_t pointIndex = 0; pointIndex < numPoints; pointIndex++) {
			if(flags[pointIndex] & UNSIGNED_X_BIT) {
				uint8_t byte = fontFile.ReadByte();
				currentX += (flags[pointIndex] & POSITIVE_X_BIT ? byte : -byte);
			} 
			else if(!(flags[pointIndex] & DUPLICATE_X_BIT)) {
				currentX += fontFile.ReadInt16();
			}
			points[pointIndex].x = (currentX - glyphArea.left + offset.x);
		}

		// read y coords
		int16_t currentY = 0;
		for(uint8_t pointIndex = 0; pointIndex < numPoints; pointIndex++) {
			if(flags[pointIndex] & UNSIGNED_Y_BIT) {
				uint8_t byte = fontFile.ReadByte();
				currentY += (flags[pointIndex] & POSITIVE_Y_BIT ? byte : -byte);
			} 
			else if(!(flags[pointIndex] & DUPLICATE_Y_BIT)) {
				currentY += fontFile.ReadInt16();
			}
			points[pointIndex].y = (currentY - glyphArea.bottom + offset.y);
		}

		// apply transform and normalize to [0,1]
		for(uint8_t pointIndex = 0; pointIndex < numPoints; pointIndex++) {
			points[pointIndex].x = (points[pointIndex].x * transform->left.x + points[pointIndex].y * transform->right.x) / glyphSize.x;
			points[pointIndex].y = (points[pointIndex].x * transform->left.y + points[pointIndex].y * transform->right.y) / glyphSize.y;
		}

		// convert contours to bezier curves
		uint16_t contourStart = 0;
		uint16_t contourEnd;
		for(int16_t contour = 0; contour < contourCount; contour++) {
			contourEnd = contourEndIndices[contour];
			uint16_t currentPoint = contourStart;
			while(currentPoint <= contourEnd) {
				uint16_t nextPoint1 = currentPoint + 1;
				uint16_t nextPoint2 = currentPoint + 2;
				if(nextPoint1 > contourEnd) {
					nextPoint1 -= (contourEnd - contourStart + 1);
				}
				if(nextPoint2 > contourEnd) {
					nextPoint2 -= (contourEnd - contourStart + 1);
				}

				if(flags[currentPoint] & ON_CURVE_BIT) {
					if(flags[nextPoint1] & ON_CURVE_BIT) {
						// on, on (straight line)
						AddCurve(BezierCurve{ 
							points[currentPoint],
							(points[currentPoint] + points[nextPoint1]) / 2.f,
							points[nextPoint1]
						});

						currentPoint += 1;
					}
					else if(flags[nextPoint2] & ON_CURVE_BIT) {
						// on, off, on
						AddCurve(BezierCurve{ 
							points[currentPoint],
							points[nextPoint1],
							points[nextPoint2]
						});

						currentPoint += 2;
					}
					else {
						// on, off, off
						AddCurve(BezierCurve{ 
							points[currentPoint],
							points[nextPoint1],
							(points[nextPoint1] + points[nextPoint2]) / 2.0f
						});

						currentPoint += 1;
					}
				} else {
					if(flags[nextPoint1] & ON_CURVE_BIT) {
						// off, on ...
						currentPoint += 1;
					}
					else if(flags[nextPoint2] & ON_CURVE_BIT) {
						// off, off, on
						AddCurve(BezierCurve{ 
							(points[currentPoint] + points[nextPoint1]) / 2.0f,
							points[nextPoint1],
							points[nextPoint2]
						});

						currentPoint += 2;
					}
					else {
						// off, off, off
						AddCurve(BezierCurve{ 
							(points[currentPoint] + points[nextPoint1]) / 2.0f,
							points[nextPoint1],
							(points[nextPoint1] + points[nextPoint2]) / 2.0f
						});

						currentPoint += 1;
					}
				}
			}

			contourStart = contourEnd + 1;
		}
	}

#define TWO_BYTE_BIT 0x0001
#define ARGS_XY_VALUES_BIT 0x0002
#define SAME_SCALE_BIT 0x0008
#define MORE_COMPONENTS_BIT 0x0020
#define XY_SCALE_BIT 0x0040
#define MATRIX_BIT 0x0080
#define SCALE_OFFSET_BIT 0x0800

    void ReadCompositeGlyph(const Matrix2D* transform, Int2 offset, const AlignedRect* baseArea) {
		uint16_t flags = MORE_COMPONENTS_BIT;
		while(flags & MORE_COMPONENTS_BIT) {
			flags = fontFile.ReadUInt16();
			uint16_t glyphIndex = fontFile.ReadUInt16();

			Int2 newOffset;
			newOffset.x = (flags & TWO_BYTE_BIT) ? fontFile.ReadInt16() : fontFile.ReadByte();
			newOffset.y = (flags & TWO_BYTE_BIT) ? fontFile.ReadInt16() : fontFile.ReadByte();
			ASSERT(flags & ARGS_XY_VALUES_BIT); // point-to-point glyph composition is not supported

			Matrix2D newTransform = { {1, 0}, {0, 1} };
			if(flags & SAME_SCALE_BIT) {
				newTransform.left.x = fontFile.ReadF2Dot14();
				newTransform.right.y = newTransform.left.x;
			}
			else if(flags & XY_SCALE_BIT) {
				newTransform.left.x = fontFile.ReadF2Dot14();
				newTransform.right.y = fontFile.ReadF2Dot14();
			}
			else if(flags & MATRIX_BIT) {
				newTransform.left.x = fontFile.ReadF2Dot14();
				newTransform.left.y = fontFile.ReadF2Dot14();
				newTransform.right.x = fontFile.ReadF2Dot14();
				newTransform.right.y = fontFile.ReadF2Dot14();
			}

			// combine new transform with old transform
			newOffset = { newOffset.x + offset.x, newOffset.y + offset.y };
			newTransform = newTransform * (*transform);
			if(flags & SCALE_OFFSET_BIT) {
				newOffset.x = newTransform.left.x * newOffset.x + newTransform.right.x * newOffset.y;
				newOffset.y = newTransform.left.y * newOffset.x + newTransform.right.y * newOffset.y;
			}

			uint64_t savedPos = fontFile.readLocation;
			ReadGlyph(glyphIndex, nullptr, &newTransform, newOffset, baseArea);
			fontFile.readLocation = savedPos;
		}
	}
};

Font AssetContainer::LoadTTF(const GraphicsAPI* graphics, MemoryArena* arena, const char* fileName) {
	FontLoader loader = {};
	Font loaded = {};

	// parse file
	char path[128] = "assets\\";
	AddStrings(path, fileName, path);
	loader.fontFile = FileIO::LoadFile(path);
	loader.fontFile.littleEndian = false;
	ASSERT(loader.fontFile.bytes != nullptr);

	// determine number of data tables in this file
	loader.fontFile.readLocation += 4;
	uint16_t tableCount = loader.fontFile.ReadUInt16();
	loader.fontFile.readLocation += 6;

	// store the byte offsets of all data tables to support file navigation
	loader.tableStarts.Initialize(arena, tableCount);
	for(uint16_t i = 0; i < tableCount; i++) {
		uint32_t tag = loader.fontFile.ReadUInt32();
		loader.fontFile.readLocation += 4;
		loader.tableStarts.Set(tag, loader.fontFile.ReadUInt32());
		loader.fontFile.readLocation += 4;
	}
	loader.locaStart = loader.tableStarts.Get('loca');

	// determine number of glyphs and other limits
	loader.fontFile.readLocation = loader.tableStarts.Get('maxp') + 4;
	uint16_t numGlyphs = loader.fontFile.ReadUInt16();
	uint16_t maxPoints = loader.fontFile.ReadUInt16();
	uint16_t maxContours = loader.fontFile.ReadUInt16();

	// load the character map
	loader.fontFile.readLocation = loader.tableStarts.Get('cmap') + 2;
	uint16_t cmapSubtables = loader.fontFile.ReadUInt16();
	uint32_t cmapOffset = 0;

	for(uint16_t i = 0; i < cmapSubtables; i++) {
		// find a usable cmap table from the subtables
		uint16_t platformID = loader.fontFile.ReadUInt16();
		uint16_t platformSpecificID = loader.fontFile.ReadUInt16();
		uint32_t subtableOffset = loader.fontFile.ReadUInt32();

		// check for a unicode encoding
		if(platformID == 0 || platformID == 3 && (platformSpecificID == 1 || platformSpecificID == 10)) {
			cmapOffset = subtableOffset;
			break;
		}
	}

	ASSERT(cmapOffset > 0); // font file must contain unicode encoding

	loader.fontFile.readLocation = loader.tableStarts.Get('cmap') + cmapOffset;
	uint16_t format = loader.fontFile.ReadUInt16();
	ASSERT(format == 4 || format == 12); // font file must support one of these cmap formats

	loaded.charToGlyphIndex.Initialize(numGlyphs, [](uint16_t let) { return (uint32_t)let; });

	if(format == 4) {
		loader.fontFile.readLocation += 4;
		uint16_t numSegments = loader.fontFile.ReadUInt16() / 2;
		loader.fontFile.readLocation += 6;

		uint16_t* endCodes = (uint16_t*)arena->Reserve(numSegments * sizeof(uint16_t));
		for(uint16_t i = 0; i < numSegments; i++) {
			endCodes[i] = loader.fontFile.ReadUInt16();
		}

		loader.fontFile.readLocation += 2;
		uint16_t* startCodes = (uint16_t*)arena->Reserve(numSegments * sizeof(uint16_t));
		for(uint16_t i = 0; i < numSegments; i++) {
			startCodes[i] = loader.fontFile.ReadUInt16();
		}

		int16_t* idDeltas = (int16_t*)arena->Reserve(numSegments * sizeof(int16_t));
		for(uint16_t i = 0; i < numSegments; i++) {
			idDeltas[i] = loader.fontFile.ReadInt16();
		}

		uint16_t idRangeOffsetStart = loader.fontFile.readLocation;
		uint16_t* idRangeOffsets = (uint16_t*)arena->Reserve(numSegments * sizeof(uint16_t));
		for(uint16_t i = 0; i < numSegments; i++) {
			idRangeOffsets[i] = loader.fontFile.ReadUInt16();
		}

		for(uint16_t segment = 0; segment < numSegments - 1; segment++) { // last segment has no mappings
			for(uint16_t currCode = startCodes[segment]; currCode <= endCodes[segment]; currCode++) {
				uint16_t glyphIndex;
				if(idRangeOffsets[segment] == 0) {
					// calculate glyph index directly
					glyphIndex = (currCode + idDeltas[segment]) % 65536;
				} else {
					// look up glyph index from an array in the file
					uint16_t rangeOffsetLocation = idRangeOffsetStart + 2*segment + idRangeOffsets[segment];
					loader.fontFile.readLocation = 2 * (currCode - startCodes[segment]) + rangeOffsetLocation;
					glyphIndex = loader.fontFile.ReadUInt16();
					if(glyphIndex != 0) glyphIndex = (glyphIndex + idDeltas[segment]) % 65536;
				}

				loaded.charToGlyphIndex.Set(currCode, glyphIndex);
			}
		}
	}
	else if(format == 12) {
		loader.fontFile.readLocation += 10;
		uint32_t numGroups = loader.fontFile.ReadUInt32();
		for(uint32_t group = 0; group < numGroups; group++) {
			uint32_t startCharCode = loader.fontFile.ReadUInt32();
			uint32_t endCharCode = loader.fontFile.ReadUInt32();
			uint32_t startGlyphCode = loader.fontFile.ReadUInt32();

			uint32_t groupLength = endCharCode - startCharCode + 1;
			for(uint32_t i = 0; i < groupLength; i++) {
				loaded.charToGlyphIndex.Set(startCharCode + i, startGlyphCode + i);
			}
		}
	}
	loaded.charToGlyphIndex.Set(0xFFFF, 0);

	// load glyph advance widths
	loader.fontFile.readLocation = loader.tableStarts.Get('hhea') + 34;
	uint16_t numHMetrics = loader.fontFile.ReadUInt16();
	loader.fontFile.readLocation = loader.tableStarts.Get('hmtx');
	loader.leftSideBearings = (int16_t*)arena->Reserve(numGlyphs * sizeof(int16_t));
	loader.advanceWidths = (uint16_t*)arena->Reserve(numGlyphs * sizeof(uint16_t));

	for(uint16_t glyphIndex = 0; glyphIndex < numHMetrics; glyphIndex++) {
		loader.advanceWidths[glyphIndex] = loader.fontFile.ReadUInt16();
		loader.leftSideBearings[glyphIndex] = loader.fontFile.ReadInt16();
	}
	for(uint16_t glyphIndex = numHMetrics; glyphIndex < numGlyphs; glyphIndex++) {
		// the remaining glyphs have the same advance width
		loader.advanceWidths[glyphIndex] = loader.advanceWidths[numHMetrics - 1];
		loader.leftSideBearings[glyphIndex] = loader.fontFile.ReadInt16();
	}

	// load the glyph contour data
	loader.fontFile.readLocation = loader.tableStarts.Get('head') + 18;
	float unitsPerEm = loader.fontFile.ReadUInt16();
	loader.fontFile.readLocation += 30;
	loader.locFormat = loader.fontFile.ReadInt16();
	ASSERT(loader.locFormat == 0 || loader.locFormat == 1);

	FixedList<uint32_t> glyphStartIndices;
	glyphStartIndices.Initialize(numGlyphs + 1, arena); // maps glyph index to index of first curve for that glyph

	loader.contourEndIndices = (uint16_t*)arena->Reserve(maxContours * sizeof(uint16_t));
	loader.flags = (uint8_t*)arena->Reserve(maxPoints * sizeof(uint8_t));
	loader.points = (Vector2*)arena->Reserve(maxPoints * sizeof(Vector2));

	loader.curves = (BezierCurve*)arena->Reserve(0); // this arena may not store anything other curves after this line until the end of this function
	loader.curveArena = arena;

	loaded.glyphDimensions = new Vector2[numGlyphs];
	loaded.glyphBaselines = new float[numGlyphs];
	Matrix2D identity = {{ 1, 0 }, { 0, 1 }};
	for(uint16_t glyphIndex = 0; glyphIndex < numGlyphs; glyphIndex++) {
		glyphStartIndices.Add(loader.numCurves);
		AlignedRect glyphArea;
		loader.ReadGlyph(glyphIndex, &glyphArea, &identity, Int2{0, 0});
		loaded.glyphDimensions[glyphIndex] = glyphArea.Size() / unitsPerEm;
		loaded.glyphBaselines[glyphIndex] = -glyphArea.bottom / glyphArea.Height(); // assumes baseline at Y=0
	}
	glyphStartIndices.Add(loader.numCurves);

	// send curve data to the GPU
	loaded.glyphCurves = graphics->CreateArrayBuffer(ShaderDataType::Structured, loader.numCurves, sizeof(BezierCurve));
	loaded.firstCurveIndices = graphics->CreateArrayBuffer(ShaderDataType::UInt, glyphStartIndices.size);
	graphics->WriteBuffer(loader.curves, loader.numCurves * sizeof(BezierCurve), loaded.glyphCurves.buffer);
	graphics->WriteBuffer(glyphStartIndices.data, glyphStartIndices.size * sizeof(uint32_t), loaded.firstCurveIndices.buffer);

	// clean up
	arena->Clear();
	loader.fontFile.Release();
	return loaded;
}
#pragma endregion
#pragma endregion