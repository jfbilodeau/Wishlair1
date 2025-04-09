const border.margin = 10.0 / 2.0
const border.size = 10.0

# Name of the event to trigger when the borders need to be repositioned.
const on.repositionBorders = "repositionBorders"

fun createBorders
    # Create for layer 0
    scene.createEntity "entities.border.north" 0.0 0.0 0
    scene.createEntity "entities.border.south" 0.0 0.0 0
    scene.createEntity "entities.border.east" 0.0 0.0 0
    scene.createEntity "entities.border.west" 0.0 0.0 0

    # Create for layer 1
    scene.createEntity "entities.border.north" 0.0 0.0 1
    scene.createEntity "entities.border.south" 0.0 0.0 1
    scene.createEntity "entities.border.east" 0.0 0.0 1
    scene.createEntity "entities.border.west" 0.0 0.0 1

    # Create for layer 2
    scene.createEntity "entities.border.north" 0.0 0.0 2
    scene.createEntity "entities.border.south" 0.0 0.0 2
    scene.createEntity "entities.border.east" 0.0 0.0 2
    scene.createEntity "entities.border.west" 0.0 0.0 2

    # Create for layer 3
    scene.createEntity "entities.border.north" 0.0 0.0 3
    scene.createEntity "entities.border.south" 0.0 0.0 3
    scene.createEntity "entities.border.east" 0.0 0.0 3
    scene.createEntity "entities.border.west" 0.0 0.0 3

    # Create for layer 4
    scene.createEntity "entities.border.north" 0.0 0.0 4
    scene.createEntity "entities.border.south" 0.0 0.0 4
    scene.createEntity "entities.border.east" 0.0 0.0 4
    scene.createEntity "entities.border.west" 0.0 0.0 4

    repositionBorders
end

fun initBorderEntity room.x:float room.y:float
    this.sensor = true
    this.body.rectangle body.static this.width this.height
    this.mask = mask.ui
    this.collisionMask = mask.player

    this.on.collisionStart fun
        log.info "Contact"

        # Reposition room index.
        scene.room.x = scene.room.x + this.room.deltaX
        scene.room.y = scene.room.y + this.room.deltaY

        select.byName "camera"

        other.pauseOthers

        other.moveTo room.current.left + room.middle.x room.current.top + room.middle.y 600.0 fun
            log.info $"Move to: {scene.room.x}, {scene.room.y}"

            repositionBorders

            scene.unpauseAll
        end
    end

    #log.info $"x: {this.x}, y: {this.y}, width: {this.width}, height: {this.height}"
end

fun repositionBorders
    scene.events.trigger on.repositionBorders

#    select.all.byName "border.north"
#    other.x = room.x + room.middle.x
#    other.y = room.y + (-border.size) - border.margin
#    other.room.x = room.x
#    other.room.y = room.y - room.height
#
#    other.x = 10.0
#    other.y = room.y + 10.0
#
#    select.all.byName "border.south"
#    other.x = room.x + room.middle.x
#    other.y = room.y + room.height + border.margin + border.size
#    other.room.x = room.x
#    other.room.y = room.y + room.height
#
#    select.all.byName "border.east"
#    other.x = room.x + room.width + border.size + border.margin
#    other.y = room.y + room.middle.y
#    other.room.x = room.x + room.width
#    other.room.y = room.y
#
#    select.all.byName "border.west"
#    other.x = room.x + (-border.size) - border.margin
#    other.y = room.y + room.middle.y
#    other.room.x = room.x - room.width
#    other.room.y = room.y
end

fun entities.border.north
    this.name = "border.north"

    this.x = room.middle.x
    this.y = -border.size - border.margin
    this.width = room.width + border.margin * 2.0
    this.height = border.size

    this.room.deltaX = 0.0
    this.room.deltaY = -1.0

    initBorderEntity 0.0 (-room.height)

    this.on on.repositionBorders fun
        this.x = room.current.left + room.middle.x
        this.y = room.current.top + (-border.size) - border.margin
    end
end

fun entities.border.south
    this.name = "border.south"

    this.x = room.width / 2.0
    this.y = room.height + border.margin + border.size
    this.width = room.width + border.margin * 2.0
    this.height = border.size

    this.room.deltaX = 0.0
    this.room.deltaY = 1.0

    initBorderEntity 0.0 room.height

    this.on on.repositionBorders fun
        this.x = room.current.left + room.middle.x
        this.y = room.current.top + room.height + border.margin + border.size
    end
end

fun entities.border.east
    this.name = "border.east"

    this.x = room.width + border.size + border.margin
    this.y = room.height / 2.0
    this.width = border.size
    this.height = room.height + border.margin * 2.0

    this.room.deltaX = 1.0
    this.room.deltaY = 0.0

    initBorderEntity room.width 0.0

    this.on on.repositionBorders fun
        this.x = room.current.left + room.width + border.size + border.margin
        this.y = room.current.top + room.middle.y
    end
end

fun entities.border.west
    this.name = "border.west"

    this.x = -border.size - border.margin
    this.y = room.height / 2.0
    this.width = border.size
    this.height = room.height + border.margin * 2.0

    this.room.deltaX = -1.0
    this.room.deltaY = 0.0

    initBorderEntity (-room.width) 0.0

    this.on on.repositionBorders fun
        this.x = room.current.left + (-border.size) - border.margin
        this.y = room.current.top + room.middle.y
    end
end
