import {Wishlair} from '../wishlair/Wishlair'

declare module 'phaser' {
    namespace Phaser {
        interface Game {
            wishlair: Wishlair
        }
    }
}
