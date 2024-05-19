# Init game
game.organization = "Chronogears"
game.name = "Wishlair 1"

# Init window
resolution.x = 640
resolution.y = 480

window.fps = 60
window.setResolution resolution.x resolution.y
window.setSize resolution.x resolution.y
window.setTitle $"Wishlair 1"

# Load resources
global.font.default = game.loadFont "fonts/PressStart/PressStart2P-vaV7.ttf" 8

# Init scripts
init.player

# Start initial scene
game.createScene "scenes.test.player"