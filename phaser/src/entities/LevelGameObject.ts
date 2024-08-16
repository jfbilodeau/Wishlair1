import WishlairScene from '../scenes/WishlairScene'
import {WishlairSprite} from './WishlairSprite'
import Container = Phaser.GameObjects.Container
import GameObject = Phaser.GameObjects.GameObject

export class LayerGameObject extends Container {
    tiles = new Container(this.scene)
    entities = new Container(this.scene)

    constructor(public level: LevelGameObject) {
        super(level.scene)

        this.add(this.tiles)
        this.add(this.entities)
    }
}

export class LevelGameObject extends GameObject {
    roomX = 0
    roomY = 0
    root = this.scene.add.container(0, 0)
    layers = [
        new LayerGameObject(this),
        new LayerGameObject(this),
        new LayerGameObject(this),
    ]

    constructor(public scene: WishlairScene, roomX: number, roomY: number) {
        super(scene, 'WorldGameObject')

        this.layers.forEach(layer => {
            this.root.add(layer)
        })

        this.setRoom(roomX, roomY)
    }

    setRoom(roomX: number, roomY: number) {
        this.roomX = roomX
        this.roomY = roomY

        this.root.x = -roomX * this.scene.wishlair.system.roomWidth
        this.root.y = -roomY * this.scene.wishlair.system.roomHeight
    }

    createEntity(x: number, y: number, layer: number, controllerId: string) {
        const entitySprite = new WishlairSprite(this.scene, x, y, controllerId)

        this.layers[layer].entities.add(entitySprite)

        return entitySprite
    }
}