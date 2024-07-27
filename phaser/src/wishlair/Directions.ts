export enum Cardinal {
    North = 1,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest,
}

export function normalizeAngle(angle: number) {
    if (angle >= 0 && angle < 360) {
        return angle
    }

    return angle % 360
}

export function getAngleFromCardinal(cardinal: Cardinal) {
    switch (cardinal) {
        case Cardinal.North:
            return 0
        case Cardinal.NorthEast:
            return 45
        case Cardinal.East:
            return 90
        case Cardinal.SouthEast:
            return 135
        case Cardinal.South:
            return 180
        case Cardinal.SouthWest:
            return 225
        case Cardinal.West:
            return 270
        case Cardinal.NorthWest:
            return 315
        default:
            console.error(`[Directions.ts]getAngleFromCardinal(${cardinal}): Parameter cardinal is not valid`)
    }
}

export function getCardinalFromAngle(angle: number) {
    if (angle >= -22.5 && angle < 22.5) {
        return Cardinal.East
    } else if (angle >= 22.5 && angle < 67.5) {
        return Cardinal.SouthEast
    } else if (angle >= 67.5 && angle < 112.5) {
        return Cardinal.South
    } else if (angle >= 112.5 && angle < 157.5) {
        return Cardinal.SouthWest
    } else if (angle >= 157.5 || angle < -157.5) {
        return Cardinal.West
    } else if (angle >= -157.5 && angle < -112.5) {
        return Cardinal.NorthWest
    } else if (angle >= -112.5 && angle < -67.5) {
        return Cardinal.North
    } else if (angle >= -67.5 && angle < -22.5) {
        return Cardinal.NorthEast
    } else {
        console.error(`[Directions.ts]getCardinalFromAngle(${angle}): Parameter angle is not valid`)
    }
}

export function getCardinalEWFromAngle(angle: number) {
    if (angle >= -22.5 && angle < 22.5) {
        return Cardinal.East
    } else if (angle >= 157.5 || angle < -157.5) {
        return Cardinal.West
    } else {
        console.error(`[Directions.ts]getCardinalEWFromAngle(${angle}): Parameter angle is not valid`)
    }
}

export function getCardinalNSEWFromAngle(angle: number) {
    if (angle >= -45 && angle < 45) {
        return Cardinal.East
    } else if (angle >= 45 && angle < 135) {
        return Cardinal.South
    } else if (angle >= 135 || angle < -135) {
        return Cardinal.West
    } else if (angle >= -135 && angle < -45) {
        return Cardinal.North
    } else {
        console.error(`[Directions.ts]getCardinalNSEWFromAngle(${angle}): Parameter angle is not valid`)
    }
}

export function getCardinalName(cardinal: Cardinal) {
    switch (cardinal) {
        case Cardinal.North:
            return 'north'
        case Cardinal.NorthEast:
            return 'northEast'
        case Cardinal.East:
            return 'east'
        case Cardinal.SouthEast:
            return 'southEast'
        case Cardinal.South:
            return 'south'
        case Cardinal.SouthWest:
            return 'southWest'
        case Cardinal.West:
            return 'west'
        case Cardinal.NorthWest:
            return 'northWest'
        default:
            console.error(`[Directions.ts]getCardinalName(${cardinal}): Parameter cardinal is not valid`)
    }
}