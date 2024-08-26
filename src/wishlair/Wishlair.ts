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
import {CollisionType, EntityBody} from '../entities/Entity'

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
        this.controllers.addController('player', new PlayerController())

        // Props
        this.controllers.addController('conifer1', new PropController('conifer1', new EntityBody(CollisionType.Obstacle).setCircle(24).setOffset(85, 250)))
        this.controllers.addController('tree1', new PropController('tree1', new EntityBody(CollisionType.Obstacle).setCircle(34).setOffset(88, 210)))
        this.controllers.addController('rock1', new PropController('rock1', new EntityBody(CollisionType.Obstacle).setCircle(42).setOffset(92/2, 83/2)))
        this.controllers.addController('rock2', new PropController('rock2', new EntityBody(CollisionType.Obstacle).setCircle(14).setOffset(44/2, 37/2)))
        this.controllers.addController('rock3', new PropController('rock3', new EntityBody(CollisionType.Obstacle).setCircle(20).setOffset(64/2, 47/2)))
        this.controllers.addController('rock4', new PropController('rock4', new EntityBody(CollisionType.Obstacle).setCircle(45).setOffset(127/2, 107/2)))
    }

    initializeScene(scene: WishlairScene) {
        this.inputs.reset(scene)
    }
}

// export const wishlair = new Wishlair()
