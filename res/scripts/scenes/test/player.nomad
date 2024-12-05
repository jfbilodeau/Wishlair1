scene.loadInputMapping "mappings/player"

#scene.createEntity "entities.player" 800.0 / 2.0 480.0 / 2.0 1
scene.createEntity "entities.player" toFloat window.resolution.x / 2 toFloat window.resolution.y / 2 1
scene.createEntity "entities.test.internalTest" 0.0 0.0 4
