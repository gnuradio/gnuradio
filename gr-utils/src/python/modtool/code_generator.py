""" A code generator (needed by ModToolAdd) """

from templates import Templates
import Cheetah.Template
from util_functions import str_to_fancyc_comment
from util_functions import str_to_python_comment
from util_functions import strip_default_values
from util_functions import strip_arg_types

### Code generator class #####################################################
class GRMTemplate(Cheetah.Template.Template):
    """ An extended template class """
    def __init__(self, src, searchList):
        self.grtypelist = {
                'sync': 'gr_sync_block',
                'sink': 'gr_sync_block',
                'source': 'gr_sync_block',
                'decimator': 'gr_sync_decimator',
                'interpolator': 'gr_sync_interpolator',
                'general': 'gr_block',
                'hier': 'gr_hier_block2',
                'noblock': ''}
        searchList['str_to_fancyc_comment'] = str_to_fancyc_comment
        searchList['str_to_python_comment'] = str_to_python_comment
        searchList['strip_default_values'] = strip_default_values
        searchList['strip_arg_types'] = strip_arg_types
        Cheetah.Template.Template.__init__(self, src, searchList=searchList)
        self.grblocktype = self.grtypelist[searchList['blocktype']]

def get_template(tpl_id, **kwargs):
    """ Return the template given by tpl_id, parsed through Cheetah """
    return str(GRMTemplate(Templates[tpl_id], searchList=kwargs))
