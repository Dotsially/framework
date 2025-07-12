#ifndef TEXTURE_PACKER
#define TEXTURE_PACKER

#include "common.cpp"

#define PIXEL_SIZE 4

struct TexturePacker{
    uint8_t* memory;
    std::unordered_map<std::string, glm::vec2> texturesIndex;
    uint32_t textureSize;
    uint32_t textureInstanceSize;
    uint32_t size;
    uint32_t verticalSlot;
    uint32_t horizontalSlot;
};

/**
 *  size amount of textures in a column and row
 *  textureInstanceSize size of a single texture in pixels
 */
void PackerInitialize(TexturePacker* packer, uint32_t size, uint32_t textureInstanceSize){
    packer-> size = size;
    packer-> textureInstanceSize = textureInstanceSize;
    packer-> textureSize = size * textureInstanceSize; 
    packer-> horizontalSlot = 0;
    packer-> verticalSlot = 0;

    uint32_t textureByteSize = size*textureInstanceSize*size*textureInstanceSize*PIXEL_SIZE;
    packer->memory = (uint8_t*)calloc(textureByteSize, sizeof(uint8_t));
}


void PackerInsertTexture(TexturePacker* packer, uint8_t* texture, std::string fileName){

    if (packer->horizontalSlot == packer->size){
        if (packer->verticalSlot + 1 < packer->size){
            packer->horizontalSlot = 0;
            packer->verticalSlot += 1;
        }
        else{
            return;
        }
    }

    uint32_t size = packer->textureInstanceSize;
    uint32_t lastHeight = packer->verticalSlot * size;
    uint32_t lastWidth = packer->horizontalSlot * size;

    for(int x = lastHeight; x < (lastHeight+size); x++){
        for(int y = lastWidth; y < (lastWidth+size); y++){
            for(int i = 0; i < PIXEL_SIZE; i++){
                packer->memory[(y*PIXEL_SIZE)+((x*PIXEL_SIZE)*128)+i] = 
                    texture[((y-lastWidth)*PIXEL_SIZE)+(((x-lastHeight)*PIXEL_SIZE)*size)+i];       
                }
        }
    }

    size_t slashPos = fileName.find_last_of('/');

    if (slashPos == std::string::npos) slashPos = -1; // no slash

    std::string blockName = fileName.substr(slashPos + 1, fileName.length());
    std::cout << blockName << std::endl;

    packer->texturesIndex[blockName].x = packer->horizontalSlot;
    packer->texturesIndex[blockName].y = packer->verticalSlot;

    packer->horizontalSlot += 1;
}


void PackerAddTexture(TexturePacker* packer, std::string fileName){
    std::string filePath = "resources/textures/" + fileName;
    // load and generate the textures
    int32_t width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);  
    uint8_t* texture = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (!texture) {
        std::cout << "Failed to load texture: " << filePath << "\n";
        return;
    }

    PackerInsertTexture(packer, texture, fileName);
    stbi_image_free(texture);
}


glm::vec2 PackerGetTexture(TexturePacker* packer, std::string fileName){
    auto iterator = packer->texturesIndex.find(fileName);
    if (iterator != packer->texturesIndex.end()) {
        return iterator->second;
    } else {
        std::cerr << "Missing texture: " << fileName << std::endl;
        return glm::vec2(0.0f);
    }
}

#endif