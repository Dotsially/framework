#ifndef BITMAP
#define BITMAP

#include "common.cpp"
#include "texture_packer.cpp"

struct BitmapGlyph {
    glm::vec2 uvOffset;  // Top-left in atlas (normalized)
    glm::vec2 uvSize;    // Size in atlas (normalized)
    glm::vec2 size;      // Pixel size
    glm::vec2 offset;    // Offset from pen position
};

struct BitmapFont {
    std::unordered_map<char, BitmapGlyph> glyphs;
};

void BitmapLoad(BitmapFont* font, TexturePacker* packer, std::string fontFileName) {
    using json = nlohmann::json;
    std::string filePath = "resources/fonts/" + fontFileName;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "File not found: " << fontFileName << std::endl;
        return;
    }

    json metadata = json::parse(file);

    int margin        = metadata["margin"];
    int glyphPadding  = metadata["glyph_padding"];
    int glyphWidth    = metadata["glyph_width"];
    int glyphHeight   = metadata["glyph_height"];
    int asciiStart    = metadata["ascii_start"];
    int asciiEnd      = metadata["ascii_end"];
    std::string textureFile = metadata["texture"];

    // Add the font texture to the atlas
    PackerAddTexture(packer, textureFile);

    // Get the UV data for the font image in the atlas
    TextureData texData = PackerGetTexture(packer, textureFile);

    int atlasSize = packer->atlasSize;

    // Calculate dimensions of the subimage
    int subImageWidth  = texData.textureSize.x;
    int subImageHeight = texData.textureSize.y;

    int usableWidth  = subImageWidth  - 2 * margin;
    int usableHeight = subImageHeight - 2 * margin;

    int glyphsPerRow = (usableWidth + glyphPadding) / (glyphWidth + glyphPadding);
    int glyphIndex = 0;

    for (int ascii = asciiStart; ascii <= asciiEnd; ++ascii) {
        int row = glyphIndex / glyphsPerRow;
        int col = glyphIndex % glyphsPerRow;

        // Local pixel coordinates of glyph inside the font texture region
        int localX = margin + col * (glyphWidth + glyphPadding);
        int localY = margin + row * (glyphHeight + glyphPadding);

        // Convert local pixel coords to UV space within the atlas
        float u = (texData.uvOffset.x * atlasSize + localX) / atlasSize;
        
        int flippedY = subImageHeight - localY - glyphHeight;
        float v = (texData.uvOffset.y * atlasSize + flippedY) / atlasSize;

        BitmapGlyph glyph;
        glyph.uvOffset = { u, v };
        glyph.uvSize   = {
            float(glyphWidth) / atlasSize,
            float(glyphHeight) / atlasSize
        };
        glyph.size   = { glyphWidth, glyphHeight };
        glyph.offset = { 0.0f, 0.0f };

        font->glyphs[static_cast<char>(ascii)] = glyph;
        ++glyphIndex;
    }
}

#endif