import {Wishlair} from '../../wishlair/Wishlair'
import {Entity, EntityBody} from '../Entity'
import {EntityController} from './EntityController'

export class PropController extends EntityController {
    constructor(
        public originX: number,
        public originY: number,
        public animationId: string,
        public body: EntityBody
    ) {
        super()
    }

    protected onInitialize(wishlair: Wishlair, entity: Entity): void {
        entity.originX = this.originX
        entity.originY = this.originY
        entity.animationId = this.animationId

        // Reposition the prop so it's aligned to its origin.
        entity.x += entity.width * entity.originX
        entity.y += entity.height * entity.originY

        // Create body
        entity.body.copy(this.body)
    }

    protected onTick(wishlair: Wishlair, entity: Entity): void {
        // Nothing to do...
    }
}