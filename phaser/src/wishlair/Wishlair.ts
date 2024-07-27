import 'phaser'
import {Configuration} from './Configuration'
import {ControllerManager} from './ControllerManager'
import {Game} from 'phaser'
import {Inputs} from './Inputs'
import {PlayerController} from '../controllers/PlayerController'
import {Inventory} from './Inventory'

export class Wishlair {
    config = new Configuration()
    inputs = new Inputs(this)
    inventory = new Inventory()
    controllers = new ControllerManager(this)

    constructor(public game: Game) {
        this.initializeControllers()
    }

    loadResources() {
    }

    private initializeControllers() {
        this.controllers.addController('player', new PlayerController(this))
    }
}

// export const wishlair = new Wishlair()