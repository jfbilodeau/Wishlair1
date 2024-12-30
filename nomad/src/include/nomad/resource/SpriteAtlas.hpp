//
// Created by Jean-François Bilodeau on 2023-06-17.
//

#ifndef NOMAD_SPRITEATLAS_H
#define NOMAD_SPRITEATLAS_H

#include "nomad/geometry/Rectangle.hpp"

#include "nomad/resource/Resource.hpp"

namespace nomad {

class SpriteAtlas {

};

void load_sprite_atlas(ResourceManager* resource_manager, const NomadString& resource_name);

} // nomad

#endif //NOMAD_SPRITEATLAS_H
