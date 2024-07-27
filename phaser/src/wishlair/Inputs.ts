import {Game, Scene} from 'phaser'
import CursorKeys = Phaser.Types.Input.Keyboard.CursorKeys
import {Wishlair} from './Wishlair'
import Key = Phaser.Input.Keyboard.Key

export class Inputs {
    private cursorKeys: CursorKeys

    public left1: Key
    public left2: Key
    public right1: Key
    public right2: Key
    private up1: Key
    private up2: Key
    private down1: Key
    private down2: Key

    constructor (private wishlair: Wishlair) {

    }

    public reset(scene: Scene) {
        this.cursorKeys = scene.input.keyboard.createCursorKeys()

        this.left1 = scene.input.keyboard.addKey(this.wishlair.config.keyMap.left1)
        this.left2 = scene.input.keyboard.addKey(this.wishlair.config.keyMap.left2)

        this.right1 = scene.input.keyboard.addKey(this.wishlair.config.keyMap.right1)
        this.right2 = scene.input.keyboard.addKey(this.wishlair.config.keyMap.right2)

        this.up1 = scene.input.keyboard.addKey(this.wishlair.config.keyMap.up1)
        this.up2 = scene.input.keyboard.addKey(this.wishlair.config.keyMap.up2)

        this.down1 = scene.input.keyboard.addKey(this.wishlair.config.keyMap.down1)
        this.down2 = scene.input.keyboard.addKey(this.wishlair.config.keyMap.down2)

    }
}