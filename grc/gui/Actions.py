"""
Copyright 2007-2011 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


import logging

from gi.repository import Gtk, Gdk, Gio, GLib, GObject

from . import Utils

log = logging.getLogger(__name__)


def filter_from_dict(vars):
    return filter(lambda x: isinstance(x[1], Action), vars.items())


class Namespace(object):
    def __init__(self):
        self._actions = {}

    def add(self, action):
        key = action.props.name
        self._actions[key] = action

    def connect(self, name, handler):
        #log.debug("Connecting action <{}> to handler <{}>".format(name, handler.__name__))
        self._actions[name].connect('activate', handler)

    def register(self,
                 name,
                 parameter=None,
                 handler=None,
                 label=None,
                 tooltip=None,
                 icon_name=None,
                 keypresses=None,
                 preference_name=None,
                 default=None):
        # Check types
        if not isinstance(name, str):
            raise TypeError("Cannot register function: 'name' must be a str")
        if parameter and not isinstance(parameter, str):
            raise TypeError(
                "Cannot register function: 'parameter' must be a str")
        if handler and not callable(handler):
            raise TypeError(
                "Cannot register function: 'handler' must be callable")

        # Check if the name has a prefix.
        prefix = None
        if name.startswith("app.") or name.startswith("win."):
            # Set a prefix for later and remove it
            prefix = name[0:3]
            name = name[4:]

        if handler:
            log.debug(
                "Register action [{}, prefix={}, param={}, handler={}]".format(
                    name, prefix, parameter, handler.__name__))
        else:
            log.debug(
                "Register action [{}, prefix={}, param={}, handler=None]".
                format(name, prefix, parameter))

        action = Action(name,
                        parameter,
                        label=label,
                        tooltip=tooltip,
                        icon_name=icon_name,
                        keypresses=keypresses,
                        prefix=prefix,
                        preference_name=preference_name,
                        default=default)
        if handler:
            action.connect('activate', handler)

        key = name
        if prefix:
            key = "{}.{}".format(prefix, name)
            if prefix == "app":
                pass
                # self.app.add_action(action)
            elif prefix == "win":
                pass
                # self.win.add_action(action)

        #log.debug("Registering action as '{}'".format(key))
        self._actions[key] = action
        return action

    # If the actions namespace is called, trigger an action
    def __call__(self, name):
        # Try to parse the action string.
        valid, action_name, target_value = Action.parse_detailed_name(name)
        if not valid:
            raise Exception("Invalid action string: '{}'".format(name))
        if action_name not in self._actions:
            raise Exception(
                "Action '{}' is not registered!".format(action_name))

        if target_value:
            self._actions[action_name].activate(target_value)
        else:
            self._actions[action_name].activate()

    def __getitem__(self, key):
        return self._actions[key]

    def __iter__(self):
        return self._actions.itervalues()

    def __repr__(self):
        return str(self)

    def get_actions(self):
        return self._actions

    def __str__(self):
        s = "{Actions:"
        for key in self._actions:
            s += " {},".format(key)
        s = s.rstrip(",") + "}"
        return s


class Action(Gio.SimpleAction):

    # Change these to normal python properties.
    # prefs_name

    def __init__(self,
                 name,
                 parameter=None,
                 label=None,
                 tooltip=None,
                 icon_name=None,
                 keypresses=None,
                 prefix=None,
                 preference_name=None,
                 default=None):
        self.name = name
        self.label = label
        self.tooltip = tooltip
        self.icon_name = icon_name
        if keypresses:
            self.keypresses = [
                kp.replace("<Ctrl>", Utils.get_modifier_key(True))
                for kp in keypresses
            ]
        else:
            self.keypresses = None
        self.prefix = prefix
        self.preference_name = preference_name
        self.default = default

        # Don't worry about checking types here, since it's done in register()
        # Save the parameter type to use for converting in __call__
        self.type = None

        variant = None
        state = None
        if parameter:
            variant = GLib.VariantType.new(parameter)
        if preference_name:
            state = GLib.Variant.new_boolean(True)
        Gio.SimpleAction.__init__(self,
                                  name=name,
                                  parameter_type=variant,
                                  state=state)

    def enable(self):
        self.props.enabled = True

    def disable(self):
        self.props.enabled = False

    def set_enabled(self, state):
        if not isinstance(state, bool):
            raise TypeError("State must be True/False.")
        self.props.enabled = state

    def __str__(self):
        return self.props.name

    def __repr__(self):
        return str(self)

    def get_active(self):
        if self.props.state:
            return self.props.state.get_boolean()
        return False

    def set_active(self, state):
        if not isinstance(state, bool):
            raise TypeError("State must be True/False.")
        self.change_state(GLib.Variant.new_boolean(state))

    # Allows actions to be directly called.
    def __call__(self, parameter=None):
        if self.type and parameter:
            # Try to convert it to the correct type.
            try:
                param = GLib.Variant(self.type, parameter)
                self.activate(param)
            except TypeError:
                raise TypeError(
                    "Invalid parameter type for action '{}'. Expected: '{}'".
                    format(self.get_name(), self.type))
        else:
            self.activate()

    def load_from_preferences(self, *args):
        log.debug("load_from_preferences({})".format(args))
        if self.preference_name is not None:
            config = Gtk.Application.get_default().config
            self.set_active(
                config.entry(self.preference_name, default=bool(self.default)))

    def save_to_preferences(self, *args):
        log.debug("save_to_preferences({})".format(args))
        if self.preference_name is not None:
            config = Gtk.Application.get_default().config
            config.entry(self.preference_name, value=self.get_active())


actions = Namespace()


def get_actions():
    return actions.get_actions()


def connect(action, handler=None):
    return actions.connect(action, handler=handler)


########################################################################
# Old Actions
########################################################################
PAGE_CHANGE = actions.register("win.page_change")
EXTERNAL_UPDATE = actions.register("app.external_update")
VARIABLE_EDITOR_UPDATE = actions.register("app.variable_editor_update")
FLOW_GRAPH_NEW = actions.register(
    "app.flowgraph.new",
    label='_New',
    tooltip='Create a new flow graph',
    icon_name='document-new',
    keypresses=["<Ctrl>n"],
)
FLOW_GRAPH_NEW_TYPE = actions.register(
    "app.flowgraph.new_type",
    parameter="s",
)
FLOW_GRAPH_OPEN = actions.register(
    "app.flowgraph.open",
    label='_Open',
    tooltip='Open an existing flow graph',
    icon_name='document-open',
    keypresses=["<Ctrl>o"],
)
FLOW_GRAPH_OPEN_RECENT = actions.register(
    "app.flowgraph.open_recent",
    label='Open _Recent',
    tooltip='Open a recently used flow graph',
    icon_name='document-open-recent',
    parameter="s",
)
FLOW_GRAPH_CLEAR_RECENT = actions.register("app.flowgraph.clear_recent")
FLOW_GRAPH_SAVE = actions.register(
    "app.flowgraph.save",
    label='_Save',
    tooltip='Save the current flow graph',
    icon_name='document-save',
    keypresses=["<Ctrl>s"],
)
FLOW_GRAPH_SAVE_AS = actions.register(
    "app.flowgraph.save_as",
    label='Save _As',
    tooltip='Save the current flow graph as...',
    icon_name='document-save-as',
    keypresses=["<Ctrl><Shift>s"],
)
FLOW_GRAPH_SAVE_COPY = actions.register(
    "app.flowgraph.save_copy",
    label='Save Copy',
    tooltip='Save a copy of current flow graph',
)
FLOW_GRAPH_DUPLICATE = actions.register(
    "app.flowgraph.duplicate",
    label='_Duplicate',
    tooltip='Create a duplicate of current flow graph',
    # stock_id=Gtk.STOCK_COPY,
    keypresses=["<Ctrl><Shift>d"],
)
FLOW_GRAPH_CLOSE = actions.register(
    "app.flowgraph.close",
    label='_Close',
    tooltip='Close the current flow graph',
    icon_name='window-close',
    keypresses=["<Ctrl>w"],
)
APPLICATION_INITIALIZE = actions.register("app.initialize")
APPLICATION_QUIT = actions.register(
    "app.quit",
    label='_Quit',
    tooltip='Quit program',
    icon_name='application-exit',
    keypresses=["<Ctrl>q"],
)
FLOW_GRAPH_UNDO = actions.register(
    "win.undo",
    label='_Undo',
    tooltip='Undo a change to the flow graph',
    icon_name='edit-undo',
    keypresses=["<Ctrl>z"],
)
FLOW_GRAPH_REDO = actions.register(
    "win.redo",
    label='_Redo',
    tooltip='Redo a change to the flow graph',
    icon_name='edit-redo',
    keypresses=["<Ctrl>y"],
)
NOTHING_SELECT = actions.register("win.unselect")
SELECT_ALL = actions.register(
    "win.select_all",
    label='Select _All',
    tooltip='Select all blocks and connections in the flow graph',
    icon_name='edit-select-all',
    keypresses=["<Ctrl>a"],
)
ELEMENT_SELECT = actions.register("win.select")
ELEMENT_CREATE = actions.register("win.add")
ELEMENT_DELETE = actions.register(
    "win.delete",
    label='_Delete',
    tooltip='Delete the selected blocks',
    icon_name='edit-delete',
    keypresses=["Delete"],
)
BLOCK_MOVE = actions.register("win.block_move")
BLOCK_ROTATE_CCW = actions.register(
    "win.block_rotate_ccw",
    label='Rotate Counterclockwise',
    tooltip='Rotate the selected blocks 90 degrees to the left',
    icon_name='object-rotate-left',
    keypresses=["Left"],
)
BLOCK_ROTATE_CW = actions.register(
    "win.block_rotate",
    label='Rotate Clockwise',
    tooltip='Rotate the selected blocks 90 degrees to the right',
    icon_name='object-rotate-right',
    keypresses=["Right"],
)
BLOCK_VALIGN_TOP = actions.register(
    "win.block_align_top",
    label='Vertical Align Top',
    tooltip='Align tops of selected blocks',
    keypresses=["<Shift>t"],
)
BLOCK_VALIGN_MIDDLE = actions.register(
    "win.block_align_middle",
    label='Vertical Align Middle',
    tooltip='Align centers of selected blocks vertically',
    keypresses=["<Shift>m"],
)
BLOCK_VALIGN_BOTTOM = actions.register(
    "win.block_align_bottom",
    label='Vertical Align Bottom',
    tooltip='Align bottoms of selected blocks',
    keypresses=["<Shift>b"],
)
BLOCK_HALIGN_LEFT = actions.register(
    "win.block_align_left",
    label='Horizontal Align Left',
    tooltip='Align left edges of blocks selected blocks',
    keypresses=["<Shift>l"],
)
BLOCK_HALIGN_CENTER = actions.register(
    "win.block_align_center",
    label='Horizontal Align Center',
    tooltip='Align centers of selected blocks horizontally',
    keypresses=["<Shift>c"],
)
BLOCK_HALIGN_RIGHT = actions.register(
    "win.block_align_right",
    label='Horizontal Align Right',
    tooltip='Align right edges of selected blocks',
    keypresses=["<Shift>r"],
)
BLOCK_ALIGNMENTS = [
    BLOCK_VALIGN_TOP,
    BLOCK_VALIGN_MIDDLE,
    BLOCK_VALIGN_BOTTOM,
    None,
    BLOCK_HALIGN_LEFT,
    BLOCK_HALIGN_CENTER,
    BLOCK_HALIGN_RIGHT,
]
BLOCK_PARAM_MODIFY = actions.register(
    "win.block_modify",
    label='_Properties',
    tooltip='Modify params for the selected block',
    icon_name='document-properties',
    keypresses=["Return"],
)
BLOCK_ENABLE = actions.register(
    "win.block_enable",
    label='E_nable',
    tooltip='Enable the selected blocks',
    icon_name='network-wired',
    keypresses=["e"],
)
BLOCK_DISABLE = actions.register(
    "win.block_disable",
    label='D_isable',
    tooltip='Disable the selected blocks',
    icon_name='network-wired-disconnected',
    keypresses=["d"],
)
BLOCK_BYPASS = actions.register(
    "win.block_bypass",
    label='_Bypass',
    tooltip='Bypass the selected block',
    icon_name='media-seek-forward',
    keypresses=["b"],
)
ZOOM_IN = actions.register("win.zoom_in",
                           label='Zoom In',
                           tooltip='Increase the canvas zoom level',
                           keypresses=["<Ctrl>plus",
                                       "<Ctrl>equal", "<Ctrl>KP_Add"],
                           )
ZOOM_OUT = actions.register("win.zoom_out",
                            label='Zoom Out',
                            tooltip='Decrease the canvas zoom level',
                            keypresses=["<Ctrl>minus", "<Ctrl>KP_Subtract"],
                            )
ZOOM_RESET = actions.register("win.zoom_reset",
                              label='Reset Zoom',
                              tooltip='Reset the canvas zoom level',
                              keypresses=["<Ctrl>0", "<Ctrl>KP_0"],
                              )
TOGGLE_SNAP_TO_GRID = actions.register("win.snap_to_grid",
                                       label='_Snap to grid',
                                       tooltip='Snap blocks to a grid for an easier connection alignment',
                                       preference_name='snap_to_grid',
                                       default=True,
                                       )
TOGGLE_HIDE_DISABLED_BLOCKS = actions.register(
    "win.hide_disabled",
    label='Hide _Disabled Blocks',
    tooltip='Toggle visibility of disabled blocks and connections',
    icon_name='image-missing',
    keypresses=["<Ctrl>d"],
    preference_name='hide_disabled',
    default=False,
)
TOGGLE_HIDE_VARIABLES = actions.register(
    "win.hide_variables",
    label='Hide Variables',
    tooltip='Hide all variable blocks',
    preference_name='hide_variables',
    default=False,
)
TOGGLE_SHOW_PARAMETER_EXPRESSION = actions.register(
    "win.show_param_expression",
    label='Show parameter expressions in block',
    tooltip='Display the expression that defines a parameter inside the block',
    preference_name='show_param_expression',
    default=False,
)
TOGGLE_SHOW_PARAMETER_EVALUATION = actions.register(
    "win.show_param_expression_value",
    label='Show parameter value in block',
    tooltip='Display the evaluated value of a parameter expressions inside the block',
    preference_name='show_param_expression_value',
    default=True,
)
TOGGLE_SHOW_BLOCK_IDS = actions.register(
    "win.show_block_ids",
    label='Show All Block IDs',
    tooltip='Show all the block IDs',
    preference_name='show_block_ids',
    default=False,
)
TOGGLE_FLOW_GRAPH_VAR_EDITOR = actions.register(
    "win.toggle_variable_editor",
    label='Show _Variable Editor',
    tooltip='Show the variable editor. Modify variables and imports in this flow graph',
    icon_name='accessories-text-editor',
    default=True,
    keypresses=["<Ctrl>e"],
    preference_name='variable_editor_visable',
)
TOGGLE_FLOW_GRAPH_VAR_EDITOR_SIDEBAR = actions.register(
    "win.toggle_variable_editor_sidebar",
    label='Move the Variable Editor to the Sidebar',
    tooltip='Move the variable editor to the sidebar',
    default=False,
    preference_name='variable_editor_sidebar',
)
TOGGLE_AUTO_HIDE_PORT_LABELS = actions.register(
    "win.auto_hide_port_labels",
    label='Auto-Hide _Port Labels',
    tooltip='Automatically hide port labels',
    preference_name='auto_hide_port_labels',
    default=False,
)
TOGGLE_SHOW_BLOCK_COMMENTS = actions.register(
    "win.show_block_comments",
    label='Show Block Comments',
    tooltip="Show comment beneath each block",
    preference_name='show_block_comments',
    default=True,
)
TOGGLE_SHOW_CODE_PREVIEW_TAB = actions.register(
    "win.toggle_code_preview",
    label='Generated Code Preview',
    tooltip="Show a preview of the code generated for each Block in its "
    "Properties Dialog",
    preference_name='show_generated_code_tab',
    default=False,
)
TOGGLE_SHOW_FLOWGRAPH_COMPLEXITY = actions.register(
    "win.show_flowgraph_complexity",
    label='Show Flowgraph Complexity',
    tooltip="How many Balints is the flowgraph...",
    preference_name='show_flowgraph_complexity',
    default=False,
)
BLOCK_CREATE_HIER = actions.register(
    "win.block_create_hier",
    label='C_reate Hier',
    tooltip='Create hier block from selected blocks',
    icon_name='document-new',
    keypresses=["c"],
)
BLOCK_CUT = actions.register(
    "win.block_cut",
    label='Cu_t',
    tooltip='Cut',
    icon_name='edit-cut',
    keypresses=["<Ctrl>x"],
)
BLOCK_COPY = actions.register(
    "win.block_copy",
    label='_Copy',
    tooltip='Copy',
    icon_name='edit-copy',
    keypresses=["<Ctrl>c"],
)
BLOCK_PASTE = actions.register(
    "win.block_paste",
    label='_Paste',
    tooltip='Paste',
    icon_name='edit-paste',
    keypresses=["<Ctrl>v"],
)
ERRORS_WINDOW_DISPLAY = actions.register(
    "app.errors",
    label='Flowgraph _Errors',
    tooltip='View flow graph errors',
    icon_name='dialog-error',
)
TOGGLE_CONSOLE_WINDOW = actions.register(
    "win.toggle_console_window",
    label='Show _Console Panel',
    tooltip='Toggle visibility of the console',
    keypresses=["<Ctrl>r"],
    preference_name='console_window_visible',
    default=True)
# TODO: Might be able to convert this to a Gio.PropertyAction eventually.
#       actions would need to be defined in the correct class and not globally
TOGGLE_BLOCKS_WINDOW = actions.register(
    "win.toggle_blocks_window",
    label='Show _Block Tree Panel',
    tooltip='Toggle visibility of the block tree widget',
    keypresses=["<Ctrl>b"],
    preference_name='blocks_window_visible',
    default=True)
TOGGLE_SCROLL_LOCK = actions.register(
    "win.console.scroll_lock",
    label='Console Scroll _Lock',
    tooltip='Toggle scroll lock for the console window',
    preference_name='scroll_lock',
    keypresses=["Scroll_Lock"],
    default=False,
)
ABOUT_WINDOW_DISPLAY = actions.register(
    "app.about",
    label='_About',
    tooltip='About this program',
    icon_name='help-about',
)
GET_INVOLVED_WINDOW_DISPLAY = actions.register(
    "app.get",
    label='_Get Involved',
    tooltip='Get involved in the community - instructions',
    icon_name='help-faq',
)
HELP_WINDOW_DISPLAY = actions.register(
    "app.help",
    label='_Help',
    tooltip='Usage tips',
    icon_name='help-contents',
    keypresses=["F1"],
)
TYPES_WINDOW_DISPLAY = actions.register(
    "app.types",
    label='_Types',
    tooltip='Types color mapping',
    icon_name='dialog-information',
)
KEYBOARD_SHORTCUTS_WINDOW_DISPLAY = actions.register(
    "app.keys",
    label='_Keys',
    tooltip='Keyboard - Shortcuts',
    icon_name='dialog-information',
    keypresses=["<Ctrl>K"],
)
FLOW_GRAPH_GEN = actions.register(
    "app.flowgraph.generate",
    label='_Generate',
    tooltip='Generate the flow graph',
    icon_name='insert-object',
    keypresses=["F5"],
)
FLOW_GRAPH_EXEC = actions.register(
    "app.flowgraph.execute",
    label='_Execute',
    tooltip='Execute the flow graph',
    icon_name='media-playback-start',
    keypresses=["F6"],
)
FLOW_GRAPH_KILL = actions.register(
    "app.flowgraph.kill",
    label='_Kill',
    tooltip='Kill the flow graph',
    icon_name='media-playback-stop',
    keypresses=["F7"],
)
FLOW_GRAPH_SCREEN_CAPTURE = actions.register(
    "app.flowgraph.screen_capture",
    label='Screen Ca_pture',
    tooltip='Create a screen capture of the flow graph',
    icon_name='printer',
    keypresses=["<Ctrl>p"],
)
PORT_CONTROLLER_DEC = actions.register(
    "win.port_controller_dec",
    keypresses=["KP_Subtract", "minus"],
)
PORT_CONTROLLER_INC = actions.register(
    "win.port_controller_inc",
    keypresses=["KP_Add", "plus"],
)
BLOCK_INC_TYPE = actions.register(
    "win.block_inc_type",
    keypresses=["Down"],
)
BLOCK_DEC_TYPE = actions.register(
    "win.block_dec_type",
    keypresses=["Up"],
)
RELOAD_BLOCKS = actions.register("app.reload_blocks",
                                 label='Reload _Blocks',
                                 tooltip='Reload Blocks',
                                 icon_name='view-refresh')
FIND_BLOCKS = actions.register(
    "win.find_blocks",
    label='_Find Blocks',
    tooltip='Search for a block by name (and key)',
    icon_name='edit-find',
    keypresses=["<Ctrl>f", "slash"],
)
CLEAR_CONSOLE = actions.register(
    "win.console.clear",
    label='_Clear Console',
    tooltip='Clear Console',
    icon_name='edit-clear',
    keypresses=["<Ctrl>l"],
)
SAVE_CONSOLE = actions.register(
    "win.console.save",
    label='_Save Console',
    tooltip='Save Console',
    icon_name='edit-save',
    keypresses=["<Ctrl><Shift>p"],
)
OPEN_HIER = actions.register(
    "win.open_hier",
    label='Open H_ier',
    tooltip='Open the source of the selected hierarchical block',
    icon_name='go-jump',
)
BUSSIFY_SOURCES = actions.register(
    "win.bussify_sources",
    label='Toggle So_urce Bus',
    tooltip='Gang source ports into a single bus port',
    icon_name='go-jump',
)
BUSSIFY_SINKS = actions.register(
    "win.bussify_sinks",
    label='Toggle S_ink Bus',
    tooltip='Gang sink ports into a single bus port',
    icon_name='go-jump',
)
XML_PARSER_ERRORS_DISPLAY = actions.register(
    "app.xml_errors",
    label='_Parser Errors',
    tooltip='View errors that occurred while parsing XML files',
    icon_name='dialog-error',
)
FLOW_GRAPH_OPEN_QSS_THEME = actions.register(
    "app.open_qss_theme",
    label='Set Default QT GUI _Theme',
    tooltip='Set a default QT Style Sheet file to use for QT GUI',
    icon_name='document-open',
)
TOOLS_RUN_FDESIGN = actions.register(
    "app.filter_design",
    label='Filter Design Tool',
    tooltip='Execute gr_filter_design',
    icon_name='media-playback-start',
)
POST_HANDLER = actions.register("app.post_handler")
READY = actions.register("app.ready")
