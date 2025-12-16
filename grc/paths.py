import os
import logging
from pathlib import Path

log = logging.getLogger(__name__)


def get_config_file_path(config_file: str = "grc.conf", ) -> str:
    oldpath = os.path.join(os.path.expanduser("~/.gnuradio"), config_file)
    try:
        from gnuradio.gr import paths
        newpath = os.path.join(paths.userconf(), config_file)
        if os.path.exists(newpath):
            return newpath
        if os.path.exists(oldpath):
            log.warn(f"Found specification for config path '{newpath}', but file does not exist. " +
                     f"Old default config file path '{oldpath}' exists; using that. " +
                     "Please consider moving configuration to new location.")
            return oldpath
        # Default to the correct path if both are configured.
        # neither old, nor new path exist: create new path, return that
        path_parts = Path(newpath).parts[:-1]
        pathdir = os.path.join(*path_parts)
        os.makedirs(pathdir, exist_ok=True)
        return newpath
    except ImportError:
        log.warn("Could not retrieve GNU Radio configuration directory from GNU Radio. Trying defaults.")
        xdgconf = os.getenv("XDG_CONFIG_HOME", os.path.expanduser("~/.config"))
        xdgcand = os.path.join(xdgconf, config_file)
        if os.path.exists(xdgcand):
            return xdgcand
        if os.path.exists(oldpath):
            log.warn(f"Using legacy config path '{oldpath}'. Please consider moving configuration " +
                     f"files to '{xdgcand}'.")
            return oldpath
        # neither old, nor new path exist: create new path, return that
        path_parts = Path(xdgcand).parts[:-1]
        pathdir = os.path.join(*path_parts)
        os.makedirs(pathdir, exist_ok=True)
        return xdgcand


def get_state_directory() -> str:
    oldpath = os.path.expanduser("~/.grc_gnuradio")
    try:
        from gnuradio.gr import paths
        newpath = paths.persistent()
        if os.path.exists(newpath):
            return newpath
        if os.path.exists(oldpath):
            log.warn(f"Found specification for persistent state path '{newpath}', but file does not exist. " +
                     f"Old default persistent state path '{oldpath}' exists; using that. " +
                     "Please consider moving state to new location.")
            return oldpath
        # Default to the correct path if both are configured.
        # neither old, nor new path exist: create new path, return that
        os.makedirs(newpath, exist_ok=True)
        return newpath
    except (ImportError, NameError):
        log.warn("Could not retrieve GNU Radio persistent state directory from GNU Radio. Trying defaults.")
        xdgstate = os.getenv("XDG_STATE_HOME", os.path.expanduser("~/.local/state"))
        xdgcand = os.path.join(xdgstate, "gnuradio")
        if os.path.exists(xdgcand):
            return xdgcand
        if os.path.exists(oldpath):
            log.warn(f"Using legacy state path '{oldpath}'. Please consider moving state " +
                     f"files to '{xdgcand}'.")
            return oldpath
        # neither old, nor new path exist: create new path, return that
        os.makedirs(xdgcand, exist_ok=True)
        return xdgcand
