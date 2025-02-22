const border.margin = 10.0 / 2.0
const border.size = 64.0

fun createBorders
    scene.createEntity "entities.border.north" 0.0 0.0 0
    scene.createEntity "entities.border.south" 0.0 0.0 0
    scene.createEntity "entities.border.east" 0.0 0.0 0
    scene.createEntity "entities.border.west" 0.0 0.0 0
end

fun initBorderEntity
    this.body.rectangle body.static this.width this.height
    this.mask = mask.solid
    this.collisionMask = mask.player

    this.onCollisionStart fun
        log.info "Contact"

        if this.y < 0.0
            scene.camera.y = scene.camera.y - border.size
        else
            scene.camera.y = scene.camera.y + border.size
        endIf
    end

    log.info $"x: {this.x}, y: {this.y}, width: {this.width}, height: {this.height}"
end

fun entities.border.north
    this.name = "border.north"
    this.x = 0.0
    this.y = -border.margin - border.size
    this.width = room.width + border.margin * 2.0
    this.height = border.size

    initBorderEntity
end

fun entities.border.south
    this.name = "border.south"
    this.x = 0.0
    this.y = room.height + border.margin
    this.width = room.width + border.margin * 2.0
    this.height = border.size

    initBorderEntity
end

fun entities.border.east
    this.name = "border.east"
    this.x = room.width + border.margin
    this.y = 0.0
    this.width = border.size
    this.height = room.height + border.margin * 2.0

    initBorderEntity
end

fun entities.border.west
    this.name = "border.west"
    this.x = -border.margin - border.size
    this.y = 0.0
    this.width = border.size
    this.height = room.height + border.margin * 2.0

    initBorderEntity
end