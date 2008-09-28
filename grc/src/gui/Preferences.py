"""
Copyright 2008 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

from .. platforms.base.Constants import FLOW_GRAPH_DTD
from .. utils import ParseXML
import Messages
import os

##Access the loaded preferences in this module
_prefs = list()
def _set_prefs(prefs): _prefs.append(prefs)
def _get_prefs(): return _prefs[0]
def load(platform): _Preferences(platform)
def save(): _get_prefs().save()
def get_preferences(): return _get_prefs().get_preferences()

class _Preferences(object):

	def __init__(self, platform):
		#make self available to module
		_set_prefs(self)
		#get prefs block
		self._prefs_block = platform.get_prefs_block()
		#prefs file path
		self._prefs_file_path = os.path.join(os.path.expanduser('~'), self._prefs_block.get_param('prefs_file').get_value())
		#load
		try:
			ParseXML.validate_dtd(self._prefs_file_path, FLOW_GRAPH_DTD)
			n = ParseXML.from_file(self._prefs_file_path)
			self._prefs_block.import_data(n['block'])
		except: Messages.send_fail_load_preferences(self._prefs_file_path)
		##all params
		self.window_prefix_param = self._prefs_block.get_param('window_prefix')
		self.snap_to_grid_param = self._prefs_block.get_param('snap_to_grid')
		self.grid_size_param = self._prefs_block.get_param('grid_size')
		self.show_grid_param = self._prefs_block.get_param('show_grid')
		self.show_reports_param = self._prefs_block.get_param('show_reports')
		self.restore_files_param = self._prefs_block.get_param('restore_files')
		self.window_size_param = self._prefs_block.get_param('window_size')
		self.file_open_param = self._prefs_block.get_param('file_open')
		self.files_open_param = self._prefs_block.get_param('files_open')
		self.show_params_param = self._prefs_block.get_param('show_params')

	def save(self):
		try: ParseXML.to_file({'block': self._prefs_block.export_data()}, self._prefs_file_path)
		except IOError: Messages.send_fail_save_preferences(self._prefs_file_path)

	def get_preferences(self):
		##Preferences: title, notes, params
		return [
			(
				'Grid',
				'''
Show grid will draw a square grid onto the flow graph with grid points separated by grid size pixels. \
Snap to Grid forces the upper right corner of the signal block to align with a grid point.
''',
				[self.snap_to_grid_param, self.grid_size_param, self.show_grid_param],
			),
			(
				'Appearance',
				'''
Show or hide the reports window at the bottom of the main window.
Show or hide all paramater labels in the signal blocks.
''',
				[self.show_reports_param, self.show_params_param],
			),
			(
				'Misc',
				'''
Restore previously opened files on start-up.
''',
				[self.restore_files_param],
			),
		]

###########################################################################
# Special methods for specific program functionalities
###########################################################################

def window_prefix():
	return _get_prefs().window_prefix_param.get_value()

def window_size(size=None):
	if size: _get_prefs().window_size_param.set_value(size)
	else:
		try: return eval(_get_prefs().window_size_param.get_value())
		except: return (-1, -1)

def restore_files():
	return _get_prefs().restore_files_param.get_value() == 'yes'

def file_open(file=None):
	if file is not None: _get_prefs().file_open_param.set_value(file)
	else: return _get_prefs().file_open_param.get_value()

def files_open(files=None):
	if files is not None: _get_prefs().files_open_param.set_value('\n'.join(files))
	else: return _get_prefs().files_open_param.get_value().split('\n')

def show_reports_window():
	return _get_prefs().show_reports_param.get_value() == 'show'

def get_grid_size():
	return int(_get_prefs().grid_size_param.get_value())

def snap_to_grid():
	return _get_prefs().snap_to_grid_param.get_value() == 'on'

def show_grid():
	return _get_prefs().show_grid_param.get_value() == 'show'

def show_params():
	return _get_prefs().show_params_param.get_value() == 'show'
