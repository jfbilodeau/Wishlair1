import {Player} from '../entities/Player'
import {Entity} from '../entities/Entity'
import {EntitySprite} from '../entities/EntitySprite'
import {Wishlair} from '../wishlair/Wishlair'

export default class WishlairScene extends Phaser.Scene {
    constructor(public wishlair: Wishlair) {
        super({
            key: 'wishlair'
        })
    }

    create() {
        // const player = new Player(this, 100, 100)
        const player = this.createEntity(100, 100, 'player')
    }

    createEntity(x: number, y: number, controllerId: string) {
        const entitySprite = new EntitySprite(this, x, y, controllerId)

        return entitySprite
    }
}