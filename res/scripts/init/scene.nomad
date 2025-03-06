# Size constants
const tile.width = 32.0
const tile.height = 32.0

const room.tileWidth = 25.0
const room.tileHeight = 15.0

const room.width = room.tileWidth * tile.width
const room.height = room.tileHeight * tile.height

const camera.on.changeRoom = "camera.changeRoom"

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

