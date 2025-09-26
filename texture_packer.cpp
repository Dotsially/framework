#ifndef TEXTURE_PACKER
#define TEXTURE_PACKER

#include "common.cpp"
#include "common_graphics.cpp"

struct TextureData{
    glm::vec2 uvOffset;
    glm::vec2 uvSize;
    glm::vec2 textureSize;
};

class TexturePacker {
public:
    #define PIXEL_SIZE 4

    uint8_t* m_memory;
    std::unordered_map<std::string, TextureData> m_texturesIndex;
    uint32_t m_atlasSize;
    stbrp_context m_context;
    stbrp_node* m_nodes;

    void Initialize(uint32_t atlasSize){
        m_atlasSize = atlasSize;

        uint32_t textureByteSize = m_atlasSize * m_atlasSize * PIXEL_SIZE;
        m_memory = (uint8_t*)calloc(textureByteSize, sizeof(uint8_t));

        m_nodes = (stbrp_node*)malloc(sizeof(stbrp_node) * m_atlasSize);
        stbrp_init_target(&m_context, m_atlasSize, m_atlasSize, m_nodes, m_atlasSize);
    }

    std::string InsertTexture(uint8_t* texture, int32_t width, int32_t height, std::string fileName) {
        stbrp_rect rect = {};
        rect.id = 0;
        rect.w = width;
        rect.h = height;

        stbrp_pack_rects(&m_context, &rect, 1);

        if (!rect.was_packed) {
            std::cerr << "Failed to pack texture: " << fileName << std::endl;
            return "";
        }

        // Copy pixel data into atlas memory
        for (int y = 0; y < height; y++) {
            memcpy(
                &m_memory[((rect.y + y) * m_atlasSize + rect.x) * PIXEL_SIZE],
                &texture[(y * width) * PIXEL_SIZE],
                width * PIXEL_SIZE
            );
        }

        // Extract base file name
        size_t slashPos = fileName.find_last_of('/');
        if (slashPos == std::string::npos) slashPos = -1;
        std::string textureName = fileName.substr(slashPos + 1);

        // Store TextureData
        m_texturesIndex[textureName] = {
            {(float)rect.x / m_atlasSize, (float)rect.y / m_atlasSize },
            {(float)width / m_atlasSize, (float)height / m_atlasSize },
            {width, height}
        };
        
        return textureName;
    }

    std::string AddTexture(std::string fileName) {
        std::string textureName = "";
        std::string filePath = "resources/textures/" + fileName;

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        uint8_t* texture = stbi_load(filePath.c_str(), &width, &height, &channels, PIXEL_SIZE);

        if (!texture) {
            std::cout << "Failed to load texture: " << filePath << "\n";
            return textureName;
        }

        textureName = InsertTexture(texture, width, height, fileName);
        stbi_image_free(texture);

        return textureName;
    }

    TextureData GetTexture(std::string fileName) {
        auto it = m_texturesIndex.find(fileName);
        if (it != m_texturesIndex.end()) {
            return it->second;
        } else {
            std::cerr << "Missing texture: " << fileName << std::endl;
            TextureData null = {};
            return null;
        }
    }
};

#endif