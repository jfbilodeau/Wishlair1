// From Webpack DefinePlugin
declare var __BUILD_TIME__: string

export class DebugScene extends Phaser.Scene {
    constructor() {
        super('DebugScene');
    }
    
    preload() {
    }

    create() {
        // DEV:
        const text = this.add.text(10, 10, `Build: ${__BUILD_TIME__}`, {font: '16px Courier'})
    }
}