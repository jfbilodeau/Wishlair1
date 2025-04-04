const mask.ground  = 0x0001
const mask.water   = 0x0002
const mask.ice     = 0x0004
const mask.lava    = 0x0008
const mask.void    = 0x0010
const mask.hostile = 0x0020
const mask.weapon  = 0x0040
const mask.player  = 0x0080
const mask.solid   = 0x0100
const mask.ui      = 0x0200

# Composite masks
const mask.entities = mask.player | mask.hostile | mask.weapon