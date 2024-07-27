import Point = Phaser.Geom.Point

export class Entity {
    public controllerId: string

    public x = 0
    public y = 0
    // 'Distance' from the ground. Affect shadow
    public z = 0

    public velocity = new Point()

    // Animation
    public animationId = ''
}