//
// Created by jfbil on 2024-01-28.
//

#include "nomad/resource/Animation.hpp"

#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Sprite.hpp"

#include "nomad/log/Logger.hpp"

#include <algorithm>

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Animation
Animation::Animation(
    const NomadString& name,
    const NomadString& variant,
    const NomadString& direction,
    const std::vector<Sprite*>& frames
):
    m_name(name),
    m_variant(variant),
    m_direction(direction),
    m_frames(frames)
{}

bool Animation::name_is(const NomadString& name, const NomadString& variant, const NomadString& direction) const {
    return
        m_name == name &&
        m_variant == variant &&
        m_direction == direction;
}

const NomadString& Animation::get_name() const {
    return m_name;
}

const NomadString& Animation::get_variant() const {
    return m_variant;
}

const NomadString& Animation::get_direction() const {
    return m_direction;
}

NomadIndex Animation::get_frame_count() const {
    return m_frames.size();
}

const Sprite* Animation::get_frame(NomadIndex index) const {
    if (index >= m_frames.size()) {
        return nullptr;
    }

    return m_frames[index];
}

const Sprite* Animation::get_last_frame() const {
    return m_frames.back();
}

///////////////////////////////////////////////////////////////////////////////
// AnimationManager


AnimationManager::AnimationManager(ResourceManager* resources):
    m_resources(resources)
{}

NomadId AnimationManager::register_animation(
    const NomadString& name,
    const NomadString& variant,
    const NomadString& direction,
    const std::vector<Sprite*>& frames
) {
    auto animation_id = to_nomad_id(m_animations.size());

    m_animations.emplace_back(
        std::make_unique<Animation>(
            name,
            variant,
            direction,
            frames
        )
    );

    return animation_id;
}

const Animation* AnimationManager::get_animation(
    const NomadString& name,
    const NomadString& variant,
    const NomadString& direction
) {
    for (auto& animation : m_animations) {
        if (animation->name_is(name, variant, direction)) {
            return animation.get();
        }
    }

    // Animation does not exist. Create it.
    std::vector<Sprite*> frames;

    auto prefix = name + "-" + variant + "-" + direction + "-";

    m_resources->get_sprites()->get_sprites_by_prefix(prefix, frames);

    if (frames.empty()) {
        log::error("No frames found for animation prefix: " + prefix);

        return nullptr;
    }

    // Sort sprites by name.
    std::sort(frames.begin(), frames.end(), [](const Sprite* a, const Sprite* b) {
        return a->get_name() < b->get_name();
    });

    auto animation_id = register_animation(
        name,
        variant,
        direction,
        frames
    );

    return m_animations[animation_id].get();
}

} // nomad
