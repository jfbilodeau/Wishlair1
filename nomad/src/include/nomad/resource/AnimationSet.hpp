//
// Created by jfbil on 2024-02-05.
//

#ifndef ANIMATIONSET_HPP
#define ANIMATIONSET_HPP

#include "Resource.hpp"

#include <vector>

namespace nomad {

// Forward declarations
class Animation;

class AnimationSet final : public Resource {
public:
    explicit AnimationSet(const NomadString& name) : Resource(name);
    ~AnimationSet() override = default;

    void add_animation(NomadId action_id, NomadId variant_id, NomadId animation_id);
    [[nodiscard]] NomadId get_animation_id(NomadId action_id, NomadId variant_id) const;
    [[nodiscard]] Animation* get_animation(NomadId action_id, NomadId variant_id) const;

private:
    struct AnimationVariant {
        NomadId variant_id;
        NomadId animation_id;
        Animation* animation;
    };

    struct ActionAnimation {
        NomadId action_id;
        std::vector<AnimationVariant> variants;
    };

    ActionAnimation* get_action(NomadId action_id);
    const ActionAnimation* get_action(NomadId action_id) const;

    AnimationVariant* get_variant(ActionAnimation* action, NomadId variant_id);
    const AnimationVariant* get_variant(ActionAnimation* action, NomadId variant_id) const;

    std::vector<ActionAnimation> m_actions;
};

} // nomad

#endif //ANIMATIONSET_HPP
