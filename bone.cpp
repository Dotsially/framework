#ifndef BONE
#define BONE

#include "common.cpp"

struct Bone{
    std::vector<int32_t> children;
    std::string name;
    glm::vec3 origin;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    
    int32_t parentBoneID;
};

struct BoneData{
    std::vector<Bone> bones;
    std::unordered_map<std::string, std::uint8_t> boneIDs;
    std::vector<glm::mat4> transforms;
};

void BoneCalculateTransforms(BoneData* boneData){
    for (int i = 0; i < boneData->bones.size(); i++) {
        Bone& bone = boneData->bones[i];
        glm::mat4& currentTransform = boneData->transforms[i];

        glm::mat4 T_local      = glm::translate(glm::mat4(1.0f), bone.position);
        glm::mat4 T_toOrigin   = glm::translate(glm::mat4(1.0f), -bone.origin);
        glm::mat4 R            = glm::toMat4(bone.rotation);
        glm::mat4 S            = glm::scale(glm::mat4(1.0f), bone.scale);
        glm::mat4 T_fromOrigin = glm::translate(glm::mat4(1.0f), bone.origin);

        glm::mat4 localTransform = T_local * T_fromOrigin * R * S * T_toOrigin;

        if (bone.parentBoneID < 0) {
            currentTransform = localTransform;
        } else {
            currentTransform = boneData->transforms[bone.parentBoneID] * localTransform;
        }
    }
}

Bone* BoneGet(BoneData* data, std::string boneName){
    auto iterator = data->boneIDs.find(boneName);
    if (iterator == data->boneIDs.end()) return nullptr;

    const uint8_t boneID = iterator->second;

    if (boneID >= data->bones.size()) return nullptr;       
    return &data->bones[boneID];
}

#endif