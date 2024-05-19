export class LoadScene extends Phaser.Scene {
    preload() {
        this.load.image('logo', 'assets/logo.png');
    }

    create() {
        this.add.image(400, 300, 'logo');
    }
}