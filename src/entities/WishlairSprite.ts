import {EntityController} from './controllers/EntityController'
import {BodyType, Entity, EntityBody} from './Entity'
import {Wishlair} from '../wishlair/Wishlair'
import WishlairScene from '../scenes/WishlairScene'
import {Cardinal, getCardinalName} from '../wishlair/Directions'


export class WishlairSprite extends Phaser.GameObjects.Sprite {
    private readonly wishlair: Wishlair
    private readonly wishlairScene: WishlairScene
    entity: Entity = new Entity()
    private controllerId = ''
    private controller: EntityController
    private animationId = ''
    private direction = Cardinal.South
    private currentAnimation: string

    constructor(
        scene: WishlairScene,
        public readonly id: string,
        x: number,
        y: number,
        width: number,
        height: number,
        controllerId: string
    ) {
        super(scene, x, y, '')

        this.entity.width = width
        this.entity.height = height

        this.scene.physics.add.existing(this)

        this.wishlair = scene.wishlair
        this.wishlairScene = scene

        this.controllerId = controllerId
        this.updateEntity()

        this.controller = this.wishlair.controllers.getController(this.controllerId)
        if (this.controller) {
            this.controller.initialize(this.wishlair, this.entity)
            this.updateThis()
        }

        scene.add.existing(this)
    }

    public tick() {
        this.updateEntity()

        if (this.controller) {
            this.controller.tick(this.wishlair, this.entity)

            this.updateThis()

            // Update controller first to allow it to initialize and then update the entity
            if (this.controllerId !== this.entity.controllerId) {
                this.controllerId = this.entity.controllerId
                this.controller = this.wishlair.controllers.getController(this.controllerId)
                this.controller.initialize(this.wishlair, this.entity)
            }
        }
    }

    private updateEntity() {
        this.entity.x = this.x
        this.entity.y = this.y

        this.entity.controllerId = this.controllerId
        this.entity.animationId = this.animationId
    }

    private updateThis() {
        if (this.entity.body.reset) {
            const body = this.body as Phaser.Physics.Arcade.Body

            switch (this.entity.body.type) {
                case BodyType.Rectangle: {
                    body.setSize(this.entity.body.width, this.entity.body.height)
                    body.setOffset(0, 0)
                    break
                }
                case BodyType.Circle: {
                    body.setCircle(this.entity.body.radius)
                    break
                }
                case BodyType.None: {
                    body.setSize(0, 0)
                    break
                }
                default: {
                    console.error(`[WishlairSprite.ts]updateThis(): Unexpected body type: ${this.entity.body.type}`)
                    break
                }
            }
        }

        this.x = this.entity.x
        this.y = this.entity.y

        this.setOrigin(this.entity.originX, this.entity.originY)

        this.body.velocity.x = this.entity.velocity.x
        this.body.velocity.y = this.entity.velocity.y

        if (this.entity.animationId !== this.animationId || this.entity.direction !== this.direction) {
            this.animationId = this.entity.animationId
            this.direction = this.entity.direction

            this.currentAnimation = `${this.animationId}`

            if (this.entity.direction !== Cardinal.None) {
                const directionName = getCardinalName(this.direction)
                this.currentAnimation += `-${directionName}`
            }

            this.anims.play(this.currentAnimation, true)
        }
    }
}