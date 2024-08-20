import WishlairScene from '../scenes/WishlairScene'
import {WishlairSprite} from './WishlairSprite'
import Container = Phaser.GameObjects.Container
import GameObject = Phaser.GameObjects.GameObject

export class LayerGameObject extends Container {
    tiles = new Container(this.scene)
    entities = new Container(this.scene)

    constructor(public level: LevelGameObject) {
        super(level.scene)

        this.name = 'LayerGameObject'

        this.add(this.tiles)
        this.add(this.entities)
    }
}

export class LevelGameObject extends Container {
    roomX = 0
    roomY = 0
    root = new Container(this.scene)
    layers = [
        new LayerGameObject(this),
        new LayerGameObject(this),
        new LayerGameObject(this),
    ]

    constructor(public scene: WishlairScene) {
        super(scene)

        this.name = 'LevelGameObject'

        this.add(this.root)

        this.root.name = 'root'

        this.root.add(this.layers)
    }

    // createEntity(x: number, y: number, layer: number, controllerId: string) {
    //     const entitySprite = new WishlairSprite(this.scene, x, y, controllerId)
    //
    //     this.layers[layer].entities.add(entitySprite)
    //
    //     return entitySprite
    // }
}