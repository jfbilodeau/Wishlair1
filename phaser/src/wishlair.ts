import 'phaser'

export class Wishlair {
    private game: Phaser.Game

    constructor() {
        const config = {
            type: Phaser.AUTO,
            width: 800,
            height: 600,
            scene: {},
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
                mode: Phaser.Scale.ENVELOP,
                center: Phaser.Scale.CENTER_BOTH
            }
        }

        this.game = new Phaser.Game(config);
    }
}