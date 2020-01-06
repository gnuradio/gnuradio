import os
import stat


class Properties(object):
    """ Stores global properties for GRC. """

    APP_NAME = 'grc'
    DEFAULT_LANGUAGE = ['en_US']

    def __init__(self, argv):
        self.argv = argv

        # Setup sub-categories
        self.path = Paths()
        self.system = System()
        self.window = Window()
        self.colors = Colors()
        self.types = Types()


class Paths(object):
    """ Initialize GRC paths relative to current file. """

    # Flow graph
    DEFAULT_FILE = os.getcwd()
    IMAGE_FILE_EXTENSION = '.png'
    TEXT_FILE_EXTENSION = '.txt'
    NEW_FLOGRAPH_TITLE = 'untitled'
    SEPARATORS = {'/': ':', '\\': ';'}[os.path.sep]

    # Setup all the install paths
    p = os.path
    PREFERENCES = p.expanduser('~/.grc')
    INSTALL = p.abspath(p.join(p.dirname(__file__), '..'))
    RESOURCES = p.join(INSTALL, 'companion/resources')
    LANGUAGE = p.join(INSTALL, 'companion/resources/language')
    LOGO = p.join(INSTALL, 'companion/resources/logo')
    ICON = p.join(LOGO, 'gnuradio_logo_icon-square-150x150-white.png')

    # Model Paths
    MODEL = p.join(INSTALL, 'model')
    BLOCK_TREE_DTD = p.join(MODEL, 'block_tree.dtd')
    FLOW_GRAPH_DTD = p.join(MODEL, 'flow_graph.dtd')
    FLOW_GRAPH_TEMPLATE = p.join(MODEL, 'flow_graph.tmpl')
    DEFAULT_FLOW_GRAPH = os.path.join(MODEL, 'default_flow_graph.grc')

    # File creation modes
    HIER_BLOCK_FILE_MODE = stat.S_IRUSR | stat.S_IWUSR | stat.S_IWGRP | stat.S_IROTH | stat.S_IRGRP
    TOP_BLOCK_FILE_MODE = HIER_BLOCK_FILE_MODE | stat.S_IXUSR | stat.S_IXGRP

    # Setup paths
    '''
    HIER_BLOCKS_LIB_DIR = os.environ.get('GRC_HIER_PATH',
                                         os.path.expanduser('~/.grc_gnuradio'))
    PREFS_FILE = os.environ.get('GRC_PREFS_PATH', os.path.join(os.path.expanduser('~/.grc')))
    BLOCKS_DIRS = filter( #filter blank strings
        lambda x: x, PATH_SEP.join([
            os.environ.get('GRC_BLOCKS_PATH', ''),
            _gr_prefs.get_string('grc', 'local_blocks_path', ''),
            _gr_prefs.get_string('grc', 'global_blocks_path', ''),
        ]).split(PATH_SEP),
    ) + [HIER_BLOCKS_LIB_DIR]
    '''


class System(object):
    """ System specific properties """

    OS = 'Unknown'
    #XTERM_EXECUTABLE = _gr_prefs.get_string('grc', 'xterm_executable', 'xterm')


class Window(object):
    """ Properties for the main window """

    # Default window properties
    MIN_WINDOW_WIDTH = 600
    MIN_WINDOW_HEIGHT = 400

    MIN_DIALOG_WIDTH = 500
    MIN_DIALOG_HEIGHT = 500

    # How close can the mouse get to the window border before mouse events are ignored.
    BORDER_PROXIMITY_SENSITIVITY = 50

    # How close the mouse can get to the edge of the visible window before scrolling is invoked.
    SCROLL_PROXIMITY_SENSITIVITY = 30

    # When the window has to be scrolled, move it this distance in the required direction.
    SCROLL_DISTANCE = 15

    # By default Always show the menubar
    NATIVE_MENUBAR = False

    # Default sizes
    DEFAULT_BLOCKS_WINDOW_WIDTH = 100
    DEFAULT_REPORTS_WINDOW_WIDTH = 100

    DEFAULT_PARAM_TAB = 'General'
    ADVANCED_PARAM_TAB = 'Advanced'

    REPORTS_DOCK_LOCATION = 8
    BLOCK_LIBRARY_DOCK_LOCATION = 1

    # Define the top level menus.
    # This does not actually define the menus; it simply defines a list of constants that
    # to be used as keys for the actual menu dictionaries
    class menus(object):
        FILE = "file"
        EDIT = "edit"
        VIEW = "view"
        BUILD = "build"
        TOOLS = "tools"
        #PLUGINS = "plugins"
        HELP = "help"


class FlowGraph(object):
    """ Flow graph specific properites """

    # File format
    FILE_FORMAT_VERSION = 1

    # Fonts
    FONT_FAMILY = 'Sans'
    FONT_SIZE = 8
    BLOCK_FONT = "%s %f" % (FONT_FAMILY, FONT_SIZE)
    PORT_FONT = BLOCK_FONT
    PARAM_FONT = "%s %f" % (FONT_FAMILY, FONT_SIZE - 0.5)

    # The size of the state saving cache in the flow graph (for undo/redo functionality)
    STATE_CACHE_SIZE = 42

    # Shared targets for drag and drop of blocks
    #DND_TARGETS = [('STRING', gtk.TARGET_SAME_APP, 0)]

    # Label constraints
    LABEL_SEPARATION = 3
    BLOCK_LABEL_PADDING = 7
    PORT_LABEL_PADDING = 2

    # Port constraints
    PORT_SEPARATION = 32
    PORT_BORDER_SEPARATION = 9
    PORT_MIN_WIDTH = 20
    PORT_LABEL_HIDDEN_WIDTH = 10

    # Connector lengths
    CONNECTOR_EXTENSION_MINIMAL = 11
    CONNECTOR_EXTENSION_INCREMENT = 11

    # Connection arrows
    CONNECTOR_ARROW_BASE = 13
    CONNECTOR_ARROW_HEIGHT = 17

    # Rotations
    POSSIBLE_ROTATIONS = (0, 90, 180, 270)

    # How close the mouse click can be to a line and register a connection select.
    LINE_SELECT_SENSITIVITY = 5

    # canvas grid size
    CANVAS_GRID_SIZE = 8


class Colors(object):
    """ Color definitions """

    # Graphics stuff
    HIGHLIGHT = '#00FFFF'
    BORDER = 'black'
    MISSING_BLOCK_BACKGROUND = '#FFF2F2'
    MISSING_BLOCK_BORDER = 'red'
    PARAM_ENTRY_TEXT = 'black'
    ENTRYENUM_CUSTOM = '#EEEEEE'
    FLOWGRAPH_BACKGROUND = '#FFF9FF'
    BLOCK_ENABLED = '#F1ECFF'
    BLOCK_DISABLED = '#CCCCCC'
    CONNECTION_ENABLED = 'black'
    CONNECTION_DISABLED = '#999999'
    CONNECTION_ERROR = 'red'

    # Alias Colors
    COMPLEX = '#3399FF'
    FLOAT = '#FF8C69'
    INT = '#00FF99'
    SHORT = '#FFFF66'
    BYTE = '#FF66FF'

    # Type Colors
    COMPLEX_FLOAT_64 = '#CC8C69'
    COMPLEX_FLOAT_32 = '#3399FF'
    COMPLEX_INTEGER_64 = '#66CC00'
    COMPLEX_INTEGER_32 = '#33cc66'
    COMPLEX_INTEGER_16 = '#cccc00'
    COMPLEX_INTEGER_8 = '#cc00cc'
    FLOAT_64 = '#66CCCC'
    FLOAT_32 = '#FF8C69'
    INTEGER_64 = '#99FF33'
    INTEGER_32 = '#00FF99'
    INTEGER_16 = '#FFFF66'
    INTEGER_8 = '#FF66FF'
    MESSAGE_QUEUE = '#777777'
    ASYNC_MESSAGE = '#C0C0C0'
    BUS_CONNECTION = '#FFFFFF'
    WILDCARD = '#FFFFFF'

    COMPLEX_VECTOR = '#3399AA'
    FLOAT_VECTOR = '#CC8C69'
    INT_VECTOR = '#00CC99'
    SHORT_VECTOR = '#CCCC33'
    BYTE_VECTOR = '#CC66CC'
    ID = '#DDDDDD'
    WILDCARD = '#FFFFFF'
    MSG = '#777777'


class Types(object):
    """ Setup types then map them to the conversion dictionaries """

    CORE_TYPES = {  # Key: (Size, Color, Name)
        'fc64':    (16, Colors.COMPLEX_FLOAT_64,   'Complex Float 64'),
        'fc32':    (8,  Colors.COMPLEX_FLOAT_32,   'Complex Float 32'),
        'sc64':    (16, Colors.COMPLEX_INTEGER_64, 'Complex Integer 64'),
        'sc32':    (8,  Colors.COMPLEX_INTEGER_32, 'Complex Integer 32'),
        'sc16':    (4,  Colors.COMPLEX_INTEGER_16, 'Complex Integer 16'),
        'sc8':     (2,  Colors.COMPLEX_INTEGER_8,  'Complex Integer 8',),
        'f64':     (8,  Colors.FLOAT_64,           'Float 64'),
        'f32':     (4,  Colors.FLOAT_32,           'Float 32'),
        's64':     (8,  Colors.INTEGER_64,         'Integer 64'),
        's32':     (4,  Colors.INTEGER_32,         'Integer 32'),
        's16':     (2,  Colors.INTEGER_16,         'Integer 16'),
        's8':      (1,  Colors.INTEGER_8,          'Integer 8'),
        'msg':     (0,  Colors.MESSAGE_QUEUE,      'Message Queue'),
        'message': (0,  Colors.ASYNC_MESSAGE,      'Async Message'),
        'bus':     (0,  Colors.BUS_CONNECTION,     'Bus Connection'),
        '':        (0,  Colors.WILDCARD,           'Wildcard')
    }

    ALIAS_TYPES = {
        'complex': (8, Colors.COMPLEX),
        'float':   (4, Colors.FLOAT),
        'int':     (4, Colors.INT),
        'short':   (2, Colors.SHORT),
        'byte':    (1, Colors.BYTE),
    }

    # Setup conversion dictionaries
    TYPE_TO_COLOR = {}
    TYPE_TO_SIZEOF = {}
    for key, (size, color, name) in CORE_TYPES.items():
        TYPE_TO_COLOR[key] = color
        TYPE_TO_SIZEOF[key] = size
    for key, (sizeof, color) in ALIAS_TYPES.items():
        TYPE_TO_COLOR[key] = color
        TYPE_TO_SIZEOF[key] = size
