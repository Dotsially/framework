#ifndef RENDER
#define RENDER
#include "common.cpp"

enum GLBufferType{
    VERTEX_BUFFER,
    ELEMENT_BUFFER,
    INSTANCED_BUFFER,
    INDIRECT_BUFFER,
    STORAGE_BUFFER
};

enum GLDrawType{
    DRAW_ARRAY,
    DRAW_ELEMENT,
    DRAW_A_INSTANCED,
    DRAW_E_INSTANCED
};

struct GLRenderObject{
    uint32_t vao;
    uint32_t vbo;
    uint32_t ivbo;
    uint32_t ibo;
    uint32_t ebo;
    uint8_t attributeCount = 0;
};

void GLBufferData(GLRenderObject* renderObject, GLBufferType bufferType, uint32_t usage, void* data, uint32_t length, uint32_t size){
    glBindVertexArray(renderObject->vao);
    uint32_t bufferTarget = 0;

    switch (bufferType){
        case VERTEX_BUFFER:
            bufferTarget = GL_ARRAY_BUFFER;       
            glBindBuffer(bufferTarget, renderObject->vbo);
            break;
        case ELEMENT_BUFFER:
            bufferTarget = GL_ELEMENT_ARRAY_BUFFER;
            glBindBuffer(bufferTarget, renderObject->ebo);
            break;
        case INDIRECT_BUFFER:
            bufferTarget = GL_DRAW_INDIRECT_BUFFER;
            glBindBuffer(bufferTarget, renderObject->ibo);
            break;
        case INSTANCED_BUFFER:
            bufferTarget = GL_ARRAY_BUFFER;
            glBindBuffer(bufferTarget, renderObject->ivbo);
            break;
    }

    glBufferData(bufferTarget, length*size, data, usage);

    glBindVertexArray(0);
    glBindBuffer(bufferTarget, 0);
}

void GLAddAttribute(GLRenderObject* renderObject, uint32_t attributeType, uint32_t attributeVector, uint32_t stride, uint32_t offset, uint8_t isInstanced = false, uint8_t isFloat = true){
    glBindVertexArray(renderObject->vao);

    if(isInstanced){
        glBindBuffer(GL_ARRAY_BUFFER, renderObject->ivbo);
        glVertexAttribDivisor(renderObject->attributeCount, 1);
    }
    else{
        glBindBuffer(GL_ARRAY_BUFFER, renderObject->vbo);
    }
    
    if(isFloat){
        glVertexAttribPointer(renderObject->attributeCount, attributeVector, attributeType, GL_FALSE, stride, (void*)(offset));
    }    
    else{
        glVertexAttribIPointer(renderObject->attributeCount, attributeVector, attributeType, stride, (void*)(offset));
    }
    
    glEnableVertexAttribArray(renderObject->attributeCount);
    renderObject->attributeCount++;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLDraw(GLRenderObject* renderObject, GLDrawType drawType, uint32_t primitiveType, uint32_t count, uint32_t instanceCount = 0){
    glBindVertexArray(renderObject->vao);

    switch(drawType){
        case DRAW_ARRAY:
            glDrawArrays(primitiveType, 0, count);
            break;
        case DRAW_ELEMENT:  
            glDrawElements(primitiveType, count, GL_UNSIGNED_INT, 0);
            break;
        case DRAW_A_INSTANCED:
            glDrawArraysInstanced(primitiveType, 0, count, instanceCount);
            break;
        case DRAW_E_INSTANCED:
            glDrawElementsInstanced(primitiveType, count, GL_UNSIGNED_INT, 0, instanceCount);
            break;
    }
    glBindVertexArray(0);
}

#endif