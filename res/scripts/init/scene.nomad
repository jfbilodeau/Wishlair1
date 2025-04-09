# Size constants
const tile.width = 32.0
const tile.height = 32.0

const room.tileWidth = 25.0
const room.tileHeight = 15.0

const room.width = room.tileWidth * tile.width
const room.height = room.tileHeight * tile.height

const room.middle.x = room.width / 2.0
const room.middle.y = room.height / 2.0

const on.changeRoom = "camera.changeRoom"

# Initialize scene room location
scene.room.x = 0.0
scene.room.y = 0.0

fun camera.scrollTo x:float y:float
    select.all.byName "camera"

    other.x = x
    other.y = y

    scene.events.trigger on.changeRoom
end

fun room.current.left
    return scene.room.x * room.width
end

fun room.current.top
    return scene.room.y * room.height
end

# Load tileset image
game.loadImage "world/tileset"

