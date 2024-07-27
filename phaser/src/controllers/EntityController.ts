import {Entity} from '../entities/Entity'
import {Scene} from 'phaser'
import {Wishlair} from '../wishlair/Wishlair'

export abstract class EntityController {
    protected constructor() {
    }

    public initialize(wishlair: Wishlair, entity: Entity) {
        this.onInitialize(wishlair, entity)
    }

    public tick(wishlair: Wishlair, entity: Entity) {
        this.onTick(wishlair, entity)
    }

    protected abstract onInitialize(wishlair: Wishlair, entity: Entity): void
    protected abstract onTick(wishlair: Wishlair, entity: Entity): void
}