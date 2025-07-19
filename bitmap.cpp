#ifndef BITMAP
#define BITMAP

#include "common.cpp"

struct BitmapGlyph {
    glm::vec2 uvOffset;  // Top-left in atlas (normalized)
    glm::vec2 uvSize;    // Size in atlas (normalized)
    glm::vec2 size;      // Pixel size
    glm::vec2 offset;    // Offset from pen position
};

struct BitmapFont {
    std::unordered_map<char, BitmapGlyph> glyphs;
    uint32_t atlasTextureSize;
};




#endif