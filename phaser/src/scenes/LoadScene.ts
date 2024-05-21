export class LoadScene extends Phaser.Scene {
    preload() {
        this.load.atlas('daughter', 'assets/daughter.png', 'assets/daughter.json');
    }

    create() {
        this.add.image(400, 300, 'daughter', 'daughter-idle-south.png');
    }
}