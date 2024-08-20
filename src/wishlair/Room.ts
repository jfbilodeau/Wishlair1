import WishlairScene from '../scenes/WishlairScene'
import {WishlairSprite} from '../entities/WishlairSprite'

export class RoomLayer {
    group = this.scene.add.group()
    sprites = <WishlairSprite[]>[]

    constructor(public scene: WishlairScene) {
    }

    createEntity(id: string, x: number, y: number, controllerId: string) {
        const entitySprite = new WishlairSprite(this.scene, id, x, y, controllerId)

        this.sprites.push(entitySprite)
        this.group.add(entitySprite)
    }

    update() {
        this.sprites.forEach(sprite => {
            sprite.tick()
        })
    }
}

export class Room {
    public readonly layers = [
        new RoomLayer(this.scene),
        new RoomLayer(this.scene),
        new RoomLayer(this.scene),
    ]

    public constructor(public scene: WishlairScene) {

    }
}