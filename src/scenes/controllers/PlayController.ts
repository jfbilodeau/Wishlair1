import {SceneController} from './SceneController'
import {Wishlair} from '../../wishlair/Wishlair'
import WishlairScene from '../WishlairScene'

export class PlayController extends SceneController {
    constructor() {
        super()
    }

    protected onInitialize(wishlair: Wishlair, scene: WishlairScene): void {
    }

    protected onTick(wishlair: Wishlair, scene: WishlairScene): void {
        for (const entity of scene.activeEntities) {
            entity.tick()
        }
        // scene.level.layers.forEach(child => {
        //     const layer = child as LayerGameObject
        //
        //     layer.entities.each((child: GameObject) => {
        //         const sprite = child as WishlairSprite
        //
        //         sprite.tick()
        //
        //         return true
        //     })
        // })
    }
}