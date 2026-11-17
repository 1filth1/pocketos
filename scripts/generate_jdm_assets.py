#!/usr/bin/env python3
"""Build original 1-bit JDM Garage Empire car and component art for the SD card."""
import hashlib, json, pathlib, sys, math
from PIL import Image, ImageDraw, ImageFilter

root=pathlib.Path(sys.argv[1] if len(sys.argv)>1 else "sdcard/root")
out=root/"Games/JDM_Garage_Empire/assets"
out.mkdir(parents=True,exist_ok=True)
S=3
def image(w,h): return Image.new("L",(w*S,h*S),255)
def pts(p): return [(int(x*S),int(y*S)) for x,y in p]
def line(d,p,w=1): d.line(pts(p),fill=0,width=max(1,int(w*S)),joint="curve")
def poly(d,p,w=1): q=pts(p); d.polygon(q,fill=255); d.line(q+[q[0]],fill=0,width=max(1,int(w*S)),joint="curve")
def rect(d,b,w=1): d.rectangle(tuple(int(v*S) for v in b),outline=0,width=max(1,int(w*S)))
def ell(d,b,w=1,fill=255): d.ellipse(tuple(int(v*S) for v in b),outline=0,width=max(1,int(w*S)),fill=fill)
def hatch(d,x0,y0,x1,y1,step=4):
    for x in range(x0,x1,step): line(d,[(x,y1),(min(x+8,x1),y0)],.5)
def finish(im,size):
    im=im.resize(size,Image.Resampling.LANCZOS).filter(ImageFilter.UnsharpMask(.7,130,2))
    return im.point(lambda p:0 if p<174 else 255,mode="1")
def pack(im):
    p=im.load(); w,h=im.size; b=bytearray()
    for y in range(h):
        for x in range(0,w,8):
            v=0
            for k in range(8):
                if p[x+k,y]==0: v|=1<<(7-k)
            b.append(v)
    return bytes(b)

def car(i):
    im=image(240,104); d=ImageDraw.Draw(im)
    roof=[24,22,25,24,26,25][i]; rear=[39,43,42,46,38,51][i]
    poly(d,[(10,69),(18,60),(38,56),(rear,48),(58,roof),(111,roof),(157,roof+2),(177,49),(207,54),(225,63),(232,70),(229,78),(18,79)],1.5)
    line(d,[(13,73),(44,76),(191,76),(230,73)],1.2); line(d,[(20,80),(221,80)],.8)
    if i==5: win=[(58,48),(75,28),(109,28),(145,48),(126,52),(71,52)]
    elif i==4: win=[(49,49),(72,27),(112,27),(149,49),(124,53),(58,53)]
    else: win=[(48,49),(70,27),(113,27),(151,49),(126,53),(58,53)]
    poly(d,win); line(d,[(91,28),(91,52)]); line(d,[(129,29),(134,50)],.8)
    line(d,[(151,54),(193,56),(220,63)],.7); line(d,[(96,54),(94,73)],.7); line(d,[(147,53),(151,73)],.7)
    for cx in (62,190):
        ell(d,(cx-20,58,cx+20,98),1.5); ell(d,(cx-15,63,cx+15,93),1,0)
        ell(d,(cx-10,68,cx+10,88),.8); ell(d,(cx-4,74,cx+4,82),.7,0)
        for a in range(0,360,45):
            x=cx+math.cos(math.radians(a))*9; y=78+math.sin(math.radians(a))*9
            line(d,[(cx,78),(x,y)],.7)
    if i==0: rect(d,(13,63,41,69)); rect(d,(8,70,30,74)); hatch(d,177,61,207,70,3)
    elif i==1: ell(d,(13,62,35,70)); line(d,[(15,71),(39,71)]); poly(d,[(198,55),(216,57),(230,65),(215,65)])
    elif i==2: ell(d,(13,61,38,69)); line(d,[(17,72),(37,72)]); line(d,[(202,59),(225,63)])
    elif i==3: rect(d,(13,63,35,69)); hatch(d,183,61,208,70,3)
    elif i==4: rect(d,(13,62,38,69)); rect(d,(183,58,204,66)); line(d,[(52,44),(65,38),(80,38)])
    else: ell(d,(13,62,34,69)); line(d,[(176,54),(194,55)])
    if i in (0,1,3,4): line(d,[(174,49),(191,43),(220,45),(226,51)],1.2)
    for x in range(100,145,5): line(d,[(x,74),(x+2,77)],.5)
    for x,y in [(62,57),(190,57),(27,60),(220,60)]: ell(d,(x-1.5,y-1.5,x+1.5,y+1.5),.6)
    hatch(d,35,95,215,101,5)
    return finish(im,(240,104))

def part(i):
    im=image(128,128); d=ImageDraw.Draw(im); rect(d,(5,5,123,123),.5)
    if i==0:
        poly(d,[(23,35),(91,29),(108,43),(104,88),(85,101),(24,94),(15,77)],1.3)
        for x in (34,51,68,85): ell(d,(x-6,39,x+6,53)); ell(d,(x-3,42,x+3,50),.6)
        for y in (62,73,84): line(d,[(25,y),(98,y+3)],.7)
    elif i==1:
        rect(d,(14,43,32,83))
        for x in (34,48,62,76,90):
            line(d,[(x,44),(x+8,31),(x+14,31),(x+20,48),(x+17,77),(x+9,85)],1.5); ell(d,(x+12,77,x+22,88),.8)
    elif i==2:
        ell(d,(28,33,87,92),1.6); ell(d,(40,45,75,80)); ell(d,(49,54,66,71),.8)
        poly(d,[(73,40),(103,29),(112,38),(103,59),(83,64)],1.2); rect(d,(91,58,111,80))
        for a in range(0,360,45): line(d,[(58,62),(58+13*math.cos(math.radians(a)),62+13*math.sin(math.radians(a)))],.8)
    elif i==3:
        rect(d,(15,34,111,91),1.4)
        for y in range(40,88,5): line(d,[(22,y),(104,y)],.8)
        rect(d,(5,49,17,76)); rect(d,(109,49,122,76))
    elif i==4:
        rect(d,(15,31,33,98))
        for x in (30,47,64,81): line(d,[(x,36),(x+3,58),(x+13,71),(x+13,91)],2); ell(d,(x+8,87,x+19,99),.8)
    elif i==5:
        poly(d,[(18,39),(43,25),(92,30),(111,48),(105,86),(79,101),(27,90),(14,69)],1.4)
        for x in (37,54,71,88): line(d,[(x,34),(x+4,89)],.8)
        ell(d,(31,50,56,75)); ell(d,(73,48,98,74)); line(d,[(17,65),(8,65)],2); line(d,[(108,63),(121,63)],2)
    elif i==6:
        line(d,[(30,100),(96,28)],4); line(d,[(24,105),(102,25)])
        for k in range(7):
            y=40+k*8; x=85-k*7; line(d,[(x-12,y+9),(x+5,y-5),(x+13,y+3),(x-4,y+17)],1.5)
        ell(d,(18,96,37,115)); rect(d,(91,18,111,34))
    elif i==7:
        ell(d,(17,22,94,105),1.5); ell(d,(27,32,84,95)); ell(d,(43,48,68,79))
        for x,y in [(37,42),(73,42),(37,84),(73,84),(55,28),(55,98)]: ell(d,(x-2,y-2,x+2,y+2),.6)
        poly(d,[(88,38),(112,35),(117,48),(109,83),(89,80)],1.2); hatch(d,91,44,110,76,4)
    elif i==8:
        ell(d,(18,18,108,108),1.5); ell(d,(32,32,94,94)); ell(d,(44,44,82,82),1.2); ell(d,(52,52,74,74))
        for a in range(0,360,60):
            x=63+34*math.cos(math.radians(a)); y=63+34*math.sin(math.radians(a)); ell(d,(x-4,y-4,x+4,y+4),.8)
    elif i==9:
        rect(d,(18,34,108,91),1.5); rect(d,(27,43,99,82),.8)
        for x in range(30,99,8): rect(d,(x,26,x+3,34),.5); rect(d,(x,91,x+3,100),.5)
        for y in range(49,79,9): line(d,[(34,y),(91,y)],.5)
    elif i==10:
        ell(d,(23,37,103,91),1.5); ell(d,(35,45,91,83),.8); rect(d,(9,54,26,73)); rect(d,(100,54,119,73))
        line(d,[(44,49),(82,79)],.8); line(d,[(44,79),(82,49)],.8)
    else:
        rect(d,(18,22,109,105),1.5)
        for y in range(30,101,6): line(d,[(26,y),(101,y)],.8)
        for x in (26,101): rect(d,(x-4,12,x+4,23),.8); rect(d,(x-4,105,x+4,116),.8)
    for x,y in [(13,18),(114,18),(13,110),(114,110)]:
        ell(d,(x-4,y-4,x+4,y+4),.8); line(d,[(x,y),(x+(7 if x<64 else -7),y+(5 if y<64 else -5))],.5)
    return finish(im,(128,128))

sprites=[(f"car_{i:02d}","car",240,104,pack(car(i))) for i in range(6)]
sprites += [(f"part_{i:02d}","part",128,128,pack(part(i))) for i in range(12)]
blob=bytearray(); records=[]
for aid,kind,w,h,data in sprites:
    offset=len(blob); blob.extend(data)
    records.append({"id":aid,"kind":kind,"width":w,"height":h,"offset":offset,"length":len(data),"sha256":hashlib.sha256(data).hexdigest()})
(OUT/"assets.bin").write_bytes(blob)
manifest={"schema_version":1,"format":"MONO_MSB_1BIT","file":"assets.bin","total_bytes":len(blob),"assets":records}
(OUT/"manifest.json").write_text(json.dumps(manifest,indent=2)+"\n",encoding="utf-8")
print(f"Generated {len(sprites)} sprites / {len(blob)} bytes")
