#!/usr/bin/env python
import base64
import sys
import os
if os.path.exists("key.h"):
  print("key.h already exists")
  sys.exit(1)
if len(sys.argv)<2:
  print("Please provide a base64 encoded advertisement key as the first parameter")
  sys.exit(2)
k=sys.argv[1]
key=base64.b64decode(k)
if len(key)!=28:
  raise Exception('Key not valid (it must be 28 bytes)')
hexkey=[]
for x in key:
  hexkey.append(hex(x))
f=open("key.h","w")  
f.write("uint8_t public_key[28] = {")
f.write(",".join(hexkey))
f.write("};")
f.close()


