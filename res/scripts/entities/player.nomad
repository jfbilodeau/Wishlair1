const player.name = "player"
const player.radius = 10.0

this.name = player.name

this.animation.name = "daughter"
this.animation.variant = "idle"
this.animation.direction = "south"
this.sprite.x = 32.0
this.sprite.y = 52.0
this.animation.speed = 10

this.direction = cardinal.south
this.moveLeft = false
this.moveRight = false
this.moveUp = false
this.moveDown = false
this.move.speed = 2.0 * 60.0

this.y = 200.0

this.body.circle body.dynamic player.radius
this.mask = mask.player
this.collisionMask = mask.solid | mask.ui

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
    this.velocity.clear

    if this.moveLeft 
        #this.x = this.x - this.move.speed
        this.velocity.x = -this.move.speed
        this.direction = cardinal.west
        this.animation.variant = "move"
        this.animation.direction = "west"
        action = MOVING
    endIf

    if this.moveRight  
        #this.x = this.x + this.move.speed
        this.velocity.x = this.move.speed
        this.direction = cardinal.east
        this.animation.variant = "move"
        this.animation.direction = "east"
        action = MOVING
    endIf

    if this.moveUp 
        #this.y = this.y - this.move.speed
        this.velocity.y = -this.move.speed
        this.direction = cardinal.north
        this.animation.variant = "move"
        this.animation.direction = "north"
        action = MOVING
    endIf

    if this.moveDown   
        #this.y = this.y + this.move.speed
        this.velocity.y = this.move.speed
        this.direction = cardinal.south
        this.animation.variant = "move"
        this.animation.direction = "south"
        action = MOVING
    endIf

    this.z = this.y

    #log.info $"this.z: {this.z}, this.y: {this.y}"
    mask = this.getCollidingMask
    this.text.color = color.white
    this.setText global.font.default $"x: {this.x}, y: {this.y}"
end