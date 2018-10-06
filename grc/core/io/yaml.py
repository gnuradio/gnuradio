# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import

from collections import OrderedDict

import six
import yaml

from ..params.param import attributed_str

class GRCDumper(yaml.SafeDumper):
    @classmethod
    def add(cls, data_type):
        def decorator(func):
            cls.add_representer(data_type, func)
            return func
        return decorator

    def represent_ordered_mapping(self, data):
        value = []
        node = yaml.MappingNode(u'tag:yaml.org,2002:map', value, flow_style=False)

        if self.alias_key is not None:
            self.represented_objects[self.alias_key] = node

        for item_key, item_value in six.iteritems(data):
            node_key = self.represent_data(item_key)
            node_value = self.represent_data(item_value)
            value.append((node_key, node_value))

        return node

    def represent_ordered_mapping_flowing(self, data):
        node = self.represent_ordered_mapping(data)
        node.flow_style = True
        return node

    def represent_list_flowing(self, data):
        node = self.represent_list(data)
        node.flow_style = True
        return node

    def represent_ml_string(self, data):
        node = self.represent_str(data)
        node.style = '|'
        return node


class OrderedDictFlowing(OrderedDict):
    pass


class ListFlowing(list):
    pass


class MultiLineString(str):
    pass


GRCDumper.add_representer(OrderedDict, GRCDumper.represent_ordered_mapping)
GRCDumper.add_representer(OrderedDictFlowing, GRCDumper.represent_ordered_mapping_flowing)
GRCDumper.add_representer(ListFlowing, GRCDumper.represent_list_flowing)
GRCDumper.add_representer(tuple, GRCDumper.represent_list)
GRCDumper.add_representer(MultiLineString, GRCDumper.represent_ml_string)
GRCDumper.add_representer(yaml.nodes.ScalarNode, lambda r, n: n)
GRCDumper.add_representer(attributed_str, GRCDumper.represent_str)


def dump(data, stream=None, **kwargs):
    config = dict(stream=stream, default_flow_style=False, indent=4, Dumper=GRCDumper)
    config.update(kwargs)
    return yaml.dump_all([data], **config)


safe_load = yaml.safe_load
__with_libyaml__ = yaml.__with_libyaml__
