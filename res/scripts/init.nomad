# Init game
game.organization = "Chronogears"
game.name = t.game.title

# Init window
resolution.x = toInteger room.width
resolution.y = toInteger room.height

window.fps = 60
window.setResolution resolution.x resolution.y
window.setSize resolution.x resolution.y
window.setTitle $"Wishlair 1 ({resolution.x}x{resolution.y})"

# Load resources
global.font.default = game.loadFont "fonts/PressStart/PressStart2P-vaV7.ttf" 8

# Init scripts
init.forest
init.player
init.scene
init.ui

# Start initial scene
game.createScene "scenes.entrance"
