this.name = "player"

this.animation.name = "daughter"
this.animation.variant = "idle"
this.animation.direction = "south"
this.animation.speed = 20
this.layer = 1

this.direction = cardinal.south
this.moveLeft = false
this.moveRight = false
this.moveUp = false
this.moveDown = false

this.onPress action.playerLeft fun
    this.moveLeft = true
end

this.onRelease action.playerLeft fun
    this.moveLeft = false
end

this.onPress action.playerRight fun
    this.moveRight = true
end

this.onRelease action.playerRight fun
    this.moveRight = false
end

this.onPress action.playerUp fun
    this.moveUp = true
end

this.onRelease action.playerUp fun
    this.moveUp = false
end

this.onPress action.playerDown fun
    this.moveDown = true
end

this.onRelease action.playerDown fun
    this.moveDown = false
end

this.onFrame fun
    action = IDLE
    this.animation.variant = "idle"

    if this.moveLeft 
        this.x = this.x - 1.0
        this.direction = cardinal.west
        this.animation.variant = "move"
        this.animation.direction = "west"
        action = MOVING
    endIf

    if this.moveRight  
        this.x = this.x + 1.0
        this.direction = cardinal.east
        this.animation.variant = "move"
        this.animation.direction = "east"
        action = MOVING
    endIf

    if this.moveUp 
        this.y = this.y - 1.0
        this.direction = cardinal.north
        this.animation.variant = "move"
        this.animation.direction = "north"
        action = MOVING
    endIf

    if this.moveDown   
        this.y = this.y + 1.0
        this.direction = cardinal.south
        this.animation.variant = "move"
        this.animation.direction = "south"
        action = MOVING
    endIf
end