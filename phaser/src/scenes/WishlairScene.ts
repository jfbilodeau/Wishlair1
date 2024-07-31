import {WishlairSprite} from '../entities/WishlairSprite'
import {Wishlair} from '../wishlair/Wishlair'
import Tilemap = Phaser.Tilemaps.Tilemap

export default class WishlairScene extends Phaser.Scene {
    public wishlair: Wishlair
    private map: Tilemap

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

        this.wishlair.initializeScene(this)

        this.map = this.make.tilemap({key: this.sceneId})

        const tileset = this.map.addTilesetImage('wishlair', 'tiles')

        const groundLayer = this.map.createLayer('ground', tileset)

        // const player = new Player(this, 100, 100)
        const player = this.createEntity(100, 100, 'player')
    }

    createEntity(x: number, y: number, controllerId: string) {
        const entitySprite = new WishlairSprite(this, x, y, controllerId)

        return entitySprite
    }
}