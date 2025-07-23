#ifndef ANIMATION
#define ANIMATION

#include "common.cpp"
#include "math.cpp"


struct AnimationBone{
    std::string name;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

struct AnimationFrame{
    uint32_t frameTime;
    std::vector<AnimationBone> bones;
};

struct Animation{
    std::string name;
    std::vector<AnimationFrame> frames;
};

struct AnimationManager{
    std::unordered_map<std::string, Animation> animations;
};

void AnimationLoad(AnimationManager* animationManager, const std::string& fileName){
    using json = nlohmann::json;
    std::string filePath = "resources/animations/" + fileName;

    std::ifstream file(filePath);
    json data = json::parse(file);

    Animation animation = {};

    animation.name = data["name"];

    for (const auto& frame : data["frames"]) {
        AnimationFrame animationFrame = {};
        animationFrame.frameTime = frame["frame_number"];
        const auto& bones = frame["bones"];

        for (auto& [boneName, boneData] : bones.items()) {
            AnimationBone frameData = {};
            frameData.name = boneName;

            frameData.position = {
                boneData["position"][0],
                boneData["position"][1],
                boneData["position"][2]
            };

            frameData.rotation = glm::quat(
                boneData["rotation"][0],
                boneData["rotation"][1],
                boneData["rotation"][2],
                boneData["rotation"][3]
            );

            frameData.scale = {
                boneData["scale"][0],
                boneData["scale"][1],
                boneData["scale"][2]
            };

            animationFrame.bones.push_back(frameData);
        }
        
        animation.frames.push_back(animationFrame);
    }

    animationManager->animations[animation.name] = animation;
}

std::vector<AnimationBone> AnimationFrameGet(AnimationManager* animationManager, const std::string& animationName, uint64_t TICK_COUNTER) {
    std::vector<AnimationBone> result;

    auto iterator = animationManager->animations.find(animationName);
    if (iterator == animationManager->animations.end()) return result;

    const Animation& animation = iterator->second;

    if (animation.frames.size() == 0) {
        return result;
    } else if (animation.frames.size() == 1) {
        return animation.frames[0].bones; // Return the single frame
    }

    // Find current frame
    const AnimationFrame* frameA = nullptr;
    const AnimationFrame* frameB = nullptr;

    uint32_t frameTick = TICK_COUNTER % animation.frames.back().frameTime;
    for (int i = 0; i < animation.frames.size() - 1; ++i) {
        if (frameTick >= animation.frames[i].frameTime && frameTick < animation.frames[i + 1].frameTime) {
            frameA = &animation.frames[i];
            frameB = &animation.frames[i + 1];
            break;
        }
    }

    if (!frameA || !frameB) {
        // Loop from last frame to first frame
        frameA = &animation.frames.back();
        frameB = &animation.frames.front();
        frameTick = 0; // Adjust as needed
    }

    float duration = float(frameB->frameTime - frameA->frameTime);
    float timeIntoFrame = float(frameTick - frameA->frameTime);
    float t = duration > 0 ? timeIntoFrame / duration : 0.0f;

    // Interpolate bones
    for (int i = 0; i < frameA->bones.size(); ++i) {
        const auto& boneA = frameA->bones[i];
        const auto& boneB = frameB->bones[i];

        AnimationBone blended;
        blended.name = boneA.name;
        blended.position = Lerp(boneA.position, boneB.position, t);
        blended.rotation = Slerp(boneA.rotation, boneB.rotation, t);
        blended.scale = Lerp(boneA.scale, boneB.scale, t);

        result.push_back(blended);
    }

    return result;
}

#endif