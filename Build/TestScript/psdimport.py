# -*- coding: utf-8 -*-
import SpriteStudio
from psd_tools import PSDImage

#from PIL import Image, ImageSequence
#im = Image.open("car.psd")
#layers = [frame.copy() for frame in ImageSequence.Iterator(im)]

def psd_test() : 
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


