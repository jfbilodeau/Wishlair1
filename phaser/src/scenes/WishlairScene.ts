import {WishlairSprite} from '../entities/WishlairSprite'
import {Wishlair} from '../wishlair/Wishlair'
import Tilemap = Phaser.Tilemaps.Tilemap
import GameObject = Phaser.GameObjects.GameObject
import Group = Phaser.GameObjects.Group
import {LayerGameObject, LevelGameObject} from './LevelGameObject'

export default class WishlairScene extends Phaser.Scene {
    wishlair: Wishlair
    // private layers: Group[] = []
    map: Tilemap
    level: LevelGameObject

    constructor(public sceneId: string) {
        super({
            key: sceneId
        })
    }

    preload() {
        const tiledFileName = `assets/world/${this.sceneId}.tmj`

        this.load.tilemapTiledJSON(this.sceneId, tiledFileName)
    }

    create() {
        this.wishlair = this.registry.get('wishlair') as Wishlair

        console.log(this.wishlair)

        this.level = new LevelGameObject(this, 0, 0)

        this.wishlair.initializeScene(this)

        this.map = this.make.tilemap({key: this.sceneId})

        const tileset = this.map.addTilesetImage('wishlair', 'tiles')

        const groundLayer = this.map.createLayer('ground', tileset)

        this.level.layers[0].tiles.add(groundLayer)

        // const player = new Player(this, 100, 100)
        const player = this.createEntity(100, 100, 1, 'player')
    }

    createEntity(x: number, y: number, layer: number, controllerId: string) {
        const entitySprite = new WishlairSprite(this, x, y, controllerId)

        this.level.layers[layer].entities.add(entitySprite)

        return entitySprite
    }

    update() {
        this.level.layers.forEach(child => {
            var layer = child as LayerGameObject

            layer.entities.each((child: GameObject) => {
                const sprite = child as WishlairSprite

                sprite.tick()

                return true
            })
        })
    }
}