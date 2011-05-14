"""A Python module for accessing Web100 statistics.


"""


import libestats
import exceptions
import fnmatch
from libestats import py_agent
from libestats import py_connection
from libestats import py_snapshot
from Web10G import *

class error(exceptions.Exception):

    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return self.msg

class Web10Gagent(py_agent):

    def __init__(self, data=None):
        super(Web10Gagent, self).__init__()
        self.read_vars = {}
        _cur = self.var_head()
        while _cur != None:
            var = Web10Gvar(_cur)
            self.read_vars[str(var)] = var
            _cur = _cur.var_next()

    def __del__(self):
        super(Web10Gagent, self).__del__()

    def all_connections(self):
        conns = []
        cur = self.connection_head()
        while cur != None:
            conns.append(Web10Gconnection(self, cur))
            cur = cur.connection_next()
        return conns

class Web10Gconnection(object):

    def __init__(self, agent, _connection):
        self.agent = agent
        self._connection = _connection
        self._readsnap = Web10Gsnapshot(_connection)
        self.cid = _connection.cid
        print("new conn")

    def __del__(self):
        print("dead conn")

class Web10Gsnapshot(py_snapshot):

    def __init__(self, conn):
        super(Web10Gsnapshot, self).__init__(conn)

    def __del__(self):
        super(Web10Gsnapshot, self).__del__()

class Web10Gvar(object):

    def __init__(self, _var):
        self._var = _var
        self._type = _var.return_type()

    def __str__(self):
        return self._var.return_name()












