import {Entity} from '../entities/Entity'
import {EntityController} from './EntityController'
import {Wishlair} from '../wishlair/Wishlair'
import {Cardinal, getAngleFromCardinal} from '../wishlair/Directions'

export class PlayerController extends EntityController {
    public constructor(public wishlair: Wishlair) {
        super()
    }

    protected override onInitialize(wishlair: Wishlair, entity: Entity) {
    }

    protected override onTick(wishlair: Wishlair, entity: Entity) {
        const moveLeft = wishlair.inputs.left1.isDown || wishlair.inputs.left2.isDown
        const moveRight = wishlair.inputs.right1.isDown || wishlair.inputs.right2.isDown
        const moveUp = wishlair.inputs.up1.isDown || wishlair.inputs.up2.isDown
        const moveDown = wishlair.inputs.down1.isDown || wishlair.inputs.down2.isDown

        entity.velocity.x = 0
        entity.velocity.y = 0

        if (moveLeft) {
            entity.velocity.x = -wishlair.inventory.player.speed
            entity.direction = Cardinal.West
        }
        if (moveRight) {
            entity.velocity.x = wishlair.inventory.player.speed
            entity.direction = Cardinal.East
        }
        if (moveUp) {
            entity.velocity.y = -wishlair.inventory.player.speed
            entity.direction = Cardinal.North
        }
        if (moveDown) {
            entity.velocity.y = wishlair.inventory.player.speed
            entity.direction = Cardinal.South
        }

        if (!moveLeft && !moveRight && !moveUp && !moveDown) {
            entity.animationId = 'daughter-idle'
        } else {
            entity.animationId = 'daughter-move'
        }
    }
}