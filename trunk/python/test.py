"""test.py: """

from Web10G import *

a = Web10Gagent()

cl = a.all_connections()

for c in cl:
    print("here, what")
    print("cid %i"%c.cid)

    print("Connection %d (%s %d  %s %d)"%(c.cid, \
                                        c.read('LocalAddress'), \
                                        c.read('LocalPort'), \
                                        c.read('RemAddress'), \
                                        c.read('RemPort')))
    print("and here's a uint64: %d"%(c.read('ThruOctetsReceived')))
#    for (name, val) in c.readall().items():
#        print("%-20s %s"%(name, str(val)))
#    print('')


