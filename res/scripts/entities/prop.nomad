#params animationName:string anchorX:float anchorY:float 

#log.info $"animationName: {animationName}, anchorX: {anchorX}, anchorY: {anchorY}, layer: {this.layer}"
#log.info $"this.animation.name: {this.animation.name}, this.x: {this.x}, this.y: {this.y}, this.z: {this.z}"

#this.animation.name = animationName
#this.repositionOnAnchor anchorX anchorY
#this.z = this.y
#this.mask = mask.solid

#log.info $"this.animation.name: {this.animation.name}, this.x: {this.x}, this.y: {this.y}, this.z: {this.z}"

fun prop.init animationName:string anchorX:float anchorY:float
    log.info $"animationName: {animationName}, anchorX: {anchorX}, anchorY: {anchorY}, layer: {this.layer}"

    this.animation.name = animationName
    this.repositionOnAnchor anchorX anchorY
    this.z = this.y
    this.mask = mask.solid
end

fun prop.initRectangle animationName:string anchorX:float anchorY:float width:float height:float
    prop.init animationName anchorX anchorY

    this.setRectangleBody width height
end

fun prop.initCircle animationName:string anchorX:float anchorY:float radius:float
    prop.init animationName anchorX anchorY

    this.setCircleBody radius
end