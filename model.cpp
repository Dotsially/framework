#ifndef MODEL
#define MODEL

#include "common.cpp"
#include "mesh.cpp"
#include "bone.cpp"
#include "animation.cpp"

//TODO 
// Will be reworked
// - Model inditifiers for entities
// - Bones and animations
// - Material support
// - Smart drawing of same materials
struct Model{
    std::vector<Bone> bones;
    std::unordered_map<std::string, std::uint8_t> boneIDs;
    std::vector<glm::mat4> boneTransforms;
    Mesh mesh;
};

// struct ModelLibrary{
//     std::vector<Model> models;
//     std::unordered_map<std::string, std::uint16_t> modelIDs;
// };


void ModelLoad(Model* model, std::string fileName){
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
    
            vertices.push_back(jsonUV[j * 2]);
            vertices.push_back(jsonUV[j * 2 + 1]);
        }
    
        // Add triangle indices (using fan triangulation for quads or more)
        for (int j = 1; j < numVertsInFace - 1; j++) {
            indices.push_back(vertexOffset);
            indices.push_back(vertexOffset + j);
            indices.push_back(vertexOffset + j + 1);
        }
    
        vertexOffset += numVertsInFace;
    }

    MeshInitialize(&model->mesh, vertices.data(), vertices.size(), indices.data(), indices.size());
}


void ModelLoadBones(Model* model, nlohmann::json* bones){
    model->bones.resize(bones->size());

    for(int i = 0; i < bones->size(); i++){
        Bone bone = {};
        bone.name = (*bones)[i]["name"];

        bone.origin.x = (*bones)[i]["location"][0]; 
        bone.origin.y = (*bones)[i]["location"][1]; 
        bone.origin.z = (*bones)[i]["location"][2];
        
        bone.position = glm::vec3(0);
        bone.rotation = glm::quat(glm::vec3(0));

        model->boneIDs[bone.name] = i;
        model->bones[i] = bone;
        
        if (!(*bones)[i]["parent"].is_null()){
            model->bones[i].parent = &model->bones[model->boneIDs[(*bones)[i]["parent"]]];
            model->bones[i].parent->children.push_back(&model->bones[i]);
        }
    }
}


void ModelLoadSkinned(Model* model, std::string fileName){
    using json = nlohmann::json;
    std::string filePath = "resources/models/" + fileName;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::fstream file(filePath);

    json data = json::parse(file);
    
    json jsonBones = data["bones"];
    if(!jsonBones.is_array()) return;
    ModelLoadBones(model, &jsonBones);

    model->boneTransforms.resize(model->bones.size());

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
    
            vertices.push_back(jsonUV[j * 2]);
            vertices.push_back(jsonUV[j * 2 + 1]);
            
            glm::vec4 boneIDS = glm::vec4(-1);
            glm::vec4 weights = glm::vec4(-1);

            for(int k = 0; k < jsonVertexGroups[idx].size(); k++){
                boneIDS[k] = model->boneIDs[jsonVertexGroups[idx][k]];
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

    MeshSkinnedInitialize(&model->mesh, vertices.data(), vertices.size(), indices.data(), indices.size());
}


void ModelAnimate(Model* model, Animation* animation, uint64_t tick) {
    for (const auto& [bone, index] : model->boneIDs) {
        const auto& keyframes = animation->boneIndex[bone];
        int frameCount = keyframes.size();

        if (frameCount < 2){
            if (keyframes.size() < 1) continue;
            model->bones[index].position = keyframes[0].position;
            model->bones[index].rotation = keyframes[0].rotation;
            continue;
        }

        int animationLength = keyframes[frameCount - 1].frameTime;
        uint64_t timeInCycle = tick % animationLength;

        int frameIndex = 0;
        while (frameIndex < frameCount - 1 && keyframes[frameIndex + 1].frameTime <= timeInCycle) {
            ++frameIndex;
        }

        const AnimationBone& currentFrame = keyframes[frameIndex];
        const AnimationBone& nextFrame = keyframes[(frameIndex + 1) % frameCount];

        int startTime = currentFrame.frameTime;
        int endTime = nextFrame.frameTime;

        if (endTime <= startTime) {
            endTime += animationLength;
        }

        float t = float(timeInCycle - startTime) / float(endTime - startTime);

        model->bones[index].position = glm::mix(currentFrame.position, nextFrame.position, t);
        model->bones[index].rotation = glm::slerp(currentFrame.rotation, nextFrame.rotation, t);
    }

    BoneCalculateTransforms(&model->bones, &model->boneTransforms, &model->boneIDs);
}

void ModelSkinnedDraw(Model* model, glm::mat4& projectionView,  glm::mat4& transform){
    glUniformMatrix4fv(2, model->boneTransforms.size(), GL_FALSE, glm::value_ptr(model->boneTransforms[0]));
    MeshDraw(&model->mesh, projectionView, transform);
}

#endif