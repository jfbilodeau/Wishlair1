import {WishlairSprite} from '../entities/WishlairSprite'
import {Wishlair} from '../wishlair/Wishlair'
import {SceneController} from './controllers/SceneController'
import {PlayController} from './controllers/PlayController'
import Tilemap = Phaser.Tilemaps.Tilemap
import GameObject = Phaser.GameObjects.GameObject
import TilemapLayer = Phaser.Tilemaps.TilemapLayer
import Camera = Phaser.Cameras.Scene2D.Camera

const LayerSize = 10000

export default class WishlairScene extends Phaser.Scene {
    wishlair: Wishlair
    map: Tilemap
    // level: LevelGameObject
    camera: Camera
    roomX = 0
    roomY = 0
    // Props are temporary visual objects that are not part of the game logic
    props: GameObject[] = []
    // Active entities are entities that receive ticks
    activeEntities: WishlairSprite[] = []

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

        // this.level = new LevelGameObject(this)
        // this.add.existing(this.level)

        this.wishlair.initializeScene(this)

        this.map = this.make.tilemap({key: this.sceneId})

        const tileset = this.map.addTilesetImage('wishlair', 'tiles')

        // this.level.layers.forEach((layer, index) => {
        for (const index of [0, 1, 2]) {
            const layerName = `layer-${index}`

            const layerIndex = this.map.layers.findIndex(layer => layer.name === layerName)

            if (layerIndex !== -1) {
                // const tileLayer = new TilemapLayer(this, this.map, layerIndex, tileset, 0, 0)
                const tileLayer = this.map.createLayer(layerName, tileset)

                // layer.tiles.add(tileLayer)
            }

            // Setup entities
            const entityLayerName = `${layerName}-entities`

            const entityLayerExists = this.map.objects.find(entityLayer => entityLayer.name === entityLayerName) !== undefined

            if (entityLayerExists) {
                const entityLayer = this.map.getObjectLayer(entityLayerName)

                entityLayer.objects.forEach(object => {
                    // Calculate object position
                    // const x = object.x + object.width / 2
                    // const y = object.y - object.height / 2
                    const entityX = object.x
                    const entityY = object.y - object.height

                    const entity = this.createEntity(
                        object.id.toString(),
                        entityX,
                        entityY,
                        object.width,
                        object.height,
                        index,
                        object.properties[0].value
                    )
                })
            }
        }

        // Setup camera
        this.camera = this.cameras.main
        this.camera.setBounds(0, 0, this.map.widthInPixels, this.map.heightInPixels)

        const player = this.createEntity('player', 100, 100 + (this.wishlair.system.roomHeightInPixels * 2), 64, 64, 1, 'player')

        this.setRoom(0, 3)
    }

    createEntity(
        id: string,
        x: number,
        y: number,
        width: number,
        height: number,
        layer: number,
        controllerId: string
    ) {
        const entitySprite = new WishlairSprite(this, id, x, y, width, height, controllerId)

        // this.level.layers[layer].entities.add(entitySprite)
        this.add.existing(entitySprite)
        this.activeEntities.push(entitySprite)

        return entitySprite.entity
    }

    update() {
        this.controller.tick(this.wishlair, this)

        if (this.nextController) {
            this.controller = this.nextController
            this.nextController = null
        }

        for (const activeEntity of this.activeEntities) {
            activeEntity.depth = activeEntity.y + (activeEntity.entity.layer * LayerSize)
        }

        // // Sort entities by their 'z' value
        // this.level.layers.forEach(layer => {
        //     layer.entities.sort('y')
        // })
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

    private computeDepth(entity: WishlairSprite) {
        return entity.y + entity.entity.layer * LayerSize
    }
}