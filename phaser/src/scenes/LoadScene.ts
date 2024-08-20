import {Scene} from 'phaser'
import {DebugScene} from './DebugScene'

const assets = `assets`
const sprites = `${assets}/sprites`
const world = `${assets}/world`

export class LoadScene extends Scene {
    preload() {
        // Load tileset image
        this.load.image('tiles', `${world}/tileset.png`)

        this.load.atlas('daughter', `${sprites}/daughter.png`, `${sprites}/daughter.json`)
        this.load.atlas('forest', `${sprites}/forest.png`, `${sprites}/forest.json`)
    }

    private createAnimation(key: string, textureKey: string, frameRate: number) {
        const texture = this.textures.get(textureKey)
        const frames = []
        let frameIndex = 0

        do {
            const frameIndexString = frameIndex.toString().padStart(2, '0')
            const frameName = `${key}-${frameIndexString}.png`

            if (texture.has(frameName)) {
                // const frame = this.textures.getFrame(textureKey, frameName)

                // const animationFrame = new AnimationFrame(textureKey, frameName, frameIndex, frame)
                const animationFrame = {
                    key: textureKey,
                    frame: frameName,
                }

                frames.push(animationFrame)

                frameIndex++
            } else {
                if (frameIndex === 0) {
                    console.error(`Texture ${textureKey} does not exist`)
                }

                break
            }
        } while (true)

        // There seems to be a bug in `Phaser.d.ts` where `this.anims.create({frames})` is typed
        // as `Phaser.Types.Animations.AnimationFrame[]` instead of `Phaser.Animations.AnimationFrame[]`
        // so we have to cast it to the correct type
        this.anims.create({
            key,
            frames: <Phaser.Types.Animations.AnimationFrame[]><unknown>frames,
            frameRate,
            // duration: 10,
            repeat: -1,
        })
    }

    create() {
        // Create daughter animations
        this.createAnimation('daughter-idle-north', 'daughter', 0)
        this.createAnimation('daughter-idle-south', 'daughter', 0)
        this.createAnimation('daughter-idle-east', 'daughter', 0)
        this.createAnimation('daughter-idle-west', 'daughter', 0)

        this.createAnimation('daughter-move-north', 'daughter', 6)
        this.createAnimation('daughter-move-south', 'daughter', 6)
        this.createAnimation('daughter-move-east', 'daughter', 6)
        this.createAnimation('daughter-move-west', 'daughter', 6)

        // Create forest animations
        // this.createAnimation()

        // this.scene.start('wishlair')
        const wishlair = this.game.registry.get('wishlair')
        wishlair.startScene('wishlair-entrance-1')

        this.scene.add('DebugScene', DebugScene, true)

        this.scene.remove(this)
    }
}