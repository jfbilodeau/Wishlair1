scene.loadInputMapping "mappings/player"

#scene.createEntity "entities.player" toFloat window.width / 2 1260.0 1
scene.createEntity "entities.player" toFloat window.width / 2 toFloat window.height / 2 1
scene.createEntity "entities.test.internalTest" 0.0 0.0 4

scene.wall.mask = mask.solid

scene.loadMap "world/wishlair-entrance" "world/tileset"


