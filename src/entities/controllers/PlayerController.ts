import {CollisionType, EntityBody} from '../Entity'
import {EntityController} from './EntityController'
import {Wishlair} from '../../wishlair/Wishlair'
import {Cardinal} from '../../wishlair/Directions'
import {WishlairSprite} from '../WishlairSprite'

export class PlayerController extends EntityController {
    private body = new EntityBody().setOffset(32, 52).setCircle(10).setCollision(CollisionType.Player)

    public constructor() {
        super()
    }

    protected override onInitialize(wishlair: Wishlair, sprite: WishlairSprite) {
        // entity.originX = 0.5
        // entity.originY = 50/64
        sprite.entity.width = 64
        sprite.entity.height = 64
        sprite.entity.direction = Cardinal.South
        sprite.setBody(this.body)
        sprite.entity.animationId = 'daughter-idle'
    }

    protected override onTick(wishlair: Wishlair, sprite: WishlairSprite) {
        const moveLeft = wishlair.inputs.left1.isDown || wishlair.inputs.left2.isDown
        const moveRight = wishlair.inputs.right1.isDown || wishlair.inputs.right2.isDown
        const moveUp = wishlair.inputs.up1.isDown || wishlair.inputs.up2.isDown
        const moveDown = wishlair.inputs.down1.isDown || wishlair.inputs.down2.isDown

        let velocityX = 0
        let velocityY = 0
        let direction: Cardinal = sprite.entity.direction

        if (moveLeft) {
            velocityX = -wishlair.inventory.player.speed
            direction = Cardinal.West
        }
        if (moveRight) {
            velocityX = wishlair.inventory.player.speed
            direction = Cardinal.East
        }
        if (moveUp) {
            velocityY = -wishlair.inventory.player.speed
            direction = Cardinal.North
        }
        if (moveDown) {
            velocityY = wishlair.inventory.player.speed
            direction = Cardinal.South
        }

        sprite.body.velocity.x = velocityX
        sprite.body.velocity.y = velocityY

        sprite.entity.direction = direction

        if (moveLeft || moveRight || moveUp || moveDown) {
            sprite.entity.animationId = 'daughter-move'
        } else {
            sprite.entity.animationId = 'daughter-idle'
        }
    }
}