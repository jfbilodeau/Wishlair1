//
// Created by jfbil on 2024-01-28.
//

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "nomad/nomad.hpp"

#include <vector>

namespace nomad {

class Sprite;
class ResourceManager;

///////////////////////////////////////////////////////////////////////////////
// Animation
///////////////////////////////////////////////////////////////////////////////
class Animation final {
public:
    Animation(
        const NomadString& name,
        const NomadString& variant,
        const NomadString& direction,
        const std::vector<Sprite*>& frames
    );
    Animation(const Animation& other) = delete;

    [[nodiscard]] bool name_is(const NomadString& name, const NomadString& variant, const NomadString& direction) const;
    [[nodiscard]] const NomadString& get_name() const;
    [[nodiscard]] const NomadString& get_variant() const;
    [[nodiscard]] const NomadString& get_direction() const;
    [[nodiscard]] NomadIndex get_frame_count() const;
    [[nodiscard]] const Sprite* get_frame(NomadIndex index) const;
    [[nodiscard]] const Sprite* get_last_frame() const;

private:
    NomadString m_name;
    NomadString m_variant;
    NomadString m_direction;
    std::vector<Sprite*> m_frames;
};

///////////////////////////////////////////////////////////////////////////////
// AnimationManager
///////////////////////////////////////////////////////////////////////////////
class AnimationManager {
public:
    explicit AnimationManager(ResourceManager* resources);
    AnimationManager(const AnimationManager& other) = delete;
    AnimationManager(AnimationManager&& other) noexcept = delete;
    AnimationManager& operator=(const AnimationManager& other) = delete;

    NomadId register_animation(
        const NomadString& name,
        const NomadString& variant,
        const NomadString& direction,
        const std::vector<Sprite*>& frames
    );
    [[nodiscard]] const Animation* get_animation(const NomadString& name, const NomadString& variant, const NomadString& direction);

private:
    ResourceManager* m_resources;
    std::vector<std::unique_ptr<Animation>> m_animations;
};

} // nomad

#endif //ANIMATION_HPP
