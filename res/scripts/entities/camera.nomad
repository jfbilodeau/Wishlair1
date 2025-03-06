this.name = "camera"

# Hack: the Tiled camera 'tile' is 64x64. Need to move camera down by 64px.
this.y = this.y + 64.0

log.info $"camera: {this.x}, {this.y}"

scene.camera.follow this

this.on "changeRoom" fun
    log.info "Change room"
end