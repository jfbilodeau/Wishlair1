import 'phaser'
import {LoadScene} from "./scenes/load-scene";

export class Wishlair {
    private game: Phaser.Game

    constructor() {
        const config = {
            type: Phaser.AUTO,
            width: 800,
            height: 600,
            scene: LoadScene,
            physics: {
                default: 'arcade',
                arcade: {
                    gravity: {
                        x: 0.0,
                        y: 200.0,
                    }
                }
            },
            scale: {
                parent: 'game',
                mode: Phaser.Scale.FIT,
                autoCenter: Phaser.Scale.CENTER_BOTH
            },
        }

        this.game = new Phaser.Game(config);
    }
}