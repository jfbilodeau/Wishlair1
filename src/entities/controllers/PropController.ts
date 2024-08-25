import {Wishlair} from '../../wishlair/Wishlair'
import {EntityBody} from '../Entity'
import {EntityController} from './EntityController'
import {WishlairSprite} from '../WishlairSprite'
import {Cardinal} from '../../wishlair/Directions'

export class PropController extends EntityController {
    constructor(
        public animationId: string,
        public body: EntityBody,
    ) {
        super()
    }

    protected onInitialize(wishlair: Wishlair, sprite: WishlairSprite): void {
        // entity.originX = this.body.offsetX
        // entity.originY = this.body.offsetY
        sprite.entity.animationId = this.animationId
        sprite.setBody(this.body)

        // Reposition the prop so it's aligned to its origin.
        // sprite.x += sprite.entity.width * this.body.offsetX
        // sprite.y += sprite.entity.height * this.body.offsetY


    }

    protected onTick(wishlair: Wishlair, sprite: WishlairSprite) {
        // Nothing to do...
    }
}