import numpy as np
from PIL import Image

if __name__ == "__main__":
	img = Image.open("Lenna.png")
	img_np = np.array(img)
	f_name = ["red_pixel1.txt", "green_pixel1.txt", "blue_pixel1.txt"]
	files = []
	for f_n in f_name:
		files.append(open(f_n, "w"))
	
	channels = np.array([img_np[:,:, 0], img_np[:,:, 1], img_np[:,:, 2]])

	for c_idx, c in enumerate(channels):
		for ro in c:
			for co in ro:
				files[c_idx].write(str(co)+'\n')

	for f in files:
		f.close()


