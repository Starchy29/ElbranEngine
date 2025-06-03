#include "Font.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "FixedList.h"
#include <iostream>
#include <fstream>
#include <cassert>

void Font::Release() {
    app->graphics->ReleaseComputeTexture(&glyphAtlas);
}

struct Table {
    char tag[4];
    uint32_t startByte;
};

struct BezierCurve {
    Vector2 start;
    Vector2 end;
    Vector2 control;
};

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
Font Font::Load(std::wstring file) {
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

    // determine number of glyphs
    fileReader.seekg(FindStartByte(tables, "maxp"));
    fileReader.ignore(4);
    uint16_t numGlyphs = ReadUInt16(fileReader);

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
    FixedList<BezierCurve> curves;
    FixedList<unsigned int> glyphStartIndices; // maps glyph index to index of first curve for that glyph

    fileReader.close();
    
    // generate glyph atlas
    //loaded.glyphAtlas = app->graphics->CreateComputeTexture(100, 100);
    //app->graphics->SetComputeTexture(&loaded.glyphAtlas, 0);
    //app->graphics->SetArray(ShaderStage::Compute, nullptr, 0);

    // set compute shader data
    // run compute shader

    delete[] tables;
    return loaded;
}
