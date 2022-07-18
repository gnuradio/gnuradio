import json
import jsonschema
from jsonschema import validate
from regex import D
import yaml
import os

import jsonschema_default

def validate_json(d, schema):
    try:
        validate(instance=d, schema=schema)
    except jsonschema.exceptions.ValidationError as err:
        return False
    return True


filedir = os.path.dirname(__file__)
with open (os.path.join(filedir, '../../../blocklib/blocks/vector_sink/vector_sink.yml')) as f:
    d = yaml.safe_load(f)

schema_filename = os.path.join(filedir, 'gnuradio-block.json')
with open(schema_filename,'r') as f:
    schema = json.load(f)
    

isvalid = validate_json(d, schema)
isvalid = validate_json(d['ports'][0], schema['definitions']['Port'])

default_obj = jsonschema_default.create_from(schema_filename)
default_obj.update(d)

x = 3
