#ifndef TEXTURE_ARRAY
#define TEXTURE_ARRAY

#include "common.cpp"
#include "common_graphics.cpp"

struct TextureArray{
    uint32_t textureID;
    uint32_t textureUnitCount;
};

/*
 * textureSize is the size of each texture in the array. Must be constant.
 * arrayDepth how many textures the array holds.
 */
void TextureArrayInitialize(TextureArray* textureArray, int textureSize, int arrayDepth){
    glGenTextures(1, &textureArray->textureID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray->textureID);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, textureSize, textureSize, arrayDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  

    textureArray->textureUnitCount = 0;
}

void TextureArrayAdd(TextureArray* textureArray, std::string& fileName){
    std::string filePath = "resources/textures/" + fileName;
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray->textureID); 

    // load and generate the textures
    int32_t width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);  
    uint8_t* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 4);
    
    if (data == NULL){
        std::cout << "Failed to load texture" << std::endl;
        return;
    }
    
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, textureArray->textureUnitCount, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    stbi_image_free(data);

    textureArray->textureUnitCount += 1;
}


void TextureArrayUse(TextureArray* textureArray, uint32_t textureUnit){
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray->textureID); 
}

#endif