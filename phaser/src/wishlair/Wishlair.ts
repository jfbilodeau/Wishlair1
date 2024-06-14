import 'phaser'
import {LoadScene} from "../scenes/LoadScene";
import {Configuration} from './Configuration'

export class Wishlair {
    public config: Configuration = new Configuration()

    constructor() {
    }

    public loadResources() {
    }
}

export const wishlair = new Wishlair()