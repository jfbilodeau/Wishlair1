this.name = "player"

this.setAnimation "player/idleSouth" 12 true false

this.direction = SOUTH
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

    if this.moveLeft 
        this.x = this.x - 1
        this.direction = WEST
        action = MOVING
    endIf

    if this.moveRight  
        this.x = this.x + 1
        this.direction = EAST
        action = MOVING
    endIf

    if this.moveUp 
        this.y = this.y - 1
        this.direction = NORTH
        action = MOVING
    endIf

    if this.moveDown   
        this.y = this.y + 1
        this.direction = SOUTH
        action = MOVING
    endIf
end