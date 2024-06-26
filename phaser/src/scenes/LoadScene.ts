import {Scene} from 'phaser'
import AnimationFrame = Phaser.Animations.AnimationFrame

export class LoadScene extends Scene {
    preload() {
        this.load.atlas('daughter', 'assets/daughter.png', 'assets/daughter.json')
        this.load.atlas('forest', 'assets/forest.png', 'assets/forest.json')
    }

    private createAnimation(key: string, textureKey: string, frameRate: number) {
        // this.anims.create({
        //     key,
        //     frames: this.anims.generateFrameNames(textureKey, {
        //         prefix: `${key}-`,
        //         suffix: '.png',
        //         zeroPad: 2,
        //         start: 0,
        //         end: 0,
        //
        //     }),
        //     frameRate,
        //     repeat: -1
        // })

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
            repeat: -1
        })
    }

    create() {
        this.createAnimation('daughter-idle-north', 'daughter', 0)
        this.createAnimation('daughter-idle-south', 'daughter', 0)
        this.createAnimation('daughter-idle-east', 'daughter', 0)
        this.createAnimation('daughter-idle-west', 'daughter', 0)

        this.createAnimation('daughter-move-north', 'daughter', 10)
        this.createAnimation('daughter-move-south', 'daughter', 10)
        this.createAnimation('daughter-move-east', 'daughter', 10)
        this.createAnimation('daughter-move-west', 'daughter', 10)

        this.scene.start('wishlair')
    }
}