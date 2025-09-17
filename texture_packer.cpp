#ifndef TEXTURE_PACKER
#define TEXTURE_PACKER

#include "common.cpp"
#include "common_graphics.cpp"

#define PIXEL_SIZE 4

struct TextureData{
    glm::vec2 uvOffset;
    glm::vec2 uvSize;
    glm::vec2 textureSize;
};

struct TexturePacker {
    uint8_t* memory;
    std::unordered_map<std::string, TextureData> texturesIndex;
    uint32_t atlasSize;
    stbrp_context context;
    stbrp_node* nodes;
};

void PackerInitialize(TexturePacker* packer, uint32_t atlasSize){
    packer->atlasSize = atlasSize;

    uint32_t textureByteSize = atlasSize * atlasSize * PIXEL_SIZE;
    packer->memory = (uint8_t*)calloc(textureByteSize, sizeof(uint8_t));

    packer->nodes = (stbrp_node*)malloc(sizeof(stbrp_node) * atlasSize);
    stbrp_init_target(&packer->context, atlasSize, atlasSize, packer->nodes, atlasSize);
}


std::string PackerInsertTexture(TexturePacker* packer, uint8_t* texture, int32_t width, int32_t height, std::string fileName) {
    stbrp_rect rect = {};
    rect.id = 0;
    rect.w = width;
    rect.h = height;

    stbrp_pack_rects(&packer->context, &rect, 1);

    if (!rect.was_packed) {
        std::cerr << "Failed to pack texture: " << fileName << std::endl;
        return "";
    }

    // Copy pixel data into atlas memory
    for (int y = 0; y < height; y++) {
        memcpy(
            &packer->memory[((rect.y + y) * packer->atlasSize + rect.x) * PIXEL_SIZE],
            &texture[(y * width) * PIXEL_SIZE],
            width * PIXEL_SIZE
        );
    }

    // Extract base file name
    size_t slashPos = fileName.find_last_of('/');
    if (slashPos == std::string::npos) slashPos = -1;
    std::string textureName = fileName.substr(slashPos + 1);

    // Store TextureData
    packer->texturesIndex[textureName] = {
        {(float)rect.x / packer->atlasSize, (float)rect.y / packer->atlasSize },
        {(float)width / packer->atlasSize, (float)height / packer->atlasSize },
        {width, height}
    };
    
    return textureName;
}

std::string PackerAddTexture(TexturePacker* packer, std::string fileName) {
    std::string textureName = "";
    std::string filePath = "resources/textures/" + fileName;

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t* texture = stbi_load(filePath.c_str(), &width, &height, &channels, PIXEL_SIZE);

    if (!texture) {
        std::cout << "Failed to load texture: " << filePath << "\n";
        return textureName;
    }

    textureName = PackerInsertTexture(packer, texture, width, height, fileName);
    stbi_image_free(texture);

    return textureName;
}

TextureData PackerGetTexture(TexturePacker* packer, std::string fileName) {
    auto it = packer->texturesIndex.find(fileName);
    if (it != packer->texturesIndex.end()) {
        return it->second;
    } else {
        std::cerr << "Missing texture: " << fileName << std::endl;
        TextureData null = {};
        return null;
    }
}

#endif