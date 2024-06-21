import {Player} from '../entities/Player'

export default class WishlairScene extends Phaser.Scene {
    constructor() {
        super({
            key: 'wishlair'
        })
    }

    create() {
        const player = new Player(this, 100, 100)
    }
}