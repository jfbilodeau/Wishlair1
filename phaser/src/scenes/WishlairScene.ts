import {WishlairSprite} from '../entities/WishlairSprite'
import {Wishlair} from '../wishlair/Wishlair'

export default class WishlairScene extends Phaser.Scene {
    public wishlair: Wishlair

    constructor() {
        super({
            key: 'wishlair'
        })
    }

    create() {
        this.wishlair = this.registry.get('wishlair') as Wishlair

        this.wishlair.initializeScene(this)

        // const player = new Player(this, 100, 100)
        const player = this.createEntity(100, 100, 'player')
    }

    createEntity(x: number, y: number, controllerId: string) {
        const entitySprite = new WishlairSprite(this, x, y, controllerId)

        return entitySprite
    }
}