"""test.py: """

from Web10G import *

a = Web10Gagent()

cl = a.all_connections()

for c in cl:
    print("here, what")
    print("cid %i"%c.cid)

for item in a.read_vars:
    print item
