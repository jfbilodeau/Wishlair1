import Point = Phaser.Geom.Point
import {Cardinal} from '../wishlair/Directions'

export class Entity {
    controllerId: string

    x = 0
    y = 0
    // 'Distance' from the ground. Affect shadow
    z = 0

    // Width and height from Tiled
    width = 0
    height = 0

    originX = 0
    originY = 0

    // Which layer the entity is on
    layer = 0

    velocity = new Point()
    direction = Cardinal.None

    // Animation
    animationId = ''
}