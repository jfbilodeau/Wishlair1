const border.margin = 10.0 / 2.0
const border.size = 10.0

fun createBorders
    scene.createEntity "entities.border.north" 0.0 0.0 1
    scene.createEntity "entities.border.south" 0.0 0.0 1
    scene.createEntity "entities.border.east" 0.0 0.0 1
    scene.createEntity "entities.border.west" 0.0 0.0 1
end

fun initBorderEntity offsetX:float offsetY:float
    this.sensor = true
    this.body.rectangle body.static this.width this.height
    this.mask = mask.ui
    this.collisionMask = mask.player

    this.roomOffsetX = offsetX
    this.roomOffsetY = offsetY

    this.on.collisionStart fun
        log.info "Contact"

#        if this.y < 0.0
#            scene.camera.y = scene.camera.y - border.size
#        else
#            scene.camera.y = scene.camera.y + border.size
#        endIf

        select.byName "camera"

        other.trigger "changeRoom"
    end

    #log.info $"x: {this.x}, y: {this.y}, width: {this.width}, height: {this.height}"
end

fun entities.border.north
    this.name = "border.north"

    this.x = room.width / 2.0
    this.y = -border.size - border.margin
    this.width = room.width + border.margin * 2.0
    this.height = border.size

    initBorderEntity 0.0 (-room.height)
end

fun entities.border.south
    this.name = "border.south"

    this.x = room.width / 2.0
    this.y = room.height + border.margin + border.size
    this.width = room.width + border.margin * 2.0
    this.height = border.size

    initBorderEntity 0.0 room.height
end

fun entities.border.east
    this.name = "border.east"

    this.x = room.width + border.size + border.margin
    this.y = room.height / 2.0
    this.width = border.size
    this.height = room.height + border.margin * 2.0

    initBorderEntity room.width 0.0
end

fun entities.border.west
    this.name = "border.west"

    this.x = -border.size - border.margin
    this.y = room.height / 2.0
    this.width = border.size
    this.height = room.height + border.margin * 2.0

    initBorderEntity (-room.width) 0.0
end