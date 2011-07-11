"""A Python module for accessing tcp_estats statistics.


"""

import exceptions
import fnmatch
from libestats import *


class error(exceptions.Exception):

    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return self.msg

class Web10Gagent(s_agent):

    def __init__(self, data=None):
        super(Web10Gagent, self).__init__()
        self.read_vars = {}
        _cur = self.s_get_var_head()
        while _cur != None:
            var = Web10Gvar(_cur)
            self.read_vars[str(var)] = var
            _cur = _cur.s_next()

    def __del__(self):
        super(Web10Gagent, self).__del__()

    def all_connections(self):
        conns = []
        cur = self.s_get_connection_head()
        while cur != None:
            if (cur.s_access()):
                conns.append(Web10Gconnection(self, cur))
            cur = cur.s_next()
        return conns

    def connection_match(self, local_address, local_port, remote_address, remote_port):
        """All connections matching a (partial) 4-tuple

        Ports are integers, addresses are strings.  Does UNIX-style pattern
        matching on address strings.

        Returns a list of Web100Connection objects.
        """

        cl = self.all_connections()
        match = []
        for c in cl:
            if (local_port == None or c.read('LocalPort') == local_port) and \
               (remote_port == None or c.read('RemPort') == remote_port) and \
               (local_address == None or \
                fnmatch.fnmatch(c.read('LocalAddress'), local_address)) and \
               (remote_address == None or \
                fnmatch.fnmatch(c.read('RemAddress'), remote_address)):
                    match.append(c)
        return match
	
class Web10Gconnection(object):

    def __init__(self, agent, _connection):
        self.agent = agent
        self._connection = _connection
        self.readsnap = Web10Gsnapshot(_connection)
        self.cid = _connection.cid

    def read(self, name):
        """Read the value of a single variable."""

        try:
            var = self.agent.read_vars[name]
        except KeyError:
            raise error("No variable named '%s' found."%name)

        val = self.read_value(var)
        return val.num()

    def read_value(self, p_var):
        _val = self._connection.s_read_value(p_var._var)
        return Web10Gvalue(_val)

    def write(self, name, num):
        """Write a value to a variable."""
        
        try:
            var = self.agent.read_vars[name]
        except KeyError:
            raise error("No variable named '%s' found."%name)

        self._connection.s_write_value(str(num), p_var._var)
        
    def readall(self):
        
        """Take a snapshot of all variables from a connection.

        This is much more efficient than reading all variables
        individually.  Currently, for local agents, it also guarantees
        consistency between all read-only variables.
        """
	self.readsnap.take_snapshot()

        snap = {}
        for (name, var) in self.agent.read_vars.items():
            val = self.readsnap.read_value(var)
            snap[name] = val
        return snap

class Web10Gsnapshot(s_snapshot):

    def __init__(self, conn):
        super(Web10Gsnapshot, self).__init__(conn)

    def __del__(self):
        super(Web10Gsnapshot, self).__del__()

    def take_snapshot(self):
        self.s_take_snapshot()

    def read_value(self, n_var):
        _val = self.s_read_value(n_var._var)
        return Web10Gvalue(_val)

class Web10Gvar(object):

    def __init__(self, _var):
        self._var = _var
        self._type = _var.s_get_type()

    def __str__(self):
        return self._var.s_get_name()

class Web10Gvalue(object):

    def __init__(self, _val):
        self.s_val = _val
        self.s_type = _val.s_get_type()

    def __str__(self):
        return self.s_val.s_as_string()

    def num(self):
        if  self.s_type == ESTATS_VALUE_TYPE_UINT16 or \
            self.s_type == ESTATS_VALUE_TYPE_UINT32 or \
            self.s_type == ESTATS_VALUE_TYPE_INT32 or \
            self.s_type == ESTATS_VALUE_TYPE_OCTET:
            return self.s_val.s_as_int()
        elif self.s_type == ESTATS_VALUE_TYPE_UINT64:
            return self.s_val.s_as_long()
        else:
            return self.s_val.s_as_string()


