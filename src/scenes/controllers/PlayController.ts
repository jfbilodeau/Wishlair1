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
    }
}