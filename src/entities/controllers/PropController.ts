import {Wishlair} from '../../wishlair/Wishlair'
import {Entity, EntityBody} from '../Entity'
import {EntityController} from './EntityController'

export class PropController extends EntityController {
    constructor(
        public animationId: string,
        public body: EntityBody,
    ) {
        super()
    }

    protected onInitialize(wishlair: Wishlair, entity: Entity): void {
        // entity.originX = this.body.offsetX
        // entity.originY = this.body.offsetY
        entity.animationId = this.animationId

        // Reposition the prop so it's aligned to its origin.
        entity.x += entity.width * entity.body.offsetX
        entity.y += entity.height * entity.body.offsetY

        // Create body
        entity.body.copy(this.body)
    }

    protected onTick(wishlair: Wishlair, entity: Entity): void {
        // Nothing to do...
    }
}