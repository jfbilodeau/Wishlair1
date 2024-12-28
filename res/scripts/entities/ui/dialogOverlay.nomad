fun initDialogOverlay x:float
    this.x = x

    this.sprite.name = "dialog-box-v"

    this.text.x = 80.0
    this.text.y = 85.0
    this.text.width = 240.0
    this.text.color = color.white
    this.text.alignment = alignment.topLeft
    this.text.lineSpacing = 10.0

    this.font = global.font.ui
end

fun entities.ui.dialogOverlayLeft
    initDialogOverlay 0.0
end

fun entities.ui.dialogOverlayRight
    initDialogOverlay toFloat window.width / 2
end
