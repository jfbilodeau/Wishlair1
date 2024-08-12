import WishlairScene from './WishlairScene'
import Group = Phaser.GameObjects.Group
import Container = Phaser.GameObjects.Container
import GameObject = Phaser.GameObjects.GameObject

export class LayerGameObject extends GameObject {

}

export class WorldGameObject extends GameObject {
    roomX = 0
    roomY = 0
    root: Container
    layers: Container[]

    constructor(public scene: WishlairScene, roomX: number, roomY: number) {
        super(scene, 'WorldGameObject')

        this.roomX = roomX
        this.roomY = roomY
    }

    create() {
        this.root = this.scene.add.container(0, 0)

        this.layers = [
            this.root.add(new Container(this.scene)),
            this.root.add(new Container(this.scene)),
            this.root.add(new Container(this.scene)),
        ]
    }

    setRoom(roomX: number, roomY: number) {
        this.roomX = roomX
        this.roomY = roomY

        this.root.x = -roomX * this.scene.wishlair.system.roomWidth
        this.root.y = -roomY * this.scene.wishlair.system.roomHeight
    }
}