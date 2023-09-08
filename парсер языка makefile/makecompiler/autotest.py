#!/usr/bin/python3
import os

os.chdir("tests/")

for files in os.listdir():
    if files != "autotest.py":
        print(f"{files}:")
        os.system(f"..\/maker {files} 0 | grep '!ERROR! line'")
