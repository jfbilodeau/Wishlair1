import Group = Phaser.GameObjects.Group
import Layer = Phaser.GameObjects.Layer
import WishlairScene from '../scenes/WishlairScene'
import {WishlairSprite} from './WishlairSprite'
import {BodyType, CollisionType} from './Entity'

export class LevelLayer extends Layer {
    // Collision groups
    private readonly obstacleGroup: Group  // Impassable obstacles
    private readonly entityGroup: Group  // Moveable entities
    private readonly playerGroup: Group  // Player
    private readonly hostileGroup: Group  // Hostiles
    private readonly weaponGroup: Group  // Player weapons
    private readonly shardGroup: Group  // Wishshards
    private readonly interactive: Group  // Interactive objects

    private groups: Group[] = []

    constructor(public scene: WishlairScene, public layerIndex: number) {
        super(scene)

        this.depth = layerIndex

        // Create collision groups
        this.obstacleGroup = scene.add.group()
        this.entityGroup = scene.add.group()
        this.playerGroup = scene.add.group()
        this.hostileGroup = scene.add.group()
        this.weaponGroup = scene.add.group()
        this.shardGroup = scene.add.group()
        this.interactive = scene.add.group()

        this.groups = [
            this.obstacleGroup,
            this.entityGroup,
            this.playerGroup,
            this.hostileGroup,
            this.weaponGroup,
            this.shardGroup,
            this.interactive,
        ]

        scene.physics.add.collider(
            this.obstacleGroup,
            this.entityGroup,
            this.collideObstacle,
            () => {
                return true
            },
            this,
        )
    }

    initialize() {
        this.scene.add.existing(this)

        const layerName = `layer-${this.layerIndex}`

        const tileLayer = this.addTileMapLayer(layerName)

        if (tileLayer) {
            tileLayer.setCollisionByProperty({type: ['obstacle', 'void']})
            this.scene.physics.add.collider(
                tileLayer,
                this.entityGroup,
                this.collideObstacle,
                () => {
                    return true
                },
            )
            // this.obstacleGroup.add(obstacleLayer)
        }

        // Setup entities
        const entityLayerName = `${layerName}-entities`

        const entityLayerExists = this.tileMapObjectLayerExists(entityLayerName)

        if (entityLayerExists) {
            const entityLayer = this.scene.map.getObjectLayer(entityLayerName)

            entityLayer.objects.forEach(object => {
                const entityX = object.x
                const entityY = object.y - object.height

                const controllerId = object.properties.find((p: any) => {
                    return p.name === `entityId`
                }).value

                const entity = this.scene.createSprite(
                    object.id.toString(),
                    entityX,
                    entityY,
                    object.width,
                    object.height,
                    this.layerIndex,
                    controllerId
                )
                //
                // this.addSprite(entity)
            })
        }
    }

    private tileMapLayerExists(layerName: string) {
        return this.scene.map.layers.findIndex(layer => layer.name === layerName) !== -1
    }

    private tileMapObjectLayerExists(layerName: string) {
        return this.scene.map.objects.findIndex(layer => layer.name === layerName) !== -1
    }

    private addTileMapLayer(layerName: string) {
        if (this.tileMapLayerExists(layerName)) {
            const tileLayer = this.scene.map.createLayer(layerName, this.scene.tileset)

            return tileLayer
        }

        return null
    }

    addSprite(sprite: WishlairSprite) {
        this.add(sprite)

        // this.arcadeBody.reset(0, 0)
        const body = sprite.getEntityBody()

        switch (body.type) {
            case BodyType.Rectangle: {
                sprite.arcadeBody.setSize(
                    body.width,
                    body.height,
                    true
                )

                break
            }
            case BodyType.Circle: {
                sprite.arcadeBody.setCircle(
                    body.radius,
                    body.offsetX - body.radius,
                    body.offsetY - body.radius,
                )

                break
            }
            case BodyType.None: {
                sprite.arcadeBody.setSize(0, 0)
                break
            }
            default: {
                console.error(`[WishlairSprite.ts]updateThis(): Unexpected body type: ${body.type}`)
                break
            }
        }

        switch (body.collision) {
            case CollisionType.None:
                // Nothing
                break;
            case CollisionType.Player:
                this.playerGroup.add(sprite)
                this.entityGroup.add(sprite)
                break;
            case CollisionType.Obstacle:
                this.obstacleGroup.add(sprite)
                sprite.arcadeBody.immovable = true
                sprite.arcadeBody.moves = false
                break;
            case CollisionType.Hostile:
                break;
            case CollisionType.Weapon:
                break;
            case CollisionType.Shard:
                break;
            case CollisionType.Interactive:
                break;
        }
    }

    removeSprite(sprite: WishlairSprite) {
        this.groups.forEach(g => g.remove(sprite))

        this.remove(sprite)
    }

    private collideObstacle() {

    }
}
