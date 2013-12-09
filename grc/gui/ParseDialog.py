"""
Copyright 2013 Free Software Foundation, Inc.
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

import pygtk
pygtk.require('2.0')
import gtk, glib

from Dialogs import TextDisplay
from Constants import MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT

def get_title_label(title):
    """
    Get a title label for the params window.
    The title will be bold, underlined, and left justified.
    
    Args:
        title: the text of the title
    
    Returns:
        a gtk object
    """
    label = gtk.Label()
    label.set_markup('\n<b><span underline="low">%s</span>:</b>\n'%title)
    hbox = gtk.HBox()
    hbox.pack_start(label, False, False, padding=11)
    return hbox

class ParseDialog(gtk.Dialog):
    """
    A dialog for viewing parser errors
    """

    def __init__(self, errors):
        """
        Properties dialog contructor.
        
        Args:
            block: a block instance
        """
        self._hash = 0
        LABEL_SPACING = 7
        gtk.Dialog.__init__(self,
            title='Parser Errors',
            buttons=(gtk.STOCK_OK, gtk.RESPONSE_ACCEPT),
        )

        self._errors = errors;
 
        # set up data model   
        model = gtk.TreeStore(str)
        for k in self._errors.keys():
            n = model.append(None, [str(k)]);
            for e in self._errors[k]:
                # http://lxml.de/api/lxml.etree._LogEntry-class.html
                em = model.append(n, [ "(%s:%s:%s) %s %s %s"%(e.filename, e.line, e.column, e.level_name, e.domain_name, e.message) ] )
                try:
                    sf = open(e.filename,'r');
                    lc = sf.readlines()[e.line].rstrip('\n');
                    model.append(em, [ lc]  )
                except:
                    model.append(em, ["could not access source file"]  )
                    

        view = gtk.TreeView(model)
        tvcolumn = gtk.TreeViewColumn('XML Parser Errors by Filename')
        view.append_column(tvcolumn)
        cell = gtk.CellRendererText()
        tvcolumn.pack_start(cell, True)
        tvcolumn.add_attribute(cell, 'text', 0)
        view.set_search_column(0)
        tvcolumn.set_sort_column_id(0)
        view.set_reorderable(True)

        self.vbox.add(view);
        self.show_all()

    def run(self):
        """
        Run the dialog and get its response.
        
        Returns:
            true if the response was accept
        """
        response = gtk.Dialog.run(self)
        self.destroy()
        return response == gtk.RESPONSE_ACCEPT
