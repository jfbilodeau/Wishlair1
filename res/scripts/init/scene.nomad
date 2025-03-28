# Size constants
const tile.width = 32.0
const tile.height = 32.0

const room.tileWidth = 25.0
const room.tileHeight = 15.0

const room.width = room.tileWidth * tile.width
const room.height = room.tileHeight * tile.height

const room.middle.x = room.width / 2.0
const room.middle.y = room.height / 2.0

const camera.on.changeRoom = "camera.changeRoom"

# Initialize scene room location
scene.room.x = 0.0
scene.room.y = 0.0

fun camera.scrollTo roomX:float roomY:float
    select.all.byName "camera"

    other.roomX = roomX
    other.roomY = roomY

    other.trigger camera.on.changeRoom
end

fun room.current.x
    return scene.camera.x - room.width / 2.0
end

fun room.current.y
    return scene.camera.y - room.height / 2.0
end

# Load tileset image
game.loadImage "world/tileset"

