import {LoadScene} from './scenes/LoadScene'

// const wishlair = new Wishlair()

const config = {
    type: Phaser.AUTO,
    width: 800,
    height: 600,
    pixelArt: true,
    scene: LoadScene,
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

