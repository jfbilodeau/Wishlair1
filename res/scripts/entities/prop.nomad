#params animationName:string anchorX:float anchorY:float 

#log.info $"animationName: {animationName}, anchorX: {anchorX}, anchorY: {anchorY}, layer: {this.layer}"
#log.info $"this.animation.name: {this.animation.name}, this.x: {this.x}, this.y: {this.y}, this.z: {this.z}"

#this.animation.name = animationName
#this.repositionOnAnchor anchorX anchorY
#this.z = this.y
#this.mask = mask.solid

fun prop.init spriteName:string anchorX:float anchorY:float
    this.sprite.name = spriteName
    this.repositionOnAnchor anchorX anchorY
    this.z = this.y
    this.mask = mask.solid
    this.collisionMask = mask.entities
end

fun prop.initRectangle spriteName:string anchorX:float anchorY:float width:float height:float
    prop.init spriteName anchorX anchorY

    this.body.rectangle body.static width height
end

fun prop.initCircle spriteName:string anchorX:float anchorY:float radius:float
    prop.init spriteName anchorX anchorY

    this.body.circle body.static radius
end