#ifndef MESH
#define MESH

#include "common.cpp"
#include "render.cpp"


struct Mesh{
    GLRenderObject renderable;
    void* vertexData;
    void* indexData;
    uint32_t vertexSize;
    uint32_t indexSize;
    uint8_t initialized = 0;
};

void MeshAllocate(Mesh* mesh, uint32_t vertexSize, uint32_t indexSize){
    mesh->vertexData = calloc(vertexSize, sizeof(float));
    mesh->vertexSize = vertexSize;
    mesh->indexData = calloc(indexSize, sizeof(uint32_t));
    mesh->indexSize = indexSize;
}

void MeshFillVertices(Mesh* mesh, void* vertexBuffer){
    for(int i = 0; i < mesh->vertexSize; i++){
        ((float*)mesh->vertexData)[i] = ((float*)vertexBuffer)[i];
    }
}

void MeshFillIndices(Mesh* mesh, void* indexBuffer){
    for(int i = 0; i < mesh->indexSize; i++){
        ((uint32_t*)mesh->indexData)[i] = ((uint32_t*)indexBuffer)[i];
    }
}

void MeshSkinnedCreate(Mesh* mesh){
    glGenBuffers(1, &mesh->renderable.vbo);
    glGenBuffers(1, &mesh->renderable.ebo);
    glGenVertexArrays(1, &mesh->renderable.vao);
      
    GLBufferData(&mesh->renderable, VERTEX_BUFFER, GL_STATIC_DRAW, mesh->vertexData, mesh->vertexSize, sizeof(float));
    GLBufferData(&mesh->renderable, ELEMENT_BUFFER, GL_STATIC_DRAW, mesh->indexData, mesh->indexSize, sizeof(uint32_t));
    GLAddAttribute(&mesh->renderable, GL_FLOAT, 3, 16*sizeof(float), 0);
    GLAddAttribute(&mesh->renderable, GL_FLOAT, 3, 16*sizeof(float), 3*sizeof(float));
    GLAddAttribute(&mesh->renderable, GL_FLOAT, 2, 16*sizeof(float), 6*sizeof(float));
    GLAddAttribute(&mesh->renderable, GL_FLOAT, 4, 16*sizeof(float), 8*sizeof(float));
    GLAddAttribute(&mesh->renderable, GL_FLOAT, 4, 16*sizeof(float), 12*sizeof(float));
}

void MeshCreate(Mesh* mesh){
    glGenBuffers(1, &mesh->renderable.vbo);
    glGenBuffers(1, &mesh->renderable.ebo);
    glGenVertexArrays(1, &mesh->renderable.vao);
      
    GLBufferData(&mesh->renderable, VERTEX_BUFFER, GL_STATIC_DRAW, mesh->vertexData, mesh->vertexSize, sizeof(float));
    GLBufferData(&mesh->renderable, ELEMENT_BUFFER, GL_STATIC_DRAW, mesh->indexData, mesh->indexSize, sizeof(uint32_t));
    GLAddAttribute(&mesh->renderable, GL_FLOAT, 3, 8*sizeof(float), 0);
    GLAddAttribute(&mesh->renderable, GL_FLOAT, 3, 8*sizeof(float), 3*sizeof(float));
    GLAddAttribute(&mesh->renderable, GL_FLOAT, 2, 8*sizeof(float), 6*sizeof(float));
}

void MeshInitialize(Mesh* mesh, void* vertexBuffer, uint32_t vertexSize, void* indexBuffer, uint32_t indexSize){
    MeshAllocate(mesh, vertexSize, indexSize);
    MeshFillVertices(mesh, vertexBuffer);
    MeshFillIndices(mesh, indexBuffer);
    MeshCreate(mesh);
}

void MeshSkinnedInitialize(Mesh* mesh, void* vertexBuffer, uint32_t vertexSize, void* indexBuffer, uint32_t indexSize){
    MeshAllocate(mesh, vertexSize, indexSize);
    MeshFillVertices(mesh, vertexBuffer);
    MeshFillIndices(mesh, indexBuffer);
    MeshSkinnedCreate(mesh);
}

void MeshDraw(Mesh* mesh, glm::mat4& projectionView, glm::mat4& transform){
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projectionView));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(transform));
    GLDraw(&mesh->renderable, DRAW_ELEMENT, GL_TRIANGLES, mesh->indexSize);
}


#endif