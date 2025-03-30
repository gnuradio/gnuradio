"""
SPDX-License-Identifier: GPL-3.0-or-later
"""

from collections import OrderedDict, defaultdict

from . import Block, register_build_in
from ._build import build_params
from ._templates import MakoTemplates
from ..base import Element


@register_build_in
class Options(Block):
    """Implementation of Options block for codegen purpose.

    This block will be used to select workflow and provide informations for
    code generator.
    """

    key = 'options'  # block id
    label = 'Options'  # block name

    def __init__(self, parent):
        self.raw_params_data = [
            dict(id='title',
                 label='Title',
                 workflow='',
                 dtype='string',
                 default='',
                 hide="${ ('none' if title else 'part') }",
                 ),
            dict(id='author',
                 label='Author',
                 workflow='',
                 dtype='string',
                 default='',
                 hide="${ ('none' if author else 'part') }",
                 ),
            dict(id='copyright',
                 label='Copyright',
                 workflow='',
                 dtype='string',
                 default='',
                 hide="${ ('none' if copyright else 'part') }",
                 ),
            dict(id='description',
                 label='Description',
                 workflow='',
                 dtype='string',
                 default='',
                 hide="${ ('none' if description else 'part') }",
                 ),
            dict(id='output_language',
                 label='Output Language',
                 workflow='',
                 dtype='enum',
                 default='python',
                 options=['python'],
                 option_labels=['Python'],
                 hide="none",
                 ),
            dict(id='generate_options',
                 label='Generate Options',
                 workflow='',
                 dtype='enum',
                 default='qt_gui',
                 options=['qt_gui'],
                 option_labels=['QT GUI'],
                 hide="none",
                 ),
            dict(id='gen_linking',
                 label='Linking',
                 workflow='',
                 dtype='enum',
                 default='dynamic',
                 options=['dynamic', 'static'],
                 option_labels=['dynamic', 'static'],
                 hide="all",
                 ),
            dict(id='thread_safe_setters',
                 label='Thread-safe setters',
                 workflow='',
                 category='Advanced',
                 dtype='enum',
                 options=['', '1'],
                 option_labels=['Off', 'On'],
                 hide="part",
                 ),
            dict(id='catch_exceptions',
                 label='Catch Block Exceptions',
                 workflow='',
                 category='Advanced',
                 dtype='enum',
                 default='True',
                 options=['False', 'True'],
                 option_labels=['Off', 'On'],
                 hide="part",
                 ),
            dict(id='hier_block_src_path',
                 label='Hier Block Source Path',
                 workflow='',
                 category='Advanced',
                 dtype='string',
                 default='.:',
                 hide="part",
                 ),
            dict(id='generator_class_name',
                 workflow='',
                 dtype='string',
                 hide="all",
                 ),
            dict(id='generator_module',
                 workflow='',
                 dtype='string',
                 hide="all",
                 ),
        ]

        self.default_ids = [
            'title',
            'author',
            'copyright',
            'description',
            'output_language',
            'generate_options',
            'gen_linking',
            'thread_safe_setters',
            'catch_exceptions',
            'hier_block_src_path',
        ]

        self.parent_platform = parent.parent_platform
        self.workflows = self.parent_platform.workflow_manager.workflows  # get all available workflow objects
        self.codegen_options = {}
        self.current_workflow = None
        self.workflow_params = []
        """
        self.workflow_params would be:
        [
            {
                'workflow_id': workflow id,
                'params': Param,
            },
            ...
        ]
        """

        self.parse_workflows()

        # construct output_language parameter
        temp_dct = dict(
            id='output_language',
            label='Output Language',
            dtype='enum',
            options=[],
            option_labels=[],
        )

        for output_language, output_language_label in self.codegen_options.keys():
            temp_dct['options'].append(output_language)
            temp_dct['option_labels'].append(output_language_label)

        self.raw_params_data[4] = temp_dct
        self.parameters_data = build_params(
            params_raw=self.raw_params_data,
            have_inputs=False,
            have_outputs=False,
            flags=Block.flags,
            block_id=self.key,
        )

        super().__init__(parent)

        self.cpp_templates = MakoTemplates()
        self.documentation['documentation'] = """
            The options block sets special parameters for the flow graph. Only one option block is allowed per flow graph.
            Title, author, and description parameters are for identification purposes.
            The window size controls the dimensions of the flow graph editor. The window size (width, height) must be between (300, 300) and (4096, 4096).
            The generate options controls the type of code generated. Non-graphical flow graphs should avoid using graphical sinks or graphical variable controls.
            In a graphical application, run can be controlled by a variable to start and stop the flowgraph at runtime.
            The id of this block determines the name of the generated file and the name of the class. For example, an id of my_block will generate the file my_block.py and class my_block(gr....
            The category parameter determines the placement of the block in the block selection window. The category only applies when creating hier blocks. To put hier blocks into the root category, enter / for the category.
            The Max Number of Output is the maximum number of output items allowed for any block in the flowgraph; to disable this set the max_nouts equal to 0.Use this to adjust the maximum latency a flowgraph can exhibit.
        """

    def parse_workflows(self) -> None:
        """
        Read all workflow yml files
        for each workflow, use pair for output_language, generate_options and their label
        then, put it on codegen_options

        Also make new params from current workflow

        structure of codegen_options will be
        codegen_options = {
            ...
            (output_language, output_language_label): [(generated_options, generated_options_label), ..]
        }
        """
        for workflow in self.workflows:
            output_language_pair = (workflow.output_language, workflow.output_language_label)
            generate_options_pair = (workflow.generate_options, workflow.generate_options_label)
            if output_language_pair not in self.codegen_options.keys():
                self.codegen_options[output_language_pair] = []
            if generate_options_pair not in self.codegen_options[output_language_pair]:
                self.codegen_options[output_language_pair].append(generate_options_pair)

        for workflow in self.workflows:
            tmp = {'workflow_id': workflow.id}
            tmp['params'] = self.new_params_from_list(self.raw_params_data + workflow.parameters)
            tmp['params']['id'].set_value(tmp['workflow_id'])
            tmp['params']['output_language'].set_value(workflow.output_language)
            tmp['params']['generate_options'].set_value(workflow.generate_options)

            tmp_dct = OrderedDict()
            tmp_dct.attributes = defaultdict(dict)
            for output_language, output_language_label in self.codegen_options.keys():
                tmp_dct[output_language] = output_language_label
            tmp['params']['output_language'].options = tmp_dct

            chosen_language = workflow.output_language
            output_language_pair = ()
            for output_language, output_language_label in self.codegen_options.keys():
                if chosen_language == output_language or chosen_language == output_language_label:
                    output_language_pair = (output_language, output_language_label)
                    break

            list_of_generated_options_pair = self.codegen_options[output_language_pair]

            tmp_dct = OrderedDict()
            tmp_dct.attributes = defaultdict(dict)
            for generated_options, generated_options_label in list_of_generated_options_pair:
                tmp_dct[generated_options] = generated_options_label
            tmp['params']['generate_options'].options = tmp_dct

            self.workflow_params.append(tmp)

    def _run_asserts(self):
        if self.current_workflow:
            self.asserts = self.current_workflow.asserts
        super()._run_asserts()

    def insert_grc_parameters(self, grc_parameters):
        """
        Update parameters with values from the grc file
        Args:
            grc_parameters (str): option block parameter values from grc
        """
        self.params['output_language'].value = grc_parameters.get('output_language')
        self.params['generate_options'].value = grc_parameters.get('generate_options')
        self.update_current_workflow()

        # fill default values of each workflow parameters from grc_parameters
        for i in range(len(self.workflow_params)):
            self.workflow_params[i]['params']['id'].value = grc_parameters.get('id')
            for default_id in self.default_ids:
                # the value of output_language and generate_options already been defined in parse_workflows
                if default_id == 'output_language' or default_id == 'generate_options':
                    continue
                self.workflow_params[i]['params'][default_id].value = grc_parameters.get(default_id)

        self.params = self.get_params_by_workflow_id(self.current_workflow.id)
        self.parent.validate()  # validiate the flow graph

    def rewrite(self) -> None:
        """
        Get parameters based on current workflow
        Update generated_options each time the value of output_language is changed
        Update generator class based on current workflow
        """
        self.update_current_workflow()
        # Make a copy of the current parameter values
        backup_params = {
            k: self.params[k].value
            for k in ['title', 'author', 'copyright', 'description', 'output_language', 'generate_options']
        }
        # Create new params dictionary based on current workflow
        self.params = self.get_params_by_workflow_id(self.current_workflow.id)
        # Re-assign old values
        for k, v in backup_params.items():
            self.params[k].value = v

        # re assign options of generate_options based on current output language
        chosen_language = self.params['output_language'].value
        output_language_pair = ()
        for output_language, output_language_label in self.codegen_options.keys():
            if chosen_language == output_language:
                output_language_pair = (output_language, output_language_label)
                break

        list_of_generated_options_pair = self.codegen_options[output_language_pair]

        tmp_dct = OrderedDict()
        tmp_dct.attributes = defaultdict(dict)
        for generated_options, generated_options_label in list_of_generated_options_pair:
            tmp_dct[generated_options] = generated_options_label
        self.params['generate_options'].options = tmp_dct

        # Update the generator_class_name and generator_module parameters based
        # on the current workflow
        self.params['generator_class_name'].set_value(self.current_workflow.generator_class)
        self.params['generator_module'].set_value(self.current_workflow.generator_module)
        super().rewrite()

    def update_current_workflow(self) -> None:
        """Update value of self.current_workflow.

        This takes into account the current value of output_language and
        generate_options. Note that not all output languages have the same
        generate options, so we take the liberty to update those if we have to.
        """
        # First, see if we can find a workflow that matches the current output
        # language and generate options.
        for workflow in self.workflows:
            if workflow.output_language == self.params['output_language'].get_value() \
                    and workflow.generate_options == self.params['generate_options'].get_value():
                self.current_workflow = workflow
                return
        # Otherwise, see if we can find a workflow that matches the current
        # output language, but ignore the generate options. We'll pick the first
        # available workflow, and update the generate options to match.
        for workflow in self.workflows:
            if workflow.output_language == self.params['output_language'].get_value():
                self.current_workflow = workflow
                self.params['generate_options'].set_value(workflow.generate_options)
                return
        assert False, f"No workflow found for output_language: " \
                      f"{self.params['output_language'].get_value()} and " \
                      f"generate_options: {self.params['generate_options'].get_value()}"

    def get_params_by_workflow_id(self, id):
        """
        Get params based on workflow id
        Args:
            id (int): workflow id
        Return:
            parameters: OrderedDict[str, Param]
        """
        for wf in self.workflow_params:
            if wf['workflow_id'] == id:
                return wf['params']

    def new_params_from_list(self, lst):
        """
        Make new params from list of dictonary
        Args:
            lst (list): list of dictionary
        Return:
            parameters: OrderedDict[str, Param]
        """
        param_factory = self.parent_platform.make_param
        new_params_data = build_params(
            params_raw=lst,
            have_inputs=False,
            have_outputs=False,
            flags=Block.flags,
            block_id=self.key
        )
        new_params = (OrderedDict(
            (data['id'], param_factory(parent=self, **data)) for data in new_params_data))
        return new_params
