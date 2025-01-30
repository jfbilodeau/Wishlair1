# Size constants
const tile.width = 32
const tile.height = 32

const room.tileWidth = 25
const room.tileHeight = 15

const room.width = room.tileWidth * tile.width
const room.height = room.tileHeight * tile.height

# Load tileset image
game.loadImage "world/tileset"
