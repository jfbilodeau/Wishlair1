import {Wishlair} from '../../wishlair/Wishlair'
import WishlairScene from '../WishlairScene'

export abstract class SceneController {
    protected constructor() {
    }

    public initialize(wishlair: Wishlair, scene: WishlairScene) {
        this.onInitialize(wishlair, scene)
    }

    public tick(wishlair: Wishlair, scene: WishlairScene) {
        this.onTick(wishlair, scene)
    }

    protected abstract onInitialize(wishlair: Wishlair, scene: WishlairScene): void
    protected abstract onTick(wishlair: Wishlair, scene: WishlairScene): void
}