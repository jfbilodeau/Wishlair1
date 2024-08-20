import 'phaser'
import {Configuration} from './Configuration'
import {ControllerManager} from './ControllerManager'
import {Game} from 'phaser'
import {Inputs} from './Inputs'
import {PlayerController} from '../entities/controllers/PlayerController'
import {Inventory} from './Inventory'
import WishlairScene from '../scenes/WishlairScene'
import {System} from './System'
import {PropController} from '../entities/controllers/PropController'

export class Wishlair {
    system = new System()
    config = new Configuration()
    inputs = new Inputs(this)
    inventory = new Inventory()
    controllers = new ControllerManager(this)

    constructor(public game: Game) {
        this.initializeControllers()
    }

    loadResources() {
    }

    startScene(sceneId: string) {
        const newScene = new WishlairScene(sceneId)

        this.game.scene.add(sceneId, newScene, true)
    }

    private initializeControllers() {
        this.controllers.addController('player', new PlayerController(this))
        this.controllers.addController('conifer1', new PropController(0, 0, 'conifer1'))
    }

    initializeScene(scene: WishlairScene) {
        this.inputs.reset(scene)
    }
}

// export const wishlair = new Wishlair()