import {Wishlair} from '../../wishlair/Wishlair'
import {Entity} from '../Entity'
import {EntityController} from './EntityController'

export class PropController extends EntityController {
    constructor(
        public originX: number,
        public originY: number,
        public animationId: string
    ) {
        super()
    }

    protected onInitialize(wishlair: Wishlair, entity: Entity): void {
        entity.originX = this.originX
        entity.originY = this.originY
        entity.animationId = this.animationId
    }

    protected onTick(wishlair: Wishlair, entity: Entity): void {
        // Nothing to do...
    }
}