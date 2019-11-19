Naming: NESW

N = North
E = East
S = South
W = West

For every roadtile, set the bits based off of north/east/south/west neighbours.

(Until grass/dirt/etc ground is introduced):
For every non-roadtile, use 0000.

Note: Might have to swap North and South due to the signedness of Y in the map implementation (Y:0 = top row, not bottom row!)