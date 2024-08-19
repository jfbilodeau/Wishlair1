import {WishlairSprite} from '../entities/WishlairSprite'
import {Wishlair} from '../wishlair/Wishlair'
import Tilemap = Phaser.Tilemaps.Tilemap
import GameObject = Phaser.GameObjects.GameObject
import Group = Phaser.GameObjects.Group
import {LayerGameObject, LevelGameObject} from '../entities/LevelGameObject'
import TilemapLayer = Phaser.Tilemaps.TilemapLayer
import Camera = Phaser.Cameras.Scene2D.Camera

// From Webpack DefinePlugin
declare var __BUILD_TIME__: string

export default class WishlairScene extends Phaser.Scene {
    wishlair: Wishlair
    map: Tilemap
    level: LevelGameObject
    camera: Camera
    roomX = 0
    roomY = 0

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

        this.level = new LevelGameObject(this)
        this.add.existing(this.level)

        this.wishlair.initializeScene(this)

        this.map = this.make.tilemap({key: this.sceneId})

        const tileset = this.map.addTilesetImage('wishlair', 'tiles')

        this.level.layers.forEach((layer, index) => {
            const layerName = `layer-${index}`

            const layerIndex = this.map.layers.findIndex(layer => layer.name === layerName)

            if (layerIndex !== -1) {
                const tileLayer = new TilemapLayer(this, this.map, layerIndex, tileset, 0, 0)
                // const tileLayer = this.map.createLayer(layerName, tileset)

                layer.tiles.add(tileLayer)
            }

            // Setup entities
            const entityLayerName = `${layerName}-entities`

            const entityLayerExists = this.map.layers.find(layer => layer.name === entityLayerName) !== undefined

            if (entityLayerExists) {
                const entityLayer = this.map.getObjectLayer(entityLayerName)

                entityLayer.objects.forEach(object => {
                    console.log(object)
                    const entity = this.createEntity(object.id.toString(), object.x, object.y, index, object.properties[0].value)
                })
            }
        })

        // Setup camera
        this.camera = this.cameras.main
        this.camera.setBounds(0, 0, this.map.widthInPixels, this.map.heightInPixels)

        // const player = new Player(this, 100, 100)
        const player = this.createEntity('player', 100, 100 + (this.wishlair.system.roomHeightInPixels*2), 1, 'player')

        this.setRoom(0, 3)

        // DEV:
        this.add.text(10, 10, `Build: ${__BUILD_TIME__}`, {font: '16px Courier'})
    }

    createEntity(id: string, x: number, y: number, layer: number, controllerId: string) {
        const entitySprite = new WishlairSprite(this, id, x, y, controllerId)

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

    setRoom(roomX: number, roomY: number) {
        this.roomX = roomX
        this.roomY = roomY

        const roomPixelX = roomX * this.wishlair.system.roomWidthInPixels
        const roomPixelY = roomY * this.wishlair.system.roomHeightInPixels

        // this.level.root.x = roomPixelX
        // this.level.root.y = roomPixelY
        this.camera.setScroll(roomPixelX, roomPixelY)

        // Tilemap doesn't seem to follow the root container
        // Adjust the tilemap position manually
        // this.map.layers.forEach((layer, index) => {
        //     const layerGameObject = this.level.layers[index]
        //
        //     layerGameObject.tiles.list.forEach((tileLayer: TilemapLayer) => {
        //         tileLayer.x = roomPixelX
        //         tileLayer.y = roomPixelY
        //
        //         const tilemapLayer = tileLayer as TilemapLayer
        //
        //         tilemapLayer.
        //     })
        // })

        // console.log(this.root.x, this.root.y)
    }
}