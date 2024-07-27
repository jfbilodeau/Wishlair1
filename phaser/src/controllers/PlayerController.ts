import {Entity} from '../entities/Entity'
import {EntityController} from './EntityController'
import {Scene} from 'phaser'
import {Wishlair} from '../wishlair/Wishlair'
import {wishlair} from '../index'

export class PlayerController extends EntityController {
    private wishlair: Wishlair

    public constructor(wishlair: Wishlair) {
        super()
        this.wishlair = wishlair
    }

    protected override onInitialize(wishlair: Wishlair, entity: Entity): void {
        console.log('PlayerController initialized')
    }

    protected override onTick(wishlair: Wishlair, entity: Entity): void {
        console.log('PlayerController ticked')

        const moveLeft = wishlair.inputs.left1.isDown || wishlair.inputs.left2.isDown
        const moveRight = wishlair.inputs.right1.isDown || wishlair.inputs.right2.isDown

        if (moveLeft) {
            // this.x -= 1
            entity.velocity.x = -wishlair.inventory.player.speed
            entity.animationId = 'daughter-move-west'
        }
        if (moveRight) {
            // this.x += 1
            entity.velocity.x = wishlair.inventory.player.speed
            entity.animationId = 'daughter-move-east'
            // this.anims.play('daughter-move-east', true)
        }

        if (!moveLeft && !moveRight) {
            entity.velocity.x = 0
            entity.animationId = 'daughter-idle-south'
            // this.anims.play('daughter-idle-south', true)
        }
    }
}