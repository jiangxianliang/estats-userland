"""readall.py: Read all tcp_estats variables from all connections."""

from Web10G import *

a = Web10Gagent()

cl = a.all_connections()

for c in cl:

    print("Connection %d (%s %d  %s %d)"%(c.cid, \
                                        c.read('LocalAddress'), \
                                        c.read('LocalPort'), \
                                        c.read('RemAddress'), \
                                        c.read('RemPort')))

    for (name, val) in c.readall().items():
        print("%-20s %s"%(name, str(val)))
    print('')


