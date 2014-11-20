import logging


class GRCHandler(logging.Handler):  # Inherit from logging.Handler
    """ Custom log handler for GRC. Stores log entries to be viewed using the GRC debug window. """

    def __init__(self, maxLength=256):
        # run the regular Handler __init__
        logging.Handler.__init__(self)
        # Our custom argument
        self.log = collections.deque(maxlen=maxLength)

    def emit(self, record):
        self.log.append(record)

    def getLogs(self, level):
        pass


class ConsoleFormatter(logging.Formatter):
    """
     Custom log formatter that nicely truncates the log message and log levels
      - Verbose mode outputs: time, level, message, name, filename, and line number
      - Normal mode output varies based on terminal size:
            w < 80       - Level, Message (min length 40)
            80 < w < 120 - Level, Message, File, Line (25)
            120 < w      - Level, Message, Name, File, Line
      - Color mode ouptuts the same variable sizes and uses the blessings module
        to add color
    """

    # TODO: Better handle multi line messages. Need to indent them or something

    def __init__(self, verbose=False, short_level=True):
        # Test for blessings formatter
        try:
            from blessings import Terminal
            self.terminal = Terminal()
            self.formatLevel = self.formatLevelColor
        except:
            self.terminal = None
            self.formatLevel = self.formatLevelPlain

        # Default to short
        self.formatLevelLength = self.formatLevelShort
        if not short_level:
            self.formatLevelLength = self.formatLevelLong

        # Setup the format function as a pointer to the correct formatting function
        # Determine size and mode
        # TODO: Need to update the sizes depending on short or long outputs
        import shutil
        size = shutil.get_terminal_size()
        width = max(40, size.columns - 10)
        if size.columns < 80:
            self.format = self.short
            self.width = width
        elif size.columns < 120:
            self.format = self.medium
            self.width = width - 30
        elif size.columns >= 120:
            self.format = self.long
            self.width = width - 45
        # Check if verbose mode. If so override other options
        if verbose:
            self.format = self.verbose

    # Normal log formmatters
    def short(self, record):
        message = self.formatMessage(record.msg, self.width)
        level = self.formatLevel(record.levelname)
        return "{0} -- {1}".format(level, message)

    def medium(self, record):
        message = self.formatMessage(record.msg, self.width)
        level = self.formatLevel(record.levelname)
        output = '{0} -- {1:<' + str(self.width) + '} ({2}:{3})'
        return output.format(level, message, record.filename, record.lineno)

    def long(self, record):
        message = self.formatMessage(record.msg, self.width)
        level = self.formatLevel(record.levelname)
        output = '{0} -- {1:<' + str(self.width) + '} {2} ({3}:{4})'
        return output.format(level, message, record.name, record.filename, record.lineno)

    ''' Verbose formatter '''
    def verbose(self, record):
        # TODO: Still need to implement this
        pass

    ''' Level and message formatters '''
    # Nicely format the levelname
    # Level name can be formated to either short or long, and also with color

    def formatLevelColor(self, levelname):
        term = self.terminal
        output = "{0}{1}{2}{3}"
        level = self.formatLevelLength(levelname)
        if levelname == "DEBUG":
            return output.format(term.blue, "", level, term.normal)
        elif levelname == "INFO":
            return output.format(term.green, "", level, term.normal)
        elif levelname == "WARNING":
            return output.format(term.yellow, "", level, term.normal)
        elif levelname == "ERROR":
            return output.format(term.red, term.bold, level, term.normal)
        elif levelname == "CRITICAL":
            return output.format(term.red, term.bold, level, term.normal)
        else:
            return output.format(term.blue, "", level, term.normal)

    def formatLevelPlain(self, levelname):
        ''' Format the level name without color. formatLevelLength points to the right function '''
        return self.formatLevelLength(levelname)

    def formatLevelShort(self, levelname):
        return "[{0}]".format(levelname[0:1])

    def formatLevelLong(self, levelname):
        output = "{0:<10}"
        if levelname in ["DEBUG", "INFO", "WARNING"]:
            return output.format("[{0}]".format(levelname.capitalize()))
        else:
            return output.format("[{0}]".format(levelname.upper()))

    def formatMessage(self, message, width):
        # First, strip out any newline for console output
        message = message.rstrip()
        if len(message) > width:
            return (message[:(width - 3)] + "...")
        return message
