import {EntityController} from './controllers/EntityController'
import {BodyType, Entity, EntityBody} from './Entity'
import {Wishlair} from '../wishlair/Wishlair'
import WishlairScene from '../scenes/WishlairScene'
import {Cardinal, getCardinalName} from '../wishlair/Directions'
import Body = Phaser.Physics.Arcade.Body


export class WishlairSprite extends Phaser.GameObjects.Sprite {
    private readonly wishlair: Wishlair
    private readonly wishlairScene: WishlairScene
    entity: Entity = new Entity()
    private controllerId = ''
    private controller: EntityController
    private currentAnimationId: string
    private currentDirection = Cardinal.None
    // Convenience property to make TypeScript happy and not have to cast `body` to Arcade.Body every time
    readonly arcadeBody: Body

    constructor(
        scene: WishlairScene,
        public readonly id: string,
        x: number,
        y: number,
        width: number,
        height: number,
        layer: number,
        controllerId: string
    ) {
        super(scene, x, y, '')

        this.entity.width = width
        this.entity.height = height
        this.entity.layer = layer

        this.scene.physics.add.existing(this)

        this.wishlair = scene.wishlair
        this.wishlairScene = scene

        this.entity.controllerId = controllerId
        this.updateEntity() // Allow entity to initialize itself

        this.setOrigin(0, 0)

        this.arcadeBody = this.body as Body
        this.arcadeBody.setCollideWorldBounds(true)
        this.arcadeBody.debugShowBody = true
        this.arcadeBody.debugShowVelocity = true

        this.controller = this.wishlair.controllers.getController(this.controllerId)
        if (this.controller) {
            this.controller.initialize(this.wishlair, this)
            this.updateThis()
        }

        scene.add.existing(this)
    }

    public tick() {
        this.updateEntity()

        this.controller?.tick(this.wishlair, this)

        // Update controller first to allow it to initialize and then update the entity
        if (this.controllerId !== this.entity.controllerId) {
            this.controllerId = this.entity.controllerId
            this.controller = this.wishlair.controllers.getController(this.controllerId)

            this.controller?.initialize(this.wishlair, this)
        }

        this.updateThis()
    }

    private updateEntity() {
        this.entity.x = this.x
        this.entity.y = this.y
    }

    private updateThis() {
        // if (this.entity.body.reset) {
        //     const body = this.arcadeBody
        //
        //     body.setOffset(this.entity.body.offsetX, this.entity.body.offsetY)
        //     this.setOrigin(
        //         this.entity.body.offsetX / this.entity.width,
        //         this.entity.body.offsetY / this.entity.height,
        //     )
        //
        //     switch (this.entity.body.type) {
        //         case BodyType.Rectangle: {
        //             body.setSize(
        //                 this.entity.body.width,
        //                 this.entity.body.height
        //             )
        //             break
        //         }
        //         case BodyType.Circle: {
        //             body.setCircle(
        //                 this.entity.body.radius,
        //             )
        //             break
        //         }
        //         case BodyType.None: {
        //             body.setSize(0, 0)
        //             break
        //         }
        //         default: {
        //             console.error(`[WishlairSprite.ts]updateThis(): Unexpected body type: ${this.entity.body.type}`)
        //             break
        //         }
        //     }
        // }

        // this.x = this.entity.x
        // this.y = this.entity.y

        if (this.currentAnimationId !== this.entity.animationId || this.currentDirection != this.entity.direction) {
            this.currentAnimationId = this.entity.animationId
            this.currentDirection = this.entity.direction

            let animationId = this.currentAnimationId

            if (this.currentDirection !== Cardinal.None) {
                const directionName = getCardinalName(this.currentDirection)
                animationId += `-${directionName}`
            }

            this.anims.play(animationId, true)
        }
        // this.body.velocity.x = this.entity.velocity.x
        // this.body.velocity.y = this.entity.velocity.y

        // if (this.entity.animationId !== this.animationId || this.entity.direction !== this.direction) {
        //     this.animationId = this.entity.animationId
        //     this.direction = this.entity.direction
        //
        //     this.currentAnimation = `${this.animationId}`
        //
        //     if (this.entity.direction !== Cardinal.None) {
        //         const directionName = getCardinalName(this.direction)
        //         this.currentAnimation += `-${directionName}`
        //     }
        //
        //     this.anims.play(this.currentAnimation, true)
        // }
        //
        // // Playing an animation reset the origin to (0,0) for a frame
        // // so let's set the origin _after_ the animation.
        // // TODO: Do we really need to be able to change the origin after onInitialize() or will it always be the same?
        // if (this.originX !== this.entity.originX || this.originY !== this.entity.originY) {
        //     // console.log(`setOrigin${this.entity.originX}, ${this.entity.originY}) (${this.originX}, ${this.originY})`)
        //     this.setOrigin(this.entity.originX, this.entity.originY)
        // }
    }

    // setAnimation(animationId: string) {
    //     this.entity.animationId = animationId
    // }
    //
    // setController(controllerId: string) {
    //     this.entity.controllerId = controllerId
    // }

    setBody(body: EntityBody) {
        // this.arcadeBody.reset(0, 0)

        switch (body.type) {
            case BodyType.Rectangle: {
                this.arcadeBody.setSize(
                    body.width,
                    body.height,
                    true
                )
                //
                // this.arcadeBody.setOffset(
                //     body.offsetX - body.width / 2,
                //     body.offsetY - body.height / 2,
                // )

                break
            }
            case BodyType.Circle: {
                this.arcadeBody.setCircle(
                    body.radius,
                    body.offsetX - body.radius,
                    body.offsetY - body.radius,
                )

                break
            }
            case BodyType.None: {
                this.arcadeBody.setSize(0, 0)
                break
            }
            default: {
                console.error(`[WishlairSprite.ts]updateThis(): Unexpected body type: ${body.type}`)
                break
            }
        }
    }
}
