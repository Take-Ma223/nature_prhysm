# -*- coding: utf-8 -*-
import os
import glob
import pprint

npsfiles = glob.glob("./songs/user/*/*.nps")

datfiles = glob.glob("./songs/user/*/*.dat")

files = npsfiles+datfiles

#ファイルをUnicodeで保存

for filename in files:
	try:
		f        = open(filename,"r",encoding="shift_JIS")
		fout_uni = open("./songs/user/tmp.nps","w",encoding="utf-16")
		for row in f:
			fout_uni.write(row)
		f.close()
		os.remove(filename)
		fout_uni.close()
		os.rename("./songs/user/tmp.nps",filename)
	except UnicodeDecodeError:
		pass

fout_uni.close()
try:
	os.remove("./songs/user/tmp.nps")
except FileNotFoundError:
	pass

#pprint.pprint(files)

