export class System {
    readonly tileWidth = 32
    readonly tileHeight = 32

    readonly roomWidth = 25
    readonly roomHeight = 15

    readonly roomWidthInPixels = this.roomWidth * this.tileWidth
    readonly roomHeightInPixels = this.roomHeight * this.tileHeight
}

