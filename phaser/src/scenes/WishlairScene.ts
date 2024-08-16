import {WishlairSprite} from '../entities/WishlairSprite'
import {Wishlair} from '../wishlair/Wishlair'
import Tilemap = Phaser.Tilemaps.Tilemap
import GameObject = Phaser.GameObjects.GameObject
import Group = Phaser.GameObjects.Group
import {LayerGameObject, LevelGameObject} from '../entities/LevelGameObject'

declare var __BUILD_TIME__: string

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

        this.level = new LevelGameObject(this, 0, 3)
        this.add.existing(this.level)

        this.wishlair.initializeScene(this)

        this.map = this.make.tilemap({key: this.sceneId})

        const tileset = this.map.addTilesetImage('wishlair', 'tiles')

        this.level.layers.forEach((layer, index) => {
            const layerName = `layer-${index}`

            const layerExists = this.map.layers.find(layer => layer.name === layerName) !== undefined

            if (layerExists) {
                const tileLayer = this.map.createLayer(layerName, tileset)

                layer.tiles.add(tileLayer)
            }

            // Setup entities
            const entityLayerName = `layer-${index}-entities`

            const entityLayerExists = this.map.layers.find(layer => layer.name === entityLayerName) !== undefined

            if (entityLayerExists) {
                const entityLayer = this.map.getObjectLayer(entityLayerName)

                entityLayer.objects.forEach(object => {
                    console.log(object)
                    const entity = this.createEntity(object.x, object.y, index, object.properties[0].value)
                })
            }
        })

        // const player = new Player(this, 100, 100)
        const player = this.createEntity(100, 100, 1, 'player')

        // DEV:
        this.add.text(10, 10, `Build: ${ __BUILD_TIME__ }`, {font: '16px Courier'})
    }

    createEntity(x: number, y: number, layer: number, controllerId: string) {
        const entitySprite = new WishlairSprite(this, x, y, controllerId)

        this.level.layers[layer].entities.add(entitySprite)

        return entitySprite
    }

    update() {
        this.level.layers.forEach(child => {
            const layer = child as LayerGameObject

            layer.entities.each((child: GameObject) => {
                const sprite = child as WishlairSprite

                sprite.tick()

                return true
            })
        })
    }
}