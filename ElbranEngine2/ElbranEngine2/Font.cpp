#include "Font.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "AssetContainer.h"
#include "FixedList.h"
#include "ShaderConstants.h"
#include <iostream>
#include <fstream>
#include <cassert>

#define ON_CURVE_BIT 0b00000001
#define UNSIGNED_X_BIT 0b00000010
#define UNSIGNED_Y_BIT 0b00000100
#define REPEAT_BIT 0b00001000
#define POSITIVE_X_BIT 0b00010000
#define POSITIVE_Y_BIT 0b00100000
#define DUPLICATE_X_BIT 0b00010000
#define DUPLICATE_Y_BIT 0b00100000

struct Table {
    char tag[4];
    uint32_t startByte;
};

// must match GlyphAtlasDrawCS.hlsl
struct BezierCurve {
    Vector2 start;
    Vector2 end;
    Vector2 control;
};

void Font::Release() {
    app->graphics->ReleaseComputeTexture(&glyphAtlas);
}

int16_t ReadInt16(std::ifstream &fileReader) {
    int16_t result;
    fileReader.read((char*)&result, 2);
    result = (result >> 8 | result << 8); // swap bytes because file is big-endian
    return result;
}

uint16_t ReadUInt16(std::ifstream &fileReader) {
    uint16_t result;
    fileReader.read((char*)&result, 2);
    result = (result >> 8 | result << 8); // swap bytes because file is big-endian
    return result;
}

uint32_t ReadUInt32(std::ifstream &fileReader) {
    uint32_t result;
    fileReader.read((char*)&result, 4);
    result = (result >> 24 | result << 24 | (result >> 8 & 0x0000FF00) | (result << 8 & 0x00FF0000)); // swap bytes because file is big-endian
    return result;
}

uint32_t FindStartByte(Table* tables, const char tag[4]) {
    int index = 0;
    while(!(tables[index].tag[0] == tag[0] && tables[index].tag[1] == tag[1] && tables[index].tag[2] == tag[2] && tables[index].tag[3] == tag[3])) {
        index++;
    }
    return tables[index].startByte;
}

// loads a .ttf font file. Note: .ttf files are big-endian
// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
Font Font::Load(std::wstring file, const AssetContainer* assets) {
    Font loaded = {};

    // parse file
    std::ifstream fileReader;
    fileReader.open(app->filePath + L"Assets\\" + file, std::ifstream::binary);
    assert(!fileReader.fail() && "failed to read font file");

    // determine number of data tables in this file
    fileReader.ignore(4);
    uint16_t tableCount = ReadUInt16(fileReader);
    fileReader.ignore(6);

    // store the byte offsets of all data tables to support file navigation
    Table* tables = new Table[tableCount];
    for(uint16_t i = 0; i < tableCount; i++) {
        fileReader.read(tables[i].tag, 4);
        fileReader.ignore(4);
        tables[i].startByte = ReadUInt32(fileReader);
        fileReader.ignore(4);
    }

    // determine number of glyphs and other limits
    fileReader.seekg(FindStartByte(tables, "maxp"));
    fileReader.ignore(4); // skip version
    uint16_t numGlyphs = ReadUInt16(fileReader);
    uint16_t maxPoints = ReadUInt16(fileReader);
    uint16_t maxContours = ReadUInt16(fileReader);

    // load the character map
    fileReader.seekg(FindStartByte(tables, "cmap"));
    fileReader.ignore(2);
    uint16_t cmapSubtables = ReadUInt16(fileReader);
    uint32_t cmapOffset = 0;

    for(uint16_t i = 0; i < cmapSubtables; i++) {
        // find a usable cmap table from the subtables
        uint16_t platformID = ReadUInt16(fileReader);
        uint16_t platformSpecificID = ReadUInt16(fileReader);
        uint32_t subtableOffset = ReadUInt32(fileReader);

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

    fileReader.seekg(FindStartByte(tables, "cmap") + cmapOffset);
    uint16_t format = ReadUInt16(fileReader);
    assert((format == 4 || format == 12) && "font file uses an unsupported cmap format");

    if(format == 4) {
        fileReader.ignore(4);
        uint16_t numSegments = ReadUInt16(fileReader) / 2;
        fileReader.ignore(6);

        uint16_t* endCodes = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            endCodes[i] = ReadUInt16(fileReader);
        }

        fileReader.ignore(2);
        uint16_t* startCodes = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            startCodes[i] = ReadUInt16(fileReader);
        }

        uint16_t* idDeltas = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            idDeltas[i] = ReadUInt16(fileReader);
        }

        std::streampos idRangeOffsetStart = fileReader.tellg();
        uint16_t* idRangeOffsets = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            idRangeOffsets[i] = ReadUInt16(fileReader);
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
                    glyphIndex = ReadUInt16(fileReader);
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
        uint32_t numGroups = ReadUInt32(fileReader);
        for(uint32_t group = 0; group < numGroups; group++) {
            uint32_t startCharCode = ReadUInt32(fileReader);
            uint32_t endCharCode = ReadUInt32(fileReader);
            uint32_t startGlyphCode = ReadUInt32(fileReader);

            uint32_t groupLength = endCharCode - startCharCode + 1;
            for(uint32_t i = 0; i < groupLength; i++) {
                loaded.charToGlyphIndex[startCharCode + i] = startGlyphCode + i;
            }
        }
    }

    // load the glyph data
    fileReader.seekg(FindStartByte(tables, "glyf"));
    FixedList<BezierCurve> curves(numGlyphs * maxPoints / 2);
    FixedList<uint32_t> glyphStartIndices(numGlyphs + 1); // maps glyph index to index of first curve for that glyph

    uint16_t* contourEndIndices = new uint16_t[maxContours];
    uint8_t* flags = new uint8_t[maxPoints];
    Vector2* points = new Vector2[maxPoints];

    for(uint16_t glyph = 0; glyph < 1; glyph++) {
        int16_t contourCount = ReadInt16(fileReader);
        float xMin = (float)ReadInt16(fileReader);
        float yMin = (float)ReadInt16(fileReader);
        float xMax = (float)ReadInt16(fileReader);
        float yMax = (float)ReadInt16(fileReader);
        float xWidth = xMax - xMin;
        float yHeight = yMax - yMin;

        glyphStartIndices.Add(curves.GetSize());
        if(contourCount >= 0) {
            // simple glyph
            for(int16_t i = 0; i < contourCount; i++) {
                contourEndIndices[i] = ReadUInt16(fileReader);
            }
            uint8_t numPoints = contourEndIndices[contourCount - 1] + 1;
            uint16_t instructionLength = ReadUInt16(fileReader);
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
                    currentX += ReadInt16(fileReader);
                }
                points[pointIndex].x = (currentX - xMin) / xWidth; // scale to 0-1 range
            }

            // read y coords
            int16_t currentY = 0;
            for(uint8_t pointIndex = 0; pointIndex < numPoints; pointIndex++) {
                if(flags[pointIndex] & UNSIGNED_Y_BIT) {
                    uint8_t byte = fileReader.get();
                    currentY += (flags[pointIndex] & POSITIVE_Y_BIT ? byte : -byte);
                } 
                else if(!(flags[pointIndex] & DUPLICATE_Y_BIT)) {
                    currentY += ReadInt16(fileReader);
                }
                points[pointIndex].y = (currentY - yMin) / yHeight; // scale to 0-1 range
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

            // DEBUG TEST
            for(int i = 0; i < numPoints; i++) {
                Vector2 checker = points[i];
                float stopper = 2.0f;
            }
            // END TEST
        } else {
            // compound glyph
        }
    }

    glyphStartIndices.Add(curves.GetSize());

    delete[] contourEndIndices;
    delete[] flags;
    delete[] points;

    delete[] tables;
    fileReader.close();

    // generate glyph atlas

    // DEBUG TEST
    //curves.Clear();
    //curves.Add(BezierCurve { Vector2(0.5, 1), Vector2(0.5, 0), Vector2(0.0, 0.5) } );
    //glyphStartIndices.Clear();
    //glyphStartIndices.Add(0);
    //glyphStartIndices.Add(1);
    // END DEBUG

    ArrayBuffer curveBuffer = app->graphics->CreateArrayBuffer(ShaderDataType::Structured, curves.GetSize(), sizeof(BezierCurve));
    ArrayBuffer glyphBuffer = app->graphics->CreateArrayBuffer(ShaderDataType::UInt, glyphStartIndices.GetSize());
    app->graphics->WriteBuffer(curves.GetArray(), curves.GetSize() * sizeof(BezierCurve), curveBuffer.buffer);
    app->graphics->WriteBuffer(glyphStartIndices.GetArray(), glyphStartIndices.GetSize() * sizeof(unsigned int), glyphBuffer.buffer);
    app->graphics->SetArray(ShaderStage::Compute, &curveBuffer, 0);
    app->graphics->SetArray(ShaderStage::Compute, &glyphBuffer, 1);

    GlyphAtlasDrawCSConstants constants;
    constants.numGlyphs = 1; // numGlyphs
    constants.glyphSize = 500;
    constants.rowsCols = 1;
    //while(numGlyphs > constants.rowsCols * constants.rowsCols) {
        // find atlas dimensions with enough spots for all glyphs
        //constants.rowsCols++;
    //}

    unsigned int atlasSize = constants.glyphSize * constants.rowsCols;
    loaded.glyphAtlas = app->graphics->CreateComputeTexture(atlasSize, atlasSize);
    app->graphics->SetComputeTexture(&loaded.glyphAtlas, 0);

    app->graphics->WriteBuffer(&constants, sizeof(constants), assets->glyphAtlasDrawCS.constants.data);
    app->graphics->SetConstants(ShaderStage::Compute, &assets->glyphAtlasDrawCS.constants, 0);
    app->graphics->RunComputeShader(&assets->glyphAtlasDrawCS, constants.rowsCols, atlasSize);
    app->graphics->SetComputeTexture(nullptr, 0); // unbind the texture so it can be used as input later

    curves.Release();
    glyphStartIndices.Release();
    app->graphics->ReleaseArrayBuffer(&curveBuffer);
    app->graphics->ReleaseArrayBuffer(&glyphBuffer);
    return loaded;
}
