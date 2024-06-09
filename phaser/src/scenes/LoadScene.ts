import {Player} from "../entities/Player"
import {Wishlair} from '../wishlair/Wishlair'

export class LoadScene extends Phaser.Scene {
    preload() {
        this.load.atlas('daughter', 'assets/daughter.png', 'assets/daughter.json')
    }

    create() {
        this.game.wishlair = new Wishlair()

        const player = new Player(this, 100, 100)
    }
}