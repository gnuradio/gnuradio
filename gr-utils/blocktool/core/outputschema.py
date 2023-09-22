#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Schema to be strictly followed be parsed header output """


RESULT_SCHEMA = {
    "title": "JSON SCHEMA TO BE FOLLOWED BY BLOCK HEADER PARSING TOOL",
    "description": "Schema designed for the header file parsed python dict output",
    "type": "object",
    "properties": {
        "namespace": {
            "description": "List of nested namespace",
            "type": "array",
            "minItems": 1,
            "uniqueItems": True,
            "items": {
                "type": "string",
                "minLength": 1
            }
        },
        "class": {
            "description": "Class name",
            "type": "string",
            "minLength": 1
        },
        "io_signature": {
            "description": "I/O signature",
            "type": "object",
            "properties": {
                "input": {
                    "description": "Input ports",
                    "type": "object"
                },
                "output": {
                    "description": "Output ports",
                    "type": "object"
                }
            },
            "required": ["input", "output"]
        },
        "make": {
            "description": "Make function",
            "type": "object",
            "properties": {
                "arguments": {
                    "description": "Arguments of make function",
                    "type": "array",
                    "minItems": 1,
                    "uniqueItems": True,
                    "items": {
                        "type": "object",
                        "properties": {
                            "name": {
                                "type": "string",
                                "minLength": 1
                            },
                            "dtype": {
                                "type": "string",
                                "minLength": 1
                            },
                            "default": {
                                "type": "string"
                            }
                        },
                        "required": ["name"],
                        "dependencies": {
                            "name": [
                                "dtype",
                                "default"
                            ]
                        }
                    }
                }
            }
        },
        "methods": {
            "description": "Setters",
            "type": "array",
            "minItems": 0,
            "items": {
                "type": "object",
                "properties": {
                    "name": {
                        "type": "string",
                        "minLength": 1
                    },
                    "arguments_type": {
                        "type": "array",
                        "uniqueItems": True,
                        "properties": {
                            "name": {
                                "type": "string",
                                "minLength": 1
                            },
                            "dtype": {
                                "type": "string",
                                "minLength": 1
                            }
                        },
                        "required": ["name"],
                        "dependencies": {
                            "name": ["dtype"]
                        }
                    }
                },
                "required": ["name"]
            }
        },
        "properties": {
            "description": "Getters",
            "type": "array",
            "uniqueItems": True,
            "items": {
                "type": "object",
                "properties": {
                    "name": {
                        "type": "string",
                        "minLength": 1
                    },
                    "dtype": {
                        "type": "string",
                        "minLength": 1
                    },
                    "read_only": {
                        "type": "boolean"
                    }
                },
                "required": ["name"],
                "dependencies": {
                    "name": [
                        "dtype",
                        "read_only"
                    ]
                }
            }
        },
        "docstring": {
            "description": "documentation of the header file",
            "type": "array"
        }
    },
    "required": [
        "namespace",
        "class",
        "io_signature",
        "make",
        "methods",
        "properties",
        "docstring"
    ]
}
