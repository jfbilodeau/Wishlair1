import WishlairScene from './WishlairScene'
import {WishlairSprite} from '../entities/WishlairSprite'
import Container = Phaser.GameObjects.Container
import GameObject = Phaser.GameObjects.GameObject

export class LayerGameObject extends GameObject {
    tiles: Container
    entities: Container

    constructor(public level: LevelGameObject) {
        super(level.scene, 'LayerGameObject')

        this.tiles = this.scene.add.container()
        this.entities = this.scene.add.container()
    }
}

export class LevelGameObject extends GameObject {
    roomX = 0
    roomY = 0
    root: Container
    layers: LayerGameObject[]

    constructor(public scene: WishlairScene, roomX: number, roomY: number) {
        super(scene, 'WorldGameObject')

        this.roomX = roomX
        this.roomY = roomY

        this.root = this.scene.add.container(0, 0)

        this.layers = [
            new LayerGameObject(this),
            new LayerGameObject(this),
            new LayerGameObject(this),
        ]

        // this.layers.forEach(layer => {
        //     this.root.add(layer)
        // })
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