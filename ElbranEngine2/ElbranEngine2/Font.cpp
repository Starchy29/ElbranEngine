#include "Font.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "FixedList.h"
#include "ShaderConstants.h"
#include <iostream>
#include <fstream>
#include <cassert>

// must match TextRasterizePS.hlsl
struct BezierCurve {
    Vector2 start;
    Vector2 end;
    Vector2 control;
};

struct Table {
    char tag[4];
    uint32_t startByte;
};

struct Matrix2D {
    Vector2 left;
    Vector2 right;
};

class FontLoader {
public:
    std::ifstream fileReader;
    FixedList<BezierCurve> curves;
    Table* tables;
    uint32_t locaStart;
    int16_t locFormat;
    uint16_t* contourEndIndices;
    uint8_t* flags;
    Vector2* points;
    int16_t* leftSideBearings;
    uint16_t* advanceWidths;

    Font LoadFile(std::wstring file);

    uint32_t FindTableStart(const char tag[4]);
    uint16_t ReadUInt16();
    int16_t ReadInt16();
    uint32_t ReadUInt32();
    float ReadF2Dot14();
    void ReadGlyph(uint16_t glyphIndex, AlignedRect* outArea);
    void ReadSimpleGlyph(int16_t contourCount, AlignedRect glyphArea);
    void ReadCompositeGlyph();
};

void Font::Release() {
    app->graphics->ReleaseArrayBuffer(&glyphCurves);
    app->graphics->ReleaseArrayBuffer(&firstCurveIndices);
    delete[] glyphBaselines;
    delete[] glyphDimensions;
}

Font Font::Load(std::wstring file) {
    FontLoader loader = {};
    return loader.LoadFile(file);
}

#define ON_CURVE_BIT 0b00000001
#define UNSIGNED_X_BIT 0b00000010
#define UNSIGNED_Y_BIT 0b00000100
#define REPEAT_BIT 0b00001000
#define POSITIVE_X_BIT 0b00010000
#define POSITIVE_Y_BIT 0b00100000
#define DUPLICATE_X_BIT 0b00010000
#define DUPLICATE_Y_BIT 0b00100000

// loads a .ttf font file. Note: .ttf files are big-endian
// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
Font FontLoader::LoadFile(std::wstring file) {
    Font loaded = {};

    // parse file
    fileReader.open(app->filePath + L"Assets\\" + file, std::ifstream::binary);
    assert(!fileReader.fail() && "failed to read font file");

    // determine number of data tables in this file
    fileReader.ignore(4);
    uint16_t tableCount = ReadUInt16();
    fileReader.ignore(6);

    // store the byte offsets of all data tables to support file navigation
    tables = new Table[tableCount];
    for(uint16_t i = 0; i < tableCount; i++) {
        fileReader.read(tables[i].tag, 4);
        fileReader.ignore(4);
        tables[i].startByte = ReadUInt32();
        fileReader.ignore(4);
    }

    // determine number of glyphs and other limits
    fileReader.seekg(FindTableStart("maxp"));
    fileReader.ignore(4);
    uint16_t numGlyphs = ReadUInt16();
    uint16_t maxPoints = ReadUInt16();
    uint16_t maxContours = ReadUInt16();

    // load the character map
    fileReader.seekg(FindTableStart("cmap"));
    fileReader.ignore(2);
    uint16_t cmapSubtables = ReadUInt16();
    uint32_t cmapOffset = 0;

    for(uint16_t i = 0; i < cmapSubtables; i++) {
        // find a usable cmap table from the subtables
        uint16_t platformID = ReadUInt16();
        uint16_t platformSpecificID = ReadUInt16();
        uint32_t subtableOffset = ReadUInt32();

        if(i == 2) {
            cmapOffset = subtableOffset;
            break;
        }

        // check for a unicode encoding
        if(platformID == 0 || platformID == 3 && (platformSpecificID == 1 || platformSpecificID == 10)) {
            cmapOffset = subtableOffset;
            break;
        }
    }

    assert(cmapOffset > 0 && "font file contains no unicode encoding");

    fileReader.seekg(FindTableStart("cmap") + cmapOffset);
    uint16_t format = ReadUInt16();
    assert((format == 4 || format == 12) && "font file uses an unsupported cmap format");

    if(format == 4) {
        fileReader.ignore(4);
        uint16_t numSegments = ReadUInt16() / 2;
        fileReader.ignore(6);

        uint16_t* endCodes = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            endCodes[i] = ReadUInt16();
        }

        fileReader.ignore(2);
        uint16_t* startCodes = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            startCodes[i] = ReadUInt16();
        }

        uint16_t* idDeltas = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            idDeltas[i] = ReadUInt16();
        }

        std::streampos idRangeOffsetStart = fileReader.tellg();
        uint16_t* idRangeOffsets = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            idRangeOffsets[i] = ReadUInt16();
        }

        for(uint16_t segment = 0; segment < numSegments - 1; segment++) { // last segment has no mappings
            for(uint16_t currCode = startCodes[segment]; currCode <= endCodes[segment]; currCode++) {
                uint16_t startTest = startCodes[segment];
                uint16_t endTest = endCodes[segment];

                uint16_t glyphIndex;
                if(idRangeOffsets[segment] == 0) {
                    // calculate glyph index directly
                    glyphIndex = (currCode + idDeltas[segment]) % 65536;
                } else {
                    // look up glyph index from an array in the file
                    fileReader.seekg((uint16_t)idRangeOffsetStart + 2 * segment + idRangeOffsets[segment] / 2 + (currCode - startCodes[segment]));
                    glyphIndex = ReadUInt16();
                    if(glyphIndex != 0) glyphIndex = (currCode + idDeltas[segment]) % 65536;
                }

                loaded.charToGlyphIndex[(char)currCode] = glyphIndex;
            }
        }
        loaded.charToGlyphIndex[0xFFFF] = 0;

        delete[] endCodes;
        delete[] startCodes;
        delete[] idDeltas;
        delete[] idRangeOffsets;
    }
    else if(format == 12) {
        fileReader.ignore(10);
        uint32_t numGroups = ReadUInt32();
        for(uint32_t group = 0; group < numGroups; group++) {
            uint32_t startCharCode = ReadUInt32();
            uint32_t endCharCode = ReadUInt32();
            uint32_t startGlyphCode = ReadUInt32();

            uint32_t groupLength = endCharCode - startCharCode + 1;
            for(uint32_t i = 0; i < groupLength; i++) {
                loaded.charToGlyphIndex[startCharCode + i] = startGlyphCode + i;
            }
        }
    }

    // load glyph advance widths
    fileReader.seekg(FindTableStart("hhea") + 34);
    uint16_t numHMetrics = ReadUInt16();
    fileReader.seekg(FindTableStart("hmtx"));
    leftSideBearings = new int16_t[numGlyphs];
    advanceWidths = new uint16_t[numGlyphs];
    
    for(uint16_t glyphIndex = 0; glyphIndex < numHMetrics; glyphIndex++) {
        advanceWidths[glyphIndex] = ReadUInt16();
        leftSideBearings[glyphIndex] = ReadInt16();
    }
    for(uint16_t glyphIndex = numHMetrics; glyphIndex < numGlyphs; glyphIndex++) {
        // the remaining glyphs have the same advance width
        advanceWidths[glyphIndex] = advanceWidths[numHMetrics - 1];
        leftSideBearings[glyphIndex] = ReadInt16();
    }

    // load the glyph contour data
    fileReader.seekg(FindTableStart("head") + 18);
    float unitsPerEm = ReadUInt16();
    fileReader.ignore(30);
    locFormat = ReadInt16();
    assert((locFormat == 0 || locFormat == 1) && "font file has an invalid loc format");
    locaStart = FindTableStart("loca");

    curves = FixedList<BezierCurve>(numGlyphs * maxPoints / 2);
    FixedList<uint32_t> glyphStartIndices(numGlyphs + 1); // maps glyph index to index of first curve for that glyph

    contourEndIndices = new uint16_t[maxContours];
    flags = new uint8_t[maxPoints];
    points = new Vector2[maxPoints];

    loaded.glyphDimensions = new Vector2[numGlyphs];
    loaded.glyphBaselines = new float[numGlyphs];
    for(uint16_t glyphIndex = 0; glyphIndex < numGlyphs; glyphIndex++) {
        glyphStartIndices.Add(curves.GetSize());
        AlignedRect glyphArea;
        ReadGlyph(glyphIndex, &glyphArea);
        loaded.glyphDimensions[glyphIndex] = glyphArea.Size() / unitsPerEm;
        loaded.glyphBaselines[glyphIndex] = -glyphArea.bottom / glyphArea.Height(); // assumes baseline at Y=0
    }
    glyphStartIndices.Add(curves.GetSize());
    fileReader.close();

    // send curve data to the GPU
    loaded.glyphCurves = app->graphics->CreateArrayBuffer(ShaderDataType::Structured, curves.GetSize(), sizeof(BezierCurve));
    loaded.firstCurveIndices = app->graphics->CreateArrayBuffer(ShaderDataType::UInt, glyphStartIndices.GetSize());
    app->graphics->WriteBuffer(curves.GetArray(), curves.GetSize() * sizeof(BezierCurve), loaded.glyphCurves.buffer);
    app->graphics->WriteBuffer(glyphStartIndices.GetArray(), glyphStartIndices.GetSize() * sizeof(unsigned int), loaded.firstCurveIndices.buffer);
    curves.Release();
    glyphStartIndices.Release();

    // clean up
    delete[] contourEndIndices;
    delete[] flags;
    delete[] points;
    delete[] tables;
    delete[] advanceWidths;
    delete[] leftSideBearings;

    return loaded;
}

void FontLoader::ReadGlyph(uint16_t glyphIndex, AlignedRect* outArea) {
    fileReader.seekg(locaStart + glyphIndex * (locFormat == 0 ? 2 : 4));
    uint32_t glyphOffset = (locFormat == 0 ? ReadUInt16() : ReadUInt32());
    fileReader.seekg(FindTableStart("glyf") + glyphOffset);

    int16_t contourCount = ReadInt16();
    AlignedRect glyphArea;
    glyphArea.left = (float)(ReadInt16() - leftSideBearings[glyphIndex]);
    glyphArea.bottom = (float)ReadInt16();
    fileReader.ignore(2); // skip xMax
    glyphArea.right = glyphArea.left + advanceWidths[glyphIndex];
    glyphArea.top = (float)ReadInt16();

    if(outArea) {
        *outArea = glyphArea;
    }

    if(contourCount >= 0) {
        ReadSimpleGlyph(contourCount, glyphArea);
    } else {
        ReadCompositeGlyph();
    }
}

void FontLoader::ReadSimpleGlyph(int16_t contourCount, AlignedRect glyphArea) {
    Vector2 glyphSize = glyphArea.Size();

    for(int16_t i = 0; i < contourCount; i++) {
        contourEndIndices[i] = ReadUInt16();
    }
    uint8_t numPoints = contourEndIndices[contourCount - 1] + 1;
    uint16_t instructionLength = ReadUInt16();
    fileReader.ignore(instructionLength);

    // read flags
    for(uint8_t pointIndex = 0; pointIndex < numPoints; pointIndex++) {
        flags[pointIndex] = fileReader.get();
        if(flags[pointIndex] & REPEAT_BIT) {
            uint8_t repetitions = fileReader.get();
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
            uint8_t byte = fileReader.get();
            currentX += (flags[pointIndex] & POSITIVE_X_BIT ? byte : -byte);
        } 
        else if(!(flags[pointIndex] & DUPLICATE_X_BIT)) {
            currentX += ReadInt16();
        }
        points[pointIndex].x = (currentX - glyphArea.left) / glyphSize.x; // scale to 0-1 range
    }

    // read y coords
    int16_t currentY = 0;
    for(uint8_t pointIndex = 0; pointIndex < numPoints; pointIndex++) {
        if(flags[pointIndex] & UNSIGNED_Y_BIT) {
            uint8_t byte = fileReader.get();
            currentY += (flags[pointIndex] & POSITIVE_Y_BIT ? byte : -byte);
        } 
        else if(!(flags[pointIndex] & DUPLICATE_Y_BIT)) {
            currentY += ReadInt16();
        }
        points[pointIndex].y = (currentY - glyphArea.bottom) / glyphSize.y; // scale to 0-1 range
    }

    // convert contours to bezier curves
    uint16_t contourStart = 0;
    uint16_t contourEnd;
    for(int16_t contour = 0; contour < contourCount; contour++) {
        contourEnd = contourEndIndices[contour];
        uint16_t currentPoint = contourStart;
        while(!(flags[currentPoint] & ON_CURVE_BIT)) {
            // always start with an on-curve point
            currentPoint++;
        }

        while(currentPoint <= contourEnd) {
            uint16_t nextPoint1 = currentPoint + 1;
            uint16_t nextPoint2 = currentPoint + 2;
            uint16_t nextPoint3 = currentPoint + 3;
            if(nextPoint1 > contourEnd) {
                nextPoint1 -= (contourEnd - contourStart + 1);
            }
            if(nextPoint2 > contourEnd) {
                nextPoint2 -= (contourEnd - contourStart + 1);
            }
            if(nextPoint3 > contourEnd) {
                nextPoint3 -= (contourEnd - contourStart + 1);
            }

            if(flags[nextPoint1] & ON_CURVE_BIT) {
                // straight line
                curves.Add(BezierCurve{ 
                    points[currentPoint],
                    points[nextPoint1],
                    (points[currentPoint] + points[nextPoint1]) / 2.f
                });

                currentPoint += 1;
            }
            else if(flags[nextPoint2] & ON_CURVE_BIT) {
                // bezier curve
                curves.Add(BezierCurve{ 
                    points[currentPoint], 
                    points[nextPoint2], 
                    points[nextPoint1] 
                });

                currentPoint += 2;
            }
            else {
                // two bezier curves joined by an implied on-curve point between the off-curve points
                Vector2 impliedMidPoint = (points[nextPoint1] + points[nextPoint2]) / 2.0f;
                curves.Add(BezierCurve{ 
                    points[currentPoint], 
                    impliedMidPoint, 
                    points[nextPoint1] 
                });
                curves.Add(BezierCurve{ 
                    impliedMidPoint, 
                    points[nextPoint3], 
                    points[nextPoint2] 
                });

                currentPoint += 3;
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

void FontLoader::ReadCompositeGlyph() {
    return;
    uint16_t flags = MORE_COMPONENTS_BIT;
    while(flags & MORE_COMPONENTS_BIT) {
        flags = ReadUInt16();
        uint16_t glyphIndex = ReadUInt16();

        int16_t offsetX;
        int16_t offsetY;
        if(flags & ARGS_XY_VALUES_BIT) {
            offsetX = (flags & TWO_BYTE_BIT) ? ReadInt16() : fileReader.get();
            offsetY = (flags & TWO_BYTE_BIT) ? ReadInt16() : fileReader.get();
        }

        Matrix2D transform = {};
        if(flags & SAME_SCALE_BIT) {
            transform.left.x = ReadF2Dot14();
            transform.right.y = transform.left.x;
        }
        else if(flags & XY_SCALE_BIT) {
            transform.left.x = ReadF2Dot14();
            transform.right.y = ReadF2Dot14();
        }
        else if(flags & MATRIX_BIT) {
            transform.left.x = ReadF2Dot14();
            transform.left.y = ReadF2Dot14();
            transform.right.x = ReadF2Dot14();
            transform.right.y = ReadF2Dot14();
        }

        ReadGlyph(glyphIndex, nullptr);
    }
}

uint32_t FontLoader::FindTableStart(const char tag[4]) {
    int index = 0;
    while(!(tables[index].tag[0] == tag[0] && tables[index].tag[1] == tag[1] && tables[index].tag[2] == tag[2] && tables[index].tag[3] == tag[3])) {
        index++;
    }
    return tables[index].startByte;
}

uint16_t FontLoader::ReadUInt16() {
    uint16_t result;
    fileReader.read((char*)&result, 2);
    result = (result >> 8 | result << 8); // swap bytes because file is big-endian
    return result;
}

int16_t FontLoader::ReadInt16() {
    return (int16_t)ReadUInt16();
}

uint32_t FontLoader::ReadUInt32() {
    uint32_t result;
    fileReader.read((char*)&result, 4);
    result = (result >> 24 | result << 24 | (result >> 8 & 0x0000FF00) | (result << 8 & 0x00FF0000)); // swap bytes because file is big-endian
    return result;
}

float FontLoader::ReadF2Dot14() {
    return 0.f;
}