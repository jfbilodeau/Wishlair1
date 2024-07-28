import {EntityController} from '../controllers/EntityController'
import {Entity} from './Entity'
import {Wishlair} from '../wishlair/Wishlair'
import WishlairScene from '../scenes/WishlairScene'
import {Cardinal, getCardinalName} from '../wishlair/Directions'


export class WishlairSprite extends Phaser.GameObjects.Sprite {
    private readonly wishlair: Wishlair
    private readonly wishlairScene: WishlairScene
    private entity: Entity = new Entity()
    private controllerId = ""
    private controller: EntityController
    private animationId = ""
    private direction = Cardinal.South
    private currentAnimation: string

    constructor(scene: WishlairScene, x: number, y: number, controllerId: string) {
        super(scene, x, y, '')

        this.scene.physics.add.existing(this)

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

        this.body.velocity.x = this.entity.velocity.x
        this.body.velocity.y = this.entity.velocity.y

        if (this.controllerId !== this.entity.controllerId) {
            this.controllerId = this.entity.controllerId
            this.controller = this.wishlair.controllers.getController(this.controllerId)
            this.controller.initialize(this.wishlair, this.entity)
        }

        if (this.entity.animationId !== this.animationId || this.entity.direction !== this.direction) {
            this.animationId = this.entity.animationId
            this.direction = this.entity.direction
            
            const directionName = getCardinalName(this.direction)
            this.currentAnimation = `${this.animationId}-${directionName}`
            
            this.anims.play(this.currentAnimation, true)
        }
    }
}