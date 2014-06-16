import os.path
from PIL import Image
from psd_tools import PSDImage

psd = PSDImage.load('car.psd')
print (psd.header)
print (psd.layers)

layer0 = psd.layers[0]
#print (layer0.bbox)
#print (layer_image)

layer0 = psd.layers[0]
print layer0
layer_image = layer0.as_PIL()
layer_image.save('layer0.png')

layer1 = psd.layers[1]
layer_image = layer1.as_PIL()
layer_image.save('layer1.png')

layer2 = psd.layers[2]
layer_image = layer2.as_PIL()
layer_image.save('layer2.png')
