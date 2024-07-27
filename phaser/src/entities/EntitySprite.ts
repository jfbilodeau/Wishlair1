import {EntityController} from '../controllers/EntityController'
import {Entity} from './Entity'
import {Wishlair} from '../wishlair/Wishlair'
import WishlairScene from '../scenes/WishlairScene'


export class EntitySprite extends Phaser.GameObjects.Sprite {
    private wishlair: Wishlair
    private wishlairScene: WishlairScene
    private entity: Entity = new Entity()
    private controllerId = ""
    private controller: EntityController
    private animationId = ""

    constructor(scene: WishlairScene, x: number, y: number, controllerId: string) {
        super(scene, x, y, '')

        this.wishlair = scene.wishlair
        this.wishlairScene = scene

        this.controllerId = controllerId
        this.controller = this.wishlair.controllers.getController(this.controllerId)

        this.updateEntity()

        scene.add.existing(this)

        this.scene.events.on("update", this.tick, this)
    }

    public tick() {
        this.updateEntity()

        if (this.controller) {
            this.controller.tick(this.wishlair, this.entity)

            this.updateThis()
        }

        console.log('controller: ', this.controller)

    }

    private updateEntity() {
        this.entity.x = this.x
        this.entity.y = this.y
        this.entity.controllerId = this.controllerId
        this.entity.animationId = this.animationId
    }

    private updateThis() {
        this.x = this.entity.x
        this.y = this.entity.y

        if (this.controllerId !== this.entity.controllerId) {
            this.controllerId = this.entity.controllerId
            this.controller = this.wishlair.controllers.getController(this.controllerId)
            this.controller.initialize(this.wishlair, this.entity)
        }

        if (this.entity.animationId !== this.animationId) {
            this.animationId = this.entity.animationId
            this.anims.play(this.animationId)
        }
    }
}