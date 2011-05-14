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

class Web10GAgent(py_agent):

    def __init__(self, data=None):
        super(Web10GAgent, self).__init__()

    def __del__(self):
        super(Web10GAgent, self).__del__()

    def all_connections(self):
        conns = []
        cur = self.connection_head()
        while cur != None:
            conns.append(Web10GConnection(self, cur))
            cur = cur.connection_next()
        return conns

class Web10GConnection(object):

    def __init__(self, agent, _connection):
        self.agent = agent
        self._connection = _connection
        self._readsnap = Web10Gsnapshot(_connection)
        self.cid = _connection.cid
        print("new conn")

    def __del__(self):
        print("dead conn")

    def dumb(self):
        print("im dumb")

class Web10Gsnapshot(py_snapshot):

    def __init__(self, conn):
        super(Web10Gsnapshot, self).__init__(conn)

    def __del__(self):
        super(Web10Gsnapshot, self).__del__()















