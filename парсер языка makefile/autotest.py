#!/usr/bin/python3
import os
import subprocess

os.chdir("tests/")

command = "maker.exe {files} 0 | findstr'!ERROR! line'"
for files in os.listdir():
	if files != "autotest.py":
		print(f"{files}:")
		subprocess.run(command)
		#os.system(command)
