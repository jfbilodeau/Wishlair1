import { Wishlair } from '../../wishlair/Wishlair'
import { Entity } from '../Entity'
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
        entity.animationId = this.animationId
    }

    protected onTick(wishlair: Wishlair, entity: Entity): void {
        throw new Error('Method not implemented.')
    }
}