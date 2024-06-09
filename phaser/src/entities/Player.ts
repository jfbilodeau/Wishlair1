import 'phaser'

export class Player extends Phaser.GameObjects.Sprite {
    private cursorKeys: Phaser.Types.Input.Keyboard.CursorKeys
    private left1: Phaser.Input.Keyboard.Key
    private left2: Phaser.Input.Keyboard.Key
    private right1: Phaser.Input.Keyboard.Key
    private right2: Phaser.Input.Keyboard.Key

    private moveDown = false
    private moveUp = false
    private moveLeft = false
    private moveRight = false

    private speed = 1

    constructor(scene: Phaser.Scene, x: number, y: number) {
        super(scene, x, y, 'daughter', 'daughter-idle-south.png')

        this.scene.add.existing(this)
        this.scene.physics.add.existing(this)

        this.setOrigin(0.5, 0.90)

        this.cursorKeys = this.scene.input.keyboard.createCursorKeys()

        this.left1 = this.scene.input.keyboard.addKey(scene.game.wishlair.config.keyMap.left1)
        this.left2 = this.scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.LEFT)

        this.right1 = this.scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.D)
        this.right2 = this.scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.RIGHT)

        this.scene.events.on("update", this.update, this)
    }

    update() {
        this.moveLeft = this.left1.isDown || this.left2.isDown
        this.moveRight = this.right1.isDown || this.right2.isDown

        if (this.moveLeft) {
            this.x -= 1
        }
        if (this.moveRight) {
            this.x += 1
        }
    }
}