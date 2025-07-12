#ifndef ANIMATION
#define ANIMATION

#include "common.cpp"

struct AnimationBone{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    int32_t frameTime;
};


struct Animation{
    std::unordered_map<std::string, std::vector<AnimationBone>> boneIndex;
};


void AnimationLoad(Animation* animation, std::string fileName){
    using json = nlohmann::json;
    std::string filePath = "resources/animations/" + fileName;

    std::fstream file(filePath);
    json data = json::parse(file);

    for (const auto& [bone, frames] : data.items()) {
        if(!frames.is_array()) break;
        animation->boneIndex[bone].resize(frames.size());

        for(int i = 0; i < frames.size(); i++){
            AnimationBone frameData;
            frameData.frameTime = frames[i]["frame_number"];

            frameData.position.x = frames[i]["position"][0];
            frameData.position.y = frames[i]["position"][1];
            frameData.position.z = frames[i]["position"][2];
            
            frameData.rotation.w = frames[i]["rotation"][0];
            frameData.rotation.x = frames[i]["rotation"][1];
            frameData.rotation.y = frames[i]["rotation"][2];
            frameData.rotation.z = frames[i]["rotation"][3];

            frameData.scale.x = frames[i]["scale"][0];
            frameData.scale.y = frames[i]["scale"][1];
            frameData.scale.z = frames[i]["scale"][2];

            animation->boneIndex[bone][i] = frameData;
        }
    }
}





#endif