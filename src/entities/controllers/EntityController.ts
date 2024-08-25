import {Entity, EntityBody} from '../Entity'
import {Scene} from 'phaser'
import {Wishlair} from '../../wishlair/Wishlair'
import {WishlairSprite} from '../WishlairSprite'

export abstract class EntityController {
    protected constructor() {
    }

    public initialize(wishlair: Wishlair, sprite: WishlairSprite) {
        this.onInitialize(wishlair, sprite)
    }

    public tick(wishlair: Wishlair, sprite: WishlairSprite) {
        this.onTick(wishlair, sprite)
    }

    protected abstract onInitialize(wishlair: Wishlair, sprite: WishlairSprite): void
    protected abstract onTick(wishlair: Wishlair, sprite: WishlairSprite): void
}