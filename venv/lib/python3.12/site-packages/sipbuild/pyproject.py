# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from .exceptions import deprecated, UserFileException
from .py_versions import OLDEST_SUPPORTED_MINOR
from .toml import toml_loads


class PyProjectException(UserFileException):
    """ An exception related to a pyproject.toml file. """

    def __init__(self, text, *, detail=None):
        """ Initialise the exception. """

        super().__init__("pyproject.toml", text, detail=detail)


class PyProjectOptionException(PyProjectException):
    """ An exception related to a specific option of a pyproject.toml file. """

    def __init__(self, name, text, *, section_name=None, detail=None):
        """ Initialise the exception. """

        if section_name is None:
            section_name = 'tool.sip.project'

        super().__init__("'{0}.{1}': {2}".format(section_name, name, text),
                detail=detail)


class PyProjectTypeOptionException(PyProjectOptionException):
    """ An exception related to a badly typed option of a pyproject.toml file.
    """

    def __init__(self, name, types, *, section_name=None):
        """ Initialise the exception. """

        super().__init__(name, "value must be " + types,
                section_name=section_name)


class PyProjectUndefinedOptionException(PyProjectOptionException):
    """ An exception related to an undefined option of a pyproject.toml file.
    """

    def __init__(self, name, *, section_name=None):
        """ Initialise the exception. """

        super().__init__(name, "must be defined", section_name=section_name)


class PyProject:
    """ Encapsulate a parsed pyproject.toml file. """

    # The mapping of PEP 621 metadata to core metadata.
    _PEP621_CORE_MAP = {
        'name': 'name',
        'version': 'version',
        'description': 'summary',
        'requires-python': 'requires-python',
        'classifiers': 'classifier',
    }

    def __init__(self):
        """ Initialise the object. """

        try:
            with open('pyproject.toml', encoding='UTF-8') as f:
                self._raw_toml = f.read()

            self.pyproject = toml_loads(self._raw_toml)

        except FileNotFoundError:
            # Delay the exception in case the user is asking for help.
            self.pyproject = None

        except UnicodeDecodeError as e:
            raise PyProjectException("is not a UTF-8 encoded file",
                    details=str(e))

        except Exception as e:
            # Raise the exception now about a possibly badly formed file.
            raise PyProjectException(str(e))

    def get_metadata(self):
        """ Return a dict containing the PEP 566 metadata. """

        if self.pyproject is None:
            # Provide a minimal default.
            return dict(name='unknown', version='0.1')

        # See if PEP 621 project metadata is supplied.  Make this required when
        # support for the legacy metadata is removed.
        metadata = self.get_section('project')
        if metadata is None:
            core_metadata = self._get_legacy_metadata()
        else:
            # The main validation we do is to check that the keys are defined
            # by PEP 621.
            core_metadata = {'metadata-version': '2.1'}

            for md_name, md_value in metadata.items():
                md_name = md_name.lower()

                # See if special handling is required.
                if md_name == 'readme':
                    self._handle_readme(md_value, core_metadata)
                elif md_name == 'license':
                    self._handle_license(md_value, core_metadata)
                elif md_name == 'authors':
                    self._handle_people(md_value, 'author', core_metadata)
                elif md_name == 'maintainers':
                    self._handle_people(md_value, 'maintainer', core_metadata)
                elif md_name == 'keywords':
                    self._handle_keywords(md_value, core_metadata)
                elif md_name == 'urls':
                    self._handle_urls(md_value, core_metadata)
                elif md_name == 'dependencies':
                    self._handle_dependencies(md_value, core_metadata)
                elif md_name == 'optional-dependencies':
                    self._handle_optional_dependencies(md_value, core_metadata)
                elif md_name in ('scripts', 'gui-scripts', 'entry-points'):
                    # We ignore these for the moment and stick with using
                    # [tool.sip.project].
                    pass
                else:
                    core_name = self._PEP621_CORE_MAP.get(md_name)
                    if core_name is None:
                        raise PyProjectOptionException(md_name,
                                "is an unsupported project key",
                                section_name='project')

                    core_metadata[core_name] = md_value

            # Check that the name has been specified.
            if 'name' not in core_metadata:
                raise PyProjectUndefinedOptionException('name',
                        section_name='project')

        if 'version' not in core_metadata:
            core_metadata['version'] = '0.1'

        if 'requires-python' not in core_metadata:
            # The minimal version of Python we support.
            core_metadata['requires-python'] = '>=3.{}'.format(
                    OLDEST_SUPPORTED_MINOR)

        return core_metadata

    def get_section(self, section_name, *, required=False):
        """ Return a sub-section with a dotted name. """

        if self.pyproject is None:
            return None

        section = self.pyproject

        for part in section_name.split('.'):
            try:
                section = section[part]
            except KeyError:
                if required:
                    raise PyProjectException(
                            "the '[{0}]' section is missing".format(
                                    section_name))

                return None

        if not self._is_section(section):
            raise PyProjectException(
                    "'{0}' is not a section".format(section_name))

        return section

    def _get_legacy_metadata(self):
        """ Return the pre-PEP 621 metadata. """

        line_nr = self._get_line_nr('[tool.sip.metadata]')

        if line_nr is None:
            # There is no metadata specified.
            raise PyProjectException("the '[project]' section is missing")

        deprecated(
                "using '[tool.sip.metadata]' to specify the project metadata",
                instead="'[project]'", filename='pyproject.toml',
                line_nr=line_nr)

        core_metadata = dict()
        name = None
        version = None
        core_metadata_version = None

        for md_name, md_value in self.get_section('tool.sip.metadata', required=True).items():
            md_name = md_name.lower()

            # Extract specific string values.
            if md_name in ('name', 'metadata-version'):
                if not isinstance(md_value, str):
                    raise PyProjectTypeOptionException(md_name, "a string",
                            section_name='tool.sip')

                if md_name == 'name':
                    if not md_value.replace('-', '_').isidentifier():
                        raise PyProjectOptionException('name',
                                "'{0}' is an invalid project name".format(
                                        md_value),
                                section_name='tool.sip')

                    name = md_value
                elif md_name == 'metadata-version':
                    core_metadata_version = md_value
            else:
                # Any other value may be a string or a list of strings.
                value_list = md_value if isinstance(md_value, list) else [md_value]

                for value in value_list:
                    if not isinstance(value, str):
                        raise PyProjectTypeOptionException(md_name,
                                "a string or a list of strings",
                                section_name='tool.sip')

            core_metadata[md_name] = md_value

        if name is None:
            raise PyProjectUndefinedOptionException('name',
                    section_name='tool.sip')

        if core_metadata_version is None:
            # Default to PEP 566.
            core_metadata['metadata-version'] = '2.1'

        return core_metadata

    def _get_line_nr(self, target):
        """ Return the number of the line in pyproject.toml containing a target
        string.
        """

        for line_nr, line in enumerate(self._raw_toml.split('\n')):
            line = line.strip()

            # Ignore comments.
            if line.startswith('#'):
                continue

            if target in line:
                return line_nr + 1

        # The target wasn't present.
        return None

    @classmethod
    def _handle_dependencies(cls, value, core_metadata):
        """ Handle the 'dependencies' key. """

        if not isinstance(value, list):
            raise PyProjectTypeOptionException('dependencies',
                    "an array of strings", section_name='project')

        cls._update_requires_dist(value, core_metadata)

    @staticmethod
    def _handle_keywords(value, core_metadata):
        """ Handle the 'keywords' key. """

        if not isinstance(value, list):
            raise PyProjectTypeOptionException('keywords',
                    "an array of strings", section_name='project')

        core_metadata['keywords'] = ', '.join(value)

    @staticmethod
    def _handle_license(value, core_metadata):
        """ Handle the 'license' key. """

        if not isinstance(value, dict):
            raise PyProjectTypeOptionException('license', "a table",
                    section_name='project')

        text = value.get('text')
        file = value.get('file')

        if text is not None:
            if file is not None:
                raise PyProjectOptionException('license',
                        "'file' and 'text' cannot both be specified",
                        section_name='project')
        elif file is not None:
            try:
                with open(file, encoding='UTF-8') as f:
                    text = f.read()
            except FileNotFoundError:
                raise PyProjectOptionException('file',
                        f"unable to read '{file}'",
                        section_name='project.license')
        else:
            raise PyProjectOptionException('readme',
                    "either 'file' or 'text' must be specified",
                    section_name='project')

        core_metadata['license'] = text

    @classmethod
    def _handle_optional_dependencies(cls, value, core_metadata):
        """ Handle the 'optional-dependencies' key. """

        if not isinstance(value, dict):
            raise PyProjectTypeOptionException('optional-dependencies',
                    "a table", section_name='project')

        provides_extra = []
        requires_dist = []

        for extra, extra_deps in value.items():
            if not isinstance(extra_deps, list):
                raise PyProjectOptionException('optional-dependencies',
                        "each table value must be a list of strings",
                        section_name='project')

            provides_extra.append(extra)

            for dep in extra_deps:
                # Note that this is broken if the dependency is not a simple
                # name.
                requires_dist.append(f'{dep}[{extra}]')

        core_metadata['provides-extra'] = provides_extra
        cls._update_requires_dist(value, core_metadata)

    @staticmethod
    def _handle_people(value, role, core_metadata):
        """ Handle the people-related keys. """

        key = role + 's'

        if not isinstance(value, list):
            raise PyProjectTypeOptionException(key, "an array of tables",
                    section_name='project')

        all_names = []
        all_emails = []

        for table in value:
            if not isinstance(table, dict):
                raise PyProjectTypeOptionException(key, "an array of tables",
                        section_name='project')

            name = table.get('name')
            email = table.get('email')

            if name is None and email is None:
                raise PyProjectOptionException(key,
                        "each table must contain 'name' and/or 'email'",
                        section_name='project')

            if name is not None and not isinstance(name, str):
                raise PyProjectOptionException(key, "'name' must be a string",
                        section_name='project')

            if email is not None and not isinstance(email, str):
                raise PyProjectOptionException(key, "'email' must be a string",
                        section_name='project')

            if email is not None:
                if name is not None:
                    all_emails.append(f'{name} <{email}>')
                else:
                    all_emails.append(email)
            else:
                all_names.append(name)

        if all_names:
            core_metadata[role] = ', '.join(all_names)

        if all_emails:
            core_metadata[role + '-email'] = ', '.join(all_emails)

    @staticmethod
    def _handle_readme(value, core_metadata):
        """ Handle the 'readme' key. """

        if isinstance(value, str):
            readme = value.lower()

            if readme.endswith('.md'):
                content_type = 'text/markdown'
            elif readme.endswith('.rst'):
                content_type = 'text/x-rst'
            else:
                raise PyProjectOptionException('readme',
                        f"'{value}' has an unsupported file type",
                        section_name='project')

            description_file = value

        elif isinstance(value, dict):
            content_type = value.get('content-type')
            if content_type is None:
                raise PyProjectUndefinedOptionException('content-type',
                        section_name='project.readme')

            description_text = value.get('text')
            description_file = value.get('file')

            if description_text is not None:
                if description_file is not None:
                    raise PyProjectOptionException('readme',
                            "'file' and 'text' cannot both be specified",
                            section_name='project')

                core_metadata['description'] = text
            elif description_file is None:
                raise PyProjectOptionException('readme',
                        "either 'file' or 'text' must be specified",
                        section_name='project')

        else:
            raise PyProjectTypeOptionException('readme', "a string or a table",
                    section_name='project')

        core_metadata['description-content-type'] = content_type

        if description_file is not None:
            # This is a local, internal extension.
            core_metadata['description-file'] = description_file

    @staticmethod
    def _handle_urls(value, core_metadata):
        """ Handle the 'urls' key. """

        if not isinstance(value, dict):
            raise PyProjectTypeOptionException(key, "a table",
                    section_name='project')

        core_metadata['project-url'] = [f'{label}, {url}' for label, url in value.items()]

    @staticmethod
    def _is_section(value):
        """ Returns True if a section value is itself a section. """

        return isinstance(value, (dict, list))

    @staticmethod
    def _update_requires_dist(value, core_metadata):
        """ Update 'requires-dist' in the core metadata. """

        try:
            requires_dist = core_metadata['requires-dist']
        except KeyError:
            requires_dist = []

        requires_dist.extend(value)
        core_metadata['requires-dist'] = requires_dist
