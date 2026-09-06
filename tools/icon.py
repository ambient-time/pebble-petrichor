# Copyright 2026 Luke Steuber. MIT License.
from pathlib import Path
from PIL import Image,ImageDraw
im=Image.new('1',(25,25));d=ImageDraw.Draw(im)
for x,y,r in ((6,7,5),(18,8,5),(12,19,5)):
 for radius in (2,4,5):d.ellipse((x-radius,y-radius,x+radius,y+radius),outline=1)
 d.point((x,y),fill=1)
im.save(Path(__file__).resolve().parents[1]/'resources/menu-icon.png')
