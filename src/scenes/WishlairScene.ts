import {WishlairSprite} from '../entities/WishlairSprite'
import {Wishlair} from '../wishlair/Wishlair'
import {SceneController} from './controllers/SceneController'
import {PlayController} from './controllers/PlayController'
import Tilemap = Phaser.Tilemaps.Tilemap
import GameObject = Phaser.GameObjects.GameObject
import Camera = Phaser.Cameras.Scene2D.Camera
import Group = Phaser.GameObjects.Group
import * as Constants from '../wishlair/Constants'
import Tileset = Phaser.Tilemaps.Tileset
import {LevelLayer} from '../entities/LevelLayer'

export default class WishlairScene extends Phaser.Scene {
    wishlair: Wishlair
    map: Tilemap
    tileset: Tileset
    camera: Camera
    roomX = 0
    roomY = 0
    layers: LevelLayer[] = []
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

        this.wishlair.initializeScene(this)

        this.map = this.make.tilemap({key: this.sceneId})

        this.physics.world.setBounds(0, 0, this.map.widthInPixels, this.map.heightInPixels)

        this.tileset = this.map.addTilesetImage('wishlair', 'tiles')

        for (const index of [0, 1, 2]) {
            const layer = new LevelLayer(this, index)

            this.layers.push(layer)

            this.layers[index].initialize()
        }

        // Setup camera
        this.camera = this.cameras.main
        this.camera.setBounds(0, 0, this.map.widthInPixels, this.map.heightInPixels)

        const player = this.createSprite('player', 100, 100 + (Constants.roomHeightInPixels * 2), 64, 64, 1, 'player')

        this.setRoom(0, 3)
    }

    createSprite(
        id: string,
        x: number,
        y: number,
        width: number,
        height: number,
        layer: number,
        controllerId: string
    ) {
        const sprite = new WishlairSprite(this, id, x, y, width, height, layer, controllerId)

        this.add.existing(sprite)
        this.physics.add.existing(sprite)
        this.activeEntities.push(sprite)
        this.computeDepth(sprite)
        this.layers[layer].addSprite(sprite)

        return sprite
    }

    update() {
        this.controller.tick(this.wishlair, this)

        if (this.nextController) {
            this.controller = this.nextController
            this.nextController = null
        }

        for (const activeEntity of this.activeEntities) {
            this.computeDepth(activeEntity)
        }
    }

    setRoom(roomX: number, roomY: number) {
        this.roomX = roomX
        this.roomY = roomY

        const roomPixelX = roomX * Constants.roomWidthInPixels
        const roomPixelY = roomY * Constants.roomHeightInPixels

        this.camera.setScroll(roomPixelX, roomPixelY)
    }

    setController(nextController: SceneController) {
        this.nextController = nextController
    }

    private computeDepth(sprite: WishlairSprite) {
        sprite.depth = sprite.y + sprite.baseline
    }
}
