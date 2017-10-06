video : https://www.youtube.com/watch?v=tVaE9p4LIdc

XBot from mixamo with few already baked animations with retargeting from original mixamo skel. That's why there a lot copy constraint on bones of armature.

to exporting skel with skins:
select only tringle(No_Draw)
then select BCRY-> Extor to Cryengine
uncheck option the  "Apply modifiers" !
press export (must got few files, chr + 2skin)

to export anims: 
fix Blender sample frame rate to 30 fps 
select aramture
select proper action for it (in actions window)
delete old group for armature, if it already assigned
then BCRY add animation node
make name for new expotred animation the same as action name
fix the length of the timeline (end time), (it's important! othervise you got pause(gap) in Character Tool at the end of animation)




