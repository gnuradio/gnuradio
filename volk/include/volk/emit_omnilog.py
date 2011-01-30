def emit_prolog():
    tempstring = "";
    tempstring = tempstring + '#ifdef __cplusplus\n';
    tempstring = tempstring + 'extern "C" {\n';
    tempstring = tempstring + '#endif\n';
    return tempstring;
def emit_epilog():
    tempstring = "";
    tempstring = tempstring + '#ifdef __cplusplus\n';
    tempstring = tempstring + '}\n';
    tempstring = tempstring + '#endif\n';
    return tempstring;

