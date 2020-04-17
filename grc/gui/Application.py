"""
Copyright 2007-2011 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


from __future__ import absolute_import, print_function

import logging
import os
import subprocess

from gi.repository import Gtk, Gio, GLib, GObject
from getpass import getuser

from . import Constants, Dialogs, Actions, Executor, FileDialogs, Utils, Bars

from .MainWindow import MainWindow
# from .ParserErrorsDialog import ParserErrorsDialog
from .PropsDialog import PropsDialog

from ..core import Messages


log = logging.getLogger(__name__)


class Application(Gtk.Application):
    """
    The action handler will setup all the major window components,
    and handle button presses and flow graph operations from the GUI.
    """

    def __init__(self, file_paths, platform):
        Gtk.Application.__init__(self)
        """
        Application constructor.
        Create the main window, setup the message handler, import the preferences,
        and connect all of the action handlers. Finally, enter the gtk main loop and block.

        Args:
            file_paths: a list of flow graph file passed from command line
            platform: platform module
        """
        self.clipboard = None
        self.dialog = None

        # Setup the main window
        self.platform = platform
        self.config = platform.config

        log.debug("Application()")
        # Connect all actions to _handle_action
        for x in Actions.get_actions():
            Actions.connect(x, handler=self._handle_action)
            Actions.actions[x].enable()
            if x.startswith("app."):
                self.add_action(Actions.actions[x])
            # Setup the shortcut keys
            # These are the globally defined shortcuts
            keypress = Actions.actions[x].keypresses
            if keypress:
                self.set_accels_for_action(x, keypress)

        # Initialize
        self.init_file_paths = [os.path.abspath(file_path) for file_path in file_paths]
        self.init = False

    def do_startup(self):
        Gtk.Application.do_startup(self)
        log.debug("Application.do_startup()")

        # Setup the menu
        log.debug("Creating menu")
        '''
        self.menu = Bars.Menu()
        self.set_menu()
        if self.prefers_app_menu():
            self.set_app_menu(self.menu)
        else:
            self.set_menubar(self.menu)
        '''

    def do_activate(self):
        Gtk.Application.do_activate(self)
        log.debug("Application.do_activate()")

        self.main_window = MainWindow(self, self.platform)
        self.main_window.connect('delete-event', self._quit)
        self.get_focus_flag = self.main_window.get_focus_flag

        #setup the messages
        Messages.register_messenger(self.main_window.add_console_line)
        Messages.send_init(self.platform)

        log.debug("Calling Actions.APPLICATION_INITIALIZE")
        Actions.APPLICATION_INITIALIZE()

    def _quit(self, window, event):
        """
        Handle the delete event from the main window.
        Generated by pressing X to close, alt+f4, or right click+close.
        This method in turns calls the state handler to quit.

        Returns:
            true
        """
        Actions.APPLICATION_QUIT()
        return True

    def _handle_action(self, action, *args):
        log.debug("_handle_action({0}, {1})".format(action, args))
        main = self.main_window
        page = main.current_page
        flow_graph = page.flow_graph if page else None

        def flow_graph_update(fg=flow_graph):
            main.vars.update_gui(fg.blocks)
            fg.update()

        ##################################################
        # Initialize/Quit
        ##################################################
        if action == Actions.APPLICATION_INITIALIZE:
            log.debug("APPLICATION_INITIALIZE")
            file_path_to_show = self.config.file_open()
            for file_path in (self.init_file_paths or self.config.get_open_files()):
                if os.path.exists(file_path):
                    main.new_page(file_path, show=file_path_to_show == file_path)
            if not main.current_page:
                main.new_page()  # ensure that at least a blank page exists

            main.btwin.search_entry.hide()

            """
            Only disable certain actions on startup. Each of these actions are
            conditionally enabled in _handle_action, so disable them first.
             - FLOW_GRAPH_UNDO/REDO are set in gui/StateCache.py
             - XML_PARSER_ERRORS_DISPLAY is set in RELOAD_BLOCKS

            TODO: These 4 should probably be included, but they are not currently
            enabled anywhere else:
             - PORT_CONTROLLER_DEC, PORT_CONTROLLER_INC
             - BLOCK_INC_TYPE, BLOCK_DEC_TYPE

            TODO: These should be handled better. They are set in
            update_exec_stop(), but not anywhere else
             - FLOW_GRAPH_GEN, FLOW_GRAPH_EXEC, FLOW_GRAPH_KILL
            """
            for action in (
                Actions.ERRORS_WINDOW_DISPLAY,
                Actions.ELEMENT_DELETE,
                Actions.BLOCK_PARAM_MODIFY,
                Actions.BLOCK_ROTATE_CCW,
                Actions.BLOCK_ROTATE_CW,
                Actions.BLOCK_VALIGN_TOP,
                Actions.BLOCK_VALIGN_MIDDLE,
                Actions.BLOCK_VALIGN_BOTTOM,
                Actions.BLOCK_HALIGN_LEFT,
                Actions.BLOCK_HALIGN_CENTER,
                Actions.BLOCK_HALIGN_RIGHT,
                Actions.BLOCK_CUT,
                Actions.BLOCK_COPY,
                Actions.BLOCK_PASTE,
                Actions.BLOCK_ENABLE,
                Actions.BLOCK_DISABLE,
                Actions.BLOCK_BYPASS,
                Actions.BLOCK_CREATE_HIER,
                Actions.OPEN_HIER,
                Actions.BUSSIFY_SOURCES,
                Actions.BUSSIFY_SINKS,
                Actions.FLOW_GRAPH_SAVE,
                Actions.FLOW_GRAPH_UNDO,
                Actions.FLOW_GRAPH_REDO,
                Actions.XML_PARSER_ERRORS_DISPLAY
            ):
                action.disable()

            # Load preferences
            for action in (
                Actions.TOGGLE_BLOCKS_WINDOW,
                Actions.TOGGLE_CONSOLE_WINDOW,
                Actions.TOGGLE_HIDE_DISABLED_BLOCKS,
                Actions.TOGGLE_SCROLL_LOCK,
                Actions.TOGGLE_AUTO_HIDE_PORT_LABELS,
                Actions.TOGGLE_SNAP_TO_GRID,
                Actions.TOGGLE_SHOW_BLOCK_COMMENTS,
                Actions.TOGGLE_SHOW_CODE_PREVIEW_TAB,
                Actions.TOGGLE_SHOW_FLOWGRAPH_COMPLEXITY,
                Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR,
                Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR_SIDEBAR,
                Actions.TOGGLE_HIDE_VARIABLES,
                Actions.TOGGLE_SHOW_BLOCK_IDS,
            ):
                action.set_enabled(True)
                if hasattr(action, 'load_from_preferences'):
                    action.load_from_preferences()

            # Hide the panels *IF* it's saved in preferences
            main.update_panel_visibility(main.BLOCKS, Actions.TOGGLE_BLOCKS_WINDOW.get_active())
            main.update_panel_visibility(main.CONSOLE, Actions.TOGGLE_CONSOLE_WINDOW.get_active())
            main.update_panel_visibility(main.VARIABLES, Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR.get_active())

            # Force an update on the current page to match loaded preferences.
            # In the future, change the __init__ order to load preferences first
            page = main.current_page
            if page:
                page.flow_graph.update()

            self.init = True
        elif action == Actions.APPLICATION_QUIT:
            if main.close_pages():
                while Gtk.main_level():
                    Gtk.main_quit()
                exit(0)
        ##################################################
        # Selections
        ##################################################
        elif action == Actions.ELEMENT_SELECT:
            pass #do nothing, update routines below
        elif action == Actions.NOTHING_SELECT:
            flow_graph.unselect()
        elif action == Actions.SELECT_ALL:
            if main.btwin.search_entry.has_focus():
                main.btwin.search_entry.select_region(0, -1)
            else:
                flow_graph.select_all()
        ##################################################
        # Enable/Disable
        ##################################################
        elif action in (Actions.BLOCK_ENABLE, Actions.BLOCK_DISABLE, Actions.BLOCK_BYPASS):
            changed = flow_graph.change_state_selected(new_state={
                Actions.BLOCK_ENABLE: 'enabled',
                Actions.BLOCK_DISABLE: 'disabled',
                Actions.BLOCK_BYPASS: 'bypassed',
            }[action])
            if changed:
                flow_graph_update()
                page.state_cache.save_new_state(flow_graph.export_data())
                page.saved = False
        ##################################################
        # Cut/Copy/Paste
        ##################################################
        elif action == Actions.BLOCK_CUT:
            Actions.BLOCK_COPY()
            Actions.ELEMENT_DELETE()
        elif action == Actions.BLOCK_COPY:
            self.clipboard = flow_graph.copy_to_clipboard()
        elif action == Actions.BLOCK_PASTE:
            if self.clipboard:
                flow_graph.paste_from_clipboard(self.clipboard)
                flow_graph_update()
                page.state_cache.save_new_state(flow_graph.export_data())
                page.saved = False
        ##################################################
        # Create hier block
        ##################################################
        elif action == Actions.BLOCK_CREATE_HIER:

            selected_blocks = []

            pads = []
            params = set()

            for block in flow_graph.selected_blocks():
                selected_blocks.append(block)
                # Check for string variables within the blocks
                for param in block.params.values():
                    for variable in flow_graph.get_variables():
                        # If a block parameter exists that is a variable, create a parameter for it
                        if param.get_value() == variable.name:
                            params.add(param.get_value())
                    for flow_param in flow_graph.get_parameters():
                        # If a block parameter exists that is a parameter, create a parameter for it
                        if param.get_value() == flow_param.name:
                            params.add(param.get_value())

            x_min = min(block.coordinate[0] for block in selected_blocks)
            y_min = min(block.coordinate[1] for block in selected_blocks)


            for connection in flow_graph.connections:

                # Get id of connected blocks
                source = connection.source_block
                sink = connection.sink_block

                if source not in selected_blocks and sink in selected_blocks:
                    # Create Pad Source
                    pads.append({
                        'key': connection.sink_port.key,
                        'coord': source.coordinate,
                        'block_index': selected_blocks.index(sink) + 1, # Ignore the options block
                        'direction': 'source'
                    })

                elif sink not in selected_blocks and source in selected_blocks:
                    # Create Pad Sink
                    pads.append({
                        'key': connection.source_port.key,
                        'coord': sink.coordinate,
                        'block_index': selected_blocks.index(source) + 1, # Ignore the options block
                        'direction': 'sink'
                    })

            # Copy the selected blocks and paste them into a new page
            #   then move the flowgraph to a reasonable position
            Actions.BLOCK_COPY()
            main.new_page()
            flow_graph = main.current_page.flow_graph
            Actions.BLOCK_PASTE()
            coords = (x_min,y_min)
            flow_graph.move_selected(coords)

            # Set flow graph to heir block type
            top_block  = flow_graph.get_block("top_block")
            top_block.params['generate_options'].set_value('hb')

            # this needs to be a unique name
            top_block.params['id'].set_value('new_hier')

            # Remove the default samp_rate variable block that is created
            remove_me  = flow_graph.get_block("samp_rate")
            flow_graph.remove_element(remove_me)

            # Add the param blocks along the top of the window
            x_pos = 150
            for param in params:
                param_id = flow_graph.add_new_block('parameter',(x_pos,10))
                param_block = flow_graph.get_block(param_id)
                param_block.params['id'].set_value(param)
                x_pos = x_pos + 100

            for pad in pads:
                # add the pad sources and sinks within the new hier block
                if pad['direction'] == 'sink':

                    # add new pad_sink block to the canvas
                    pad_id = flow_graph.add_new_block('pad_sink', pad['coord'])

                    # setup the references to the sink and source
                    pad_block = flow_graph.get_block(pad_id)
                    pad_sink = pad_block.sinks[0]

                    source_block = flow_graph.get_block(flow_graph.blocks[pad['block_index']].name)
                    source = source_block.get_source(pad['key'])

                    # ensure the port types match
                    if pad_sink.dtype != source.dtype:
                        if pad_sink.dtype == 'complex' and source.dtype == 'fc32':
                            pass
                        else:
                            pad_block.params['type'].value = source.dtype
                            pad_sink.dtype = source.dtype

                    # connect the pad to the proper sinks
                    new_connection = flow_graph.connect(source,pad_sink)

                elif pad['direction'] == 'source':
                    pad_id = flow_graph.add_new_block('pad_source', pad['coord'])

                    # setup the references to the sink and source
                    pad_block = flow_graph.get_block(pad_id)
                    pad_source = pad_block.sources[0]

                    sink_block = flow_graph.get_block(flow_graph.blocks[pad['block_index']].name)
                    sink = sink_block.get_sink(pad['key'])

                    # ensure the port types match
                    if pad_source.dtype != sink.dtype:
                        if pad_source.dtype == 'complex' and sink.dtype == 'fc32':
                            pass
                        else:
                            pad_block.params['type'].value = sink.dtype
                            pad_source.dtype = sink.dtype

                    # connect the pad to the proper sinks
                    new_connection = flow_graph.connect(pad_source, sink)

            flow_graph_update(flow_graph)
        ##################################################
        # Move/Rotate/Delete/Create
        ##################################################
        elif action == Actions.BLOCK_MOVE:
            page.state_cache.save_new_state(flow_graph.export_data())
            page.saved = False
        elif action in Actions.BLOCK_ALIGNMENTS:
            if flow_graph.align_selected(action):
                page.state_cache.save_new_state(flow_graph.export_data())
                page.saved = False
        elif action == Actions.BLOCK_ROTATE_CCW:
            if flow_graph.rotate_selected(90):
                flow_graph_update()
                page.state_cache.save_new_state(flow_graph.export_data())
                page.saved = False
        elif action == Actions.BLOCK_ROTATE_CW:
            if flow_graph.rotate_selected(-90):
                flow_graph_update()
                page.state_cache.save_new_state(flow_graph.export_data())
                page.saved = False
        elif action == Actions.ELEMENT_DELETE:
            if flow_graph.remove_selected():
                flow_graph_update()
                page.state_cache.save_new_state(flow_graph.export_data())
                Actions.NOTHING_SELECT()
                page.saved = False
        elif action == Actions.ELEMENT_CREATE:
            flow_graph_update()
            page.state_cache.save_new_state(flow_graph.export_data())
            Actions.NOTHING_SELECT()
            page.saved = False
        elif action == Actions.BLOCK_INC_TYPE:
            if flow_graph.type_controller_modify_selected(1):
                flow_graph_update()
                page.state_cache.save_new_state(flow_graph.export_data())
                page.saved = False
        elif action == Actions.BLOCK_DEC_TYPE:
            if flow_graph.type_controller_modify_selected(-1):
                flow_graph_update()
                page.state_cache.save_new_state(flow_graph.export_data())
                page.saved = False
        elif action == Actions.PORT_CONTROLLER_INC:
            if flow_graph.port_controller_modify_selected(1):
                flow_graph_update()
                page.state_cache.save_new_state(flow_graph.export_data())
                page.saved = False
        elif action == Actions.PORT_CONTROLLER_DEC:
            if flow_graph.port_controller_modify_selected(-1):
                flow_graph_update()
                page.state_cache.save_new_state(flow_graph.export_data())
                page.saved = False
        ##################################################
        # Window stuff
        ##################################################
        elif action == Actions.ABOUT_WINDOW_DISPLAY:
            Dialogs.show_about(main, self.platform.config)
        elif action == Actions.HELP_WINDOW_DISPLAY:
            Dialogs.show_help(main)
        elif action == Actions.TYPES_WINDOW_DISPLAY:
            Dialogs.show_types(main)
        elif action == Actions.KEYBOARD_SHORTCUTS_WINDOW_DISPLAY:
            Dialogs.show_keyboard_shortcuts(main)   
        elif action == Actions.ERRORS_WINDOW_DISPLAY:
            Dialogs.ErrorsDialog(main, flow_graph).run_and_destroy()
        elif action == Actions.TOGGLE_CONSOLE_WINDOW:
            action.set_active(not action.get_active())
            main.update_panel_visibility(main.CONSOLE, action.get_active())
            action.save_to_preferences()
        elif action == Actions.TOGGLE_BLOCKS_WINDOW:
            # This would be better matched to a Gio.PropertyAction, but to do
            # this, actions would have to be defined in the window not globally
            action.set_active(not action.get_active())
            main.update_panel_visibility(main.BLOCKS, action.get_active())
            action.save_to_preferences()
        elif action == Actions.TOGGLE_SCROLL_LOCK:
            action.set_active(not action.get_active())
            active = action.get_active()
            main.console.text_display.scroll_lock = active
            if active:
                main.console.text_display.scroll_to_end()
            action.save_to_preferences()
        elif action == Actions.CLEAR_CONSOLE:
            main.console.text_display.clear()
        elif action == Actions.SAVE_CONSOLE:
            file_path = FileDialogs.SaveConsole(main, page.file_path).run()
            if file_path is not None:
                main.console.text_display.save(file_path)
        elif action == Actions.TOGGLE_HIDE_DISABLED_BLOCKS:
            action.set_active(not action.get_active())
            flow_graph_update()
            action.save_to_preferences()
            page.state_cache.save_new_state(flow_graph.export_data())
            Actions.NOTHING_SELECT()
        elif action == Actions.TOGGLE_AUTO_HIDE_PORT_LABELS:
            action.set_active(not action.get_active())
            action.save_to_preferences()
            for page in main.get_pages():
                page.flow_graph.create_shapes()
        elif action in (Actions.TOGGLE_SNAP_TO_GRID,
                        Actions.TOGGLE_SHOW_BLOCK_COMMENTS,
                        Actions.TOGGLE_SHOW_CODE_PREVIEW_TAB):
            action.set_active(not action.get_active())
            action.save_to_preferences()
        elif action == Actions.TOGGLE_SHOW_FLOWGRAPH_COMPLEXITY:
            action.set_active(not action.get_active())
            action.save_to_preferences()
            for page in main.get_pages():
                flow_graph_update(page.flow_graph)
        elif action == Actions.TOGGLE_HIDE_VARIABLES:
            action.set_active(not action.get_active())
            active = action.get_active()
            # Either way, triggering this should simply trigger the variable editor
            # to be visible.
            varedit = Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR
            if active:
                log.debug("Variables are hidden. Forcing the variable panel to be visible.")
                varedit.disable()
            else:
                varedit.enable()
            # Just force it to show.
            varedit.set_active(True)
            main.update_panel_visibility(main.VARIABLES)
            Actions.NOTHING_SELECT()
            action.save_to_preferences()
            varedit.save_to_preferences()
            flow_graph_update()
        elif action == Actions.TOGGLE_SHOW_BLOCK_IDS:
            action.set_active(not action.get_active())
            active = action.get_active()
            Actions.NOTHING_SELECT()
            action.save_to_preferences()
            flow_graph_update()
        elif action == Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR:
            # TODO: There may be issues at startup since these aren't triggered
            # the same was as Gtk.Actions when loading preferences.
            action.set_active(not action.get_active())
            # Just assume this was triggered because it was enabled.
            main.update_panel_visibility(main.VARIABLES, action.get_active())
            action.save_to_preferences()

            # Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR_SIDEBAR.set_enabled(action.get_active())
            action.save_to_preferences()
        elif action == Actions.TOGGLE_FLOW_GRAPH_VAR_EDITOR_SIDEBAR:
            action.set_active(not action.get_active())
            if self.init:
                Dialogs.MessageDialogWrapper(
                    main, Gtk.MessageType.INFO, Gtk.ButtonsType.CLOSE,
                    markup="Moving the variable editor requires a restart of GRC."
                ).run_and_destroy()
                action.save_to_preferences()
        ##################################################
        # Param Modifications
        ##################################################
        elif action == Actions.BLOCK_PARAM_MODIFY:
            selected_block = args[0] if args[0] else flow_graph.selected_block
            if selected_block:
                self.dialog = PropsDialog(self.main_window, selected_block)
                response = Gtk.ResponseType.APPLY
                while response == Gtk.ResponseType.APPLY:  # rerun the dialog if Apply was hit
                    response = self.dialog.run()
                    if response in (Gtk.ResponseType.APPLY, Gtk.ResponseType.ACCEPT):
                        page.state_cache.save_new_state(flow_graph.export_data())
                        ### Following  lines force an complete update of io ports
                        n = page.state_cache.get_current_state()
                        flow_graph.import_data(n)
                        flow_graph_update()

                        page.saved = False
                    else:  # restore the current state
                        n = page.state_cache.get_current_state()
                        flow_graph.import_data(n)
                        flow_graph_update()
                    if response == Gtk.ResponseType.APPLY:
                        # null action, that updates the main window
                        Actions.ELEMENT_SELECT()
                self.dialog.destroy()
                self.dialog = None
        elif action == Actions.EXTERNAL_UPDATE:
            page.state_cache.save_new_state(flow_graph.export_data())
            flow_graph_update()
            if self.dialog is not None:
                self.dialog.update_gui(force=True)
            page.saved = False
        elif action == Actions.VARIABLE_EDITOR_UPDATE:
            page.state_cache.save_new_state(flow_graph.export_data())
            flow_graph_update()
            page.saved = False
        ##################################################
        # View Parser Errors
        ##################################################
        elif action == Actions.XML_PARSER_ERRORS_DISPLAY:
            pass
        ##################################################
        # Undo/Redo
        ##################################################
        elif action == Actions.FLOW_GRAPH_UNDO:
            n = page.state_cache.get_prev_state()
            if n:
                flow_graph.unselect()
                flow_graph.import_data(n)
                flow_graph_update()
                page.saved = False
        elif action == Actions.FLOW_GRAPH_REDO:
            n = page.state_cache.get_next_state()
            if n:
                flow_graph.unselect()
                flow_graph.import_data(n)
                flow_graph_update()
                page.saved = False
        ##################################################
        # New/Open/Save/Close
        ##################################################
        elif action == Actions.FLOW_GRAPH_NEW:
            main.new_page()
            args = (GLib.Variant('s', 'qt_gui'),)
            flow_graph = main.current_page.flow_graph
            flow_graph.options_block.params['generate_options'].set_value(str(args[0])[1:-1])
            flow_graph.options_block.params['author'].set_value(getuser())
            flow_graph_update(flow_graph)
        elif action == Actions.FLOW_GRAPH_NEW_TYPE:
            main.new_page()
            if args:
                flow_graph = main.current_page.flow_graph
                flow_graph.options_block.params['generate_options'].set_value(str(args[0])[1:-1])
                flow_graph_update(flow_graph)
        elif action == Actions.FLOW_GRAPH_OPEN:
            file_paths = args[0] if args[0] else FileDialogs.OpenFlowGraph(main, page.file_path).run()
            if file_paths: # Open a new page for each file, show only the first
                for i,file_path in enumerate(file_paths):
                    main.new_page(file_path, show=(i==0))
                    self.config.add_recent_file(file_path)
                    main.tool_bar.refresh_submenus()
                    #main.menu_bar.refresh_submenus()
        elif action == Actions.FLOW_GRAPH_OPEN_QSS_THEME:
            file_paths = FileDialogs.OpenQSS(main, self.platform.config.install_prefix +
                                             '/share/gnuradio/themes/').run()
            if file_paths:
                self.platform.config.default_qss_theme = file_paths[0]
        elif action == Actions.FLOW_GRAPH_CLOSE:
            main.close_page()
        elif action == Actions.FLOW_GRAPH_CLOSE_MIDDLE_MOUSE:
            main.close_page()
        elif action == Actions.FLOW_GRAPH_OPEN_RECENT:
            file_path = str(args[0])[1:-1]
            main.new_page(file_path, show=True)
            main.tool_bar.refresh_submenus()
        elif action == Actions.FLOW_GRAPH_SAVE:
            #read-only or undefined file path, do save-as
            if page.get_read_only() or not page.file_path:
                Actions.FLOW_GRAPH_SAVE_AS()
            #otherwise try to save
            else:
                try:
                    self.platform.save_flow_graph(page.file_path, flow_graph)
                    flow_graph.grc_file_path = page.file_path
                    page.saved = True
                except IOError:
                    Messages.send_fail_save(page.file_path)
                    page.saved = False
        elif action == Actions.FLOW_GRAPH_SAVE_AS:
            file_path = FileDialogs.SaveFlowGraph(main, page.file_path).run()

            if file_path is not None:
                if flow_graph.options_block.params['id'].get_value() == 'default':
                    file_name = os.path.basename(file_path).replace(".grc", "")
                    flow_graph.options_block.params['id'].set_value(file_name)
                    flow_graph_update(flow_graph)

                page.file_path = os.path.abspath(file_path)
                try:
                    self.platform.save_flow_graph(page.file_path, flow_graph)
                    flow_graph.grc_file_path = page.file_path
                    page.saved = True
                except IOError:
                    Messages.send_fail_save(page.file_path)
                    page.saved = False
                self.config.add_recent_file(file_path)
                main.tool_bar.refresh_submenus()
                #TODO
                #main.menu_bar.refresh_submenus()
        elif action == Actions.FLOW_GRAPH_SAVE_COPY:
            try:
                if not page.file_path:
                    # Make sure the current flowgraph has been saved
                    Actions.FLOW_GRAPH_SAVE_AS()
                else:
                    dup_file_path = page.file_path
                    dup_file_name = '.'.join(dup_file_path.split('.')[:-1]) + "_copy" # Assuming .grc extension at the end of file_path
                    dup_file_path_temp = dup_file_name + Constants.FILE_EXTENSION
                    count = 1
                    while os.path.exists(dup_file_path_temp):
                        dup_file_path_temp = '{}({}){}'.format(dup_file_name, count, Constants.FILE_EXTENSION)
                        count += 1
                    dup_file_path_user = FileDialogs.SaveFlowGraph(main, dup_file_path_temp).run()
                    if dup_file_path_user is not None:
                        self.platform.save_flow_graph(dup_file_path_user, flow_graph)
                        Messages.send('Saved Copy to: "' + dup_file_path_user + '"\n')
            except IOError:
                Messages.send_fail_save("Can not create a copy of the flowgraph\n")
        elif action == Actions.FLOW_GRAPH_DUPLICATE:
            previous = flow_graph
            # Create a new page
            main.new_page()
            page = main.current_page
            new_flow_graph = page.flow_graph
            # Import the old data and mark the current as not saved
            new_flow_graph.import_data(previous.export_data())
            flow_graph_update(new_flow_graph)
            page.state_cache.save_new_state(new_flow_graph.export_data())
            page.saved = False
        elif action == Actions.FLOW_GRAPH_SCREEN_CAPTURE:
            file_path, background_transparent = FileDialogs.SaveScreenShot(main, page.file_path).run()
            if file_path is not None:
                try:
                    Utils.make_screenshot(flow_graph, file_path, background_transparent)
                except ValueError:
                    Messages.send('Failed to generate screen shot\n')
        ##################################################
        # Gen/Exec/Stop
        ##################################################
        elif action == Actions.FLOW_GRAPH_GEN:
            if not page.process:
                if not page.saved or not page.file_path:
                    Actions.FLOW_GRAPH_SAVE()  # only save if file path missing or not saved
                if page.saved and page.file_path:
                    generator = page.get_generator()
                    try:
                        Messages.send_start_gen(generator.file_path)
                        generator.write()
                    except Exception as e:
                        Messages.send_fail_gen(e)
                else:
                    self.generator = None
        elif action == Actions.FLOW_GRAPH_EXEC:
            if not page.process:
                Actions.FLOW_GRAPH_GEN()
                xterm = self.platform.config.xterm_executable
                if self.config.xterm_missing() != xterm:
                    if not os.path.exists(xterm):
                        Dialogs.show_missing_xterm(main, xterm)
                    self.config.xterm_missing(xterm)
                if page.saved and page.file_path:
                    Executor.ExecFlowGraphThread(
                        flow_graph_page=page,
                        xterm_executable=xterm,
                        callback=self.update_exec_stop
                    )
        elif action == Actions.FLOW_GRAPH_KILL:
            if page.process:
                try:
                    page.process.terminate()
                except OSError:
                    print("could not terminate process: %d" % page.process.pid)

        elif action == Actions.PAGE_CHANGE:  # pass and run the global actions
            flow_graph_update()
        elif action == Actions.RELOAD_BLOCKS:
            self.platform.build_library()
            main.btwin.repopulate()

            #todo: implement parser error dialog for YAML

            # Force a redraw of the graph, by getting the current state and re-importing it
            main.update_pages()

        elif action == Actions.FIND_BLOCKS:
            flow_graph.unselect()
            main.update_panel_visibility(main.BLOCKS, True)
            main.btwin.search_entry.show()
            main.btwin.search_entry.grab_focus()
        elif action == Actions.OPEN_HIER:
            for b in flow_graph.selected_blocks():
                grc_source = b.extra_data.get('grc_source', '')
                if grc_source:
                    main.new_page(grc_source, show=True)
        elif action == Actions.BUSSIFY_SOURCES:
            for b in flow_graph.selected_blocks():
                b.bussify('source')
            flow_graph._old_selected_port = None
            flow_graph._new_selected_port = None
            Actions.ELEMENT_CREATE()

        elif action == Actions.BUSSIFY_SINKS:
            for b in flow_graph.selected_blocks():
                b.bussify('sink')
            flow_graph._old_selected_port = None
            flow_graph._new_selected_port = None
            Actions.ELEMENT_CREATE()

        elif action == Actions.TOOLS_RUN_FDESIGN:
            subprocess.Popen('gr_filter_design',
                             shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

        else:
            log.warning('!!! Action "%s" not handled !!!' % action)
        ##################################################
        # Global Actions for all States
        ##################################################
        page = main.current_page  # page and flow graph might have changed
        flow_graph = page.flow_graph if page else None

        selected_blocks = list(flow_graph.selected_blocks())
        selected_block = selected_blocks[0] if selected_blocks else None

        #update general buttons
        Actions.ERRORS_WINDOW_DISPLAY.set_enabled(not flow_graph.is_valid())
        Actions.ELEMENT_DELETE.set_enabled(bool(flow_graph.selected_elements))
        Actions.BLOCK_PARAM_MODIFY.set_enabled(bool(selected_block))
        Actions.BLOCK_ROTATE_CCW.set_enabled(bool(selected_blocks))
        Actions.BLOCK_ROTATE_CW.set_enabled(bool(selected_blocks))
        #update alignment options
        for act in Actions.BLOCK_ALIGNMENTS:
            if act:
                act.set_enabled(len(selected_blocks) > 1)
        #update cut/copy/paste
        Actions.BLOCK_CUT.set_enabled(bool(selected_blocks))
        Actions.BLOCK_COPY.set_enabled(bool(selected_blocks))
        Actions.BLOCK_PASTE.set_enabled(bool(self.clipboard))
        #update enable/disable/bypass
        can_enable = any(block.state != 'enabled'
                         for block in selected_blocks)
        can_disable = any(block.state != 'disabled'
                          for block in selected_blocks)
        can_bypass_all = (
            all(block.can_bypass() for block in selected_blocks) and
            any(not block.get_bypassed() for block in selected_blocks)
        )
        Actions.BLOCK_ENABLE.set_enabled(can_enable)
        Actions.BLOCK_DISABLE.set_enabled(can_disable)
        Actions.BLOCK_BYPASS.set_enabled(can_bypass_all)

        Actions.BLOCK_CREATE_HIER.set_enabled(bool(selected_blocks))
        Actions.OPEN_HIER.set_enabled(bool(selected_blocks))
        Actions.BUSSIFY_SOURCES.set_enabled(bool(selected_blocks))
        Actions.BUSSIFY_SINKS.set_enabled(bool(selected_blocks))
        Actions.RELOAD_BLOCKS.enable()
        Actions.FIND_BLOCKS.enable()

        self.update_exec_stop()

        Actions.FLOW_GRAPH_SAVE.set_enabled(not page.saved)
        main.update()

        flow_graph.update_selected()
        page.drawing_area.queue_draw()

        return True  # Action was handled

    def update_exec_stop(self):
        """
        Update the exec and stop buttons.
        Lock and unlock the mutex for race conditions with exec flow graph threads.
        """
        page = self.main_window.current_page
        sensitive = page.flow_graph.is_valid() and not page.process
        Actions.FLOW_GRAPH_GEN.set_enabled(sensitive)
        Actions.FLOW_GRAPH_EXEC.set_enabled(sensitive)
        Actions.FLOW_GRAPH_KILL.set_enabled(page.process is not None)
