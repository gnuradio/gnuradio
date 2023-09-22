"""
Copyright 2007 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""

from . import Actions
from .Constants import STATE_CACHE_SIZE


class StateCache(object):
    """
    The state cache is an interface to a list to record data/states and to revert to previous states.
    States are recorded into the list in a circular fassion by using an index for the current state,
    and counters for the range where states are stored.
    """

    def __init__(self, initial_state):
        """
        StateCache constructor.

        Args:
            initial_state: the initial state (nested data)
        """
        self.states = [None] * STATE_CACHE_SIZE  # fill states
        self.current_state_index = 0
        self.num_prev_states = 0
        self.num_next_states = 0
        self.states[0] = initial_state
        self.update_actions()

    def save_new_state(self, state):
        """
        Save a new state.
        Place the new state at the next index and add one to the number of previous states.

        Args:
            state: the new state
        """
        self.current_state_index = (
            self.current_state_index + 1) % STATE_CACHE_SIZE
        self.states[self.current_state_index] = state
        self.num_prev_states = self.num_prev_states + 1
        if self.num_prev_states == STATE_CACHE_SIZE:
            self.num_prev_states = STATE_CACHE_SIZE - 1
        self.num_next_states = 0
        self.update_actions()

    def get_current_state(self):
        """
        Get the state at the current index.

        Returns:
            the current state (nested data)
        """
        self.update_actions()
        return self.states[self.current_state_index]

    def get_prev_state(self):
        """
        Get the previous state and decrement the current index.

        Returns:
            the previous state or None
        """
        if self.num_prev_states > 0:
            self.current_state_index = (
                self.current_state_index + STATE_CACHE_SIZE - 1) % STATE_CACHE_SIZE
            self.num_next_states = self.num_next_states + 1
            self.num_prev_states = self.num_prev_states - 1
            return self.get_current_state()
        return None

    def get_next_state(self):
        """
        Get the nest state and increment the current index.

        Returns:
            the next state or None
        """
        if self.num_next_states > 0:
            self.current_state_index = (
                self.current_state_index + 1) % STATE_CACHE_SIZE
            self.num_next_states = self.num_next_states - 1
            self.num_prev_states = self.num_prev_states + 1
            return self.get_current_state()
        return None

    def update_actions(self):
        """
        Update the undo and redo actions based on the number of next and prev states.
        """
        Actions.FLOW_GRAPH_REDO.set_enabled(self.num_next_states != 0)
        Actions.FLOW_GRAPH_UNDO.set_enabled(self.num_prev_states != 0)
