import {LoadScene} from './scenes/LoadScene'
import WishlairScene from './scenes/WishlairScene'
import {Wishlair} from './wishlair/Wishlair'

const config = {
    type: Phaser.AUTO,
    width: 800,
    height: 480,
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

const wishlair = new Wishlair(game)

game.registry.set('wishlair', wishlair)

const debugWindow = window as any

debugWindow.wishlair = wishlair