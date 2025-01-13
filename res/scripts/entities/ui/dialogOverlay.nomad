fun initFloatingDialog
    this.z = 10000.0 # Make it always on top
    this.sensor = true
    this.mask = mask.ui
    this.collisionMask = mask.player
    this.body.rectangle body.dynamic toFloat window.width / 2 toFloat window.height

    this.sprite.name = "dialog-box-v"
    this.repositionOnAnchor toFloat this.sprite.width / 2 toFloat this.sprite.height / 2

    this.text.x = -58.0
    this.text.y = -54.0
    this.text.width = 240.0
    this.text.color = color.white
    this.text.alignment = alignment.centerLeft
    this.text.lineSpacing = 10.0

    this.font = global.font.ui

    this.onCollisionStart fun
        this.visible = false
    end

    this.onCollisionEnd fun
        this.visible = true
    end
end

fun entities.ui.dialogFloatVertical
    initFloatingDialog
end
