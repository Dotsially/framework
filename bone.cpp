#ifndef BONE
#define BONE

#include "common.cpp"

struct Bone{
    Bone* parent;
    std::vector<Bone*> children;
    std::string name;
    glm::vec3 origin;
    glm::vec3 position;
    glm::quat rotation;
};

void BoneCalculateTransforms(std::vector<Bone>* bones, std::vector<glm::mat4>* transforms, std::unordered_map<std::string, uint8_t>* boneIDs){
    for(int i = 0; i < bones->size(); i++){
        Bone bone = (*bones)[i];
        glm::mat4* currentTransform = &(*transforms)[i];

        if(bone.parent == NULL){
            glm::mat4 T1 = glm::translate(glm::mat4(1.0f), -bone.origin);
            glm::mat4 R  = glm::toMat4(bone.rotation);
            glm::mat4 T2 = glm::translate(glm::mat4(1.0f), bone.origin);

            *currentTransform = T2 * R * T1;
        }
        else{
            glm::mat4 T1 = glm::translate(glm::mat4(1.0f), -bone.origin);
            glm::mat4 R  = glm::toMat4(bone.rotation);
            glm::mat4 T2 = glm::translate(glm::mat4(1.0f), bone.origin);

            *currentTransform = T2 * R * T1;

            int parentID = (*boneIDs)[bone.parent->name];
            *currentTransform = (*transforms)[parentID] * (*currentTransform);
        }
    }
}

#endif