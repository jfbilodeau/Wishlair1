import {LoadScene} from './scenes/LoadScene'
import WishlairScene from './scenes/WishlairScene'

// const wishlair = new Wishlair()

const config = {
    type: Phaser.AUTO,
    width: 480,
    height: 320,
    pixelArt: true,
    scene: [LoadScene, WishlairScene],
    physics: {
        default: 'arcade',
        arcade: {
            // gravity: {
            //     x: 0.0,
            //     y: 200.0,
            // }
        }
    },
    scale: {
        parent: 'game',
        mode: Phaser.Scale.FIT,
        autoCenter: Phaser.Scale.CENTER_BOTH
    },
}

const game = new Phaser.Game(config)

