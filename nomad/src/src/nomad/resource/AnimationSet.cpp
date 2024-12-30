//
// Created by jfbil on 2024-02-05.
//

#include "../../../include/nomad/resource/AnimationSet.hpp"

namespace nomad {

void AnimationSet::add_animation(NomadId action_id, NomadId variant_id, NomadId animation_id) {
    auto action = get_action(action);

    if (action == nullptr) {
        m_actions.push_back(ActionAnimation{action_id, std::vector<AnimationVariant>()});

        action = &m_actions.back();
    }

    auto variant = get_variant(*action, variant_id);

    if (variant == nullptr) {
        action->variants.push_back(AnimationVariant{variant_id, animation_id, nullptr});
    } else {
        variant->animation_id = animation_id;
    }
}

NomadId AnimationSet::get_animation_id(NomadId action_id, NomadId variant_id) const {
    auto action = get_action(action_id);

    if (action == nullptr) {
        return NOMAD_INVALID_ID;
    }

    auto variant = get_variant(*action, variant);

    if (variant == nullptr) {
        return NOMAD_INVALID_ID;
    }

    return variant->animation_id;
}

Animation* AnimationSet::get_animation(NomadId action_id, NomadId variant_id) const {
}

AnimationSet::ActionAnimation* AnimationSet::get_action(NomadId action_id) {
    for (auto& action : m_actions) {
        if (action.action_id == action_id) {
            return &action;
        }
    }

    return nullptr;
}

const AnimationSet::ActionAnimation* AnimationSet::get_action(NomadId action_id) const {
    return const_cast<AnimationSet*>(this)->get_action(action_id);
}

AnimationSet::AnimationVariant* AnimationSet::get_variant(ActionAnimation* action, NomadId variant_id) {
    for (auto& variant : action->variants) {
        if (variant.variant_id == variant_id) {
            return &variant;
        }
    }

    return nullptr;
}

const AnimationSet::AnimationVariant* AnimationSet::get_variant(ActionAnimation* action, NomadId variant_id) const {
    return const_cast<AnimationSet*>(this)->get_variant(action, variant_id);
}

} // nomad