import {Player} from "../entities/Player"
import WishlairScene from './WishlairScene'

export class LoadScene extends WishlairScene {
    preload() {
        this.load.atlas('daughter', 'assets/daughter.png', 'assets/daughter.json')
        this.load.atlas('forest', 'assets/forest.png', 'assets/forest.json')
    }

    create() {
        const player = new Player(this, 100, 100)

        console.log(this.game.scale)
    }
}