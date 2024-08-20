import WishlairScene from '../scenes/WishlairScene'
import {Cardinal} from './Directions'

export enum WorldState {
    Running = 0,
    RoomTransition,
}

// Representation of the current world.
export class World {
    readonly roomWidth = 25
    readonly roomHeight = 15

    state = WorldState.Running

    private currentLevel: {
        id: string,
        roomX: number,
        roomY: number,
    }

    private nextLevel: {
        id: string,
        roomX: number,
        roomY: number,
        playerX: number,
        playerY: number,
        playerDirection: Cardinal
    }

    beginLoadLevel(
        scene: WishlairScene,
        levelId: string,
        roomX: number,
        roomY: number,
        playerX: number,
        playerY: number,
        playerDirection: Cardinal
    ) {
        if (this.currentLevel.id !== levelId) {
            // Load level
            scene.load.tilemapTiledJSON(levelId, `assets/world/${levelId}.tmx`)
            scene.load.start()
        }

        this.nextLevel.id = levelId
        this.nextLevel.roomX = roomX
        this.nextLevel.roomY = roomY
        this.nextLevel.playerX = playerX
        this.nextLevel.playerY = playerY
        this.nextLevel.playerDirection = playerDirection
    }

    endLoadLevel(scene: WishlairScene) {
        this.currentLevel = {
            id: this.nextLevel.id,
            roomX: this.nextLevel.roomX,
            roomY: this.nextLevel.roomY,
        }

        this.state = WorldState.RoomTransition
    }
}