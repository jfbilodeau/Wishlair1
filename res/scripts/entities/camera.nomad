this.name = "camera"

# Hack: the Tiled camera 'tile' is 64x64. Need to move camera down by 64px.
this.y = this.y + 64.0

# Set the position of the scene room.
scene.room.x = this.x - room.middle.x
scene.room.y = this.y - room.middle.y

scene.camera.follow this

# Setup borders
createBorders

this.on "changeRoom" fun
    log.info "Change room"
end