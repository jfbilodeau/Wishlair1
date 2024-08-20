import Point = Phaser.Geom.Point
import {Cardinal} from '../wishlair/Directions'

export class Entity {
    public controllerId: string

    public x = 0
    public y = 0
    // 'Distance' from the ground. Affect shadow
    public z = 0

    originX = 0
    originY = 0

    // Which layer the entity is on
    public layer = 0

    public velocity = new Point()
    public direction = Cardinal.None

    // Animation
    public animationId = ''
}