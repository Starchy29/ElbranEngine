#include "Font.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "FixedList.h"
#include "ShaderConstants.h"
#include "FixedMap.h"

// must match TextRasterizePS.hlsl
struct BezierCurve {
    Vector2 start;
    Vector2 control;
    Vector2 end;
};

struct Matrix2D {
    Vector2 left;
    Vector2 right;
};

struct Tag {
    char chars[4];
    Tag(){}
    Tag(const Tag& original) {
        chars[0] = original.chars[0];
        chars[1] = original.chars[1];
        chars[2] = original.chars[2];
        chars[3] = original.chars[3];
    }
    Tag(const char literal[5]) {
        chars[0] = literal[0];
        chars[1] = literal[1];
        chars[2] = literal[2];
        chars[3] = literal[3];
    }
    
    static uint32_t Hash(Tag tag) {
        return tag.chars[0] + tag.chars[1] + tag.chars[2] + tag.chars[3];
    }
};
bool operator==(const Tag& left, const Tag& right) {
    return left.chars[0] == right.chars[0] && left.chars[1] == right.chars[1] && left.chars[2] == right.chars[2] && left.chars[3] == right.chars[3];
}

Matrix2D operator*(const Matrix2D& left, const Matrix2D& right) {
    return {
        { left.left.x * right.left.x + left.right.x * right.left.y, left.left.y * right.left.x + left.right.y * right.left.y },
        { left.left.x * right.right.x + left.right.x * right.right.y, left.left.y * right.right.x + left.right.y * right.right.y }
    };
}

class FontLoader {
public:
    LoadedFile fontFile;
    DynamicFixedList<BezierCurve> curves;
    DynamicFixedMap<Tag, uint32_t> tableStarts;
    uint32_t locaStart;
    int16_t locFormat;
    uint16_t* contourEndIndices;
    uint8_t* flags;
    Vector2* points;
    int16_t* leftSideBearings;
    uint16_t* advanceWidths;

    Font LoadFile(std::wstring file);

    void ReadGlyph(uint16_t glyphIndex, AlignedRect* outArea, const Matrix2D* transform, Int2 offset, const AlignedRect* baseArea = nullptr);
    void ReadSimpleGlyph(int16_t contourCount, AlignedRect glyphArea, const Matrix2D* transform, Int2 offset);
    void ReadCompositeGlyph(const Matrix2D* transform, Int2 offset, const AlignedRect* baseArea);
};

void Font::Release() {
    app.graphics.ReleaseArrayBuffer(&glyphCurves);
    app.graphics.ReleaseArrayBuffer(&firstCurveIndices);
    delete[] glyphBaselines;
    delete[] glyphDimensions;
    charToGlyphIndex.Release();
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

// loads a .ttf font file
// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
Font FontLoader::LoadFile(std::wstring file) {
    Font loaded = {};

    // parse file
    fontFile = app.LoadFile(L"assets\\" + file);
    fontFile.littleEndian = false;
    ASSERT(fontFile.bytes != nullptr);

    // determine number of data tables in this file
    fontFile.readLocation += 4;
    uint16_t tableCount = fontFile.ReadUInt16();
    fontFile.readLocation += 6;

    // store the byte offsets of all data tables to support file navigation
    tableStarts.Initialize(tableCount, Tag::Hash);
    Tag newTag;
    for(uint16_t i = 0; i < tableCount; i++) {
        fontFile.ReadBytes(4, (uint8_t*)&newTag.chars);
        fontFile.readLocation += 4;
        tableStarts.Set(newTag, fontFile.ReadUInt32());
        fontFile.readLocation += 4;
    }
    locaStart = tableStarts.Get(Tag{"loca"});

    // determine number of glyphs and other limits
    fontFile.readLocation = tableStarts.Get(Tag{"maxp"}) + 4;
    uint16_t numGlyphs = fontFile.ReadUInt16();
    uint16_t maxPoints = fontFile.ReadUInt16();
    uint16_t maxContours = fontFile.ReadUInt16();

    // load the character map
    fontFile.readLocation = tableStarts.Get(Tag{"cmap"}) + 2;
    uint16_t cmapSubtables = fontFile.ReadUInt16();
    uint32_t cmapOffset = 0;

    for(uint16_t i = 0; i < cmapSubtables; i++) {
        // find a usable cmap table from the subtables
        uint16_t platformID = fontFile.ReadUInt16();
        uint16_t platformSpecificID = fontFile.ReadUInt16();
        uint32_t subtableOffset = fontFile.ReadUInt32();

        // check for a unicode encoding
        if(platformID == 0 || platformID == 3 && (platformSpecificID == 1 || platformSpecificID == 10)) {
            cmapOffset = subtableOffset;
            break;
        }
    }

    ASSERT(cmapOffset > 0); // font file must contain unicode encoding

    fontFile.readLocation = tableStarts.Get(Tag{"cmap"}) + cmapOffset;
    uint16_t format = fontFile.ReadUInt16();
    ASSERT(format == 4 || format == 12); // font file must support one of these cmap formats

    loaded.charToGlyphIndex.Initialize(numGlyphs, [](uint16_t let) { return (uint32_t)let; });

    if(format == 4) {
        fontFile.readLocation += 4;
        uint16_t numSegments = fontFile.ReadUInt16() / 2;
        fontFile.readLocation += 6;

        uint16_t* endCodes = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            endCodes[i] = fontFile.ReadUInt16();
        }

        fontFile.readLocation += 2;
        uint16_t* startCodes = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            startCodes[i] = fontFile.ReadUInt16();
        }

        int16_t* idDeltas = new int16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            idDeltas[i] = fontFile.ReadInt16();
        }

        uint16_t idRangeOffsetStart = fontFile.readLocation;
        uint16_t* idRangeOffsets = new uint16_t[numSegments];
        for(uint16_t i = 0; i < numSegments; i++) {
            idRangeOffsets[i] = fontFile.ReadUInt16();
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
                    fontFile.readLocation = 2 * (currCode - startCodes[segment]) + rangeOffsetLocation;
                    glyphIndex = fontFile.ReadUInt16();
                    if(glyphIndex != 0) glyphIndex = (glyphIndex + idDeltas[segment]) % 65536;
                }

                loaded.charToGlyphIndex.Set(currCode, glyphIndex);
            }
        }

        delete[] endCodes;
        delete[] startCodes;
        delete[] idDeltas;
        delete[] idRangeOffsets;
    }
    else if(format == 12) {
        fontFile.readLocation += 10;
        uint32_t numGroups = fontFile.ReadUInt32();
        for(uint32_t group = 0; group < numGroups; group++) {
            uint32_t startCharCode = fontFile.ReadUInt32();
            uint32_t endCharCode = fontFile.ReadUInt32();
            uint32_t startGlyphCode = fontFile.ReadUInt32();

            uint32_t groupLength = endCharCode - startCharCode + 1;
            for(uint32_t i = 0; i < groupLength; i++) {
                loaded.charToGlyphIndex.Set(startCharCode + i, startGlyphCode + i);
            }
        }
    }
    loaded.charToGlyphIndex.Set(0xFFFF, 0);

    // load glyph advance widths
    fontFile.readLocation = tableStarts.Get(Tag{"hhea"}) + 34;
    uint16_t numHMetrics = fontFile.ReadUInt16();
    fontFile.readLocation = tableStarts.Get(Tag{"hmtx"});
    leftSideBearings = new int16_t[numGlyphs];
    advanceWidths = new uint16_t[numGlyphs];
    
    for(uint16_t glyphIndex = 0; glyphIndex < numHMetrics; glyphIndex++) {
        advanceWidths[glyphIndex] = fontFile.ReadUInt16();
        leftSideBearings[glyphIndex] = fontFile.ReadInt16();
    }
    for(uint16_t glyphIndex = numHMetrics; glyphIndex < numGlyphs; glyphIndex++) {
        // the remaining glyphs have the same advance width
        advanceWidths[glyphIndex] = advanceWidths[numHMetrics - 1];
        leftSideBearings[glyphIndex] = fontFile.ReadInt16();
    }

    // load the glyph contour data
    fontFile.readLocation = tableStarts.Get(Tag{"head"}) + 18;
    float unitsPerEm = fontFile.ReadUInt16();
    fontFile.readLocation += 30;
    locFormat = fontFile.ReadInt16();
    ASSERT(locFormat == 0 || locFormat == 1);

    curves.Allocate(numGlyphs * maxPoints);
    DynamicFixedList<uint32_t> glyphStartIndices;
    glyphStartIndices.Allocate(numGlyphs + 1); // maps glyph index to index of first curve for that glyph

    contourEndIndices = new uint16_t[maxContours];
    flags = new uint8_t[maxPoints];
    points = new Vector2[maxPoints];

    loaded.glyphDimensions = new Vector2[numGlyphs];
    loaded.glyphBaselines = new float[numGlyphs];
    Matrix2D identity = {{ 1, 0 }, { 0, 1 }};
    for(uint16_t glyphIndex = 0; glyphIndex < numGlyphs; glyphIndex++) {
        glyphStartIndices.Add(curves.Size());
        AlignedRect glyphArea;
        ReadGlyph(glyphIndex, &glyphArea, &identity, Int2{0, 0});
        loaded.glyphDimensions[glyphIndex] = glyphArea.Size() / unitsPerEm;
        loaded.glyphBaselines[glyphIndex] = -glyphArea.bottom / glyphArea.Height(); // assumes baseline at Y=0
    }
    glyphStartIndices.Add(curves.Size());

    // send curve data to the GPU
    int curveCount = curves.Size();
    int sizeCheck = sizeof(BezierCurve);

    loaded.glyphCurves = app.graphics.CreateArrayBuffer(ShaderDataType::Structured, curves.Size(), sizeof(BezierCurve));
    loaded.firstCurveIndices = app.graphics.CreateArrayBuffer(ShaderDataType::UInt, glyphStartIndices.Size());
    app.graphics.WriteBuffer(&curves[0], curves.Size() * sizeof(BezierCurve), loaded.glyphCurves.buffer);
    app.graphics.WriteBuffer(&glyphStartIndices[0], glyphStartIndices.Size() * sizeof(uint32_t), loaded.firstCurveIndices.buffer);

    // clean up
    curves.Release();
    glyphStartIndices.Release();
    fontFile.Release();
    tableStarts.Release();
    delete[] contourEndIndices;
    delete[] flags;
    delete[] points;
    delete[] advanceWidths;
    delete[] leftSideBearings;

    return loaded;
}

void FontLoader::ReadGlyph(uint16_t glyphIndex, AlignedRect* outArea, const Matrix2D* transform, Int2 offset, const AlignedRect* baseArea) {
    fontFile.readLocation = locaStart + glyphIndex * (locFormat == 0 ? 2 : 4);
    uint32_t glyphOffset = (locFormat == 0 ? fontFile.ReadUInt16() * 2u : fontFile.ReadUInt32());
    uint32_t nextOffset = (locFormat == 0 ? fontFile.ReadUInt16() * 2u : fontFile.ReadUInt32());
    fontFile.readLocation = tableStarts.Get(Tag{"glyf"}) + glyphOffset;

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

void FontLoader::ReadSimpleGlyph(int16_t contourCount, AlignedRect glyphArea, const Matrix2D* transform, Int2 offset) {
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
                    curves.Add(BezierCurve{ 
                        points[currentPoint],
                        (points[currentPoint] + points[nextPoint1]) / 2.f,
                        points[nextPoint1]
                    });

                    currentPoint += 1;
                }
                else if(flags[nextPoint2] & ON_CURVE_BIT) {
                    // on, off, on
                    curves.Add(BezierCurve{ 
                        points[currentPoint],
                        points[nextPoint1],
                        points[nextPoint2]
                    });

                    currentPoint += 2;
                }
                else {
                    // on, off, off
                    curves.Add(BezierCurve{ 
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
                    curves.Add(BezierCurve{ 
                        (points[currentPoint] + points[nextPoint1]) / 2.0f,
                        points[nextPoint1],
                        points[nextPoint2]
                    });

                    currentPoint += 2;
                }
                else {
                    // off, off, off
                    curves.Add(BezierCurve{ 
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

void FontLoader::ReadCompositeGlyph(const Matrix2D* transform, Int2 offset, const AlignedRect* baseArea) {
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