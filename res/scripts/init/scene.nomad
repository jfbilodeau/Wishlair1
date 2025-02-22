# Size constants
const tile.width = 32.0
const tile.height = 32.0

const room.tileWidth = 25.0
const room.tileHeight = 15.0

const room.width = room.tileWidth * tile.width
const room.height = room.tileHeight * tile.height

# Load tileset image
game.loadImage "world/tileset"
