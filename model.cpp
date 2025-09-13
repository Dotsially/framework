#ifndef MODEL
#define MODEL

#include "common.cpp"
#include "mesh.cpp"
#include "bone.cpp"
#include "animation.cpp"
#include "texture_packer.cpp"
#include "fileio.cpp"

//TODO 
// Will be reworked
// - Model inditifiers for entities
// - Bones and animations
// - Material support
// - Smart drawing of same materials
struct Model{
    BoneData boneData;
    Mesh mesh;
};

struct ModelLibrary{
    std::vector<Model> models;
    std::unordered_map<std::string, std::uint32_t> modelIDs;
};

void ModelLibraryAdd(ModelLibrary* modelLibrary, Model* model, std::string fileName){
    size_t slashPos = fileName.find_last_of('/');
    if (slashPos == std::string::npos) slashPos = -1;

    size_t prefixPos = fileName.find('.', slashPos + 1);
    if (prefixPos == std::string::npos) prefixPos = fileName.size();

    std::string modelName = fileName.substr(slashPos + 1, prefixPos - (slashPos + 1));

    if (modelLibrary->modelIDs.find(modelName) == modelLibrary->modelIDs.end()) {
        modelLibrary->modelIDs[modelName] = modelLibrary->models.size();
        modelLibrary->models.push_back(*model);
    }
    else{
        uint32_t modelIndex = modelLibrary->modelIDs[modelName];
        MeshFree(&modelLibrary->models[modelIndex].mesh);
        modelLibrary->models[modelIndex] = *model;
    }
}

glm::vec2 ModelConvertUVs(glm::vec2 UV, TexturePacker* modelsTexturePacker, std::string textureName){
    if(modelsTexturePacker == nullptr) return UV;

    TextureData data = PackerGetTexture(modelsTexturePacker, textureName);

    return data.uvOffset + UV * data.uvSize;
}

void ModelLoad(ModelLibrary* modelLibrary, std::string fileName, TexturePacker* modelsTexturePacker, std::string textureName){
    Model model = {};
    using json = nlohmann::json;
    std::string filePath = "resources/models/" + fileName;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::fstream file(filePath);

    json data = json::parse(file);

    json jsonVertices = data["vertices"];
    if(!jsonVertices.is_array()) return;

    json jsonNormals = data["normals"];
    if(!jsonNormals.is_array()) return;
    
    json jsonFaces = data["faces"];
    if(!jsonFaces.is_array()) return;
    
    uint32_t vertexOffset = 0;

    for (int i = 0; i < jsonFaces.size(); i++) {
        json jsonUV = jsonFaces[i]["uv_coords"];
        if (!jsonUV.is_array()) return;
    
        json jsonIndices = jsonFaces[i]["indices"];
        if (!jsonIndices.is_array()) return;
    
        uint32_t numVertsInFace = jsonIndices.size();
    
        // Add new vertex data
        for (int j = 0; j < numVertsInFace; j++) {
            uint32_t idx = uint32_t(jsonIndices[j]);
            vertices.push_back(jsonVertices[idx * 3]);
            vertices.push_back(jsonVertices[idx * 3 + 1]);
            vertices.push_back(jsonVertices[idx * 3 + 2]);

            vertices.push_back(jsonNormals[idx * 3]);
            vertices.push_back(jsonNormals[idx * 3 + 1]);
            vertices.push_back(jsonNormals[idx * 3 + 2]);
    
            glm::vec2 UV = ModelConvertUVs({jsonUV[j * 2], jsonUV[j * 2 + 1]}, modelsTexturePacker, textureName);
            vertices.push_back(UV.x);
            vertices.push_back(UV.y);
        }
    
        // Add triangle indices (using fan triangulation for quads or more)
        for (int j = 1; j < numVertsInFace - 1; j++) {
            indices.push_back(vertexOffset);
            indices.push_back(vertexOffset + j);
            indices.push_back(vertexOffset + j + 1);
        }
    
        vertexOffset += numVertsInFace;
    }

    MeshInitialize(&model.mesh, vertices.data(), vertices.size(), indices.data(), indices.size());
    ModelLibraryAdd(modelLibrary, &model, fileName);
}

void ModelLoadBones(Model* model, nlohmann::json* bones){
    model->boneData.bones.resize(bones->size());

    for(int i = 0; i < bones->size(); i++){
        Bone bone = {};
        bone.name = (*bones)[i]["name"];

        bone.origin.x = (*bones)[i]["location"][0]; 
        bone.origin.y = (*bones)[i]["location"][1]; 
        bone.origin.z = (*bones)[i]["location"][2];
        
        bone.position = glm::vec3(0);
        bone.rotation = glm::quat(glm::vec3(0));
        bone.scale = glm::vec3(1);

        model->boneData.boneIDs[bone.name] = i;
        model->boneData.bones[i] = bone;
        
        if (!(*bones)[i]["parent"].is_null()){
            model->boneData.bones[i].parent = &model->boneData.bones[model->boneData.boneIDs[(*bones)[i]["parent"]]];
            model->boneData.bones[i].parent->children.push_back(&model->boneData.bones[i]);
        }
    }
}

void ModelLoadSkinned(ModelLibrary* modelLibrary, std::string fileName, TexturePacker* modelsTexturePacker, std::string textureName){
    Model model = {};

    using json = nlohmann::json;
    std::string filePath = "resources/models/" + fileName;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::fstream file(filePath);

    json data = json::parse(file);
    
    json jsonBones = data["bones"];
    if(!jsonBones.is_array()) return;
    ModelLoadBones(&model, &jsonBones);

    model.boneData.transforms.resize(model.boneData.bones.size());

    json jsonVertices = data["vertices"];
    if(!jsonVertices.is_array()) return;

    json jsonVertexGroups = data["vertices_bones"];
    if(!jsonVertexGroups.is_array()) return;

    json jsonWeights = data["vertices_weights"];
    if(!jsonWeights.is_array()) return;

    json jsonNormals = data["normals"];
    if(!jsonNormals.is_array()) return;
    
    json jsonFaces = data["faces"];
    if(!jsonFaces.is_array()) return;
    
    uint32_t vertexOffset = 0;

    for (int i = 0; i < jsonFaces.size(); i++) {
        json jsonUV = jsonFaces[i]["uv_coords"];
        if (!jsonUV.is_array()) return;
    
        json jsonIndices = jsonFaces[i]["indices"];
        if (!jsonIndices.is_array()) return;
    
        uint32_t numVertsInFace = jsonIndices.size();
    
        // Add new vertex data
        for (int j = 0; j < numVertsInFace; j++) {
            uint32_t idx = uint32_t(jsonIndices[j]);
            vertices.push_back(jsonVertices[idx * 3]);
            vertices.push_back(jsonVertices[idx * 3 + 1]);
            vertices.push_back(jsonVertices[idx * 3 + 2]);

            vertices.push_back(jsonNormals[idx * 3]);
            vertices.push_back(jsonNormals[idx * 3 + 1]);
            vertices.push_back(jsonNormals[idx * 3 + 2]);
    
            glm::vec2 UV = ModelConvertUVs({jsonUV[j * 2], jsonUV[j * 2 + 1]}, modelsTexturePacker, textureName);
            vertices.push_back(UV.x);
            vertices.push_back(UV.y);
            
            glm::vec4 boneIDS = glm::vec4(-1);
            glm::vec4 weights = glm::vec4(-1);

            for(int k = 0; k < jsonVertexGroups[idx].size(); k++){
                boneIDS[k] = model.boneData.boneIDs[jsonVertexGroups[idx][k]];
            }

            for(int k = 0; k < jsonWeights[idx].size(); k++){
                weights[k] = jsonWeights[idx][k];
            }

            vertices.push_back(boneIDS.x);
            vertices.push_back(boneIDS.y);
            vertices.push_back(boneIDS.z);
            vertices.push_back(boneIDS.w);
            
            vertices.push_back(weights.x);
            vertices.push_back(weights.y);
            vertices.push_back(weights.z);
            vertices.push_back(weights.w);
        }
    
        // Add triangle indices (using fan triangulation for quads or more)
        for (int j = 1; j < numVertsInFace - 1; j++) {
            indices.push_back(vertexOffset);
            indices.push_back(vertexOffset + j);
            indices.push_back(vertexOffset + j + 1);
        }
    
        vertexOffset += numVertsInFace;
    }

    MeshSkinnedInitialize(&model.mesh, vertices.data(), vertices.size(), indices.data(), indices.size());
    ModelLibraryAdd(modelLibrary, &model, fileName);
}

void ModelAnimate(Model* model, AnimationManager* animationManager, std::string animationName, uint64_t TICK_COUNTER) {
    std::vector<AnimationBone> animationBones = AnimationFrameGet(animationManager, animationName, TICK_COUNTER);

    for(int i = 0; i < animationBones.size(); ++i){
        Bone* bone = BoneGet(&model->boneData, animationBones[i].name);
        if(!bone) continue;
        
        bone->position = animationBones[i].position;
        bone->rotation = animationBones[i].rotation;
        bone->scale = animationBones[i].scale;    
    }

    BoneCalculateTransforms(&model->boneData);
}

void ModelSkinnedDraw(Model* model, glm::mat4& projectionView,  glm::mat4& transform){
    glUniformMatrix4fv(2, model->boneData.transforms.size(), GL_FALSE, glm::value_ptr(model->boneData.transforms[0]));
    MeshDraw(&model->mesh, projectionView, transform);
}

Model* ModelGet(ModelLibrary* modelLibrary, std::string modelName){
    auto it = modelLibrary->modelIDs.find(modelName);
    if (it == modelLibrary->modelIDs.end()) return nullptr;

    return &modelLibrary->models[it->second];
}

void ModelLibraryInitialize(ModelLibrary* modelLibrary, TexturePacker* modelsTexturePacker){
    using json = nlohmann::json;
    std::vector<std::string> files = ReadDirectory("resources/models");

    for(std::string file : files){
        size_t prefix_position = file.find_last_of(".");
        if(prefix_position == std::string::npos) continue;

        size_t jsonPrefix = file.find("json", prefix_position);
        if(jsonPrefix == std::string::npos) continue;

        std::fstream fileStream("resources/models/" + file);
        json jsonData = json::parse(fileStream);

        std::string textureName = jsonData["textures"][0];
        bool skinned = jsonData["skinned"];
        
        if(skinned){
            ModelLoadSkinned(modelLibrary, file, modelsTexturePacker, textureName); 
        }
        else{
            ModelLoad(modelLibrary, file, modelsTexturePacker, textureName);
        }
    }
}

#endif