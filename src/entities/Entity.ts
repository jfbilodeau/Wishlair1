import {Cardinal} from '../wishlair/Directions'

export enum BodyType {
    None = 1,
    Rectangle = 2,
    Circle = 3,
}

export class EntityBody {
    type = BodyType.None
    offsetX = 0
    offsetY = 0
    width = 0
    height = 0
    radius = 0

    copy(body: EntityBody) {
        this.type = body.type
        this.offsetX = body.offsetX
        this.offsetY = body.offsetY
        this.width = body.width
        this.height = body.height
        this.radius = body.radius
    }

    remove() {
        this.type = BodyType.None
        this.width = 0
        this.height = 0
        this.radius = 0
    }

    setRectangle(width: number, height: number) {
        this.type = BodyType.Rectangle
        this.width = width
        this.height = height

        return this
    }

    setCircle(radius: number) {
        this.type = BodyType.Circle
        this.radius = radius

        return this
    }

    setOffset(offsetX: number, offsetY: number) {
        this.offsetX = offsetX
        this.offsetY = offsetY

        return this
    }
}

export class Entity {
    id: string

    controllerId: string

    // body = new EntityBody()

    x = 0
    y = 0

    // Width and height of entity
    // Used to determine which entities are active in which room.
    width = 0
    height = 0

    // originX = 0
    // originY = 0

    // Which layer the entity is on
    layer = 0

    // velocity = new Point()
    direction = Cardinal.None

    // Animation
    animationId = ''

    // Bounds
    // get left() { return this.x - this.width / this.originX }
    // get right() { return this.x + this.width - this.width / this.originX }
    // get top() { return this.y - this.height / this.originY }
    // get bottom() { return this.y + this.height - this.height / this.originY }
}