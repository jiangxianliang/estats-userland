"""test.py: """

from Web10G import *

a = Web10GAgent()

cl = a.all_connections()

for c in cl:
    print("here, what")
    print("cid %i"%c.cid)
    
#    c.dumb()


