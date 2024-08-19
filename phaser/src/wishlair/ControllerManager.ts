import {Wishlair} from './Wishlair'
import {PlayerController} from '../entities/controllers/PlayerController'
import {EntityController} from '../entities/controllers/EntityController'

interface ControllerMap {
    [key: string]: EntityController
}

export class ControllerManager {
    constructor (public wishlair: Wishlair) {

    }

    private controllers: ControllerMap = {}

    public addController(controllerId: string, controller: EntityController) {
        if (this.controllers[controllerId] !== undefined) {
            console.warn(`[Wishlair.ts]Wishlair.addController(${controllerId}): controller already exists and being replaced`)
        }

        this.controllers[controllerId] = controller
    }

    public getController(controllerId: string) {
        const controller = this.controllers[controllerId]

        if (!controller) {
            console.error(`[Wishlair.ts]Wishlair.getController(${controllerId}): controller not found`)
        }

        return controller
    }

}