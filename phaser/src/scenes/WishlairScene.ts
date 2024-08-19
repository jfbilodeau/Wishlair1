import {WishlairSprite} from '../entities/WishlairSprite'
import {Wishlair} from '../wishlair/Wishlair'
import Tilemap = Phaser.Tilemaps.Tilemap
import GameObject = Phaser.GameObjects.GameObject
import Group = Phaser.GameObjects.Group
import {LayerGameObject, LevelGameObject} from '../entities/LevelGameObject'
import TilemapLayer = Phaser.Tilemaps.TilemapLayer
import Camera = Phaser.Cameras.Scene2D.Camera
import {SceneController} from './controllers/SceneController'
import {PlayController} from './controllers/PlayController'

// From Webpack DefinePlugin
declare var __BUILD_TIME__: string

export default class WishlairScene extends Phaser.Scene {
    wishlair: Wishlair
    map: Tilemap
    level: LevelGameObject
    camera: Camera
    roomX = 0
    roomY = 0

    private controller: SceneController
    private nextController?: SceneController = null

    constructor(public sceneId: string, controller: SceneController = new PlayController()) {
        super({
            key: sceneId
        })

        this.controller = controller
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
        this.controller.tick(this.wishlair, this)

        if (this.nextController) {
            this.controller = this.nextController
            this.nextController = null
        }
    }

    setRoom(roomX: number, roomY: number) {
        this.roomX = roomX
        this.roomY = roomY

        const roomPixelX = roomX * this.wishlair.system.roomWidthInPixels
        const roomPixelY = roomY * this.wishlair.system.roomHeightInPixels

        this.camera.setScroll(roomPixelX, roomPixelY)
    }

    setController(nextController: SceneController) {
        this.nextController = nextController
    }
}