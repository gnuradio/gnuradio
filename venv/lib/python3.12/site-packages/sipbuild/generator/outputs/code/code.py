# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


import os

from ....exceptions import UserException
from ....version import SIP_VERSION_STR

from ...python_slots import (is_hash_return_slot, is_inplace_number_slot,
        is_inplace_sequence_slot, is_int_arg_slot, is_int_return_slot,
        is_multi_arg_slot, is_number_slot, is_rich_compare_slot,
        is_ssize_return_slot, is_void_return_slot, is_zero_arg_slot)
from ...scoped_name import STRIP_GLOBAL, STRIP_NONE
from ...specification import (AccessSpecifier, Argument, ArgumentType,
        ArrayArgument, CodeBlock, DocstringSignature, GILAction, IfaceFileType,
        KwArgs, MappedType, PyQtMethodSpecifier, PySlot, QualifierType,
        Transfer, ValueType, WrappedClass, WrappedEnum)
from ...utils import (abi_has_deprecated_message, abi_version_check,
        find_method, py_as_int, same_signature)

from ..formatters import (fmt_argument_as_cpp_type, fmt_argument_as_name,
        fmt_class_as_scoped_name, fmt_copying, fmt_enum_as_cpp_type,
        fmt_scoped_py_name, fmt_signature_as_cpp_declaration,
        fmt_signature_as_cpp_definition, fmt_signature_as_type_hint,
        fmt_value_list_as_cpp_expression)


def output_code(spec, bindings, project, buildable):
    """ Output the C/C++ code and add it to the given buildable. """

    module = spec.module
    py_debug = project.py_debug

    if spec.is_composite:
        source_name = os.path.join(buildable.build_dir,
                'sip' + module.py_name + 'cmodule.c')

        with CompilationUnit(source_name, "Composite module code.", module, project, buildable, sip_api_file=False) as sf:
            _composite_module_code(sf, spec, py_debug)
    else:
        _module_code(spec, bindings, project, py_debug, buildable)


def _internal_api_header(sf, spec, bindings, py_debug, name_cache_list):
    """ Generate the C++ internal module API header file and return its path
    name.
    """

    module = spec.module
    module_name = spec.module.py_name

    # The include files.
    sf.write(
f'''#ifndef _{module_name}API_H
#define _{module_name}API_H
''')

    _declare_limited_api(sf, py_debug, module=module)
    _include_sip_h(sf, module)

    if _pyqt5(spec) or _pyqt6(spec):
        sf.write(
'''
#include <QMetaType>
#include <QThread>
''')

    # Define the qualifiers.
    qualifier_defines = []

    _append_qualifier_defines(module, bindings, qualifier_defines)

    for imported_module in module.all_imports:
        _append_qualifier_defines(imported_module, bindings, qualifier_defines)

    if len(qualifier_defines) != 0:
        sf.write('\n/* These are the qualifiers that are enabled. */\n')

        for qualifier_define in qualifier_defines:
            sf.write(qualifier_define + '\n')

        sf.write('\n')

    # Generate references to (potentially) shared strings.
    no_intro = True

    for cached_name in name_cache_list:
        if not cached_name.used:
            continue

        if no_intro:
            sf.write(
'''
/*
 * Convenient names to refer to various strings defined in this module.
 * Only the class names are part of the public API.
 */
''')

            no_intro = False

        sf.write(
f'''#define {_cached_name_ref(cached_name, as_nr=True)} {cached_name.offset}
#define {_cached_name_ref(cached_name)} &sipStrings_{module_name}[{cached_name.offset}]
''')

    # These are common to all ABI versions.
    sf.write(
f'''
#define sipMalloc                   sipAPI_{module_name}->api_malloc
#define sipFree                     sipAPI_{module_name}->api_free
#define sipBuildResult              sipAPI_{module_name}->api_build_result
#define sipCallMethod               sipAPI_{module_name}->api_call_method
#define sipCallProcedureMethod      sipAPI_{module_name}->api_call_procedure_method
#define sipCallErrorHandler         sipAPI_{module_name}->api_call_error_handler
#define sipParseResultEx            sipAPI_{module_name}->api_parse_result_ex
#define sipParseResult              sipAPI_{module_name}->api_parse_result
#define sipParseArgs                sipAPI_{module_name}->api_parse_args
#define sipParseKwdArgs             sipAPI_{module_name}->api_parse_kwd_args
#define sipParsePair                sipAPI_{module_name}->api_parse_pair
#define sipInstanceDestroyed        sipAPI_{module_name}->api_instance_destroyed
#define sipInstanceDestroyedEx      sipAPI_{module_name}->api_instance_destroyed_ex
#define sipConvertFromSequenceIndex sipAPI_{module_name}->api_convert_from_sequence_index
#define sipConvertFromSliceObject   sipAPI_{module_name}->api_convert_from_slice_object
#define sipConvertFromVoidPtr       sipAPI_{module_name}->api_convert_from_void_ptr
#define sipConvertToVoidPtr         sipAPI_{module_name}->api_convert_to_void_ptr
#define sipAddException             sipAPI_{module_name}->api_add_exception
#define sipNoFunction               sipAPI_{module_name}->api_no_function
#define sipNoMethod                 sipAPI_{module_name}->api_no_method
#define sipAbstractMethod           sipAPI_{module_name}->api_abstract_method
#define sipBadClass                 sipAPI_{module_name}->api_bad_class
#define sipBadCatcherResult         sipAPI_{module_name}->api_bad_catcher_result
#define sipBadCallableArg           sipAPI_{module_name}->api_bad_callable_arg
#define sipBadOperatorArg           sipAPI_{module_name}->api_bad_operator_arg
#define sipTrace                    sipAPI_{module_name}->api_trace
#define sipTransferBack             sipAPI_{module_name}->api_transfer_back
#define sipTransferTo               sipAPI_{module_name}->api_transfer_to
#define sipSimpleWrapper_Type       sipAPI_{module_name}->api_simplewrapper_type
#define sipWrapper_Type             sipAPI_{module_name}->api_wrapper_type
#define sipWrapperType_Type         sipAPI_{module_name}->api_wrappertype_type
#define sipVoidPtr_Type             sipAPI_{module_name}->api_voidptr_type
#define sipGetPyObject              sipAPI_{module_name}->api_get_pyobject
#define sipGetAddress               sipAPI_{module_name}->api_get_address
#define sipGetMixinAddress          sipAPI_{module_name}->api_get_mixin_address
#define sipGetCppPtr                sipAPI_{module_name}->api_get_cpp_ptr
#define sipGetComplexCppPtr         sipAPI_{module_name}->api_get_complex_cpp_ptr
#define sipCallHook                 sipAPI_{module_name}->api_call_hook
#define sipEndThread                sipAPI_{module_name}->api_end_thread
#define sipRaiseUnknownException    sipAPI_{module_name}->api_raise_unknown_exception
#define sipRaiseTypeException       sipAPI_{module_name}->api_raise_type_exception
#define sipBadLengthForSlice        sipAPI_{module_name}->api_bad_length_for_slice
#define sipAddTypeInstance          sipAPI_{module_name}->api_add_type_instance
#define sipPySlotExtend             sipAPI_{module_name}->api_pyslot_extend
#define sipAddDelayedDtor           sipAPI_{module_name}->api_add_delayed_dtor
#define sipCanConvertToType         sipAPI_{module_name}->api_can_convert_to_type
#define sipConvertToType            sipAPI_{module_name}->api_convert_to_type
#define sipForceConvertToType       sipAPI_{module_name}->api_force_convert_to_type
#define sipConvertToEnum            sipAPI_{module_name}->api_convert_to_enum
#define sipConvertToBool            sipAPI_{module_name}->api_convert_to_bool
#define sipReleaseType              sipAPI_{module_name}->api_release_type
#define sipConvertFromType          sipAPI_{module_name}->api_convert_from_type
#define sipConvertFromNewType       sipAPI_{module_name}->api_convert_from_new_type
#define sipConvertFromNewPyType     sipAPI_{module_name}->api_convert_from_new_pytype
#define sipConvertFromEnum          sipAPI_{module_name}->api_convert_from_enum
#define sipGetState                 sipAPI_{module_name}->api_get_state
#define sipExportSymbol             sipAPI_{module_name}->api_export_symbol
#define sipImportSymbol             sipAPI_{module_name}->api_import_symbol
#define sipFindType                 sipAPI_{module_name}->api_find_type
#define sipBytes_AsChar             sipAPI_{module_name}->api_bytes_as_char
#define sipBytes_AsString           sipAPI_{module_name}->api_bytes_as_string
#define sipString_AsASCIIChar       sipAPI_{module_name}->api_string_as_ascii_char
#define sipString_AsASCIIString     sipAPI_{module_name}->api_string_as_ascii_string
#define sipString_AsLatin1Char      sipAPI_{module_name}->api_string_as_latin1_char
#define sipString_AsLatin1String    sipAPI_{module_name}->api_string_as_latin1_string
#define sipString_AsUTF8Char        sipAPI_{module_name}->api_string_as_utf8_char
#define sipString_AsUTF8String      sipAPI_{module_name}->api_string_as_utf8_string
#define sipUnicode_AsWChar          sipAPI_{module_name}->api_unicode_as_wchar
#define sipUnicode_AsWString        sipAPI_{module_name}->api_unicode_as_wstring
#define sipConvertFromConstVoidPtr  sipAPI_{module_name}->api_convert_from_const_void_ptr
#define sipConvertFromVoidPtrAndSize    sipAPI_{module_name}->api_convert_from_void_ptr_and_size
#define sipConvertFromConstVoidPtrAndSize   sipAPI_{module_name}->api_convert_from_const_void_ptr_and_size
#define sipWrappedTypeName(wt)      ((wt)->wt_td->td_cname)
#define sipGetReference             sipAPI_{module_name}->api_get_reference
#define sipKeepReference            sipAPI_{module_name}->api_keep_reference
#define sipRegisterProxyResolver    sipAPI_{module_name}->api_register_proxy_resolver
#define sipRegisterPyType           sipAPI_{module_name}->api_register_py_type
#define sipTypeFromPyTypeObject     sipAPI_{module_name}->api_type_from_py_type_object
#define sipTypeScope                sipAPI_{module_name}->api_type_scope
#define sipResolveTypedef           sipAPI_{module_name}->api_resolve_typedef
#define sipRegisterAttributeGetter  sipAPI_{module_name}->api_register_attribute_getter
#define sipEnableAutoconversion     sipAPI_{module_name}->api_enable_autoconversion
#define sipInitMixin                sipAPI_{module_name}->api_init_mixin
#define sipExportModule             sipAPI_{module_name}->api_export_module
#define sipInitModule               sipAPI_{module_name}->api_init_module
#define sipGetInterpreter           sipAPI_{module_name}->api_get_interpreter
#define sipSetTypeUserData          sipAPI_{module_name}->api_set_type_user_data
#define sipGetTypeUserData          sipAPI_{module_name}->api_get_type_user_data
#define sipPyTypeDict               sipAPI_{module_name}->api_py_type_dict
#define sipPyTypeName               sipAPI_{module_name}->api_py_type_name
#define sipGetCFunction             sipAPI_{module_name}->api_get_c_function
#define sipGetMethod                sipAPI_{module_name}->api_get_method
#define sipFromMethod               sipAPI_{module_name}->api_from_method
#define sipGetDate                  sipAPI_{module_name}->api_get_date
#define sipFromDate                 sipAPI_{module_name}->api_from_date
#define sipGetDateTime              sipAPI_{module_name}->api_get_datetime
#define sipFromDateTime             sipAPI_{module_name}->api_from_datetime
#define sipGetTime                  sipAPI_{module_name}->api_get_time
#define sipFromTime                 sipAPI_{module_name}->api_from_time
#define sipIsUserType               sipAPI_{module_name}->api_is_user_type
#define sipCheckPluginForType       sipAPI_{module_name}->api_check_plugin_for_type
#define sipUnicodeNew               sipAPI_{module_name}->api_unicode_new
#define sipUnicodeWrite             sipAPI_{module_name}->api_unicode_write
#define sipUnicodeData              sipAPI_{module_name}->api_unicode_data
#define sipGetBufferInfo            sipAPI_{module_name}->api_get_buffer_info
#define sipReleaseBufferInfo        sipAPI_{module_name}->api_release_buffer_info
#define sipIsOwnedByPython          sipAPI_{module_name}->api_is_owned_by_python
#define sipIsDerivedClass           sipAPI_{module_name}->api_is_derived_class
#define sipGetUserObject            sipAPI_{module_name}->api_get_user_object
#define sipSetUserObject            sipAPI_{module_name}->api_set_user_object
#define sipRegisterEventHandler     sipAPI_{module_name}->api_register_event_handler
#define sipConvertToArray           sipAPI_{module_name}->api_convert_to_array
#define sipConvertToTypedArray      sipAPI_{module_name}->api_convert_to_typed_array
#define sipEnableGC                 sipAPI_{module_name}->api_enable_gc
#define sipPrintObject              sipAPI_{module_name}->api_print_object
#define sipLong_AsChar              sipAPI_{module_name}->api_long_as_char
#define sipLong_AsSignedChar        sipAPI_{module_name}->api_long_as_signed_char
#define sipLong_AsUnsignedChar      sipAPI_{module_name}->api_long_as_unsigned_char
#define sipLong_AsShort             sipAPI_{module_name}->api_long_as_short
#define sipLong_AsUnsignedShort     sipAPI_{module_name}->api_long_as_unsigned_short
#define sipLong_AsInt               sipAPI_{module_name}->api_long_as_int
#define sipLong_AsUnsignedInt       sipAPI_{module_name}->api_long_as_unsigned_int
#define sipLong_AsLong              sipAPI_{module_name}->api_long_as_long
#define sipLong_AsUnsignedLong      sipAPI_{module_name}->api_long_as_unsigned_long
#define sipLong_AsLongLong          sipAPI_{module_name}->api_long_as_long_long
#define sipLong_AsUnsignedLongLong  sipAPI_{module_name}->api_long_as_unsigned_long_long
#define sipLong_AsSizeT             sipAPI_{module_name}->api_long_as_size_t
#define sipVisitWrappers            sipAPI_{module_name}->api_visit_wrappers
#define sipRegisterExitNotifier     sipAPI_{module_name}->api_register_exit_notifier
''')

    # These are dependent on the specific ABI version.
    if spec.abi_version >= (13, 0):
        # ABI v13.9 and later
        if spec.abi_version >= (13, 9):
            sf.write(
f'''#define sipDeprecated               sipAPI_{module_name}->api_deprecated_13_9
''')
        else:
            sf.write(
f'''#define sipDeprecated               sipAPI_{module_name}->api_deprecated
''')
                    
        # ABI v13.6 and later.
        if spec.abi_version >= (13, 6):
            sf.write(
f'''#define sipPyTypeDictRef            sipAPI_{module_name}->api_py_type_dict_ref
''')

        # ABI v13.1 and later.
        if spec.abi_version >= (13, 1):
            sf.write(
f'''#define sipNextExceptionHandler     sipAPI_{module_name}->api_next_exception_handler
''')
            
        # ABI v13.0 and later. */
        sf.write(
f'''#define sipIsEnumFlag               sipAPI_{module_name}->api_is_enum_flag
#define sipConvertToTypeUS          sipAPI_{module_name}->api_convert_to_type_us
#define sipForceConvertToTypeUS     sipAPI_{module_name}->api_force_convert_to_type_us
#define sipReleaseTypeUS            sipAPI_{module_name}->api_release_type_us
''')
    else:
        # ABI v12.16 and later
        if spec.abi_version >= (12, 16):
            sf.write(
f'''#define sipDeprecated               sipAPI_{module_name}->api_deprecated_12_16
''')
        else:
            sf.write(
f'''#define sipDeprecated               sipAPI_{module_name}->api_deprecated
''')
            
        # ABI v12.13 and later.
        if spec.abi_version >= (12, 13):
            sf.write(
f'''#define sipPyTypeDictRef            sipAPI_{module_name}->api_py_type_dict_ref
''')

        # ABI v12.9 and later.
        if spec.abi_version >= (12, 9):
            sf.write(
f'''#define sipNextExceptionHandler     sipAPI_{module_name}->api_next_exception_handler
''')

        # ABI v12.8 and earlier.
        sf.write(
f'''#define sipSetNewUserTypeHandler    sipAPI_{module_name}->api_set_new_user_type_handler
#define sipGetFrame                 sipAPI_{module_name}->api_get_frame
#define sipSetDestroyOnExit         sipAPI_{module_name}->api_set_destroy_on_exit
#define sipEnableOverflowChecking   sipAPI_{module_name}->api_enable_overflow_checking
#define sipIsAPIEnabled             sipAPI_{module_name}->api_is_api_enabled
#define sipClearAnySlotReference    sipAPI_{module_name}->api_clear_any_slot_reference
#define sipConnectRx                sipAPI_{module_name}->api_connect_rx
#define sipConvertRx                sipAPI_{module_name}->api_convert_rx
#define sipDisconnectRx             sipAPI_{module_name}->api_disconnect_rx
#define sipFreeSipslot              sipAPI_{module_name}->api_free_sipslot
#define sipInvokeSlot               sipAPI_{module_name}->api_invoke_slot
#define sipInvokeSlotEx             sipAPI_{module_name}->api_invoke_slot_ex
#define sipSameSlot                 sipAPI_{module_name}->api_same_slot
#define sipSaveSlot                 sipAPI_{module_name}->api_save_slot
#define sipVisitSlot                sipAPI_{module_name}->api_visit_slot
''')

    if spec.abi_version >= (12, 8):
        # ABI v12.8 and later.
        sf.write(
f'''#define sipIsPyMethod               sipAPI_{module_name}->api_is_py_method_12_8
''')
    else:
        # ABI v12.7 and earlier.
        sf.write(
f'''#define sipIsPyMethod               sipAPI_{module_name}->api_is_py_method
''')

    # The name strings.
    sf.write(
f'''
/* The strings used by this module. */
extern const char sipStrings_{module_name}[];
''')

    _module_api(sf, spec, bindings)

    sf.write(
f'''
/* The SIP API, this module's API and the APIs of any imported modules. */
extern const sipAPIDef *sipAPI_{module_name};
extern sipExportedModuleDef sipModuleAPI_{module_name};
''')

    if len(module.needed_types) != 0:
        sf.write(f'extern sipTypeDef *sipExportedTypes_{module_name}[];\n')

    for imported_module in module.all_imports:
        imported_module_name = imported_module.py_name

        _imported_module_api(sf, spec, imported_module)

        if len(imported_module.needed_types) != 0:
            sf.write(f'extern sipImportedTypeDef sipImportedTypes_{module_name}_{imported_module_name}[];\n')

        if imported_module.nr_virtual_error_handlers != 0:
            sf.write(f'extern sipImportedVirtErrorHandlerDef sipImportedVirtErrorHandlers_{module_name}_{imported_module_name}[];\n')

        if imported_module.nr_exceptions != 0:
            sf.write(f'extern sipImportedExceptionDef sipImportedExceptions_{module_name}_{imported_module_name}[];\n')

    if _pyqt5(spec) or _pyqt6(spec):
        sf.write(
f'''
typedef const QMetaObject *(*sip_qt_metaobject_func)(sipSimpleWrapper *, sipTypeDef *);
extern sip_qt_metaobject_func sip_{module_name}_qt_metaobject;

typedef int (*sip_qt_metacall_func)(sipSimpleWrapper *, sipTypeDef *, QMetaObject::Call, int, void **);
extern sip_qt_metacall_func sip_{module_name}_qt_metacall;

typedef bool (*sip_qt_metacast_func)(sipSimpleWrapper *, const sipTypeDef *, const char *, void **);
extern sip_qt_metacast_func sip_{module_name}_qt_metacast;
''')

    # Handwritten code.
    sf.write_code(spec.exported_header_code)
    sf.write_code(module.module_header_code)

    # Make sure any header code needed by the default exception is included.
    if module.default_exception is not None:
        sf.write_code(module.default_exception.iface_file.type_header_code)

    # Note that we don't forward declare the virtual handlers.  This is because
    # we would need to #include everything needed for their argument types.
    sf.write('\n#endif\n')


def _make_part_name(buildable, module_name, part_nr, source_suffix):
    """ Return the filename of a source code part on the heap. """

    return os.path.join(buildable.build_dir,
            f'sip{module_name}part{part_nr}{source_suffix}')


def _composite_module_code(sf, spec, py_debug):
    """ Output the code for a composite module. """

    module = spec.module

    _declare_limited_api(sf, py_debug)
    _include_sip_h(sf, module)

    sf.write(
'''

static void sip_import_component_module(PyObject *d, const char *name)
{
    PyObject *mod;

    PyErr_Clear();

    mod = PyImport_ImportModule(name);

    /*
     * Note that we don't complain if the module can't be imported.  This
     * is a favour to Linux distro packagers who like to split PyQt into
     * different sub-packages.
     */
    if (mod)
    {
        PyDict_Merge(d, PyModule_GetDict(mod), 0);
        Py_DECREF(mod);
    }
}
''')

    _module_docstring(sf, module)
    _module_init_start(sf, spec)
    _module_definition(sf, module)

    sf.write(
'''
    PyObject *sipModule, *sipModuleDict;

    if ((sipModule = PyModule_Create(&sip_module_def)) == SIP_NULLPTR)
        return SIP_NULLPTR;

    sipModuleDict = PyModule_GetDict(sipModule);

''')

    for mod in module.all_imports:
        sf.write(
f'    sip_import_component_module(sipModuleDict, "{mod.fq_py_name}");\n')

    sf.write(
'''
    PyErr_Clear();

    return sipModule;
}
''')


def _module_code(spec, bindings, project, py_debug, buildable):
    """ Generate the C/C++ code for a module. """

    module = spec.module
    module_name = module.py_name
    parts = bindings.concatenate

    source_suffix = bindings.source_suffix
    if source_suffix is None:
        source_suffix = '.c' if spec.c_bindings else '.cpp'

    # Calculate the number of files in each part.
    if parts:
        nr_files = 1

        for iface_file in spec.iface_files:
            if iface_file.module is module and iface_file.type is not IfaceFileType.EXCEPTION:
                nr_files += 1

        max_per_part = (nr_files + parts - 1) // parts
        files_in_part = 1
        this_part = 0

        source_name = _make_part_name(buildable, module_name, 0, source_suffix)
    else:
        source_name = os.path.join(buildable.build_dir,
                'sip' + module_name + 'cmodule' + source_suffix)

    sf = CompilationUnit(source_name, "Module code.", module, project,
            buildable)

    # Include the library headers for types used by virtual handlers, module
    # level functions, module level variables, exceptions and Qt meta types.
    _used_includes(sf, module.used)

    sf.write_code(module.unit_postinclude_code)

    # If there should be a Qt support API then generate stubs values for the
    # optional parts.  These should be undefined in %ModuleCode if a C++
    # implementation is provided.
    if spec.abi_version < (13, 0) and _module_supports_qt(spec):
        sf.write(
'''
#define sipQtCreateUniversalSignal          0
#define sipQtFindUniversalSignal            0
#define sipQtEmitSignal                     0
#define sipQtConnectPySignal                0
#define sipQtDisconnectPySignal             0
''')

    # Transform the name cache.
    name_cache_list = _name_cache_as_list(spec.name_cache)

    # Define the names.
    _name_cache(sf, spec, name_cache_list)

    # Generate the C++ code blocks.
    sf.write_code(module.module_code)

    # Generate any virtual handlers.
    for handler in spec.virtual_handlers:
        _virtual_handler(sf, spec, handler)

    # Generate any virtual error handlers.
    for virtual_error_handler in spec.virtual_error_handlers:
        if virtual_error_handler.module is module:
            self_name = _use_in_code(virtual_error_handler.code, 'sipPySelf')
            state_name = _use_in_code(virtual_error_handler.code,
                    'sipGILState')

            sf.write(
f'''

void sipVEH_{module_name}_{virtual_error_handler.name}(sipSimpleWrapper *{self_name}, sip_gilstate_t {state_name})
{{
''')

            sf.write_code(virtual_error_handler.code)

            sf.write('}\n')

    # Generate the global functions.
    slot_extenders = False

    for member in module.global_functions:
        if member.py_slot is None:
            _ordinary_function(sf, spec, bindings, member)
        else:
            # Make sure that there is still an overload and we haven't moved
            # them all to classes.
            for overload in module.overloads:
                if overload.common is member:
                    _py_slot(sf, spec, bindings, member)
                    slot_extenders = True
                    break

    # Generate the global functions for any hidden namespaces.
    for klass in spec.classes:
        if klass.iface_file.module is module and klass.is_hidden_namespace:
            for member in klass.members:
                if member.py_slot is None:
                    _ordinary_function(sf, spec, bindings, member, scope=klass)

    # Generate any class specific __init__ or slot extenders.
    init_extenders = False

    for klass in module.proxies:
        if len(klass.ctors) != 0:
            _type_init(sf, spec, bindings, klass)
            init_extenders = True

        for member in klass.members:
            _py_slot(sf, spec, bindings, member, scope=klass)
            slot_extenders = True

    # Generate any __init__ extender table.
    if init_extenders:
        sf.write(
'''
static sipInitExtenderDef initExtenders[] = {
''')

        first_field = '-1, ' if spec.abi_version < (13, 0) else ''

        for klass in module.proxies:
            if len(klass.ctors) != 0:
                klass_name = klass.iface_file.fq_cpp_name.as_word
                encoded_type = _encoded_type(module, klass)

                sf.write(f'    {{{first_field}init_type_{klass_name}, {encoded_type}, SIP_NULLPTR}},\n')

        sf.write(
f'''    {{{first_field}SIP_NULLPTR, {{0, 0, 0}}, SIP_NULLPTR}}
}};
''')

    # Generate any slot extender table.
    if slot_extenders:
        sf.write(
'''
static sipPySlotExtenderDef slotExtenders[] = {\n''')

        for member in module.global_functions:
            if member.py_slot is None:
                continue

            for overload in module.overloads:
                if overload.common is member:
                    member_name = member.py_name
                    slot_name = _get_slot_name(member.py_slot)

                    sf.write(
f'    {{(void *)slot_{member_name}, {slot_name}, {{0, 0, 0}}}},\n')

                    break

        for klass in module.proxies:
            for member in klass.members:
                klass_name = klass.iface_file.fq_cpp_name.as_word
                member_name = member.py_name
                slot_name = _get_slot_name(member.py_slot)
                encoded_type = _encoded_type(module, klass)

                sf.write(f'    {{(void *)slot_{klass_name}_{member_name}, {slot_name}, {encoded_type}}},\n')

        sf.write(
'''    {SIP_NULLPTR, (sipPySlotType)0, {0, 0, 0}}
};
''')

    # Generate the global access functions.
    _access_functions(sf, spec)

    # Generate any sub-class convertors.
    nr_subclass_convertors = _subclass_convertors(sf, spec, module)

    # Generate the external classes table if needed.
    has_external = False

    for klass in spec.classes:
        if not klass.external:
            continue

        if klass.iface_file.module is not module:
            continue

        if not has_external:
            sf.write(
'''

/* This defines each external type declared in this module, */
static sipExternalTypeDef externalTypesTable[] = {
''')

            has_external = True

        type_nr = klass.iface_file.type_nr
        klass_py = klass.iface_file.fq_cpp_name.as_py

        sf.write(f'    {{{type_nr}, "{klass_py}"}},\n')

    if has_external:
        sf.write(
'''    {-1, SIP_NULLPTR}
};
''')

    # Generate any enum slot tables.
    for enum in spec.enums:
        if enum.module is not module or enum.fq_cpp_name is None:
            continue

        if len(enum.slots) == 0:
            continue

        for member in enum.slots:
            _py_slot(sf, spec, bindings, member, scope=enum)

        enum_name = enum.fq_cpp_name.as_word

        sf.write(
f'''
static sipPySlotDef slots_{enum_name}[] = {{
''')

        for member in enum.slots:
            if member.py_slot is not None:
                member_name = member.py_name
                slot_name = _get_slot_name(member.py_slot)

                sf.write(f'    {{(void *)slot_{enum_name}_{member_name}, {slot_name}}},\n')

        sf.write(
'''    {SIP_NULLPTR, (sipPySlotType)0}
};

''')

    # Generate the enum type structures while recording the order in which they
    # are generated.  Note that we go through the sorted table of needed types
    # rather than the unsorted list of all enums.
    needed_enums = []

    for needed_type in module.needed_types:
        if needed_type.type is not ArgumentType.ENUM:
            continue

        enum = needed_type.definition

        scope_type_nr = -1 if enum.scope is None else enum.scope.iface_file.type_nr

        if len(needed_enums) == 0:
            sf.write('static sipEnumTypeDef enumTypes[] = {\n')

        cpp_name = _get_normalised_cached_name(enum.cached_fq_cpp_name)
        py_name = _get_normalised_cached_name(enum.py_name)

        if spec.abi_version >= (13, 0):
            base_type = 'SIP_ENUM_' + enum.base_type.name
            nr_members = len(enum.members)

            sf.write(
f'    {{{{SIP_NULLPTR, SIP_TYPE_ENUM, sipNameNr_{cpp_name}, SIP_NULLPTR, 0}}, {base_type}, sipNameNr_{py_name}, {scope_type_nr}, {nr_members}')
        else:
            sip_type = 'SIP_TYPE_SCOPED_ENUM' if enum.is_scoped else 'SIP_TYPE_ENUM'

            sf.write(
f'    {{{{-1, SIP_NULLPTR, SIP_NULLPTR, {sip_type}, sipNameNr_{cpp_name}, SIP_NULLPTR, 0}}, sipNameNr_{py_name}, {scope_type_nr}')

        if len(enum.slots) == 0:
            sf.write(', SIP_NULLPTR')
        else:
            sf.write(', slots_' + enum.fq_cpp_name.as_word)

        sf.write('},\n')

        needed_enums.append(enum)

    if len(needed_enums) != 0:
        sf.write('};\n')

    if spec.abi_version >= (13, 0):
        nr_enum_members = -1
    else:
        nr_enum_members = _enum_member_table(sf, spec)

    # Generate the types table.
    if len(module.needed_types) != 0:
        _types_table(sf, module, needed_enums)

    # Generate the typedefs table.
    if module.nr_typedefs > 0:
        sf.write(
'''

/*
 * These define each typedef in this module.
 */
static sipTypedefDef typedefsTable[] = {
''')

        for typedef in spec.typedefs:
            if typedef.module is not module:
                continue

            cpp_name = typedef.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)

            sf.write(f'    {{"{cpp_name}", "')

            # The default behaviour isn't right in a couple of cases.
            # TODO: is this still true?
            if typedef.type.type is ArgumentType.LONGLONG:
                sf.write('long long')
            elif typedef.type.type is ArgumentType.ULONGLONG:
                sf.write('unsigned long long')
            else:
                sf.write(
                        fmt_argument_as_cpp_type(spec, typedef.type,
                                strip=STRIP_GLOBAL, use_typename=False))

            sf.write('"},\n')

        sf.write('};\n')

    # Generate the error handlers table.
    has_virtual_error_handlers = False

    for handler in spec.virtual_error_handlers:
        if handler.module is not module:
            continue

        if not has_virtual_error_handlers:
            has_virtual_error_handlers = True

            sf.write(
'''

/*
 * This defines the virtual error handlers that this module implements and
 * can be used by other modules.
 */
static sipVirtErrorHandlerDef virtErrorHandlersTable[] = {
''')

        sf.write(f'    {{"{handler.name}", sipVEH_{module_name}_{handler.name}}},\n')

    if has_virtual_error_handlers:
        sf.write(
'''    {SIP_NULLPTR, SIP_NULLPTR}
};
''')

    # Generate the tables for things we are importing.
    for imported_module in module.all_imports:
        imported_module_name = imported_module.py_name

        if len(imported_module.needed_types) != 0:
            sf.write(
f'''

/* This defines the types that this module needs to import from {imported_module_name}. */
sipImportedTypeDef sipImportedTypes_{module_name}_{imported_module_name}[] = {{
''')

            for needed_type in imported_module.needed_types:
                if needed_type.type is ArgumentType.MAPPED:
                    type_name = needed_type.definition.cpp_name
                else:
                    if needed_type.type is ArgumentType.CLASS:
                        scoped_name = needed_type.definition.iface_file.fq_cpp_name
                    else:
                        scoped_name = needed_type.definition.fq_cpp_name

                    type_name = scoped_name.cpp_stripped(STRIP_GLOBAL)

                sf.write(f'    {{"{type_name}"}},\n')

            sf.write(
'''    {SIP_NULLPTR}
};
''')

        if imported_module.nr_virtual_error_handlers > 0:
            sf.write(
f'''

/*
 * This defines the virtual error handlers that this module needs to import
 * from {imported_module_name}.
 */
sipImportedVirtErrorHandlerDef sipImportedVirtErrorHandlers_{module_name}_{imported_module_name}[] = {{
''')

            # The handlers are unordered so search for each in turn.  There
            # will probably be only one so speed isn't an issue.
            for i in range(imported_module.nr_virtual_error_handlers):
                for handler in spec.virtual_error_handlers:
                    if handler.module is imported_module and handler.handler_nr == i:
                        sf.write(f'    {{"{handler.name}"}},\n')

            sf.write(
'''    {SIP_NULLPTR}
};
''')

        if imported_module.nr_exceptions > 0:
            sf.write(
f'''

/*
 * This defines the exception objects that this module needs to import from
 * {imported_module_name}.
 */
sipImportedExceptionDef sipImportedExceptions_{module_name}_{imported_module_name}[] = {{
''')

            # The exceptions are unordered so search for each in turn.  There
            # will probably be very few so speed isn't an issue.
            for i in range(imported_module.nr_exceptions):
                for exception in spec.exceptions:
                    if exception.iface_file.module is imported_module and exception.exception_nr == i:
                        sf.write(f'    {{"{exception.py_name}"}},\n')

            sf.write(
'''    {SIP_NULLPTR}
};
''')

    if len(module.all_imports) != 0:
        sf.write(
'''

/* This defines the modules that this module needs to import. */
static sipImportedModuleDef importsTable[] = {
''')

        for imported_module in module.all_imports:
            imported_module_name = imported_module.py_name

            types = handlers = exceptions = 'SIP_NULLPTR'

            if len(imported_module.needed_types) != 0:
                types = f'sipImportedTypes_{module_name}_{imported_module_name}'

            if imported_module.nr_virtual_error_handlers != 0:
                handlers = f'sipImportedVirtErrorHandlers_{module_name}_{imported_module_name}'

            if imported_module.nr_exceptions != 0:
                exceptions = f'sipImportedExceptions_{module_name}_{imported_module_name}'

            sf.write(f'    {{"{imported_module.fq_py_name}", {types}, {handlers}, {exceptions}}},\n')

        sf.write(
'''    {SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR}
};
''')

    # Generate the table of sub-class convertors
    if nr_subclass_convertors > 0:
        sf.write(
'''

/* This defines the class sub-convertors that this module defines. */
static sipSubClassConvertorDef convertorsTable[] = {
''')

        for klass in spec.classes:
            if klass.iface_file.module is not module:
                continue

            if klass.convert_to_subclass_code is None:
                continue

            klass_name = klass.iface_file.fq_cpp_name.as_word
            encoded_type = _encoded_type(module, klass.subclass_base)

            sf.write(f'    {{sipSubClass_{klass_name}, {encoded_type}, SIP_NULLPTR}},\n')

        sf.write(
'''    {SIP_NULLPTR, {0, 0, 0}, SIP_NULLPTR}
};
''')

    # Generate any license information.
    if module.license is not None:
        license = module.license
        licensee = 'SIP_NULLPTR' if license.licensee is None else '"' + license.licensee + '"'
        timestamp = 'SIP_NULLPTR' if license.timestamp is None else '"' + license.timestamp + '"'
        signature = 'SIP_NULLPTR' if license.signature is None else '"' + license.signature + '"'

        sf.write(
f'''

/* Define the module's license. */
static sipLicenseDef module_license = {{
    "{license.type}",
    {licensee},
    {timestamp},
    {signature}
}};
''')

    # Generate each instance table.
    is_inst_class = _class_instances(sf, spec)
    is_inst_voidp = _void_pointer_instances(sf, spec)
    is_inst_char = _char_instances(sf, spec)
    is_inst_string = _string_instances(sf, spec)
    is_inst_int = _int_instances(sf, spec)
    is_inst_long = _long_instances(sf, spec)
    is_inst_ulong = _unsigned_long_instances(sf, spec)
    is_inst_longlong = _long_long_instances(sf, spec)
    is_inst_ulonglong = _unsigned_long_long_instances(sf, spec)
    is_inst_double = _double_instances(sf, spec)

    # Generate any exceptions support.
    if bindings.exceptions:
        if module.nr_exceptions > 0:
            sf.write(
f'''

PyObject *sipExportedExceptions_{module_name}[{module.nr_exceptions + 1}];
''')

        if _abi_has_next_exception_handler(spec):
            _exception_handler(sf, spec)

    # Generate any Qt support API.
    if spec.abi_version < (13, 0) and _module_supports_qt(spec):
        sf.write(
f'''

/* This defines the Qt support API. */

static sipQtAPI qtAPI = {{
    &sipExportedTypes_{module_name}[{spec.pyqt_qobject.iface_file.type_nr}],
    sipQtCreateUniversalSignal,
    sipQtFindUniversalSignal,
    sipQtCreateUniversalSlot,
    sipQtDestroyUniversalSlot,
    sipQtFindSlot,
    sipQtConnect,
    sipQtDisconnect,
    sipQtSameSignalSlotName,
    sipQtFindSipslot,
    sipQtEmitSignal,
    sipQtConnectPySignal,
    sipQtDisconnectPySignal
}};
''')

    imports_table = _optional_ptr(len(module.all_imports) != 0, 'importsTable')
    exported_types = _optional_ptr(len(module.needed_types) != 0,
            'sipExportedTypes_' + module_name)
    external_types = _optional_ptr(has_external, 'externalTypesTable')
    typedefs_table = _optional_ptr(module.nr_typedefs != 0, 'typedefsTable')

    sf.write(
f'''

/* This defines this module. */
sipExportedModuleDef sipModuleAPI_{module_name} = {{
    SIP_NULLPTR,
    {spec.abi_version[1]},
    sipNameNr_{_get_normalised_cached_name(module.fq_py_name)},
    0,
    sipStrings_{module_name},
    {imports_table},
''')

    if spec.abi_version < (13, 0):
        qt_api = _optional_ptr(_module_supports_qt(spec), '&qtAPI')
        sf.write(f'    {qt_api},\n')

    sf.write(
f'''    {len(module.needed_types)},
    {exported_types},
    {external_types},
''')

    if nr_enum_members >= 0:
        enum_members = _optional_ptr(nr_enum_members > 0, 'enummembers')
        sf.write(
f'''    {nr_enum_members},
    {enum_members},
''')

    veh_table = _optional_ptr(has_virtual_error_handlers,
            'virtErrorHandlersTable')
    convertors = _optional_ptr(nr_subclass_convertors > 0, 'convertorsTable')
    type_instances = _optional_ptr(is_inst_class, 'typeInstances')
    void_ptr_instances = _optional_ptr(is_inst_voidp, 'voidPtrInstances')
    char_instances = _optional_ptr(is_inst_char, 'charInstances')
    string_instances = _optional_ptr(is_inst_string, 'stringInstances')
    int_instances = _optional_ptr(is_inst_int, 'intInstances')
    long_instances = _optional_ptr(is_inst_long, 'longInstances')
    unsigned_long_instances = _optional_ptr(is_inst_ulong,
            'unsignedLongInstances')
    long_long_instances = _optional_ptr(is_inst_longlong, 'longLongInstances')
    unsigned_long_long_instances = _optional_ptr(is_inst_ulonglong,
            'unsignedLongLongInstances')
    double_instances = _optional_ptr(is_inst_double, 'doubleInstances')
    module_license = _optional_ptr(module.license is not None,
            '&module_license')
    exported_exceptions = _optional_ptr(module.nr_exceptions > 0,
            'sipExportedExceptions_' + module_name)
    slot_extender_table = _optional_ptr(slot_extenders, 'slotExtenders')
    init_extender_table = _optional_ptr(init_extenders, 'initExtenders')
    delayed_dtors = _optional_ptr(module.has_delayed_dtors, 'sipDelayedDtors')

    sf.write(
f'''    {module.nr_typedefs},
    {typedefs_table},
    {veh_table},
    {convertors},
    {{{type_instances}, {void_ptr_instances}, {char_instances}, {string_instances}, {int_instances}, {long_instances}, {unsigned_long_instances}, {long_long_instances}, {unsigned_long_long_instances}, {double_instances}}},
    {module_license},
    {exported_exceptions},
    {slot_extender_table},
    {init_extender_table},
    {delayed_dtors},
    SIP_NULLPTR,
''')

    if spec.abi_version < (13, 0):
        # The unused version support.
        sf.write(
'''    SIP_NULLPTR,
    SIP_NULLPTR,
''')

    exception_handler = _optional_ptr(
            (_abi_has_next_exception_handler(spec) and bindings.exceptions and module.nr_exceptions > 0),
            'sipExceptionHandler_' + module_name)

    sf.write(
f'''    {exception_handler},
}};
''')

    _module_docstring(sf, module)

    # Generate the storage for the external API pointers.
    sf.write(
f'''

/* The SIP API and the APIs of any imported modules. */
const sipAPIDef *sipAPI_{module_name};
''')

    if _pyqt5(spec) or _pyqt6(spec):
        sf.write(
f'''
sip_qt_metaobject_func sip_{module_name}_qt_metaobject;
sip_qt_metacall_func sip_{module_name}_qt_metacall;
sip_qt_metacast_func sip_{module_name}_qt_metacast;
''')

    # Generate the Python module initialisation function.
    _module_init_start(sf, spec)

    # Generate the global functions.
    sf.write('    static PyMethodDef sip_methods[] = {\n')

    _global_function_table_entries(sf, spec, bindings, module.global_functions)

    # Generate the global functions for any hidden namespaces.
    for klass in spec.classes:
        if klass.iface_file.module is module and klass.is_hidden_namespace:
            _global_function_table_entries(sf, spec, bindings, klass.members)

    sf.write(
'''        {SIP_NULLPTR, SIP_NULLPTR, 0, SIP_NULLPTR}
    };
''')

    _module_definition(sf, module, method_table='sip_methods')

    sf.write('\n    PyObject *sipModule, *sipModuleDict;\n')

    if spec.sip_module:
        sf.write('    PyObject *sip_sipmod, *sip_capiobj;\n\n')

    # Generate any pre-initialisation code.
    sf.write_code(module.preinitialisation_code)

    sf.write(
'''    /* Initialise the module and get it's dictionary. */
    if ((sipModule = PyModule_Create(&sip_module_def)) == SIP_NULLPTR)
        return SIP_NULLPTR;

    sipModuleDict = PyModule_GetDict(sipModule);

''')

    _sip_api(sf, spec)

    # Generate any initialisation code.
    sf.write_code(module.initialisation_code)

    abi_major, abi_minor = spec.abi_version

    sf.write(
f'''    /* Export the module and publish it's API. */
    if (sipExportModule(&sipModuleAPI_{module_name}, {abi_major}, {abi_minor}, 0) < 0)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}
''')

    if _pyqt5(spec) or _pyqt6(spec):
        # Import the helpers.
        sf.write(
f'''
    sip_{module_name}_qt_metaobject = (sip_qt_metaobject_func)sipImportSymbol("qtcore_qt_metaobject");
    sip_{module_name}_qt_metacall = (sip_qt_metacall_func)sipImportSymbol("qtcore_qt_metacall");
    sip_{module_name}_qt_metacast = (sip_qt_metacast_func)sipImportSymbol("qtcore_qt_metacast");

    if (!sip_{module_name}_qt_metacast)
        Py_FatalError("Unable to import qtcore_qt_metacast");

''')

    sf.write(
f'''    /* Initialise the module now all its dependencies have been set up. */
    if (sipInitModule(&sipModuleAPI_{module_name}, sipModuleDict) < 0)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}
''')

    _types_inline(sf, spec)
    _py_objects(sf, spec)

    # Create any exception objects.
    for exception in spec.exceptions:
        if exception.iface_file.module is not module:
            continue

        if exception.exception_nr < 0:
            continue

        if exception.builtin_base_exception is not None:
            exception_type = 'PyExc_' + exception.builtin_base_exception
        else:
            exception_type = 'sipException_' + exception.defined_base_exception.iface_file.fq_cpp_name.as_word

        sf.write(
f'''
    if ((sipExportedExceptions_{module_name}[{exception.exception_nr}] = PyErr_NewException(
            "{module_name}.{exception.py_name}",
            {exception_type}, SIP_NULLPTR)) == SIP_NULLPTR || PyDict_SetItemString(sipModuleDict, "{exception.py_name}", sipExportedExceptions_{module_name}[{exception.exception_nr}]) < 0)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}
''')

    if module.nr_exceptions > 0:
        sf.write(
f'''
    sipExportedExceptions_{module_name}[{module.nr_exceptions}] = SIP_NULLPTR;
''')

    # Generate the enum and QFlag meta-type registrations for PyQt6.  (It may
    # be possible to create these dynamically on demand.)
    if _pyqt6(spec):
        for enum in spec.enums:
            if enum.module is not module or enum.fq_cpp_name is None:
                continue

            if enum.is_protected:
                continue

            if isinstance(enum.scope, WrappedClass) and enum.scope.pyqt_no_qmetaobject:
                continue

            sf.write(f'    qMetaTypeId<{enum.fq_cpp_name.as_cpp}>();\n')

        for mapped_type in spec.mapped_types:
            if mapped_type.iface_file.module is not module:
                continue

            if mapped_type.pyqt_flags == 0:
                continue

            mapped_type_type = fmt_argument_as_cpp_type(spec, mapped_type.type,
                    plain=True, no_derefs=True)
            sf.write(f'    qMetaTypeId<{mapped_type_type}>();\n')

    # Generate any post-initialisation code. */
    sf.write_code(module.postinitialisation_code)

    sf.write(
'''
    return sipModule;
}
''')

    # Generate the interface source files.
    for iface_file in spec.iface_files:
        if iface_file.module is module and iface_file.type is not IfaceFileType.EXCEPTION:
            need_postinc = False
            use_sf = None

            if parts:
                if files_in_part == max_per_part:
                    # Close the old part.
                    sf.close()

                    # Create a new one.
                    files_in_part = 1
                    this_part += 1

                    source_name = _make_part_name(buildable, module_name,
                            this_part, source_suffix)
                    sf = CompilationUnit(source_name, "Module code.", module,
                            project, buildable)

                    need_postinc = True
                else:
                    files_in_part += 1

                if iface_file.file_extension is None:
                    # The interface file should use this source file rather
                    # than create one of its own.
                    use_sf = sf

            _iface_file_cpp(spec, bindings, project, buildable, py_debug,
                    iface_file, need_postinc, source_suffix, use_sf)

    sf.close()

    header_name = os.path.join(buildable.build_dir, f'sipAPI{module_name}.h')

    with SourceFile(header_name, "Internal module API header file.", module, project, buildable.headers) as sf:
        _internal_api_header(sf, spec, bindings, py_debug, name_cache_list)


def _name_cache_as_list(name_cache):
    """ Return a name cache as a correctly ordered list of CachedName objects.
    """

    name_cache_list = []

    # Create the list sorted first by descending name length and then
    # alphabetical order.
    for k in sorted(name_cache.keys(), reverse=True):
        name_cache_list.extend(sorted(name_cache[k], key=lambda k: k.name))

    # Set the offset into the string pool for every used name.
    offset = 0

    for cached_name in name_cache_list:
        if not cached_name.used:
            continue

        name_len = len(cached_name.name)

        # See if the tail of a previous used name could be used instead.
        for prev_name in name_cache_list:
            prev_name_len = len(prev_name.name)

            if prev_name_len <= name_len:
                break

            if not prev_name.used or prev_name.is_substring:
                continue

            if prev_name.name.endswith(cached_name.name):
                cached_name.is_substring = True
                cached_name.offset = prev_name.offset + prev_name_len - name_len;
                break

        if not cached_name.is_substring:
            cached_name.offset = offset
            offset += name_len + 1

    return name_cache_list


def _name_cache(sf, spec, name_cache_list):
    """ Generate the name cache definition. """

    sf.write(
f'''
/* Define the strings used by this module. */
const char sipStrings_{spec.module.py_name}[] = {{
''')

    for name in name_cache_list:
        if not name.used or name.is_substring:
            continue

        sf.write('    ')

        for ch in name.name:
            sf.write(f"'{ch}', ")

        sf.write('0,\n')

    sf.write('};\n')


def _types_table(sf, module, needed_enums):
    """ Generate the types table for a module. """

    module_name = module.py_name

    sf.write(
f'''

/*
 * This defines each type in this module.
 */
sipTypeDef *sipExportedTypes_{module_name}[] = {{
''')

    for needed_type in module.needed_types:
        if needed_type.type is ArgumentType.CLASS:
            klass = needed_type.definition

            if klass.external:
                sf.write('    0,\n')
            elif not klass.is_hidden_namespace:
                sf.write(f'    &sipTypeDef_{module_name}_{klass.iface_file.fq_cpp_name.as_word}.ctd_base,\n')

        elif needed_type.type is ArgumentType.MAPPED:
            mapped_type = needed_type.definition

            sf.write(f'    &sipTypeDef_{module_name}_{mapped_type.iface_file.fq_cpp_name.as_word}.mtd_base,\n')

        elif needed_type.type is ArgumentType.ENUM:
            enum = needed_type.definition
            enum_index = needed_enums.index(enum)

            sf.write(f'    &enumTypes[{enum_index}].etd_base,\n')

    sf.write('};\n')


def _sip_api(sf, spec):
    """ Generate the code to get the sip API. """

    sip_module_name = spec.sip_module
    module_name = spec.module.py_name

    if sip_module_name:
        # Note that we don't use PyCapsule_Import() because it doesn't handle
        # package.module.attribute.

        sf.write(
f'''    /* Get the SIP module's API. */
    if ((sip_sipmod = PyImport_ImportModule("{sip_module_name}")) == SIP_NULLPTR)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}

    sip_capiobj = PyDict_GetItemString(PyModule_GetDict(sip_sipmod), "_C_API");
    Py_DECREF(sip_sipmod);

    if (sip_capiobj == SIP_NULLPTR || !PyCapsule_CheckExact(sip_capiobj))
    {{
        PyErr_SetString(PyExc_AttributeError, "{sip_module_name}._C_API is missing or has the wrong type");
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}

''')

        if spec.c_bindings:
            c_api = f'(const sipAPIDef *)PyCapsule_GetPointer(sip_capiobj, "{sip_module_name}._C_API")'
        else:
            c_api = f'reinterpret_cast<const sipAPIDef *>(PyCapsule_GetPointer(sip_capiobj, "{sip_module_name}._C_API"))'

        sf.write(
f'''    sipAPI_{module_name} = {c_api};

    if (sipAPI_{module_name} == SIP_NULLPTR)
    {{
        Py_DECREF(sipModule);
        return SIP_NULLPTR;
    }}

''')
    else:
        # If there is no sip module name then we are getting the API from a
        # non-shared sip module.
        sf.write(
f'''    if ((sipAPI_{module_name} = sip_init_library(sipModuleDict)) == SIP_NULLPTR)
        return SIP_NULLPTR;

''')


def _module_init_start(sf, spec, generate_c=True):
    """ Generate the start of the Python module initialisation function. """

    if spec.is_composite or spec.c_bindings:
        extern_c = ''
        arg_type = 'void'
    else:
        extern_c = 'extern "C" '
        arg_type = ''

    module_name = spec.module.py_name

    sf.write(
f'''

/* The Python module initialisation function. */
#if defined(SIP_STATIC_MODULE)
{extern_c}PyObject *PyInit_{module_name}({arg_type})
#else
PyMODINIT_FUNC PyInit_{module_name}({arg_type})
#endif
{{
''')


def _module_definition(sf, module, method_table='SIP_NULLPTR'):
    """ Generate the module definition structure. """

    if module.docstring is None:
        docstring_ref = 'SIP_NULLPTR'
    else:
        docstring_ref = f'doc_mod_{module.py_name}'

    sf.write(
f'''    static PyModuleDef sip_module_def = {{
        PyModuleDef_HEAD_INIT,
        "{module.fq_py_name}",
        {docstring_ref},
        -1,
        {method_table},
        SIP_NULLPTR,
        SIP_NULLPTR,
        SIP_NULLPTR,
        SIP_NULLPTR
    }};
''')


def _subclass_convertors(sf, spec, module):
    """ Generate all the sub-class convertors for a module and return the
    number of them.
    """

    nr_subclass_convertors = 0

    for klass in spec.classes:
        if klass.iface_file.module is not module:
            continue

        if klass.convert_to_subclass_code is None:
            continue

        sf.write(
'''

/* Convert to a sub-class if possible. */
''')

        klass_name = klass.iface_file.fq_cpp_name.as_word
        base_cpp = klass.subclass_base.iface_file.fq_cpp_name.as_cpp

        if not spec.c_bindings:
            sf.write(
f'extern "C" {{static const sipTypeDef *sipSubClass_{klass_name}(void **);}}\n')

        # Allow the deprecated use of sipClass rather than sipType.
        if _is_used_in_code(klass.convert_to_subclass_code, 'sipClass'):
            decl = 'sipWrapperType *sipClass'
            result = '(sipClass ? sipClass->wt_td : 0)'
        else:
            decl = 'const sipTypeDef *sipType'
            result = 'sipType'

        sf.write(
f'''static const sipTypeDef *sipSubClass_{klass_name}(void **sipCppRet)
{{
    {base_cpp} *sipCpp = reinterpret_cast<{base_cpp} *>(*sipCppRet);
    {decl};

''')

        sf.write_code(klass.convert_to_subclass_code)

        sf.write(
f'''
    return {result};
}}
''')

        nr_subclass_convertors += 1

    return nr_subclass_convertors


def _encoded_type(module, klass, last=False):
    """ Return the structure representing an encoded type. """

    klass_module = klass.iface_file.module

    fields = [str(klass.iface_file.type_nr)]

    if klass_module is module:
        fields.append('255')
    else:
        for module_nr, imported_module in enumerate(module.all_imports):
            if imported_module is klass_module:
                fields.append(str(module_nr))
                break

    fields.append(str(int(last)))

    return '{' + ', '.join(fields) + '}'


def _ordinary_function(sf, spec, bindings, member, scope=None):
    """ Generate an ordinary function. """

    member_name = member.py_name.name

    if scope is None:
        overloads = spec.module.overloads
        py_scope = None
        py_scope_prefix = ''
    else:
        overloads = scope.overloads
        py_scope = _py_scope(scope)
        py_scope_prefix = '' if py_scope is None else py_scope.iface_file.fq_cpp_name.as_word + '_'

    sf.write('\n\n')

    # Generate the docstrings.
    if _has_member_docstring(bindings, member, overloads):
        sf.write(f'PyDoc_STRVAR(doc_{py_scope_prefix}{member_name}, "')
        has_auto_docstring = _member_docstring(sf, spec, bindings, member,
                overloads)
        sf.write('");\n\n')
    else:
        has_auto_docstring = False

    if member.no_arg_parser or member.allow_keyword_args:
        kw_fw_decl = ', PyObject *'
        kw_decl = ', PyObject *sipKwds'
    else:
        kw_fw_decl = kw_decl = ''

    sip_self_unused = False

    if py_scope is None:
        if not spec.c_bindings:
            sf.write(f'extern "C" {{static PyObject *func_{member_name}(PyObject *, PyObject *{kw_fw_decl});}}\n')
            sip_self = ''
        else:
            sip_self = 'sipSelf'
            sip_self_unused = True;

        sf.write(f'static PyObject *func_{member_name}(PyObject *{sip_self}, PyObject *sipArgs{kw_decl})\n')
    else:
        if not spec.c_bindings:
            sf.write(f'extern "C" {{static PyObject *meth_{py_scope_prefix}{member_name}(PyObject *, PyObject *{kw_fw_decl});}}\n')

        sf.write(f'static PyObject *meth_{py_scope_prefix}{member_name}(PyObject *, PyObject *sipArgs{kw_decl})\n')

    sf.write('{\n')

    need_intro = True

    for overload in overloads:
        if overload.common is not member:
            continue

        if member.no_arg_parser:
            sf.write_code(overload.method_code)
            break

        if need_intro:
            sf.write('    PyObject *sipParseErr = SIP_NULLPTR;\n')

            if sip_self_unused:
                sf.write(
'''
    (void)sipSelf;
''')

            need_intro = False

        _function_body(sf, spec, bindings, scope, overload)

    if not need_intro:
        sf.write(
f'''
    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoFunction(sipParseErr, {_cached_name_ref(member.py_name)}, ''')

        if has_auto_docstring:
            sf.write(f'doc_{py_scope_prefix}{member_name}')
        else:
            sf.write('SIP_NULLPTR')

        sf.write(''');

    return SIP_NULLPTR;
''')

    sf.write('}\n')


def _enum_member_table(sf, spec, scope=None):
    """ Generate the table of enum members for a scope.  Return the number of
    them.
    """

    enum_members = []

    for enum in spec.enums:
        if enum.module is not spec.module:
            continue

        enum_py_scope = _py_scope(enum.scope)

        if isinstance(scope, WrappedClass):
            # The scope is a class.
            if enum_py_scope is not scope or (enum.is_protected and not scope.has_shadow):
                continue

        elif scope is not None:
            # The scope is a mapped type.
            if enum.scope != scope:
                continue

        elif enum_py_scope is not None or isinstance(enum.scope, MappedType) or enum.fq_cpp_name is None:
            continue

        enum_members.extend(enum.members)

    nr_members = len(enum_members)
    if nr_members == 0:
        return 0

    enum_members.sort(key=lambda v: v.scope.type_nr)
    enum_members.sort(key=lambda v: v.py_name.name)

    if _py_scope(scope) is None:
        sf.write(
'''
/* These are the enum members of all global enums. */
static sipEnumMemberDef enummembers[] = {
''')
    else:
        sf.write(
f'''
static sipEnumMemberDef enummembers_{scope.iface_file.fq_cpp_name.as_word}[] = {{
''')

    for enum_member in enum_members:
        sf.write(f'    {{{_cached_name_ref(enum_member.py_name)}, ')
        sf.write(_enum_member(spec, enum_member))
        sf.write(f', {enum_member.scope.type_nr}}},\n')

    sf.write('};\n')

    return nr_members


def _access_functions(sf, spec, scope=None):
    """ Generate the access functions for the variables. """

    for variable in _variables_in_scope(spec, scope, check_handler=False):
        if variable.access_code is None:
            continue

        cpp_name = variable.fq_cpp_name.as_word

        sf.write('\n\n/* Access function. */\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static void *access_{cpp_name}();}}\n')

        sf.write(f'static void *access_{cpp_name}()\n{{\n')
        sf.write_code(variable.access_code)
        sf.write('}\n')


# The types that are implemented as PyObject*.
_PY_OBJECT_TYPES = (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE,
    ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE,
    ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER,
    ArgumentType.PYENUM)

def _py_objects(sf, spec):
    """ Generate the inline code to add a set of Python objects to a module
    dictionary.
    """

    # Note that we should add these via a table (like int, etc) but that will
    # require a major API version change so this will do for now.

    no_intro = True

    for variable in spec.variables:
        if variable.module is not spec.module:
            continue

        if variable.type.type not in _PY_OBJECT_TYPES:
            continue

        if variable.needs_handler:
            continue

        if no_intro:
            sf.write('\n    /* Define the Python objects wrapped as such. */\n')
            no_intro = False

        py_name = _cached_name_ref(variable.py_name)
        cpp_name = variable.fq_cpp_name.as_cpp

        sf.write(f'    PyDict_SetItemString(sipModuleDict, {py_name}, {cpp_name});\n')


def _types_inline(sf, spec):
    """ Generate the inline code to add a set of generated type instances to a
    dictionary.
    """

    no_intro = True

    for variable in spec.variables:
        if variable.module is not spec.module:
            continue

        if variable.type.type not in (ArgumentType.CLASS, ArgumentType.MAPPED, ArgumentType.ENUM):
            continue

        if variable.needs_handler:
            continue

        # Skip classes that don't need inline code.
        if spec.c_bindings or variable.access_code is not None or len(variable.type.derefs) != 0:
            continue

        if no_intro:
            sf.write(
'''
    /*
     * Define the class, mapped type and enum instances that have to be
     * added inline.
     */
''')

            no_intro = False

        if _py_scope(variable.scope) is None:
            dict_name = 'sipModuleDict'
        else:
            dict_name = f'(PyObject *)sipTypeAsPyTypeObject({_gto_name(variable.scope)})'

        py_name = _cached_name_ref(variable.py_name)

        if variable.type.is_const:
            type_name = fmt_argument_as_cpp_type(spec, variable.type,
                    plain=True, no_derefs=True)
            ptr = f'const_cast<{type_name} *>(&{variable.fq_cpp_name.as_cpp})'
        else:
            ptr = '&' + variable.fq_cpp_name.as_cpp

        sf.write(f'    sipAddTypeInstance({dict_name}, {py_name}, {ptr}, {_gto_name(variable.type.definition)});\n')


def _class_instances(sf, spec, scope=None):
    """ Generate the code to add a set of class instances to a dictionary.
    Return True if there was at least one.
    """

    instances = []

    for variable in _variables_in_scope(spec, scope):
        if variable.type.type is not ArgumentType.CLASS and (variable.type.type is not ArgumentType.ENUM or variable.type.definition.fq_cpp_name is None):
            continue

        # Skip ordinary C++ class instances which need to be done with inline
        # code rather than through a static table.  This is because C++ does
        # not guarantee the order in which the table and the instance will be
        # created.  So far this has only been seen to be a problem when
        # statically linking SIP generated modules on Windows.
        if not spec.c_bindings and variable.access_code is None and len(variable.type.derefs) == 0:
            continue

        ti_name = _cached_name_ref(variable.py_name)
        ti_ptr = '&' + variable.fq_cpp_name.as_cpp
        ti_type = '&' + _gto_name(variable.type.definition)
        ti_flags = '0'

        if variable.type.type is ArgumentType.CLASS:
            if variable.access_code is not None:
                ti_ptr = '(void *)access_' + variable.fq_cpp_name.as_word
                ti_flags = 'SIP_ACCFUNC|SIP_NOT_IN_MAP'
            elif len(variable.type.derefs) != 0:
                # This may be a bit heavy handed.
                if variable.type.is_const:
                    ti_ptr = '(void *)' + ti_ptr

                ti_flags = 'SIP_INDIRECT'
            else:
                ti_ptr = _const_cast(spec, variable.type, ti_ptr)

        instances.append((ti_name, ti_ptr, ti_type, ti_flags))

    return _write_instances_table(sf, scope, instances,
'''/* Define the class and enum instances to be added to this {dict_type} dictionary. */
static sipTypeInstanceDef typeInstances{suffix}[]''')


def _void_pointer_instances(sf, spec, scope=None):
    """ Generate the code to add a set of void pointers to a dictionary.
    Return True if there was at least one.
    """

    instances = []

    for variable in _variables_in_scope(spec, scope):
        if variable.type.type not in (ArgumentType.VOID, ArgumentType.STRUCT, ArgumentType.UNION):
            continue

        vi_name = _cached_name_ref(variable.py_name)
        vi_val = _const_cast(spec, variable.type,
                variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL))
        instances.append((vi_name, vi_val))

    return _write_instances_table(sf, scope, instances,
'''/* Define the void pointers to be added to this {dict_type} dictionary. */
"static sipVoidPtrInstanceDef voidPtrInstances{suffix}[]''')


def _char_instances(sf, spec, scope=None):
    """ Generate the code to add a set of characters to a dictionary.  Return
    True if there was at least one.
    """

    instances = []

    for variable in _variables_in_scope(spec, scope):
        if variable.type.type not in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING) or len(variable.type.derefs) != 0:
            continue

        ci_name = _cached_name_ref(variable.py_name)
        ci_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
        ci_encoding = "'" + _get_encoding(variable.type) + "'"

        instances.append((ci_name, ci_val, ci_encoding))

    return _write_instances_table(sf, scope, instances,
'''/* Define the chars to be added to this {dict_type} dictionary. */
static sipCharInstanceDef charInstances{suffix}[]''')


def _string_instances(sf, spec, scope=None):
    """ Generate the code to add a set of strings to a dictionary.  Return True
    if there is at least one.
    """

    instances = []

    for variable in _variables_in_scope(spec, scope):
        if (variable.type.type not in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING) or len(variable.type.derefs) == 0) and variable.type.type is not ArgumentType.WSTRING:
            continue

        si_name = _cached_name_ref(variable.py_name)
        si_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)

        # This is the hack for handling wchar_t and wchar_t*.
        encoding = _get_encoding(variable.type)

        if encoding == 'w':
            si_val = '(const char *)&' + si_val
        elif encoding == 'W':
            si_val = '(const char *)' + si_val

        si_encoding = "'" + encoding + "'"

        instances.append((si_name, si_val, si_encoding))

    return _write_instances_table(sf, scope, instances,
'''/* Define the strings to be added to this {dict_type} dictionary. */
static sipStringInstanceDef stringInstances{suffix}[]''')


def _int_instances(sf, spec, scope=None):
    """ Generate the code to add a set of ints.  Return True if there was at
    least one.
    """

    instances = []

    if spec.abi_version >= (13, 0):
        # Named enum members are handled as int variables but must be placed at
        # the start of the table.  Note we use the sorted table of needed types
        # rather than the unsorted table of all enums.
        for type in spec.module.needed_types:
            if type.type is not ArgumentType.ENUM:
                continue

            enum = type.definition

            if _py_scope(enum.scope) is not scope or enum.module is not spec.module:
                continue

            for enum_member in enum.members:
                ii_name = _cached_name_ref(enum_member.py_name)
                ii_val = _enum_member(spec, enum_member)
                instances.append((ii_name, ii_val))

    # Handle int variables.
    for variable in _variables_in_scope(spec, scope):
        if variable.type.type not in (ArgumentType.ENUM, ArgumentType.BYTE, ArgumentType.SBYTE, ArgumentType.UBYTE, ArgumentType.USHORT, ArgumentType.SHORT, ArgumentType.CINT, ArgumentType.INT, ArgumentType.BOOL, ArgumentType.CBOOL):
            continue

        # Named enums are handled elsewhere.
        if variable.type.type is ArgumentType.ENUM and variable.type.definition.fq_cpp_name is not None:
            continue

        ii_name = _cached_name_ref(variable.py_name)
        ii_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
        instances.append((ii_name, ii_val))

    # Anonymous enum members are handled as int variables.
    if spec.abi_version >= (13, 0) or scope is None:
        for enum in spec.enums:
            if _py_scope(enum.scope) is not scope or enum.module is not spec.module:
                continue

            if enum.fq_cpp_name is not None:
                continue

            for enum_member in enum.members:
                ii_name = _cached_name_ref(enum_member.py_name)
                ii_val = _enum_member(spec, enum_member)
                instances.append((ii_name, ii_val))

    return _write_instances_table(sf, scope, instances,
'''/* Define the enum members and ints to be added to this {dict_type}. */
static sipIntInstanceDef intInstances{suffix}[]''')


def _long_instances(sf, spec, scope=None):
    """ Generate the code to add a set of longs to a dictionary.  Return True
    if there was at least one.
    """

    return _write_int_instances(sf, spec, scope, ArgumentType.LONG, 'long')


def _unsigned_long_instances(sf, spec, scope=None):
    """ Generate the code to add a set of unsigned longs to a dictionary.
    Return True if there was at least one.
    """

    return _write_int_instances(sf, spec, scope, ArgumentType.ULONG,
            'unsigned long')


def _long_long_instances(sf, spec, scope=None):
    """ Generate the code to add a set of long longs to a dictionary.  Return
    True if there was at least one.
    """

    return _write_int_instances(sf, spec, scope, ArgumentType.LONGLONG,
            'long long')


def _unsigned_long_long_instances(sf, spec, scope=None):
    """ Generate the code to add a set of unsigned long longs to a dictionary.
    Return True if there was at least one.
    """

    return _write_int_instances(sf, spec, scope, ArgumentType.ULONGLONG,
            'unsigned long long')


def _write_int_instances(sf, spec, scope, target_type, type_name):
    """ Generate the code to add a set of a particular type to a dictionary.
    Return True if there was at least one.
    """

    instances = []

    for variable in _variables_in_scope(spec, scope):
        variable_type = variable.type.type

        # We treat unsigned and size_t as unsigned long as we don't (currently
        # anyway) generate a separate table for them.
        if variable_type in (ArgumentType.UINT, ArgumentType.SIZE) and target_type is ArgumentType.ULONG:
            variable_type = ArgumentType.ULONG

        if variable_type is not target_type:
            continue

        ii_name = _cached_name_ref(variable.py_name)
        ii_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
        instances.append((ii_name, ii_val))

    table_type_name = type_name.title().replace(' ', '')
    table_name = table_type_name[0].lower() + table_type_name[1:]

    declaration_template = f'''/* Define the {type_name}s to be added to this {{dict_type}} dictionary. */
static sip{table_type_name}InstanceDef {table_name}Instances{{suffix}}[]'''

    return _write_instances_table(sf, scope, instances, declaration_template)


def _double_instances(sf, spec, scope=None):
    """ Generate the code to add a set of doubles to a dictionary.  Return True
    if there was at least one.
    """

    instances = []

    for variable in _variables_in_scope(spec, scope):
        if variable.type.type not in (ArgumentType.FLOAT, ArgumentType.CFLOAT, ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
            continue

        di_name = _cached_name_ref(variable.py_name)
        di_val = variable.fq_cpp_name.cpp_stripped(STRIP_GLOBAL)
        instances.append((di_name, di_val))

    return _write_instances_table(sf, scope, instances,
'''/* Define the doubles to be added to this {dict_type} dictionary. */
static sipDoubleInstanceDef doubleInstances{suffix}[]''')


def _empty_iface_file(spec, iface_file):
    """ See if an interface file has any content. """

    for klass in spec.classes:
        if klass.iface_file is iface_file and not klass.is_hidden_namespace and not klass.is_protected and not klass.external:
            return False

    for mapped_type in spec.mapped_types:
        if mapped_type.iface_file is iface_file:
            return False

    return True


def _iface_file_cpp(spec, bindings, project, buildable, py_debug, iface_file,
        need_postinc, source_suffix, sf):
    """ Generate the C/C++ code for an interface. """

    # Check that there will be something in the file so that we don't get
    # warning messages from ranlib.
    if _empty_iface_file(spec, iface_file):
        return

    if sf is None:
        source_name = os.path.join(buildable.build_dir,
                'sip' + iface_file.module.py_name)

        for part in iface_file.fq_cpp_name:
            source_name += part

        if iface_file.file_extension is not None:
            source_suffix = iface_file.file_extension

        source_name += source_suffix

        sf = CompilationUnit(source_name, "Interface wrapper code.",
                iface_file.module, project, buildable)

        need_postinc = True

    sf.write('\n')

    sf.write_code(iface_file.type_header_code)
    _used_includes(sf, iface_file.used)

    if need_postinc:
        sf.write_code(iface_file.module.unit_postinclude_code)

    for klass in spec.classes:
        # Protected classes must be generated in the interface file of the
        # enclosing scope.
        if klass.is_protected:
            continue

        if klass.external:
            continue

        if klass.iface_file is iface_file:
            _class_cpp(sf, spec, bindings, klass, py_debug)

            # Generate any enclosed protected classes.
            for proto_klass in spec.classes:
                if proto_klass.is_protected and proto_klass.scope is klass:
                    _class_cpp(sf, spec, bindings, proto_klass, py_debug)

    for mapped_type in spec.mapped_types:
        if mapped_type.iface_file is iface_file:
            _mapped_type_cpp(sf, spec, bindings, mapped_type)


def _mapped_type_cpp(sf, spec, bindings, mapped_type):
    """ Generate the C++ code for a mapped type version. """

    mapped_type_name = mapped_type.iface_file.fq_cpp_name.as_word
    mapped_type_type = fmt_argument_as_cpp_type(spec, mapped_type.type,
            plain=True, no_derefs=True)

    sf.write_code(mapped_type.type_code)

    if not mapped_type.no_release:
        # Generate the assignment helper.  Note that the source pointer is not
        # const.  This is to allow the source instance to be modified as a
        # consequence of the assignment, eg. if it is implementing some sort of
        # reference counting scheme.
        if not mapped_type.no_assignment_operator:
            sf.write('\n\n')

            if not spec.c_bindings:
                sf.write(f'extern "C" {{static void assign_{mapped_type_name}(void *, Py_ssize_t, void *);}}\n')

            sf.write(f'static void assign_{mapped_type_name}(void *sipDst, Py_ssize_t sipDstIdx, void *sipSrc)\n{{\n')

            if spec.c_bindings:
                sf.write(f'    (({mapped_type_type} *)sipDst)[sipDstIdx] = *(({mapped_type_type} *)sipSrc);\n')
            else:
                sf.write(f'    reinterpret_cast<{mapped_type_type} *>(sipDst)[sipDstIdx] = *reinterpret_cast<{mapped_type_type} *>(sipSrc);\n')

            sf.write('}\n')

        # Generate the array allocation helper.
        if not mapped_type.no_default_ctor:
            sf.write('\n\n')

            if not spec.c_bindings:
                sf.write(f'extern "C" {{static void *array_{mapped_type_name}(Py_ssize_t);}}\n')

            sf.write(f'static void *array_{mapped_type_name}(Py_ssize_t sipNrElem)\n{{\n')

            if spec.c_bindings:
                sf.write(f'    return sipMalloc(sizeof ({mapped_type_type}) * sipNrElem);\n')
            else:
                sf.write(f'    return new {mapped_type_type}[sipNrElem];\n')

            sf.write('}\n')

        # Generate the copy helper.
        if not mapped_type.no_copy_ctor:
            sf.write('\n\n')

            if not spec.c_bindings:
                sf.write(f'extern "C" {{static void *copy_{mapped_type_name}(const void *, Py_ssize_t);}}\n')

            sf.write(f'static void *copy_{mapped_type_name}(const void *sipSrc, Py_ssize_t sipSrcIdx)\n{{\n')

            if spec.c_bindings:
                sf.write(
f'''    {mapped_type_type} *sipPtr = sipMalloc(sizeof ({mapped_type_type}));
    *sipPtr = ((const {mapped_type_type} *)sipSrc)[sipSrcIdx];

    return sipPtr;
''')
            else:
                sf.write(f'    return new {mapped_type_type}(reinterpret_cast<const {mapped_type_type} *>(sipSrc)[sipSrcIdx]);\n')

            sf.write('}\n')

        sf.write('\n\n/* Call the mapped type\'s destructor. */\n')

        need_state = _is_used_in_code(mapped_type.release_code, 'sipState')

        if not spec.c_bindings:
            arg_3 = ', void *' if spec.abi_version >= (13, 0) else ''
            sf.write(f'extern "C" {{static void release_{mapped_type_name}(void *, int{arg_3});}}\n')

        arg_2 = ' sipState' if spec.c_bindings or need_state else ''
        sf.write(f'static void release_{mapped_type_name}(void *sipCppV, int{arg_2}')

        if spec.abi_version >= (13, 0):
            user_state = _use_in_code(mapped_type.release_code, 'sipUserState')
            sf.write(', void *' + user_state)

        sf.write(f')\n{{\n    {_mapped_type_from_void(spec, mapped_type_type)};\n')

        if bindings.release_gil:
            sf.write('    Py_BEGIN_ALLOW_THREADS\n')

        if mapped_type.release_code is not None:
            sf.write_code(mapped_type.release_code)
        elif spec.c_bindings:
            sf.write('    sipFree(sipCpp);\n')
        else:
            sf.write('    delete sipCpp;\n')

        if bindings.release_gil:
            sf.write('    Py_END_ALLOW_THREADS\n')

        sf.write('}\n\n')

    _convert_to_definitions(sf, spec, mapped_type)

    # Generate the from type convertor.
    if mapped_type.convert_from_type_code is not None:
        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static PyObject *convertFrom_{mapped_type_name}(void *, PyObject *);}}\n')

        xfer = _use_in_code(mapped_type.convert_from_type_code,
                'sipTransferObj', spec=spec)

        sf.write(
f'''static PyObject *convertFrom_{mapped_type_name}(void *sipCppV, PyObject *{xfer})
{{
    {_mapped_type_from_void(spec, mapped_type_type)};

''')

        sf.write_code(mapped_type.convert_from_type_code)

        sf.write('}\n')

    # Generate the static methods.
    for member in mapped_type.members:
        _ordinary_function(sf, spec, bindings, member, scope=mapped_type)

    cod_nrmethods = _mapped_type_method_table(sf, bindings, spec, mapped_type)

    id_int = 'SIP_NULLPTR'

    if spec.abi_version >= (13, 0):
        if _int_instances(sf, spec, scope=mapped_type):
            id_int = 'intInstances_' + mapped_type_name

        needs_namespace = False
    else:
        cod_nrenummembers = _enum_member_table(sf, spec, scope=mapped_type)
        has_ints = False
        needs_namespace = (cod_nrenummembers > 0)

    if cod_nrmethods > 0:
        needs_namespace = True

    if _pyqt6(spec) and mapped_type.pyqt_flags != 0:
        sf.write(f'\n\nstatic pyqt6MappedTypePluginDef plugin_{mapped_type_name} = {{{mapped_type.pyqt_flags}}};\n')

        td_plugin_data = '&plugin_' + mapped_type_name
    else:
        td_plugin_data = 'SIP_NULLPTR'

    sf.write(
f'''

sipMappedTypeDef sipTypeDef_{mapped_type.iface_file.module.py_name}_{mapped_type_name} = {{
    {{
''')

    if spec.abi_version < (13, 0):
        sf.write(
'''        -1,
        SIP_NULLPTR,
''')

    flags = []

    if mapped_type.handles_none:
        flags.append('SIP_TYPE_ALLOW_NONE')

    if mapped_type.needs_user_state:
        flags.append('SIP_TYPE_USER_STATE')

    flags.append('SIP_TYPE_MAPPED')

    td_flags = '|'.join(flags)

    td_cname = _cached_name_ref(mapped_type.cpp_name, as_nr=True)

    cod_name = _cached_name_ref(mapped_type.py_name, as_nr=True) if needs_namespace else '-1'
    cod_methods = 'SIP_NULLPTR' if cod_nrmethods == 0 else 'methods_' + mapped_type_name

    sf.write(
f'''        SIP_NULLPTR,
        {td_flags},
        {td_cname},
        SIP_NULLPTR,
        {td_plugin_data},
    }},
    {{
        {cod_name},
        {{0, 0, 1}},
        {cod_nrmethods}, {cod_methods},
''')

    if spec.abi_version < (13, 0):
        cod_enummembers = 'SIP_NULLPTR' if cod_nrenummembers == 0 else 'enummembers_' + mapped_type_name

        sf.write(
f'''        {cod_nrenummembers}, {cod_enummembers},
''')

    mtd_assign = 'SIP_NULLPTR' if mapped_type.no_assignment_operator else 'assign_' + mapped_type_name
    mtd_array = 'SIP_NULLPTR' if mapped_type.no_default_ctor else 'array_' + mapped_type_name
    mtd_copy = 'SIP_NULLPTR' if mapped_type.no_copy_ctor else 'copy_' + mapped_type_name
    mtd_release = 'SIP_NULLPTR' if mapped_type.no_release else 'release_' + mapped_type_name
    mtd_cto = 'SIP_NULLPTR' if mapped_type.convert_to_type_code is None else 'convertTo_' + mapped_type_name
    mtd_cfrom = 'SIP_NULLPTR' if mapped_type.convert_from_type_code is None else 'convertFrom_' + mapped_type_name

    sf.write(
f'''        0, SIP_NULLPTR,
        {{SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, {id_int}, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR}}
    }},
    {mtd_assign},
    {mtd_array},
    {mtd_copy},
    {mtd_release},
    {mtd_cto},
    {mtd_cfrom}
}};
''')


def _class_cpp(sf, spec, bindings, klass, py_debug):
    """ Generate the C++ code for a class. """

    sf.write_code(klass.type_code)
    _class_functions(sf, spec, bindings, klass, py_debug)
    _access_functions(sf, spec, scope=klass)

    if klass.iface_file.type is not IfaceFileType.NAMESPACE:
        _convert_to_definitions(sf, spec, klass)

        # Generate the optional from type convertor.
        if klass.convert_from_type_code is not None:
            sf.write('\n\n')

            name = klass.iface_file.fq_cpp_name.as_word
            xfer = _use_in_code(klass.convert_from_type_code, 'sipTransferObj',
                    spec=spec)

            if not spec.c_bindings:
                sf.write(f'extern "C" {{static PyObject *convertFrom_{name}(void *, PyObject *);}}\n')

            sf.write(
f'''static PyObject *convertFrom_{name}(void *sipCppV, PyObject *{xfer})
{{
    {_class_from_void(spec, klass)};

''')

            sf.write_code(klass.convert_from_type_code)

            sf.write('}\n')

    _type_definition(sf, spec, bindings, klass, py_debug)


def _get_function_table(members):
    """ Return a sorted list of relevant functions for a namespace. """

    return sorted(members, key=lambda m: m.py_name.name)


def _get_method_table(klass):
    """ Return a sorted list of relevant methods (either lazy or non-lazy) for
    a class.
    """

    # Only provide an entry point if there is at least one overload that is
    # defined in this class and is a non-abstract function or slot.  We allow
    # private (even though we don't actually generate code) because we need to
    # intercept the name before it reaches a more public version further up the
    # class hierarchy.  We add the ctor and any variable handlers as special
    # entries.

    members = []

    for visible_member in klass.visible_members:
        if visible_member.member.py_slot is not None:
            continue

        need_member = False

        for overload in visible_member.scope.overloads:
            # Skip protected methods if we don't have the means to handle them.
            if overload.access_specifier is AccessSpecifier.PROTECTED and not klass.has_shadow:
                continue

            if not _skip_overload(overload, visible_member.member, klass, visible_member.scope):
                need_member = True

        if need_member:
            members.append(visible_member.member)

    return _get_function_table(members)


def _mapped_type_method_table(sf, spec, bindings, mapped_type):
    """ Generate the sorted table of static methods for a mapped type and
    return the number of entries.
    """

    members = _get_function_table(mapped_type.members)

    return _py_method_table(sf, spec, bindings, members, mapped_type)


def _class_method_table(sf, spec, bindings, klass):
    """ Generate the sorted table of methods for a class and return the number
    of entries.
    """

    if klass.iface_file.type is IfaceFileType.NAMESPACE:
        members = _get_function_table(klass.members)
    else:
        members = _get_method_table(klass)

    return _py_method_table(sf, spec, bindings, members, klass)


def _py_method_table(sf, spec, bindings, members, scope):
    """ Generate a Python method table for a class or mapped type and return
    the number of entries.
    """

    scope_name = scope.iface_file.fq_cpp_name.as_word

    no_intro = True

    for member_nr, member in enumerate(members):
        # Save the index in the table.
        member.member_nr = member_nr

        py_name = member.py_name
        cached_py_name = _cached_name_ref(py_name)
        comma = '' if member is members[-1] else ','

        if member.no_arg_parser or member.allow_keyword_args:
            cast = 'SIP_MLMETH_CAST('
            cast_suffix = ')'
            flags = '|METH_KEYWORDS'
        else:
            cast = ''
            cast_suffix = ''
            flags = ''

        if _has_member_docstring(bindings, member, scope.overloads):
            docstring = f'doc_{scope_name}_{py_name.name}'
        else:
            docstring = 'SIP_NULLPTR'

        if no_intro:
            sf.write(
f'''

static PyMethodDef methods_{scope_name}[] = {{
''')

            no_intro = False

        sf.write(f'    {{{cached_py_name}, {cast}meth_{scope_name}_{py_name.name}{cast_suffix}, METH_VARARGS{flags}, {docstring}}}{comma}\n')

    if not no_intro:
        sf.write('};\n')

    return len(members)


def _convert_to_definitions(sf, spec, scope):
    """ Generate the "to type" convertor definitions. """

    convert_to_type_code = scope.convert_to_type_code

    if convert_to_type_code is None:
        return

    scope_type = Argument(
            ArgumentType.CLASS if isinstance(scope, WrappedClass) else ArgumentType.MAPPED,
            definition=scope)

    # Sometimes type convertors are just stubs that set the error flag, so
    # check if we actually need everything so that we can avoid compiler
    # warnings.
    sip_py = _use_in_code(convert_to_type_code, 'sipPy', spec=spec)
    sip_cpp_ptr = _use_in_code(convert_to_type_code, 'sipCppPtr', spec=spec)
    sip_is_err = _use_in_code(convert_to_type_code, 'sipIsErr', spec=spec)
    xfer = _use_in_code(convert_to_type_code, 'sipTransferObj', spec=spec)

    if spec.abi_version >= (13, 0):
        need_us_arg = True
        need_us_val = (spec.c_bindings or _type_needs_user_state(scope_type))
    else:
        need_us_arg = False
        need_us_val = False

    scope_name = scope.iface_file.fq_cpp_name.as_word

    sf.write('\n\n')

    if not spec.c_bindings:
        sf.write(f'extern "C" {{static int convertTo_{scope_name}(PyObject *, void **, int *, PyObject *')

        if need_us_arg:
            sf.write(', void **')

        sf.write(');}\n')

    sip_cpp_ptr_v = sip_cpp_ptr
    if sip_cpp_ptr_v != '':
        sip_cpp_ptr_v += 'V'

    sf.write(f'static int convertTo_{scope_name}(PyObject *{sip_py}, void **{sip_cpp_ptr_v}, int *{sip_is_err}, PyObject *{xfer}')

    if need_us_arg:
        sf.write(', void **')

        if need_us_val:
            sf.write('sipUserStatePtr')

    sf.write(')\n{\n')

    if sip_cpp_ptr != '':
        type_s = fmt_argument_as_cpp_type(spec, scope_type, plain=True,
                no_derefs=True)

        if spec.c_bindings:
            cast_value = f'({type_s} **)sipCppPtrV'
        else:
            cast_value = f'reinterpret_cast<{type_s} **>(sipCppPtrV)'

        sf.write(f'    {type_s} **sipCppPtr = {cast_value};\n\n')

    sf.write_code(convert_to_type_code)

    sf.write('}\n')


def _variable_getter(sf, spec, variable):
    """ Generate a variable getter. """

    variable_type = variable.type.type
    first_arg = 'sipSelf' if spec.c_bindings or not variable.is_static else ''
    last_arg = _use_in_code(variable.get_code, 'sipPyType', spec=spec)

    needs_new = (variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED) and len(variable.type.derefs) == 0 and variable.type.is_const)

    # If the variable is itself a non-const instance of a wrapped class then
    # two things must happen.  Firstly, the getter must return the same Python
    # object each time - it must not re-wrap the the instance.  This is because
    # the Python object can contain important state information that must not
    # be lost (specifically references to other Python objects that it owns).
    # Therefore the Python object wrapping the containing class must cache a
    # reference to the Python object wrapping the variable.  Secondly, the
    # Python object wrapping the containing class must not be garbage collected
    # before the Python object wrapping the variable is (because the latter
    # references memory, ie. the variable itself, that is managed by the
    # former).  Therefore the Python object wrapping the variable must keep a
    # reference to the Python object wrapping the containing class (but only if
    # the latter is non-static).
    var_key = self_key = 0

    if variable_type is ArgumentType.CLASS and len(variable.type.derefs) == 0 and not variable.type.is_const:
        var_key = variable.type.definition.iface_file.module.next_key
        variable.type.definition.iface_file.module.next_key -= 1

        if not variable.is_static:
            self_key = variable.module.next_key
            variable.module.next_key -= 1

    second_arg = 'sipPySelf' if spec.c_bindings or var_key < 0 else ''
    variable_as_word = variable.fq_cpp_name.as_word

    sf.write('\n\n')

    if not spec.c_bindings:
        sf.write(f'extern "C" {{static PyObject *varget_{variable_as_word}(void *, PyObject *, PyObject *);}}\n')

    sf.write(
f'''static PyObject *varget_{variable_as_word}(void *{first_arg}, PyObject *{second_arg}, PyObject *{last_arg})
{{
''')

    if variable.get_code is not None:
        sip_py_decl = 'PyObject *sipPy'
    elif var_key < 0:
        if variable.is_static:
            sip_py_decl = 'static PyObject *sipPy = SIP_NULLPTR'
        else:
            sip_py_decl = 'PyObject *sipPy'
    else:
        sip_py_decl = None

    if sip_py_decl is not None:
        sf.write('    ' + sip_py_decl + ';\n')

    if variable.get_code is None:
        value_decl = _get_named_value_decl(spec, variable.scope, variable.type,
                'sipVal')
        sf.write(f'    {value_decl};\n')

    if not variable.is_static:
        scope_s = _scoped_class_name(spec, variable.scope)

        if spec.c_bindings:
            sip_self = f'({scope_s} *)sipSelf'
        else:
            sip_self = f'reinterpret_cast<{scope_s} *>(sipSelf)'

        sf.write(f'    {scope_s} *sipCpp = {sip_self};\n')

    sf.write('\n')

    # Handle any handwritten getter.
    if variable.get_code is not None:
        sf.write_code(variable.get_code)

        sf.write(
'''
    return sipPy;
}
''')

        return

    # Get any previously wrapped cached object.
    if var_key < 0:
        if variable.is_static:
            sf.write(
'''    if (sipPy)
    {
        Py_INCREF(sipPy);
        return sipPy;
    }

''')
        else:
            sf.write(
f'''    sipPy = sipGetReference(sipPySelf, {self_key});

    if (sipPy)
        return sipPy;

''')

    variable_type_s = fmt_argument_as_cpp_type(spec, variable.type, plain=True,
            no_derefs=True)

    if needs_new:
        if spec.c_bindings:
            sf.write('    *sipVal = ')
        else:
            sf.write(f'    sipVal = new {variable_type_s}(')
    else:
        sf.write('    sipVal = ')

        if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED) and len(variable.type.derefs) == 0:
            sf.write('&')

    sf.write(_variable_member(variable))

    if needs_new and not spec.c_bindings:
        sf.write(')')

    sf.write(';\n\n')

    if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        prefix_s = 'sipPy =' if var_key < 0 else 'return'
        new_s = 'New' if needs_new else ''
        sip_val_s = _const_cast(spec, variable.type, 'sipVal')

        sf.write(f'    {prefix_s} sipConvertFrom{new_s}Type({sip_val_s}, {_gto_name(variable.type.definition)}, SIP_NULLPTR);\n')

        if var_key < 0:
            if variable.is_static:
                ref_code = 'Py_INCREF(sipPy)'
            else:
                ref_code = f'sipKeepReference(sipPySelf, {self_key}, sipPy)'

            sf.write(
f'''
    if (sipPy)
    {{
        sipKeepReference(sipPy, {var_key}, sipPySelf);
        {ref_code};
    }}

    return sipPy;
''')

    elif variable_type in (ArgumentType.BOOL, ArgumentType.CBOOL):
        sf.write('    return PyBool_FromLong(sipVal);\n')

    elif variable_type is ArgumentType.ASCII_STRING:
        if len(variable.type.derefs) == 0:
            sf.write('    return PyUnicode_DecodeASCII(&sipVal, 1, SIP_NULLPTR);\n')
        else:
            sf.write(
'''    if (sipVal == SIP_NULLPTR)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyUnicode_DecodeASCII(sipVal, strlen(sipVal), SIP_NULLPTR);
''')

    elif variable_type is ArgumentType.LATIN1_STRING:
        if len(variable.type.derefs) == 0:
            sf.write('    return PyUnicode_DecodeLatin1(&sipVal, 1, SIP_NULLPTR);\n')
        else:
            sf.write(
'''    if (sipVal == SIP_NULLPTR)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyUnicode_DecodeLatin1(sipVal, strlen(sipVal), SIP_NULLPTR);
''')

    elif variable_type is ArgumentType.UTF8_STRING:
        if len(variable.type.derefs) == 0:
            sf.write('    return PyUnicode_FromStringAndSize(&sipVal, 1);\n')
        else:
            sf.write(
'''    if (sipVal == SIP_NULLPTR)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyUnicode_FromString(sipVal);
''')

    elif variable_type in (ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING):
        cast_s = '' if variable_type is ArgumentType.STRING else '(char *)'

        if len(variable.type.derefs) == 0:
            sf.write(f'    return PyBytes_FromStringAndSize({cast_s}&sipVal, 1);\n')
        else:
            sf.write(
f'''    if (sipVal == SIP_NULLPTR)
    {{
        Py_INCREF(Py_None);
        return Py_None;
    }}

    return PyBytes_FromString({cast_s}sipVal);
''')

    elif variable_type is ArgumentType.WSTRING:
        if len(variable.type.derefs) == 0:
            sf.write('    return PyUnicode_FromWideChar(&sipVal, 1);\n')
        else:
            sf.write(
'''    if (sipVal == SIP_NULLPTR)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return PyUnicode_FromWideChar(sipVal, (Py_ssize_t)wcslen(sipVal));
''')

    elif variable_type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
        sf.write('    return PyFloat_FromDouble((double)sipVal);\n')

    elif variable_type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
        sf.write('    return PyFloat_FromDouble(sipVal);\n')

    elif variable_type is ArgumentType.ENUM:
        if variable.type.definition.fq_cpp_name is None:
            sf.write('    return PyLong_FromLong(sipVal);\n')
        else:
            sip_val_s = 'sipVal' if spec.c_bindings else 'static_cast<int>(sipVal)'
            sf.write(f'    return sipConvertFromEnum({sip_val_s}, {_gto_name(variable.type.definition)});\n')

    elif variable_type in (ArgumentType.BYTE, ArgumentType.SBYTE, ArgumentType.SHORT, ArgumentType.INT, ArgumentType.CINT):
        sf.write('    return PyLong_FromLong(sipVal);\n')

    elif variable_type is ArgumentType.LONG:
        sf.write('    return PyLong_FromLong(sipVal);\n')

    elif variable_type in (ArgumentType.UBYTE, ArgumentType.USHORT):
        sf.write('    return PyLong_FromUnsignedLong(sipVal);\n')

    elif variable_type in (ArgumentType.UINT, ArgumentType.ULONG, ArgumentType.SIZE):
        sf.write('    return PyLong_FromUnsignedLong(sipVal);\n')

    elif variable_type is ArgumentType.LONGLONG:
        sf.write('    return PyLong_FromLongLong(sipVal);\n')

    elif variable_type is ArgumentType.ULONGLONG:
        sf.write('    return PyLong_FromUnsignedLongLong(sipVal);\n')

    elif variable_type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
        const_s = 'Const' if variable.type.is_const else ''
        cast_s = _get_void_ptr_cast(variable.type)

        sf.write(f'    return sipConvertFrom{const_s}VoidPtr({cast_s}sipVal);\n')

    elif variable_type is ArgumentType.CAPSULE:
        cast_s = _get_void_ptr_cast(variable.type)

        sf.write(f'    return PyCapsule_New({cast_s}sipVal, "{variable.type.definition.as_cpp}", SIP_NULLPTR);\n')

    elif variable_type in (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE, ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER, ArgumentType.PYENUM):
        sf.write(
'''    Py_XINCREF(sipVal);
    return sipVal;
''')

    sf.write('}\n')


def _variable_setter(sf, spec, variable):
    """ Generate a variable setter. """

    variable_type = variable.type.type

    # We need to keep a reference to the original Python object if it is
    # providing the memory that the C/C++ variable is pointing to.
    keep = _keep_py_reference(variable.type)

    need_sip_cpp = (spec.c_bindings or variable.set_code is None or _is_used_in_code(variable.set_code, 'sipCpp'))

    first_arg = 'sipSelf' if spec.c_bindings or not variable.is_static else ''
    if not need_sip_cpp:
        first_arg = ''

    last_arg = 'sipPySelf' if spec.c_bindings or (not variable.is_static and keep) else ''

    sip_py = 'sipPy' if spec.c_bindings or variable.set_code is None or _is_used_in_code(variable.set_code, 'sipPy') else ''
    variable_as_word = variable.fq_cpp_name.as_word

    sf.write('\n\n')

    if not spec.c_bindings:
        sf.write(f'extern "C" {{static int varset_{variable_as_word}(void *, PyObject *, PyObject *);}}\n')

    sf.write(
f'''static int varset_{variable_as_word}(void *{first_arg}, PyObject *{sip_py}, PyObject *{last_arg})
{{
''')

    if variable.set_code is None:
        if variable_type is ArgumentType.BOOL:
            value_decl = 'int sipVal'
        else:
            value_decl = _get_named_value_decl(spec, variable.scope,
                    variable.type, 'sipVal')

        sf.write(f'    {value_decl};\n')

    if not variable.is_static and need_sip_cpp:
        scope_s = _scoped_class_name(spec, variable.scope)

        if spec.c_bindings:
            statement = f'({scope_s} *)sipSelf'
        else:
            statement = f'reinterpret_cast<{scope_s} *>(sipSelf)'

        sf.write(f'    {scope_s} *sipCpp = {statement};\n\n')

    # Handle any handwritten setter.
    if variable.set_code is not None:
        sf.write('   int sipErr = 0;\n\n')
        sf.write_code(variable.set_code)
        sf.write(
'''
    return (sipErr ? -1 : 0);
}
''')

        return

    has_state = False

    if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        sf.write('    int sipIsErr = 0;\n')

        if len(variable.type.derefs) == 0:
            convert_to_type_code = variable.type.definition.convert_to_type_code

            if variable_type is ArgumentType.MAPPED and variable.type.definition.no_release:
                convert_to_type_code = None

            if convert_to_type_code is not None:
                has_state = True

                sf.write('    int sipValState;\n')

                if _type_needs_user_state(variable.type):
                    sf.write('    void *sipValUserState;\n')

    sf.write(f'    sipVal = {_variable_to_cpp(spec, variable, has_state)};\n')

    deref = ''

    if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        if len(variable.type.derefs) == 0:
            deref = '*'

        error_test = 'sipIsErr'
    elif variable_type is ArgumentType.BOOL:
        error_test = 'sipVal < 0'
    else:
        error_test = 'PyErr_Occurred() != SIP_NULLPTR'

    sf.write(
f'''
    if ({error_test})
        return -1;

''')

    member = _variable_member(variable)

    if variable_type in (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE, ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER, ArgumentType.PYENUM):
        sf.write(
f'''    Py_XDECREF({member});
    Py_INCREF(sipVal);

''')

    value = deref + 'sipVal'

    if variable_type is ArgumentType.BOOL:
        if spec.c_bindings:
            value = '(bool)' + value
        else:
            value = f'static_cast<bool>({value})'

    sf.write(f'    {member} = {value};\n')

    # Note that wchar_t * leaks here.

    if has_state:
        suffix = _user_state_suffix(spec, variable.type)

        sf.write(
f'''
    sipReleaseType{suffix}(sipVal, {_gto_name(variable.type.definition)}, sipValState''')

        if _type_needs_user_state(variable.type):
            sf.write(', sipValUserState')

        sf.write(');\n')

    # Generate the code to keep the object alive while we use its data.
    if keep:
        if variable.is_static:
            sf.write(
'''
    static PyObject *sipKeep = SIP_NULLPTR;

    Py_XDECREF(sipKeep);
    sipKeep = sipPy;
    Py_INCREF(sipKeep);
''')
        else:
            key = variable.module.next_key
            variable.module.next_key -= 1

            sf.write(
f'''
    sipKeepReference(sipPySelf, {key}, sipPy);
''')

    sf.write(
'''
    return 0;
}
''')


def _variable_member(variable):
    """ Return the member variable of a class. """

    if variable.is_static:
        scope = variable.scope.iface_file.fq_cpp_name.as_cpp + '::'
    else:
        scope = 'sipCpp->'

    return scope + variable.fq_cpp_name.base_name


def _variable_to_cpp(spec, variable, has_state):
    """ Return the statement to convert a Python variable to C/C++. """

    type_s = fmt_argument_as_cpp_type(spec, variable.type, plain=True,
            no_derefs=True)

    variable_type = variable.type.type

    if variable_type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        if spec.c_bindings:
            statement = f'({type_s} *)'
            cast_tail = ''
        else:
            statement = f'reinterpret_cast<{type_s} *>('
            cast_tail = ')'

        # Note that we don't support /Transfer/ but could do.  We could also
        # support /Constrained/ (so long as we also supported it for all
        # types).

        suffix = _user_state_suffix(spec, variable.type)
        flags = '0' if len(variable.type.derefs) != 0 else 'SIP_NOT_NONE'
        state_ptr = '&sipValState' if has_state else 'SIP_NULLPTR'

        statement += f'sipForceConvertToType{suffix}(sipPy, {_gto_name(variable.type.definition)}, SIP_NULLPTR, {flags}, {state_ptr}'

        if _type_needs_user_state(variable.type):
            statement += ', &sipValUserState'

        statement += ', &sipIsErr)' + cast_tail

    elif variable_type is ArgumentType.ENUM:
        statement = f'({type_s})sipConvertToEnum(sipPy, {_gto_name(variable.type.definition)})'

    elif variable_type is ArgumentType.SSTRING:
        if len(variable.type.derefs) == 0:
            statement = '(signed char)sipBytes_AsChar(sipPy)'
        elif variable.type.is_const:
            statement = '(const signed char *)sipBytes_AsString(sipPy)'
        else:
            statement = '(signed char *)sipBytes_AsString(sipPy)'

    elif variable_type is ArgumentType.USTRING:
        if len(variable.type.derefs) == 0:
            statement = '(unsigned char)sipBytes_AsChar(sipPy)'
        elif variable.type.is_const:
            statement = '(const unsigned char *)sipBytes_AsString(sipPy)'
        else:
            statement = '(unsigned char *)sipBytes_AsString(sipPy)'

    elif variable_type is ArgumentType.ASCII_STRING:
        if len(variable.type.derefs) == 0:
            statement = 'sipString_AsASCIIChar(sipPy)'
        elif variable.type.is_const:
            statement = 'sipString_AsASCIIString(&sipPy)'
        else:
            statement = '(char *)sipString_AsASCIIString(&sipPy)'

    elif variable_type is ArgumentType.LATIN1_STRING:
        if len(variable.type.derefs) == 0:
            statement = 'sipString_AsLatin1Char(sipPy)'
        elif variable.type.is_const:
            statement = 'sipString_AsLatin1String(&sipPy)'
        else:
            statement = '(char *)sipString_AsLatin1String(&sipPy)'

    elif variable_type is ArgumentType.UTF8_STRING:
        if len(variable.type.derefs) == 0:
            statement = 'sipString_AsUTF8Char(sipPy)'
        elif variable.type.is_const:
            statement = 'sipString_AsUTF8String(&sipPy)'
        else:
            statement = '(char *)sipString_AsUTF8String(&sipPy)'

    elif variable_type is ArgumentType.STRING:
        if len(variable.type.derefs) == 0:
            statement = 'sipBytes_AsChar(sipPy)'
        elif variable.type.is_const:
            statement = 'sipBytes_AsString(sipPy)'
        else:
            statement = '(char *)sipBytes_AsString(sipPy)'

    elif variable_type is ArgumentType.WSTRING:
        if len(variable.type.derefs) == 0:
            statement = 'sipUnicode_AsWChar(sipPy)'
        else:
            statement = 'sipUnicode_AsWString(sipPy)'

    elif variable_type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
        statement = '(float)PyFloat_AsDouble(sipPy)'

    elif variable_type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
        statement = 'PyFloat_AsDouble(sipPy)'

    elif variable_type in (ArgumentType.BOOL, ArgumentType.CBOOL):
        statement = 'sipConvertToBool(sipPy)'

    elif variable_type is ArgumentType.BYTE:
        statement = 'sipLong_AsChar(sipPy)'

    elif variable_type is ArgumentType.SBYTE:
        statement = 'sipLong_AsSignedChar(sipPy)'

    elif variable_type is ArgumentType.UBYTE:
        statement = 'sipLong_AsUnsignedChar(sipPy)'

    elif variable_type is ArgumentType.USHORT:
        statement = 'sipLong_AsUnsignedShort(sipPy)'

    elif variable_type is ArgumentType.SHORT:
        statement = 'sipLong_AsShort(sipPy)'

    elif variable_type is ArgumentType.UINT:
        statement = 'sipLong_AsUnsignedInt(sipPy)'

    elif variable_type is ArgumentType.SIZE:
        statement = 'sipLong_AsSizeT(sipPy)'

    elif variable_type in (ArgumentType.INT, ArgumentType.CINT):
        statement = 'sipLong_AsInt(sipPy)'

    elif variable_type is ArgumentType.ULONG:
        statement = 'sipLong_AsUnsignedLong(sipPy)'

    elif variable_type is ArgumentType.LONG:
        statement = 'sipLong_AsLong(sipPy)'

    elif variable_type is ArgumentType.ULONGLONG:
        statement = 'sipLong_AsUnsignedLongLong(sipPy)'

    elif variable_type is ArgumentType.LONGLONG:
        statement = 'sipLong_AsLongLong(sipPy)'

    elif variable_type in (ArgumentType.STRUCT, ArgumentType.UNION):
        statement = f'({type_s} *)sipConvertToVoidPtr(sipPy)'

    elif variable_type is ArgumentType.VOID:
        statement = 'sipConvertToVoidPtr(sipPy)'

    elif variable_type is ArgumentType.CAPSULE:
        statement = f'PyCapsule_GetPointer(sipPy, "{variable.type.definition.as_cpp}")'

    else:
        # These are just the PyObject types.
        statement = 'sipPy'

    return statement


def _py_slot(sf, spec, bindings, member, scope=None):
    """ Generate a Python slot handler for either a class, an enum or an
    extender.
    """

    if scope is None:
        prefix = ''
        py_name = None
        fq_cpp_name = None
        overloads = spec.module.overloads
    elif isinstance(scope, WrappedEnum):
        prefix = 'Type'
        py_name = scope.py_name
        fq_cpp_name = scope.fq_cpp_name
        overloads = scope.overloads
    else:
        prefix = 'Type'
        py_name = scope.py_name
        fq_cpp_name = scope.iface_file.fq_cpp_name
        overloads = scope.overloads

    if is_void_return_slot(member.py_slot) or is_int_return_slot(member.py_slot):
        ret_type = 'int '
        ret_value = '-1'
    elif is_ssize_return_slot(member.py_slot):
        ret_type = 'Py_ssize_t '
        ret_value = '0'
    elif is_hash_return_slot(member.py_slot):
        if spec.abi_version >= (13, 0):
            ret_type = 'Py_hash_t '
            ret_value = '0'
        else:
            ret_type = 'long '
            ret_value = '0L'
    else:
        ret_type = 'PyObject *'
        ret_value = 'SIP_NULLPTR'

    has_args = True

    if is_int_arg_slot(member.py_slot):
        has_args = False
        arg_str = 'PyObject *sipSelf, int a0'
        decl_arg_str = 'PyObject *, int'
    elif member.py_slot is PySlot.CALL:
        if spec.c_bindings or member.allow_keyword_args or member.no_arg_parser:
            arg_str = 'PyObject *sipSelf, PyObject *sipArgs, PyObject *sipKwds'
        else:
            arg_str = 'PyObject *sipSelf, PyObject *sipArgs, PyObject *'

        decl_arg_str = 'PyObject *, PyObject *, PyObject *'
    elif is_multi_arg_slot(member.py_slot):
        arg_str = 'PyObject *sipSelf, PyObject *sipArgs'
        decl_arg_str = 'PyObject *, PyObject *'
    elif is_zero_arg_slot(member.py_slot):
        has_args = False
        arg_str = 'PyObject *sipSelf'
        decl_arg_str = 'PyObject *'
    elif is_number_slot(member.py_slot):
        arg_str = 'PyObject *sipArg0, PyObject *sipArg1'
        decl_arg_str = 'PyObject *, PyObject *'
    elif member.py_slot is PySlot.SETATTR:
        arg_str = 'PyObject *sipSelf, PyObject *sipName, PyObject *sipValue'
        decl_arg_str = 'PyObject *, PyObject *, PyObject *'
    else:
        arg_str = 'PyObject *sipSelf, PyObject *sipArg'
        decl_arg_str = 'PyObject *, PyObject *'

    sf.write('\n\n')

    slot_decl = f'static {ret_type}slot_'

    if fq_cpp_name is not None:
        slot_decl += fq_cpp_name.as_word + '_'

    if not spec.c_bindings:
        sf.write(f'extern "C" {{{slot_decl}{member.py_name.name}({decl_arg_str});}}\n')

    sf.write(f'{slot_decl}{member.py_name.name}({arg_str})\n{{\n')

    if member.py_slot is PySlot.CALL and member.no_arg_parser:
        for overload in overloads:
            if overload.common is member:
                sf.write_code(overload.method_code)
    else:
        if is_inplace_number_slot(member.py_slot):
            sf.write(
f'''    if (!PyObject_TypeCheck(sipSelf, sipTypeAsPyTypeObject(sip{prefix}_{fq_cpp_name.as_word})))
    {{
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }}

''')

        if not is_number_slot(member.py_slot):
            as_cpp = fq_cpp_name.as_cpp
            gto_name = _gto_name(scope)

            if isinstance(scope, WrappedClass):
                sf.write(
f'''    {as_cpp} *sipCpp = reinterpret_cast<{as_cpp} *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf, {gto_name}));

    if (!sipCpp)
''')
            else:
                sf.write(
f'''    {as_cpp} sipCpp = static_cast<{as_cpp}>(sipConvertToEnum(sipSelf, {gto_name}));

    if (PyErr_Occurred())
''')

            sf.write(f'        return {ret_value};\n\n')

        if has_args:
            sf.write('    PyObject *sipParseErr = SIP_NULLPTR;\n')

        for overload in overloads:
            if overload.common is member and overload.is_abstract:
                sf.write('    PyObject *sipOrigSelf = sipSelf;\n')
                break

        scope_not_enum = not isinstance(scope, WrappedEnum)

        for overload in overloads:
            if overload.common is member:
                dereferenced = scope_not_enum and not overload.dont_deref_self

                _function_body(sf, spec, bindings, scope, overload,
                        dereferenced=dereferenced)

        if has_args:
            if member.py_slot in (PySlot.CONCAT, PySlot.ICONCAT, PySlot.REPEAT, PySlot.IREPEAT):
                sf.write(
f'''
    /* Raise an exception if the argument couldn't be parsed. */
    sipBadOperatorArg(sipSelf, sipArg, {_get_slot_name(member.py_slot)});

    return SIP_NULLPTR;
''')

            else:
                if is_rich_compare_slot(member.py_slot):
                    sf.write(
'''
    Py_XDECREF(sipParseErr);
''')
                elif is_number_slot(member.py_slot) or is_inplace_number_slot(member.py_slot):
                    sf.write(
'''
    Py_XDECREF(sipParseErr);

    if (sipParseErr == Py_None)
        return SIP_NULLPTR;
''')

                if is_number_slot(member.py_slot) or is_rich_compare_slot(member.py_slot):
                    # We can only extend class slots. */
                    if not isinstance(scope, WrappedClass):
                        sf.write(
'''
    PyErr_Clear();

    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
''')
                    elif is_number_slot(member.py_slot):
                        sf.write(
f'''
    return sipPySlotExtend(&sipModuleAPI_{spec.module.py_name}, {_get_slot_name(member.py_slot)}, SIP_NULLPTR, sipArg0, sipArg1);
''')
                    else:
                        sf.write(
f'''
    return sipPySlotExtend(&sipModuleAPI_{spec.module.py_name}, {_get_slot_name(member.py_slot)}, {_gto_name(scope)}, sipSelf, sipArg);
''')
                elif is_inplace_number_slot(member.py_slot):
                    sf.write(
'''
    PyErr_Clear();

    Py_INCREF(Py_NotImplemented);
    return Py_NotImplemented;
''')
                else:
                    member_name = '(sipValue != SIP_NULLPTR ? sipName___setattr__ : sipName___delattr__)' if member.py_slot is PySlot.SETATTR else _cached_name_ref(member.py_name)

                    sf.write(
f'''
    sipNoMethod(sipParseErr, {_cached_name_ref(py_name)}, {member_name}, SIP_NULLPTR);

    return {ret_value};
''')
        else:
            sf.write(
'''
    return 0;
''')

    sf.write('}\n')


def _class_functions(sf, spec, bindings, klass, py_debug):
    """ Generate the member functions for a class. """

    as_word = klass.iface_file.fq_cpp_name.as_word
    scope_s = _scoped_class_name(spec, klass)

    # Any shadow code.
    if klass.has_shadow:
        if not klass.export_derived:
            _shadow_class_declaration(sf, spec, bindings, klass)

        _shadow_code(sf, spec, bindings, klass)

    # The member functions.
    for visible_member in klass.visible_members:
        if visible_member.member.py_slot is None:
            _member_function(sf, spec, bindings, klass, visible_member.member,
                    visible_member.scope)

    # The slot functions.
    for member in klass.members:
        if klass.iface_file.type is IfaceFileType.NAMESPACE:
            _ordinary_function(sf, spec, bindings, member, scope=klass)
        elif member.py_slot is not None:
            _py_slot(sf, spec, bindings, member, scope=klass)

    # The cast function.
    if len(klass.superclasses) != 0:
        sf.write(
f'''

/* Cast a pointer to a type somewhere in its inheritance hierarchy. */
extern "C" {{static void *cast_{as_word}(void *, const sipTypeDef *);}}
static void *cast_{as_word}(void *sipCppV, const sipTypeDef *targetType)
{{
    {_class_from_void(spec, klass)};

    if (targetType == {_gto_name(klass)})
        return sipCppV;

''')

        for superclass in klass.superclasses:
            sc_fq_cpp_name = superclass.iface_file.fq_cpp_name
            sc_scope_s = _scoped_class_name(spec, superclass)
            sc_gto_name = _gto_name(superclass)

            if len(superclass.superclasses) != 0:
                # Delegate to the super-class's cast function.  This will
                # handle virtual and non-virtual diamonds.
                sf.write(
f'''    sipCppV = ((const sipClassTypeDef *){sc_gto_name})->ctd_cast(static_cast<{sc_scope_s} *>(sipCpp), targetType);
    if (sipCppV)
        return sipCppV;

''')
            else:
                # The super-class is a base class and so doesn't have a cast
                # function.  It also means that a simple check will do instead.
                sf.write(
f'''    if (targetType == {sc_gto_name})
        return static_cast<{sc_scope_s} *>(sipCpp);

''')

        sf.write(
'''    return SIP_NULLPTR;
}
''')

    if klass.iface_file.type is not IfaceFileType.NAMESPACE and not spec.c_bindings:
        # Generate the release function without compiler warnings.
        need_state = False
        need_ptr = need_cast_ptr = _is_used_in_code(klass.dealloc_code,
                'sipCpp')

        public_dtor = klass.dtor is AccessSpecifier.PUBLIC

        if klass.can_create or public_dtor:
            if (_pyqt5(spec) or _pyqt6(spec)) and klass.is_qobject and public_dtor:
                need_ptr = need_cast_ptr = True
            elif klass.has_shadow:
                need_ptr = need_state = True
            elif public_dtor:
                need_ptr = True

        sf.write('\n\n/* Call the instance\'s destructor. */\n')

        sip_cpp_v = 'sipCppV' if spec.c_bindings or need_ptr else ''
        sip_state = ' sipState' if spec.c_bindings or need_state else ''

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static void release_{as_word}(void *, int);}}\n')

        sf.write(f'static void release_{as_word}(void *{sip_cpp_v}, int{sip_state})\n{{\n')

        if need_cast_ptr:
            sf.write(f'    {_class_from_void(spec, klass)};\n\n')

        if len(klass.dealloc_code) != 0:
            sf.write_code(klass.dealloc_code)
            sf.write('\n')

        if klass.can_create or public_dtor:
            release_gil = _release_gil(klass.dtor_gil_action, bindings)

            # If there is an explicit public dtor then assume there is some way
            # to call it which we haven't worked out (because we don't fully
            # understand C++).

            if release_gil:
                sf.write('    Py_BEGIN_ALLOW_THREADS\n\n')

            if (_pyqt5(spec) or _pyqt6(spec)) and klass.is_qobject and public_dtor:
                # QObjects should only be deleted in the threads that they
                # belong to.
                sf.write(
'''    if (QThread::currentThread() == sipCpp->thread())
        delete sipCpp;
    else
        sipCpp->deleteLater();
''')
            elif klass.has_shadow:
                sf.write(
f'''    if (sipState & SIP_DERIVED_CLASS)
        delete reinterpret_cast<sip{as_word} *>(sipCppV);
''')

                if public_dtor:
                    sf.write(
f'''    else
        delete reinterpret_cast<{_scoped_class_name(spec, klass)} *>(sipCppV);
''')
            elif public_dtor:
                sf.write(
f'''    delete reinterpret_cast<{_scoped_class_name(spec, klass)} *>(sipCppV);
''')

            if release_gil:
                sf.write('\n    Py_END_ALLOW_THREADS\n')

        sf.write('}\n')

    # The traverse function.
    if klass.gc_traverse_code is not None:
        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static int traverse_{as_word}(void *, visitproc, void *);}}\n')

        sf.write(
f'''static int traverse_{as_word}(void *sipCppV, visitproc sipVisit, void *sipArg)
{{
    {_class_from_void(spec, klass)};
    int sipRes;

''')

        sf.write_code(klass.gc_traverse_code)

        sf.write(
'''
    return sipRes;
}
''')

    # The clear function.
    if klass.gc_clear_code is not None:
        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static int clear_{as_word}(void *);}}\n')

        sf.write(
f'''static int clear_{as_word}(void *sipCppV)
{{
    {_class_from_void(spec, klass)};
    int sipRes;

''')

        sf.write_code(klass.gc_clear_code)

        sf.write(
'''
    return sipRes;
}
''')

    # The buffer interface functions.
    if klass.bi_get_buffer_code is not None:
        code = klass.bi_get_buffer_code

        need_cpp = _is_used_in_code(code, 'sipCpp')
        sip_self = _arg_name(spec, 'sipSelf', code)
        sip_cpp_v = 'sipCppV' if spec.c_bindings or need_cpp else ''

        sf.write('\n\n')

        if not py_debug and spec.module.use_limited_api:
            if not spec.c_bindings:
                sf.write(f'extern "C" {{static int getbuffer_{as_word}(PyObject *, void *, sipBufferDef *);}}\n')

            sf.write(f'static int getbuffer_{as_word}(PyObject *{sip_self}, void *{sip_cpp_v}, sipBufferDef *sipBuffer)\n')
        else:
            if not spec.c_bindings:
                sf.write(f'extern "C" {{static int getbuffer_{as_word}(PyObject *, void *, Py_buffer *, int);}}\n')

            sip_flags = _arg_name(spec, 'sipFlags', code)

            sf.write(f'static int getbuffer_{as_word}(PyObject *{sip_self}, void *{sip_cpp_v}, Py_buffer *sipBuffer, int {sip_flags})\n')

        sf.write('{\n')

        if need_cpp:
            sf.write(f'    {_class_from_void(spec, klass)};\n')

        sf.write('    int sipRes;\n\n')
        sf.write_code(code)
        sf.write('\n    return sipRes;\n}\n')

    if klass.bi_release_buffer_code is not None:
        code = klass.bi_release_buffer_code

        need_cpp = _is_used_in_code(code, 'sipCpp')
        sip_self = _arg_name(spec, 'sipSelf', code)
        sip_cpp_v = 'sipCppV' if spec.c_bindings or need_cpp else ''

        sf.write('\n\n')

        if not py_debug and spec.module.use_limited_api:
            if not spec.c_bindings:
                sf.write(f'extern "C" {{static void releasebuffer_{as_word}(PyObject *, void *);}}\n')

            sf.write(f'static void releasebuffer_{as_word}(PyObject *{sip_self}, void *{sip_cpp_v})\n')
        else:
            if not spec.c_bindings:
                sf.write(f'extern "C" {{static void releasebuffer_{as_word}(PyObject *, void *, Py_buffer *);}}\n')

            sip_buffer = _arg_name(spec, 'sipBuffer', code)

            sf.write(f'static void releasebuffer_{as_word}(PyObject *{sip_self}, void *{sip_cpp_v}, Py_buffer *{sip_buffer})\n')

        sf.write('{\n')

        if need_cpp:
            sf.write(f'    {_class_from_void(spec, klass)};\n')

        sf.write_code(code)
        sf.write('}\n')

    # The pickle function.
    if klass.pickle_code is not None:
        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static PyObject *pickle_{as_word}(void *);}}\n')

        sf.write(
f'''static PyObject *pickle_{as_word}(void *sipCppV)
{{
    {_class_from_void(spec, klass)};
    PyObject *sipRes;

''')

        sf.write_code(klass.pickle_code)

        sf.write('\n    return sipRes;\n}\n')

    # The finalisation function.
    if klass.finalisation_code is not None:
        code = klass.finalisation_code

        need_cpp = _is_used_in_code(code, 'sipCpp')
        sip_self = _arg_name(spec, 'sipSelf', code)
        sip_cpp_v = 'sipCppV' if spec.c_bindings or need_cpp else ''
        sip_kwds = _arg_name(spec, 'sipKwds', code)
        sip_unused = _arg_name(spec, 'sipUnused', code)

        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static int final_{as_word}(PyObject *, void *, PyObject *, PyObject **);}}\n')

        sf.write(
f'''static int final_{as_word}(PyObject *{sip_self}, void *{sip_cpp_v}, PyObject *{sip_kwds}, PyObject **{sip_unused})
{{
''')

        if need_cpp:
            sf.write(f'    {_class_from_void(spec, klass)};\n\n')

        sf.write_code(code)

        sf.write('}\n')

    # The mixin initialisation function.
    if klass.mixin:
        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static int mixin_{as_word}(PyObject *, PyObject *, PyObject *);}}\n')

        sf.write(
f'''static int mixin_{as_word}(PyObject *sipSelf, PyObject *sipArgs, PyObject *sipKwds)
{{
    return sipInitMixin(sipSelf, sipArgs, sipKwds, (sipClassTypeDef *)&sipTypeDef_{spec.module.py_name}_{as_word});
}}
''')

    # The array allocation helpers.
    if spec.c_bindings or klass.needs_array_helper:
        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static void *array_{as_word}(Py_ssize_t);}}\n')

        sf.write(f'static void *array_{as_word}(Py_ssize_t sipNrElem)\n{{\n')

        if spec.c_bindings:
            sf.write(f'    return sipMalloc(sizeof ({scope_s}) * sipNrElem);\n')
        else:
            sf.write(f'    return new {scope_s}[sipNrElem];\n')

        sf.write('}\n')

        if _abi_supports_array(spec):
            sf.write('\n\n')

            if not spec.c_bindings:
                sf.write(f'extern "C" {{static void array_delete_{as_word}(void *);}}\n')

            sf.write(f'static void array_delete_{as_word}(void *sipCpp)\n{{\n')

            if spec.c_bindings:
                sf.write('    sipFree(sipCpp);\n')
            else:
                sf.write(f'    delete[] reinterpret_cast<{scope_s} *>(sipCpp);\n')

            sf.write('}\n')

    # The copy and assignment helpers.
    if spec.c_bindings or klass.needs_copy_helper:
        # The assignment helper.  We assume that there will be a valid
        # assigment operator if there is a a copy ctor.  Note that the source
        # pointer is not const.  This is to allow the source instance to be
        # modified as a consequence of the assignment, eg. if it is
        # implementing some sort of reference counting scheme.

        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static void assign_{as_word}(void *, Py_ssize_t, void *);}}\n')

        sf.write(f'static void assign_{as_word}(void *sipDst, Py_ssize_t sipDstIdx, void *sipSrc)\n{{\n')

        if spec.c_bindings:
            sf.write(f'    (({scope_s} *)sipDst)[sipDstIdx] = *(({scope_s} *)sipSrc);\n')
        else:
            sf.write(f'    reinterpret_cast<{scope_s} *>(sipDst)[sipDstIdx] = *reinterpret_cast<{scope_s} *>(sipSrc);\n')

        sf.write('}\n')

        # The copy helper.
        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static void *copy_{as_word}(const void *, Py_ssize_t);}}\n')

        sf.write(f'static void *copy_{as_word}(const void *sipSrc, Py_ssize_t sipSrcIdx)\n{{\n')

        if spec.c_bindings:
            sf.write(
f'''    {scope_s} *sipPtr = sipMalloc(sizeof ({scope_s}));
    *sipPtr = ((const {scope_s} *)sipSrc)[sipSrcIdx];

    return sipPtr;
''')
        else:
            sf.write(
f'''    return new {scope_s}(reinterpret_cast<const {scope_s} *>(sipSrc)[sipSrcIdx]);
''')

        sf.write('}\n')

    # The dealloc function.
    if _need_dealloc(spec, bindings, klass):
        sf.write('\n\n')

        if not spec.c_bindings:
            sf.write(f'extern "C" {{static void dealloc_{as_word}(sipSimpleWrapper *);}}\n')

        sf.write(f'static void dealloc_{as_word}(sipSimpleWrapper *sipSelf)\n{{\n')

        if bindings.tracing:
            sf.write(f'    sipTrace(SIP_TRACE_DEALLOCS, "dealloc_{as_word}()\\n");\n\n')

        # Disable the virtual handlers.
        if klass.has_shadow:
            sf.write(
f'''    if (sipIsDerivedClass(sipSelf))
        reinterpret_cast<sip{as_word} *>(sipGetAddress(sipSelf))->sipPySelf = SIP_NULLPTR;

''')

        if spec.c_bindings or klass.dtor is AccessSpecifier.PUBLIC or (klass.has_shadow and klass.dtor is AccessSpecifier.PROTECTED):
            sf.write('    if (sipIsOwnedByPython(sipSelf))\n    {\n')

            if klass.delay_dtor:
                sf.write('        sipAddDelayedDtor(sipSelf);\n')
            elif spec.c_bindings:
                if klass.dealloc_code:
                    sf.write(klass.dealloc_code)

                sf.write('        sipFree(sipGetAddress(sipSelf));\n')
            else:
                flag = 'sipIsDerivedClass(sipSelf)' if klass.has_shadow else '0'
                sf.write(f'        release_{as_word}(sipGetAddress(sipSelf), {flag});\n')

            sf.write('    }\n')

        sf.write('}\n')

    # The type initialisation function.
    if klass.can_create:
        _type_init(sf, spec, bindings, klass)


def _shadow_code(sf, spec, bindings, klass):
    """ Generate the shadow (derived) class code. """

    klass_name = klass.iface_file.fq_cpp_name.as_word
    klass_cpp_name = klass.iface_file.fq_cpp_name.as_cpp

    # Generate the wrapper class constructors.
    nr_virtuals = _count_virtual_overloads(spec, klass)

    for ctor in _unique_class_ctors(spec, klass):
        throw_specifier = _throw_specifier(bindings, ctor.throw_args)
        protected_call_args = _protected_call_args(spec, ctor.cpp_signature)
        args = fmt_signature_as_cpp_definition(spec, ctor.cpp_signature,
                scope=klass.iface_file)

        sf.write(f'\nsip{klass_name}::sip{klass_name}({args}){throw_specifier}: {_scoped_class_name(spec, klass)}({protected_call_args}), sipPySelf(SIP_NULLPTR)\n{{\n')

        if bindings.tracing:
            args = fmt_signature_as_cpp_declaration(spec, ctor.cpp_signature,
                    scope=klass.iface_file)

            sf.write(f'    sipTrace(SIP_TRACE_CTORS, "sip{klass_name}::sip{klass_name}({args}){throw_specifier} (this=0x%08x)\\n", this);\n\n')

        if nr_virtuals > 0:
            sf.write('    memset(sipPyMethods, 0, sizeof (sipPyMethods));\n')

        sf.write('}\n')

    # The destructor.
    if klass.dtor is not AccessSpecifier.PRIVATE:
        throw_specifier = _throw_specifier(bindings, klass.dtor_throw_args)

        sf.write(f'\nsip{klass_name}::~sip{klass_name}(){throw_specifier}\n{{\n')

        if bindings.tracing:
            sf.write(f'    sipTrace(SIP_TRACE_DTORS, "sip{klass_name}::~sip{klass_name}(){throw_specifier} (this=0x%08x)\\n", this);\n\n')

        if klass.dtor_virtual_catcher_code is not None:
            sf.write_code(klass.dtor_virtual_catcher_code)

        sf.write('    sipInstanceDestroyedEx(&sipPySelf);\n}\n')

    # The meta methods if required.
    if (_pyqt5(spec) or _pyqt6(spec)) and klass.is_qobject:
        module_name = spec.module.py_name
        gto_name = _gto_name(klass)

        if not klass.pyqt_no_qmetaobject:
            sf.write(
f'''
const QMetaObject *sip{klass_name}::metaObject() const
{{
    if (sipGetInterpreter())
        return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : sip_{module_name}_qt_metaobject(sipPySelf, {gto_name});

    return {klass_cpp_name}::metaObject();
}}
''')

        sf.write(
f'''
int sip{klass_name}::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{{
    _id = {klass_cpp_name}::qt_metacall(_c, _id, _a);

    if (_id >= 0)
    {{
        SIP_BLOCK_THREADS
        _id = sip_{module_name}_qt_metacall(sipPySelf, {gto_name}, _c, _id, _a);
        SIP_UNBLOCK_THREADS
    }}

    return _id;
}}

void *sip{klass_name}::qt_metacast(const char *_clname)
{{
    void *sipCpp;

    return (sip_{module_name}_qt_metacast(sipPySelf, {gto_name}, _clname, &sipCpp) ? sipCpp : {klass_cpp_name}::qt_metacast(_clname));
}}
''')

    # Generate the virtual catchers.
    for virt_nr, virtual_overload in enumerate(_unique_class_virtual_overloads(spec, klass)):
        _virtual_catcher(sf, spec, bindings, klass, virtual_overload, virt_nr)

    # Generate the wrapper around each protected member function.
    _protected_definitions(sf, spec, klass)


def _protected_enums(sf, spec, klass):
    """ Generate the protected enums for a class. """

    for enum in spec.enums:
        if not enum.is_protected:
            continue

        # See if the class defining the enum is in our class hierachy.
        for mro_klass in klass.mro:
            if mro_klass is enum.scope:
                break
        else:
            continue

        sf.write(
'''
    /* Expose this protected enum. */
    enum''')

        if enum.fq_cpp_name is not None:
            sf.write(' sip' + enum.fq_cpp_name.base_name)

        sf.write(' {')

        eol = '\n'
        scope_cpp_name = enum.scope.iface_file.fq_cpp_name.as_cpp

        for member in enum.members:
            member_cpp_name = member.cpp_name

            sf.write(f'{eol}        {member_cpp_name} = {scope_cpp_name}::{member_cpp_name}')

            eol = ',\n'

        sf.write('\n    };\n')


def _virtual_catcher(sf, spec, bindings, klass, virtual_overload, virt_nr):
    """ Generate the catcher for a virtual function. """

    overload = virtual_overload.overload
    result = overload.cpp_signature.result

    result_type = fmt_argument_as_cpp_type(spec, result,
            scope=klass.iface_file, make_public=True)
    klass_name = klass.iface_file.fq_cpp_name.as_word
    overload_cpp_name = _overload_cpp_name(overload)
    throw_specifier = _throw_specifier(bindings, overload.throw_args)
    const = ' const' if overload.is_const else ''

    protection_state = set()
    _remove_protections(overload.cpp_signature, protection_state)

    args = fmt_signature_as_cpp_definition(spec, overload.cpp_signature,
            scope=klass.iface_file)
    sf.write(f'\n{result_type} sip{klass_name}::{overload_cpp_name}({args}){const}{throw_specifier}\n{{\n')

    if bindings.tracing:
        args = fmt_signature_as_cpp_declaration(spec, overload.cpp_signature,
                scope=klass.iface_file)
        sf.write(f'    sipTrace(SIP_TRACE_CATCHERS, "{result_type} sip{klass_name}::{overload_cpp_name}({args}){const}{throw_specifier} (this=0x%08x)\\n", this);\n\n')

    _restore_protections(protection_state)

    sf.write(
'''    sip_gilstate_t sipGILState;
    PyObject *sipMeth;

''')

    if overload.is_const:
        const_cast_char = 'const_cast<char *>('
        const_cast_sw = 'const_cast<sipSimpleWrapper **>('
        const_cast_tail = ')'
    else:
        const_cast_char = ''
        const_cast_sw = ''
        const_cast_tail = ''

    abi_12_8_arg = f'{const_cast_sw}&sipPySelf{const_cast_tail}, ' if spec.abi_version >= (12, 8) else ''

    klass_py_name_ref = _cached_name_ref(klass.py_name) if overload.is_abstract else 'SIP_NULLPTR'
    member_py_name_ref = _cached_name_ref(overload.common.py_name)

    sf.write(f'    sipMeth = sipIsPyMethod(&sipGILState, {const_cast_char}&sipPyMethods[{virt_nr}]{const_cast_tail}, {abi_12_8_arg}{klass_py_name_ref}, {member_py_name_ref});\n')

    # The rest of the common code.

    if result is not None and result.type is ArgumentType.VOID and len(result.derefs) == 0:
        result = None

    sf.write('\n    if (!sipMeth)\n')

    if overload.virtual_call_code is not None:
        sf.write('    {\n')

        if result is not None:
            sip_res = fmt_argument_as_cpp_type(spec, result, name='sipRes',
                    scope=klass.iface_file)
            sf.write(f'        {sip_res};\n')

        sf.write('\n')

        sf.write_code(overload.virtual_call_code)

        sip_res = ' sipRes' if result is not None else ''
        sf.write(
f'''
        return{sip_res};
    }}
''')

    elif overload.is_abstract:
        _default_instance_return(sf, spec, result)

    else:
        if result is None:
            sf.write('    {\n        ')
        else:
            sf.write('        return ')

        args = []
        for arg_nr, arg in enumerate(overload.cpp_signature.args):
            args.append(fmt_argument_as_name(spec, arg, arg_nr))
        args = ', '.join(args)
 
        sf.write(f'{klass.iface_file.fq_cpp_name.as_cpp}::{overload_cpp_name}({args});\n')
 
        if result is None:
            # Note that we should also generate this if the function returns a
            # value, but we are lazy and this is all that is needed by PyQt.
            if overload.new_thread:
                sf.write('        sipEndThread();\n')

            sf.write('        return;\n    }\n')

    sf.write('\n')

    _virtual_handler_call(sf, spec, klass, virtual_overload, result)

    sf.write('}\n')


def _virtual_handler_call(sf, spec, klass, virtual_overload, result):
    """ Generate a call to a single virtual handler. """

    module = spec.module
    overload = virtual_overload.overload
    handler = virtual_overload.handler

    module_name = module.py_name

    protection_state = _fake_protected_args(handler.cpp_signature)

    result_type = fmt_argument_as_cpp_type(spec, overload.cpp_signature.result,
            scope=klass.iface_file)

    sf.write(f'    extern {result_type} sipVH_{module_name}_{handler.handler_nr}(sip_gilstate_t, sipVirtErrorHandlerFunc, sipSimpleWrapper *, PyObject *')

    if len(handler.cpp_signature.args) > 0:
        sf.write(', ' + fmt_signature_as_cpp_declaration(spec,
                handler.cpp_signature, scope=klass.iface_file))

    _restore_protected_args(protection_state)

    # Add extra arguments for all the references we need to keep.
    args_keep = False
    result_keep = False
    saved_keys = {}

    if result is not None and _keep_py_reference(result):
        result_keep = True
        saved_keys[result] = result.key
        result.key = module.next_key
        module.next_key -= 1
        sf.write(', int')

    for arg in overload.cpp_signature.args:
        if arg.is_out and _keep_py_reference(arg):
            args_keep = True
            saved_keys[arg] = arg.key
            arg.key = module.next_key
            module.next_key -= 1
            sf.write(', int')

    sf.write(');\n\n    ')

    trailing = ''

    if not overload.new_thread and result is not None:
        sf.write('return ')

        if result.type is ArgumentType.ENUM and result.definition.is_protected:
            protection_state = set()
            _remove_protection(result, protection_state)

            enum_type = fmt_enum_as_cpp_type(result.definition)
            sf.write(f'static_cast<{enum_type}>(')
            trailing = ')'

            _restore_protections(protection_state)

    error_handler = handler.virtual_error_handler

    if error_handler is None:
        error_handler_ref = '0'
    elif error_handler.module is module:
        error_handler_ref = f'sipVEH_{module_name}_{error_handler.name}'
    else:
        error_handler_ref = f'sipImportedVirtErrorHandlers_{module_name}_{error_handler.module.py_name}[{error_handler.handler_nr}].iveh_handler'

    sf.write(f'sipVH_{module_name}_{handler.handler_nr}(sipGILState, {error_handler_ref}, sipPySelf, sipMeth')

    for arg_nr, arg in enumerate(overload.cpp_signature.args):
        prefix = ''

        if arg.type is ArgumentType.CLASS and arg.definition.is_protected:
            if arg.is_reference or len(arg.derefs) == 0:
                prefix = '&'
        elif arg.type is ArgumentType.ENUM and arg.definition.is_protected:
            prefix = '(' + fmt_enum_as_cpp_type(arg.definition) + ')'

        arg_name = fmt_argument_as_name(spec, arg, arg_nr)

        sf.write(f', {prefix}{arg_name}')

    # Pass the keys to maintain the kept references.
    if result_keep:
        sf.write(', ' + str(result.key))

    if args_keep:
        for arg in overload.cpp_signature.args:
            if arg.is_out and _keep_py_reference(arg):
                sf.write(', ' + str(arg.key))

    for type, key in saved_keys.items():
        type.key = key

    sf.write(f'){trailing};\n')

    if overload.new_thread:
        sf.write('\n    sipEndThread();\n')


def _cast_zero(spec, arg):
    """ Return a cast to zero. """

    if arg.type is ArgumentType.ENUM:
        enum = arg.definition
        enum_type = fmt_enum_as_cpp_type(enum)

        if len(enum.members) == 0:
            return f'({enum_type})0'

        if enum.is_scoped:
            scope = enum_type
        elif enum.scope is not None:
            scope = _enum_class_scope(spec, enum)
        else:
            scope = ''

        return scope + '::' + enum.members[0].cpp_name

    if arg.type in (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE, ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER, ArgumentType.PYENUM, ArgumentType.ELLIPSIS):
        return 'SIP_NULLPTR'

    return '0'


def _default_instance_return(sf, spec, result):
    """ Generate a statement to return the default instance of a type typically
    on error (ie. when there is nothing sensible to return).
    """

    # Handle the trivial case.
    if result is None:
        sf.write('        return;\n')
        return

    result_type_plain = fmt_argument_as_cpp_type(spec, result, plain=True,
            no_derefs=True)

    # Handle any %InstanceCode.
    if len(result.derefs) == 0 and result.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        instance_code = result.definition.instance_code
    else:
        instance_code = None

    if instance_code is not None:
        sf.write(
f'''    {{
        static {result_type_plain} *sipCpp = SIP_NULLPTR;

        if (!sipCpp)
        {{
''')

        sf.write_code(instance_code)

        sf.write(
'''        }

        return *sipCpp;
    }
''')

        return

    sf.write('        return ')

    if result.type is ArgumentType.MAPPED and len(result.derefs) == 0:
        # We don't know anything about the mapped type so we just hope is has a
        # default ctor.

        if result.is_reference:
            sf.write('*new ')

        sf.write(result_type_plain + '()')

    elif result.type is ArgumentType.CLASS and len(result.derefs) == 0:
        # If we don't have a suitable ctor then the generated code will issue
        # an error message.

        ctor = result.definition.default_ctor

        if ctor is not None and ctor.access_specifier is AccessSpecifier.PUBLIC and ctor.cpp_signature is not None:
            # If this is a badly designed class.  We can only generate correct
            # code by leaking memory.
            if result.is_reference:
                sf.write('*new ')

            sf.write(result_type_plain)
            sf.write(_call_default_ctor(spec, ctor))
        else:
            raise UserException(
                    result.definition.iface_file.fq_cpp_name.as_cpp + " must have a default constructor")

    else:
        sf.write(_cast_zero(spec, result))

    sf.write(';\n')


def _call_default_ctor(spec, ctor):
    """ Return the call to a default ctor. """

    args = []

    for arg in ctor.cpp_signature.args:
        if arg.default_value is not None:
            break

        # Do what we can to provide type information to the compiler.
        if arg.type is ArgumentType.CLASS and len(arg.derefs) > 0 and not arg.is_reference:
            class_type = fmt_argument_as_cpp_type(spec, arg)
            arg_s = f'static_cast<{class_type}>(0)'
        elif arg.type is ArgumentType.ENUM:
            enum_type = fmt_enum_as_cpp_type(arg.definition)
            arg_s = f'static_cast<{enum_type}>(0)'
        elif arg.type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
            arg_s = '0.0F'
        elif arg.type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
            arg_s = '0.0'
        elif arg.type in (ArgumentType.UINT, ArgumentType.SIZE):
            arg_s = '0U'
        elif arg.type in (ArgumentType.LONG, ArgumentType.LONGLONG):
            arg_s = '0L'
        elif arg.type in (ArgumentType.ULONG, ArgumentType.ULONGLONG):
            arg_s = '0UL'
        elif arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.USTRING, ArgumentType.SSTRING, ArgumentType.STRING) and len(arg.derefs) == 0:
            arg_s = "'\\0'"
        elif arg.type is ArgumentType.WSTRING and len(arg.derefs) == 0:
            arg_s = "L'\\0'"
        else:
            arg_s = '0'

        args.append(arg_s)

    return '(' + ', '.join(args) + ')'


def _protected_declarations(sf, spec, klass):
    """ Generate the declarations of the protected wrapper functions for a
    class.
    """

    no_intro = True

    for visible_member in klass.visible_members:
        if visible_member.member.py_slot is not None:
            continue

        for overload in visible_member.scope.overloads:
            if overload.common is not visible_member.member or overload.access_specifier is not AccessSpecifier.PROTECTED:
                continue

            # Check we haven't already handled this signature (eg. if we have
            # specified the same method with different Python names.
            if _is_duplicate_protected(spec, klass, overload):
                continue

            if no_intro:
                sf.write(
'''
    /*
     * There is a public method for every protected method visible from
     * this class.
     */
''')

                no_intro = False

            sf.write('    ')

            if overload.is_static:
                sf.write('static ')

            result_type = fmt_argument_as_cpp_type(spec,
                    overload.cpp_signature.result, scope=klass.iface_file)

            if not overload.is_static and not overload.is_abstract and (overload.is_virtual or overload.is_virtual_reimplementation):
                sf.write(f'{result_type} sipProtectVirt_{overload.cpp_name}(bool')

                if len(overload.cpp_signature.args) > 0:
                    sf.write(', ')
            else:
                sf.write(f'{result_type} sipProtect_{overload.cpp_name}(')

            args = fmt_signature_as_cpp_declaration(spec,
                    overload.cpp_signature, scope=klass.iface_file)
            const_s = ' const' if overload.is_const else ''

            sf.write(f'{args}){const_s};\n')


def _protected_definitions(sf, spec, klass):
    """ Generate the definitions of the protected wrapper functions for a
    class.
    """

    klass_name = klass.iface_file.fq_cpp_name.as_word

    for visible_member in klass.visible_members:
        if visible_member.member.py_slot is not None:
            continue

        for overload in visible_member.scope.overloads:
            if overload.common is not visible_member.member or overload.access_specifier is not AccessSpecifier.PROTECTED:
                continue

            # Check we haven't already handled this signature (eg. if we have
            # specified the same method with different Python names.
            if _is_duplicate_protected(spec, klass, overload):
                continue

            overload_name = overload.cpp_name
            result = overload.cpp_signature.result
            result_type = fmt_argument_as_cpp_type(spec, result,
                    scope=klass.iface_file)

            sf.write('\n')

            if not overload.is_static and not overload.is_abstract and (overload.is_virtual or overload.is_virtual_reimplementation):
                sf.write(f'{result_type} sip{klass_name}::sipProtectVirt_{overload_name}(bool sipSelfWasArg')

                if len(overload.cpp_signature.args) > 0:
                    sf.write(', ')
            else:
                sf.write(f'{result_type} sip{klass_name}::sipProtect_{overload_name}(')

            args = fmt_signature_as_cpp_definition(spec,
                    overload.cpp_signature, scope=klass.iface_file)
            const_s = ' const' if overload.is_const else ''

            sf.write(f'{args}){const_s}\n{{\n')

            closing_parens = ')'

            if result.type is ArgumentType.VOID and len(result.derefs) == 0:
                sf.write('    ')
            else:
                sf.write('    return ')

                if result.type is ArgumentType.CLASS and result.definition.is_protected:
                    scope_s = _scoped_class_name(spec, klass)
                    sf.write(f'static_cast<{scope_s} *>(')
                    closing_parens += ')'
                elif result.type is ArgumentType.ENUM and result.definition.is_protected:
                    # One or two older compilers can't handle a static_cast
                    # here so we revert to a C-style cast.
                    sf.write('(' + fmt_enum_as_cpp_type(result.definition) + ')')

            protected_call_args = _protected_call_args(spec,
                    overload.cpp_signature)

            if not overload.is_abstract:
                visible_scope_s = _scoped_class_name(spec, visible_member.scope)

                if overload.is_virtual or overload.is_virtual_reimplementation:
                    sf.write(f'(sipSelfWasArg ? {visible_scope_s}::{overload_name}({protected_call_args}) : ')
                    closing_parens += ')'
                else:
                    sf.write(visible_scope_s + '::')

            sf.write(f'{overload_name}({protected_call_args}{closing_parens};\n}}\n')


def _is_duplicate_protected(spec, klass, target_overload):
    """ Return True if a protected method is a duplicate. """

    for visible_member in klass.visible_members:
        if visible_member.member.py_slot is not None:
            continue

        for overload in visible_member.scope.overloads:
            if overload.common is not visible_member.member or overload.access_specifier is not AccessSpecifier.PROTECTED:
                continue

            if overload is target_overload:
                return False

            if overload.cpp_name == target_overload.cpp_name and same_signature(spec, overload.cpp_signature, target_overload.cpp_signature):
                return True

    # We should never get here.
    return False


def _protected_call_args(spec, signature):
    """ Return the arguments for a call to a protected method. """

    args = []

    for arg_nr, arg in enumerate(signature.args):
        if arg.type is ArgumentType.ENUM and arg.definition.is_protected:
            cast_s = '(' + arg.definition.fq_cpp_name.as_cpp + ')'
        else:
            cast_s = ''

        args.append(cast_s + fmt_argument_as_name(spec, arg, arg_nr))

    return ', '.join(args)


def _virtual_handler(sf, spec, handler):
    """ Generate the function that does most of the work to handle a particular
    virtual function.
    """

    module = spec.module
    result = handler.cpp_signature.result

    result_decl = fmt_argument_as_cpp_type(spec, result)

    result_is_returned = (result.type is not ArgumentType.VOID or len(result.derefs) != 0)
    result_is_reference = False
    result_instance_code = None

    if result_is_returned:
        # If we are returning a reference to an instance then we take care to
        # handle Python errors but still return a valid C++ instance.
        if result.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and len(result.derefs) == 0:
            if result.is_reference:
                result_is_reference = True
            else:
                result_instance_code = result.definition.instance_code

    sf.write(
f'''
{result_decl} sipVH_{module.py_name}_{handler.handler_nr}(sip_gilstate_t sipGILState, sipVirtErrorHandlerFunc sipErrorHandler, sipSimpleWrapper *sipPySelf, PyObject *sipMethod''')

    if len(handler.cpp_signature.args) > 0:
        sf.write(', ' + fmt_signature_as_cpp_definition(spec,
                handler.cpp_signature))

    # Define the extra arguments for kept references.
    if result_is_returned and _keep_py_reference(result):
        sf.write(', int')

        if handler.virtual_catcher_code is None or _is_used_in_code(handler.virtual_catcher_code, 'sipResKey'):
            sf.write(' sipResKey')

    for arg_nr, arg in enumerate(handler.cpp_signature.args):
        if arg.is_out and _keep_py_reference(arg):
            arg_name = fmt_argument_as_name(spec, arg, arg_nr)
            sf.write(f', int {arg_name}Key')

    sf.write(')\n{\n')

    if result_is_returned:
        result_plain_decl = fmt_argument_as_cpp_type(spec, result, plain=True)

        if result_instance_code is not None:
            sf.write(
f'''    static {result_plain_decl} *sipCpp = SIP_NULLPTR;

    if (!sipCpp)
    {{
''')

            sf.write_code(result_instance_code)

            sf.write('    }\n\n')

        sf.write('    ')

        # wchar_t * return values are always on the heap.  To reduce memory
        # leaks we keep the last result around until we have a new one.  This
        # means that ownership of the return value stays with the function
        # returning it - which is consistent with how other types work, even
        # though it may not be what's required in all cases.  Note that we
        # should do this in the code that calls the handler instead of here (as
        # we do with strings) so that it doesn't get shared between all
        # callers.
        if result.type is ArgumentType.WSTRING and len(result.derefs) == 1:
            sf.write('static ')

        sf.write(result_plain_decl)

        sf.write(' {}sipRes'.format('*' if result_is_reference else ''))

        sipres_value = ''

        if result.type in (ArgumentType.CLASS, ArgumentType.MAPPED, ArgumentType.TEMPLATE) and len(result.derefs) == 0:
            if result_instance_code is not None:
                sipres_value = ' = *sipCpp'
            elif result.type is ArgumentType.CLASS:
                ctor = result.definition.default_ctor

                if ctor is not None and ctor.access_specifier is AccessSpecifier.PUBLIC and ctor.cpp_signature is not None and len(ctor.cpp_signature.args) > 0 and ctor.cpp_signature.args[0].default_value is None:
                    sipres_value = _call_default_ctor(spec, ctor)
        elif result.type is ArgumentType.ENUM and result.definition.is_protected:
            # Currently SIP generates the virtual handlers before any shadow
            # classes which means that the compiler doesn't know about the
            # handling of protected enums.  Therefore we can only initialise to
            # 0.
            sipres_value = ' = 0'
        else:
            # We initialise the result to try and suppress a compiler warning.
            sipres_value = ' = ' + _cast_zero(spec, result)

        sf.write(sipres_value + ';\n')

        if result.type is ArgumentType.WSTRING and len(result.derefs) == 1:
            free_arg = 'const_cast<wchar_t *>(sipRes)' if result.is_const else 'sipRes'

            sf.write(
f'''
    if (sipRes)
    {{
        // Return any previous result to the heap.
        sipFree({free_arg});
        sipRes = SIP_NULLPTR;
    }}

''')

    if handler.virtual_catcher_code is not None:
        error_flag = _need_error_flag(handler.virtual_catcher_code)
        old_error_flag = _need_old_error_flag(handler.virtual_catcher_code)

        if error_flag:
            sf.write('    sipErrorState sipError = sipErrorNone;\n')
        elif old_error_flag:
            sf.write('    int sipIsErr = 0;\n')

        sf.write('\n')

        sf.write_code(handler.virtual_catcher_code)

        sf.write(
'''
    Py_DECREF(sipMethod);
''')

        if error_flag or old_error_flag:
            error_test = 'sipError != sipErrorNone' if error_flag else 'sipIsErr'

            sf.write(
f'''
    if ({error_test})
        sipCallErrorHandler(sipErrorHandler, sipPySelf, sipGILState);
''')

        sf.write(
'''
    SIP_RELEASE_GIL(sipGILState)
''')

        if result_is_returned:
            sf.write(
'''
    return sipRes;
''')

        sf.write('}\n')

        return

    # See how many values we expect.
    nr_values = 1 if result_is_returned else 0

    for arg in handler.py_signature.args:
        if arg.is_out:
            nr_values += 1

    # Call the method.
    if nr_values == 0:
        sf.write(
'    sipCallProcedureMethod(sipGILState, sipErrorHandler, sipPySelf, sipMethod, ')
    else:
        sf.write(
'    PyObject *sipResObj = sipCallMethod(SIP_NULLPTR, sipMethod, ')

    sf.write(_tuple_builder(spec, handler.py_signature))

    if nr_values == 0:
        sf.write(''');
}
''')

        return

    # Generate the call to sipParseResultEx().
    params = ['sipGILState', 'sipErrorHandler', 'sipPySelf', 'sipMethod',
            'sipResObj']

    # Build the format string.
    fmt = '"'

    if nr_values == 0:
        fmt += 'Z'
    else:
        if nr_values > 1:
            fmt += '('

        if result_is_returned:
            fmt += _get_parse_result_format(result, spec,
                    result_is_reference=result_is_reference,
                    transfer_result=handler.transfer_result)

        for arg in handler.py_signature.args:
            if arg.is_out:
                fmt += _get_parse_result_format(arg, spec)

        if nr_values > 1:
            fmt += ')'

    fmt += '"'

    params.append(fmt)

    # Add the destination pointers.
    if result_is_returned:
        _add_parse_result_extra_params(params, spec, module, result)
        params.append('&sipRes')

    for arg_nr, arg in enumerate(handler.py_signature.args):
        if arg.is_out:
            _add_parse_result_extra_params(params, spec, module, arg, arg_nr)

            arg_ref = '&' if arg.is_reference else ''
            arg_name = fmt_argument_as_name(spec, arg, arg_nr)
            params.append(arg_ref + arg_name)

    params = ', '.join(params)

    return_code = 'int sipRc = ' if result_is_reference or handler.abort_on_exception else ''

    sf.write(f''');

    {return_code}sipParseResultEx({params});
''')

    if result_is_returned:
        if result_is_reference or handler.abort_on_exception:
            sf.write(
'''
    if (sipRc < 0)
''')

            if handler.abort_on_exception:
                sf.write('        abort();\n')
            else:
                _default_instance_return(sf, spec, result)

        result_ref = '*' if result_is_reference else ''

        sf.write(
f'''
    return {result_ref}sipRes;
''')

    sf.write('}\n')


def _add_parse_result_extra_params(params, spec, module, arg, arg_nr=-1):
    """ Add any extra parameters needed by sipParseResultEx() for a particular
    type to a list.
    """

    if arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED, ArgumentType.ENUM):
        params.append(_gto_name(arg.definition))
    elif arg.type is ArgumentType.PYTUPLE:
        params.append('&PyTuple_Type')
    elif arg.type is ArgumentType.PYLIST:
        params.append('&PyList_Type')
    elif arg.type is ArgumentType.PYDICT:
        params.append('&PyDict_Type')
    elif arg.type is ArgumentType.PYSLICE:
        params.append('&PySlice_Type')
    elif arg.type is ArgumentType.PYTYPE:
        params.append('&PyType_Type')
    elif arg.type is ArgumentType.CAPSULE:
        params.append('"' + arg.definition.as_cpp + '"')
    elif _keep_py_reference(arg):
        if arg_nr < 0:
            params.append('sipResKey')
        else:
            params.append(fmt_argument_as_name(spec, arg, arg_nr) + 'Key')


def _get_parse_result_format(arg, spec, result_is_reference=False,
        transfer_result=False):
    """ Return the format characters used by sipParseResultEx() for a
    particular type.
    """

    nr_derefs = len(arg.derefs)
    no_derefs = (nr_derefs == 0)

    if arg.type in (ArgumentType.MAPPED, ArgumentType.FAKE_VOID, ArgumentType.CLASS):
        f = 0x00

        if nr_derefs == 0:
            f |= 0x01

            if not result_is_reference:
                f |= 0x04
        elif nr_derefs == 1:
            if arg.is_out:
                f |= 0x04
            elif arg.disallow_none:
                f |= 0x01

        if transfer_result:
            f |= 0x02

        return 'H' + str(f)

    if arg.type in (ArgumentType.BOOL, ArgumentType.CBOOL):
        return 'b'

    if arg.type is ArgumentType.ASCII_STRING:
        return 'aA' if no_derefs else 'AA'

    if arg.type is ArgumentType.LATIN1_STRING:
        return 'aL' if no_derefs else 'AL'

    if arg.type is ArgumentType.UTF8_STRING:
        return 'a8' if no_derefs else 'A8'

    if arg.type in (ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING):
        return 'c' if no_derefs else 'B'

    if arg.type is ArgumentType.WSTRING:
        return 'w' if no_derefs else 'x'

    if arg.type is ArgumentType.ENUM:
        return 'F' if arg.definition.fq_cpp_name is not None else 'e'

    if arg.type is ArgumentType.BYTE:
        return 'I' if _abi_has_working_char_conversion(spec) else 'L'

    if arg.type is ArgumentType.SBYTE:
        return 'L'

    if arg.type is ArgumentType.UBYTE:
        return 'M'

    if arg.type is ArgumentType.USHORT:
        return 't'

    if arg.type is ArgumentType.SHORT:
        return 'h'

    if arg.type in (ArgumentType.INT, ArgumentType.CINT):
        return 'i'

    if arg.type is ArgumentType.UINT:
        return 'u'

    if arg.type is ArgumentType.SIZE:
        return '='

    if arg.type is ArgumentType.LONG:
        return 'l'

    if arg.type is ArgumentType.ULONG:
        return 'm'

    if arg.type is ArgumentType.LONGLONG:
        return 'n'

    if arg.type is ArgumentType.ULONGLONG:
        return 'o'

    if arg.type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
        return 'V'

    if arg.type is ArgumentType.CAPSULE:
        return 'z'

    if arg.type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
        return 'f'

    if arg.type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
        return 'd'

    if arg.type is ArgumentType.PYOBJECT:
        return 'O'

    if arg.type in (ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.SLICE, ArgumentType.PYTYPE):
        return 'N' if arg.allow_none else 'T'

    if arg.type is ArgumentType.PYBUFFER:
        return '$' if arg.allow_none else '!'

    if arg.type is ArgumentType.PYENUM:
        return '^' if arg.allow_none else '&'

    # We should never get here.
    return ' '


def _tuple_builder(spec, signature):
    """ Return the code to build a tuple of Python arguments. """

    array_len_arg_nr = -1
    format_s = '"'

    for arg_nr, arg in enumerate(signature.args):
        if not arg.is_in:
            continue

        format_ch = ''
        nr_derefs = len(arg.derefs)
        not_a_pointer = (nr_derefs == 0 or (nr_derefs == 1 and arg.is_out))

        if arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING):
            format_ch = 'a' if not_a_pointer else 'A'

        elif arg.type in (ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING):
            if not_a_pointer:
                format_ch = 'c'
            elif arg.array is ArrayArgument.ARRAY:
                format_ch = 'g'
            else:
                format_ch = 's'

        elif arg.type is ArgumentType.WSTRING:
            if not_a_pointer:
                format_ch = 'w'
            elif arg.array is ArrayArgument.ARRAY:
                format_ch = 'G'
            else:
                format_ch = 'x'

        elif arg.type in (ArgumentType.BOOL, ArgumentType.CBOOL):
            format_ch = 'b'

        elif arg.type is ArgumentType.ENUM:
            format_ch = 'e' if arg.definition.fq_cpp_name is None else 'F'

        elif arg.type is ArgumentType.CINT:
            format_ch = 'i'

        elif arg.type is ArgumentType.UINT:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = 'u'

        elif arg.type is ArgumentType.INT:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = 'i'

        elif arg.type is ArgumentType.SIZE:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = '='

        elif arg.type in (ArgumentType.BYTE, ArgumentType.SBYTE):
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                # Note that this is the correct thing to do even if char is
                # unsigned.
                format_ch = 'L'

        elif arg.type is ArgumentType.UBYTE:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = 'M'

        elif arg.type is ArgumentType.USHORT:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = 't'

        elif arg.type is ArgumentType.SHORT:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = 'h'

        elif arg.type is ArgumentType.LONG:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = 'l'

        elif arg.type is ArgumentType.ULONG:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = 'm'

        elif arg.type is ArgumentType.LONGLONG:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = 'n'

        elif arg.type is ArgumentType.ULONGLONG:
            if arg.array is ArrayArgument.ARRAY_SIZE:
                array_len_arg_nr = arg_nr
            else:
                format_ch = 'o'

        elif arg.type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
            format_ch = 'V'

        elif arg.type is ArgumentType.CAPSULE:
            format_ch = 'z'

        elif arg.type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
            format_ch = 'f'

        elif arg.type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
            format_ch = 'd'

        elif arg.type in (ArgumentType.MAPPED, ArgumentType.CLASS):
            if arg.array is ArrayArgument.ARRAY:
                format_ch = 'r'
            else:
                format_ch = 'N' if _needs_heap_copy(arg) else 'D'

        elif arg.type is ArgumentType.FAKE_VOID:
            format_ch = 'D'

        elif arg.type in (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE, ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER, ArgumentType.PYENUM):
            format_ch = 'S'

        format_s += format_ch

    format_s += '"'

    args = [format_s]

    for arg_nr, arg in enumerate(signature.args):
        if not arg.is_in:
            continue

        nr_derefs = len(arg.derefs)

        if arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING, ArgumentType.WSTRING):
            if not (nr_derefs == 0 or (nr_derefs == 1 and arg.is_out)):
                nr_derefs -= 1

        elif arg.type in (ArgumentType.MAPPED, ArgumentType.CLASS, ArgumentType.FAKE_VOID):
            if nr_derefs > 0:
                nr_derefs -= 1

        elif arg.type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
            nr_derefs -= 1

        if arg.type in (ArgumentType.MAPPED, ArgumentType.CLASS, ArgumentType.FAKE_VOID):
            prefix = ''
            ref = ''

            needs_copy = _needs_heap_copy(arg)

            if needs_copy:
                prefix = 'new ' + fmt_argument_as_cpp_type(spec, arg, plain=True, no_derefs=True) + '('
            else:
                if arg.is_const:
                    prefix = 'const_cast<' + fmt_argument_as_cpp_type(spec, arg, plain=True, no_derefs=True, use_typename=False) + ' *>('

                if len(arg.derefs) == 0:
                    ref = '&'
                else:
                    ref = '*' * nr_derefs

            suffix = '' if prefix == '' else ')'

            arg_ref = prefix + ref + fmt_argument_as_name(spec, arg, arg_nr) + suffix

            args.append(arg_ref)

            if arg.array is ArrayArgument.ARRAY:
                array_len_arg_name = fmt_argument_as_name(spec,
                        signature.args[array_len_arg_nr], array_len_arg_nr)
                args.append('(Py_ssize_t)' + array_len_arg_name)

            args.append(_gto_name(arg.definition))

            if arg.array is not ArrayArgument.ARRAY:
                args.append('SIP_NULLPTR')

        elif arg.type is ArgumentType.CAPSULE:
            args.append('"' + arg.definition.as_cpp + '"')

        else:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                args.append(
                        '*' * nr_derefs + fmt_argument_as_name(spec, arg, arg_nr))

            if arg.array is ArrayArgument.ARRAY:
                array_len_arg_name = fmt_argument_as_name(spec,
                        signature.args[array_len_arg_nr], array_len_arg_nr)
                args.append('(Py_ssize_t)' + array_len_arg_name)
            elif arg.type is ArgumentType.ENUM and arg.definition.fq_cpp_name is not None:
                args.append(_gto_name(arg.definition))

    return ', '.join(args)


def _used_includes(sf, used):
    """ Generate the library header #include directives required by either a
    class or a module.
    """

    sf.write('\n')

    for iface_file in used:
        sf.write_code(iface_file.type_header_code)


def _module_api(sf, spec, bindings):
    """ Generate the API details for a module. """

    module = spec.module
    module_name = module.py_name

    for klass in spec.classes:
        if klass.iface_file.module is module:
            _class_api(sf, spec, klass)

        if klass.export_derived:
            sf.write_code(klass.iface_file.type_header_code)
            _shadow_class_declaration(sf, spec, bindings, klass)

    for mapped_type in spec.mapped_types:
        if mapped_type.iface_file.module is module:
            _mapped_type_api(sf, spec, mapped_type)

    no_exceptions = True

    for exception in spec.exceptions:
        if exception.iface_file.module is module and exception.exception_nr >= 0:
            if no_exceptions:
                sf.write(
f'''
/* The exceptions defined in this module. */
extern PyObject *sipExportedExceptions_{module_name}[];

''')

                no_exceptions = False

            sf.write(f'#define sipException_{exception.iface_file.fq_cpp_name.as_word} sipExportedExceptions_{module_name}[{exception.exception_nr}]\n')

    _enum_macros(sf, spec)

    for virtual_error_handler in spec.virtual_error_handlers:
        if virtual_error_handler.module is module:
            sf.write(f'\nvoid sipVEH_{module_name}_{virtual_error_handler.name}(sipSimpleWrapper *, sip_gilstate_t);\n')


def _imported_module_api(sf, spec, imported_module):
    """ Generate the API details for an imported module. """

    module_name = spec.module.py_name

    for klass in spec.classes:
        iface_file = klass.iface_file

        if iface_file.module is imported_module:
            if iface_file.needed:
                gto_name = _gto_name(klass)

                if iface_file.type is IfaceFileType.NAMESPACE:
                    sf.write(f'\n#if !defined({gto_name})')

                sf.write(f'\n#define {gto_name} sipImportedTypes_{module_name}_{iface_file.module.py_name}[{iface_file.type_nr}].it_td\n')

                if iface_file.type is IfaceFileType.NAMESPACE:
                    sf.write('#endif\n')

            _enum_macros(sf, spec, scope=klass,
                    imported_module=imported_module)

    for mapped_type in spec.mapped_types:
        iface_file = mapped_type.iface_file

        if iface_file.module is imported_module:
            if iface_file.needed:
                sf.write(f'\n#define {_gto_name(mapped_type)} sipImportedTypes_{module_name}_{iface_file.module.py_name}[{iface_file.type_nr}].it_td\n')

            _enum_macros(sf, spec, scope=mapped_type,
                    imported_module=imported_module)

    for exception in spec.exceptions:
        iface_file = exception.iface_file

        if iface_file.module is imported_module and exception.exception_nr >= 0:
            sf.write(f'\n#define sipException_{iface_file.fq_cpp_name.as_word} sipImportedExceptions_{module_name}_{iface_file.module.py_name}[{exception.exception_nr}].iexc_object\n')

    _enum_macros(sf, spec, imported_module=imported_module)


def _mapped_type_api(sf, spec, mapped_type):
    """ Generate the API details for a mapped type. """

    iface_file = mapped_type.iface_file

    module_name = spec.module.py_name
    mapped_type_name = iface_file.fq_cpp_name.as_word

    sf.write(
f'''
#define {_gto_name(mapped_type)} sipExportedTypes_{module_name}[{iface_file.type_nr}]

extern sipMappedTypeDef sipTypeDef_{module_name}_{mapped_type_name};
''')

    _enum_macros(sf, spec, scope=mapped_type)


def _class_api(sf, spec, klass):
    """ Generate the C++ API for a class. """

    iface_file = klass.iface_file

    module_name = spec.module.py_name

    sf.write('\n')

    if klass.real_class is None and not klass.is_hidden_namespace:
        sf.write(f'#define {_gto_name(klass)} sipExportedTypes_{module_name}[{iface_file.type_nr}]\n')

    _enum_macros(sf, spec, scope=klass)

    if not klass.external and not klass.is_hidden_namespace:
        klass_name = iface_file.fq_cpp_name.as_word
        sf.write(f'\nextern sipClassTypeDef sipTypeDef_{module_name}_{klass_name};\n')


def _enum_macros(sf, spec, scope=None, imported_module=None):
    """ Generate the type macros for enums. """

    for enum in spec.enums:
        if enum.fq_cpp_name is None:
            continue

        # Continue unless the scopes match.
        if scope is not None:
            if enum.scope is not scope:
                continue
        elif enum.scope is not None:
            continue

        value = None

        if imported_module is None:
            if enum.module is spec.module:
                value = f'sipExportedTypes_{spec.module.py_name}[{enum.type_nr}]'
        elif enum.module is imported_module and enum.needed:
            value = f'sipImportedTypes_{spec.module.py_name}_{enum.module.py_name}[{enum.type_nr}].it_td'

        if value is not None:
            sf.write(f'\n#define {_gto_name(enum)} {value}\n')


def _shadow_class_declaration(sf, spec, bindings, klass):
    """ Generate the shadow class declaration. """

    klass_name = klass.iface_file.fq_cpp_name.as_word

    sf.write(
f'''

class sip{klass_name} : public {_scoped_class_name(spec, klass)}
{{
public:
''')

    # Define a shadow class for any protected classes we have.
    for protected_klass in spec.classes:
        if not protected_klass.is_protected:
            continue

        # See if the class defining the class is in our class hierachy.
        for mro in klass.mro:
            if mro is protected_klass.scope:
                break
        else:
            continue

        protected_klass_base_name = protected_klass.iface_file.fq_cpp_name.base_name

        sf.write(
f'''    class sip{protected_klass_base_name} : public {protected_klass_base_name} {{
    public:
''')

        _protected_enums(sf, spec, protected_klass)

        sf.write('    };\n\n')

    # The constructor declarations.
    for ctor in _unique_class_ctors(spec, klass):
        args = fmt_signature_as_cpp_declaration(spec, ctor.cpp_signature,
                scope=klass.iface_file)
        throw_specifier = _throw_specifier(bindings, ctor.throw_args)

        sf.write(f'    sip{klass_name}({args}){throw_specifier};\n')

    # The destructor.
    if klass.dtor is not AccessSpecifier.PRIVATE:
        virtual_s = 'virtual ' if len(klass.virtual_overloads) != 0 else ''
        throw_specifier = _throw_specifier(bindings, klass.dtor_throw_args)

        sf.write(f'    {virtual_s}~sip{klass_name}(){throw_specifier};\n')

    # The metacall methods if required.
    if (_pyqt5(spec) or _pyqt6(spec)) and klass.is_qobject:
        sf.write(
'''
    int qt_metacall(QMetaObject::Call, int, void **) SIP_OVERRIDE;
    void *qt_metacast(const char *) SIP_OVERRIDE;
''')

        if not klass.pyqt_no_qmetaobject:
            sf.write('    const QMetaObject *metaObject() const SIP_OVERRIDE;\n')

    # The exposure of protected enums.
    _protected_enums(sf, spec, klass)

    # The wrapper around each protected member function.
    _protected_declarations(sf, spec, klass)

    # The catcher around each virtual function in the hierarchy.
    for virt_nr, virtual_overload in enumerate(_unique_class_virtual_overloads(spec, klass)):
        if virt_nr == 0:
            sf.write(
'''
    /*
     * There is a protected method for every virtual method visible from
     * this class.
     */
protected:
''')

        sf.write('    ')
        _overload_decl(sf, spec, bindings, klass, virtual_overload.overload)
        sf.write(';\n')

    sf.write(
'''
public:
    sipSimpleWrapper *sipPySelf;
''')

    # The private declarations.
    sf.write(
f'''
private:
    sip{klass_name}(const sip{klass_name} &);
    sip{klass_name} &operator = (const sip{klass_name} &);
''')

    nr_virtual_overloads = _count_virtual_overloads(spec, klass)
    if nr_virtual_overloads > 0:
        sf.write(f'\n    char sipPyMethods[{nr_virtual_overloads}];\n')

    sf.write('};\n')


def _overload_decl(sf, spec, bindings, klass, overload):
    """ Generate the C++ declaration for an overload. """

    cpp_signature = overload.cpp_signature

    # Counter the handling of protected enums by the argument formatter.
    protection_state = set()
    _remove_protections(cpp_signature, protection_state)

    result_type = fmt_argument_as_cpp_type(spec, cpp_signature.result,
            scope=klass.iface_file)
 
    args = []
    for arg in cpp_signature.args:
        args.append(
                fmt_argument_as_cpp_type(spec, arg, scope=klass.iface_file))

    args = ', '.join(args)
 
    const_s = ' const' if overload.is_const else ''
    throw_specifier = _throw_specifier(bindings, overload.throw_args)

    sf.write(f'{result_type} {_overload_cpp_name(overload)}({args}){const_s}{throw_specifier} SIP_OVERRIDE')

    _restore_protections(protection_state)


def _call_args(sf, spec, cpp_signature, py_signature):
    """ Generate typed arguments for a call. """

    for arg_nr, arg in enumerate(cpp_signature.args):
        if arg_nr > 0:
            sf.write(', ')

        # See if the argument needs dereferencing or it's address taking.
        indirection = ''
        nr_derefs = len(arg.derefs)

        if arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING, ArgumentType.WSTRING):
            if nr_derefs > (0 if arg.is_out else 1) and not arg.is_reference:
                indirection = '&'

        elif arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            if nr_derefs == 2:
                indirection = '&'
            elif nr_derefs == 0:
                indirection = '*'

        elif arg.type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
            if nr_derefs == 2:
                indirection = '&'

        else:
            if nr_derefs == 1:
                indirection = '&'

        # See if we need to cast a Python void * to the correct C/C++ pointer
        # type.  Note that we assume that the arguments correspond and are just
        # different types.
        need_cast = False

        if py_signature is not cpp_signature and len(py_signature.args) == len(cpp_signature.args):
            py_arg = py_signature.args[arg_nr]

            VOID_TYPES = (ArgumentType.VOID, ArgumentType.CAPSULE)

            if py_arg.type in VOID_TYPES and arg.type not in VOID_TYPES and len(py_arg.derefs) == nr_derefs:
                need_cast = True

        arg_name = fmt_argument_as_name(spec, arg, arg_nr)
        arg_cpp_type_name = fmt_argument_as_cpp_type(spec, arg, plain=True,
                no_derefs=True)

        if need_cast:
            if spec.c_bindings:
                sf.write(f'({arg_cpp_type_name} *){arg_name}')
            else:
                sf.write(f'reinterpret_cast<{arg_cpp_type_name} *>({arg_name})')
        else:
            sf.write(indirection)

            if arg.array is ArrayArgument.ARRAY_SIZE:
                sf.write(f'({arg_cpp_type_name})')

            sf.write(arg_name)


def _get_named_value_decl(spec, scope, type, name):
    """ Return the declaration of a named variable to hold a C++ value. """

    saved_derefs = type.derefs
    saved_is_const = type.is_const
    saved_is_reference = type.is_reference

    if len(type.derefs) == 0:
        if type.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            type.derefs = [False]
        else:
            type.is_const = False

    type.is_reference = False

    named_value_decl = fmt_argument_as_cpp_type(spec, type, name=name,
            scope=scope.iface_file if isinstance(scope, (WrappedClass, MappedType)) else None)

    type.derefs = saved_derefs
    type.is_const = saved_is_const
    type.is_reference = saved_is_reference

    return named_value_decl


def _argument_variable(sf, spec, scope, arg, arg_nr):
    """ Generate the definition of an argument variable and any supporting
    variables.
    """

    scope_iface_file = scope.iface_file if isinstance(scope, (WrappedClass, MappedType)) else None
    arg_name = fmt_argument_as_name(spec, arg, arg_nr)
    supporting_default_value = ' = 0' if arg.default_value is not None else ''
    nr_derefs = len(arg.derefs)

    if arg.is_in and arg.default_value is not None and arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and (nr_derefs == 0 or arg.is_reference):
        arg_cpp_type = fmt_argument_as_cpp_type(spec, arg,
                scope=scope_iface_file)

        # Generate something to hold the default value as it cannot be assigned
        # straight away.
        expression = fmt_value_list_as_cpp_expression(spec, arg.default_value)
        sf.write(f'        {arg_cpp_type} {arg_name}def = {expression};\n')

    # Adjust the type so we have the type that will really handle it.
    saved_derefs = arg.derefs
    saved_type = arg.type
    saved_is_reference = arg.is_reference
    saved_is_const = arg.is_const

    if arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING, ArgumentType.WSTRING):
        if not arg.is_reference:
            if nr_derefs == 2:
                arg.derefs = arg.derefs[0:1]
            elif nr_derefs == 1 and arg.is_out:
                arg.derefs = []

    elif arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED, ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
        arg.derefs = [arg.derefs[0] if len(arg.derefs) != 0 else False]

    else:
        arg.derefs = []

    # Array sizes are always Py_ssize_t.
    if arg.array is ArrayArgument.ARRAY_SIZE:
        arg.type = ArgumentType.SSIZE

    arg.is_reference = False

    if len(arg.derefs) == 0:
        arg.is_const = False

    modified_arg_cpp_type = fmt_argument_as_cpp_type(spec, arg,
            scope=scope_iface_file)

    sf.write(f'        {modified_arg_cpp_type} {arg_name}')

    # Restore the argument to its original state.
    arg.derefs = saved_derefs
    arg.type = saved_type
    arg.is_reference = saved_is_reference
    arg.is_const = saved_is_const

    # Generate any default value.
    if arg.is_in and arg.default_value is not None:
        sf.write(' = ')

        if arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and (nr_derefs == 0 or arg.is_reference):
            sf.write(f'&{arg_name}def')
        else:
            sf.write(fmt_value_list_as_cpp_expression(spec, arg.default_value))

    sf.write(';\n')

    # Some types have supporting variables.
    if arg.is_in:
        if arg.get_wrapper:
            sf.write(f'        PyObject *{arg_name}Wrapper{supporting_default_value};\n')
        elif arg.key is not None:
            sf.write(f'        PyObject *{arg_name}Keep{supporting_default_value};\n')

        if arg.type is ArgumentType.CLASS:
            if arg.array is ArrayArgument.ARRAY and _abi_supports_array(spec):
                if _abi_supports_array(spec):
                    sf.write(f'        int {arg_name}IsTemp = 0;\n')
            else:
                if arg.definition.convert_to_type_code is not None and not arg.is_constrained:
                    sf.write(f'        int {arg_name}State = 0;\n')

                    if _type_needs_user_state(arg):
                        sf.write(f'        void *{arg_name}UserState = SIP_NULLPTR;\n')

        elif arg.type is ArgumentType.MAPPED:
            if not arg.definition.no_release and not arg.is_constrained:
                sf.write(f'        int {arg_name}State = 0;\n')

                if _type_needs_user_state(arg):
                    sf.write(f'        void *{arg_name}UserState = SIP_NULLPTR;\n')

        elif arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING):
            if arg.key is None and nr_derefs == 1:
                sf.write(f'        PyObject *{arg_name}Keep{supporting_default_value};\n')


def _type_definition(sf, spec, bindings, klass, py_debug):
    """ Generate the type structure that contains all the information needed by
    the meta-type.  A sub-set of this is used to extend namespaces.
    """

    module = spec.module
    klass_name = klass.iface_file.fq_cpp_name.as_word

    # The super-types table.
    if len(klass.superclasses) != 0:
        encoded_types = []

        for superclass in klass.superclasses:
            last = superclass is klass.superclasses[-1]
            encoded_types.append(_encoded_type(module, superclass, last=last))

        encoded_types = ', '.join(encoded_types)

        sf.write(
f'''

/* Define this type's super-types. */
static sipEncodedTypeDef supers_{klass_name}[] = {{{encoded_types}}};
''')

    # The slots table.
    is_slots = False

    for member in klass.members:
        if member.py_slot is None:
            continue

        if not is_slots:
            sf.write(
f'''

/* Define this type's Python slots. */
static sipPySlotDef slots_{klass_name}[] = {{
''')

            is_slots = True

        slot_name = _get_slot_name(member.py_slot)
        member_name = member.py_name
        sf.write(f'    {{(void *)slot_{klass_name}_{member_name}, {slot_name}}},\n')

    if is_slots:
        sf.write('    {0, (sipPySlotType)0}\n};\n')

    # The attributes tables.
    nr_methods = _class_method_table(sf, spec, bindings, klass)

    if spec.abi_version >= (13, 0):
        nr_enum_members = -1
    else:
        nr_enum_members = _enum_member_table(sf, spec, scope=klass)

    # The property and variable handlers.
    nr_variables = 0

    if klass.has_variable_handlers:
        for variable in spec.variables:
            if variable.scope is klass and variable.needs_handler:
                nr_variables += 1

                _variable_getter(sf, spec, variable)

                if _can_set_variable(variable):
                    _variable_setter(sf, spec, variable)

    # Generate any property docstrings.
    for prop in klass.properties:
        nr_variables += 1

        if prop.docstring is not None:
            docstring = _docstring_text(prop.docstring)
            sf.write(f'\nPyDoc_STRVAR(doc_{klass_name}_{prop.name}, "{docstring}");\n')

    # The variables table.
    if nr_variables != 0:
        sf.write(f'\nsipVariableDef variables_{klass_name}[] = {{\n')

    for prop in klass.properties:
        fields = ['PropertyVariable', _cached_name_ref(prop.name)]

        getter_nr = find_method(klass, prop.getter).member_nr
        fields.append(f'&methods_{klass_name}[{getter_nr}]')

        if prop.setter is None:
            fields.append('SIP_NULLPTR')
        else:
            setter_nr = find_method(klass, prop.setter).member_nr
            fields.append(f'&methods_{klass_name}[{setter_nr}]')

        # We don't support a deleter yet.
        fields.append('SIP_NULLPTR')

        if prop.docstring is None:
            fields.append('SIP_NULLPTR')
        else:
            fields.append(f'doc_{klass_name}_{prop.name}')

        fields = ', '.join(fields)
        sf.write(f'    {{{fields}}},\n')

    if klass.has_variable_handlers:
        for variable in spec.variables:
            if variable.scope is klass and variable.needs_handler:
                variable_name = variable.fq_cpp_name.as_word

                fields = []

                fields.append('ClassVariable' if variable.is_static else 'InstanceVariable')
                fields.append(_cached_name_ref(variable.py_name))
                fields.append('(PyMethodDef *)varget_' + variable_name)

                if _can_set_variable(variable):
                    fields.append('(PyMethodDef *)varset_' + variable_name)
                else:
                    fields.append('SIP_NULLPTR')

                fields.append('SIP_NULLPTR')
                fields.append('SIP_NULLPTR')

                fields = ', '.join(fields)
                sf.write(f'    {{{fields}}},\n')

    if nr_variables != 0:
        sf.write('};\n')

    # Generate each instance table.
    is_inst_class = _class_instances(sf, spec, scope=klass)
    is_inst_voidp = _void_pointer_instances(sf, spec, scope=klass)
    is_inst_char = _char_instances(sf, spec, scope=klass)
    is_inst_string = _string_instances(sf, spec, scope=klass)
    is_inst_int = _int_instances(sf, spec, scope=klass)
    is_inst_long = _long_instances(sf, spec, scope=klass)
    is_inst_ulong = _unsigned_long_instances(sf, spec, scope=klass)
    is_inst_longlong = _long_long_instances(sf, spec, scope=klass)
    is_inst_ulonglong = _unsigned_long_long_instances(sf, spec, scope=klass)
    is_inst_double = _double_instances(sf, spec, scope=klass)

    # Generate the docstrings.
    if _has_class_docstring(bindings, klass):
        docstring_ref = 'doc_' + klass_name

        sf.write(f'\nPyDoc_STRVAR({docstring_ref}, "')
        _class_docstring(sf, spec, bindings, klass)
        sf.write('");\n')
    else:
        docstring_ref = 'SIP_NULLPTR'

    # Generate any plugin-specific data structures.
    plugin_ref = 'SIP_NULLPTR'

    if _pyqt5(spec) or _pyqt6(spec):
        if _pyqt_class_plugin(sf, spec, bindings, klass):
            plugin_ref = '&plugin_' + klass_name

    # The type definition structure itself.
    base_fields = []
    container_fields = []
    class_fields = []

    if spec.abi_version < (13, 0):
        base_fields.append('-1')
        base_fields.append('SIP_NULLPTR')

    base_fields.append('SIP_NULLPTR')

    flags = []

    if klass.is_abstract:
        flags.append('SIP_TYPE_ABSTRACT')

    if klass.subclass_base is not None:
        flags.append('SIP_TYPE_SCC')

    if klass.handles_none:
        flags.append('SIP_TYPE_ALLOW_NONE')

    if klass.has_nonlazy_method:
        flags.append('SIP_TYPE_NONLAZY')

    if module.call_super_init:
        flags.append('SIP_TYPE_SUPER_INIT')

    if not py_debug and module.use_limited_api:
        flags.append('SIP_TYPE_LIMITED_API')

    flags.append('SIP_TYPE_NAMESPACE' if klass.iface_file.type is IfaceFileType.NAMESPACE else 'SIP_TYPE_CLASS')

    if len(flags) == 0:
        flags.append('0')

    base_fields.append('|'.join(flags))

    base_fields.append(_cached_name_ref(klass.iface_file.cpp_name, as_nr=True))
    base_fields.append('SIP_NULLPTR')
    base_fields.append(plugin_ref)

    container_fields.append(_cached_name_ref(klass.py_name, as_nr=True) if klass.real_class is None else '-1')

    if klass.real_class is not None:
        encoded_type = _encoded_type(module, klass.real_class)
    elif _py_scope(klass.scope) is not None:
        encoded_type = _encoded_type(module, klass.scope)
    else:
        encoded_type = '{0, 0, 1}'

    container_fields.append(encoded_type)

    if nr_methods == 0:
        container_fields.append('0, SIP_NULLPTR')
    else:
        container_fields.append(str(nr_methods) + ', methods_' + klass_name)

    if nr_enum_members == 0:
        container_fields.append('0, SIP_NULLPTR')
    elif nr_enum_members > 0:
        container_fields.append(str(nr_enum_members) + ', enummembers_' + klass_name)

    if nr_variables == 0:
        container_fields.append('0, SIP_NULLPTR')
    else:
        container_fields.append(str(nr_variables) + ', variables_' + klass_name)

    instances = []

    instances.append(
            _class_object_ref(is_inst_class, 'typeInstances', klass_name))
    instances.append(
            _class_object_ref(is_inst_voidp, 'voidPtrInstances', klass_name))
    instances.append(
            _class_object_ref(is_inst_char, 'charInstances', klass_name))
    instances.append(
            _class_object_ref(is_inst_string, 'stringInstances', klass_name))
    instances.append(
            _class_object_ref(is_inst_int, 'intInstances', klass_name))
    instances.append(
            _class_object_ref(is_inst_long, 'longInstances', klass_name))
    instances.append(
            _class_object_ref(is_inst_ulong, 'unsignedLongInstances',
                    klass_name))
    instances.append(
            _class_object_ref(is_inst_longlong, 'longLongInstances',
                    klass_name))
    instances.append(
            _class_object_ref(is_inst_ulonglong, 'unsignedLongLongInstances',
                    klass_name))
    instances.append(
            _class_object_ref(is_inst_double, 'doubleInstances', klass_name))

    container_fields.append('{' + ', '.join(instances) + '}')

    class_fields.append(docstring_ref)
    class_fields.append(_cached_name_ref(klass.metatype, as_nr=True) if klass.metatype is not None else '-1')
    class_fields.append(_cached_name_ref(klass.supertype, as_nr=True) if klass.supertype is not None else '-1')
    class_fields.append(
            _class_object_ref((len(klass.superclasses) != 0), 'supers',
                    klass_name))
    class_fields.append(_class_object_ref(is_slots, 'slots', klass_name))
    class_fields.append(
                _class_object_ref(klass.can_create, 'init_type', klass_name))
    class_fields.append(
                _class_object_ref((klass.gc_traverse_code is not None),
                        'traverse', klass_name))
    class_fields.append(
                _class_object_ref((klass.gc_clear_code is not None), 'clear',
                        klass_name))
    class_fields.append(
                _class_object_ref((klass.bi_get_buffer_code is not None),
                        'getbuffer', klass_name))
    class_fields.append(
                _class_object_ref((klass.bi_release_buffer_code is not None),
                        'releasebuffer', klass_name))
    class_fields.append(
                _class_object_ref(_need_dealloc(spec, bindings, klass),
                        'dealloc', klass_name))
    class_fields.append(
                _class_object_ref((spec.c_bindings or klass.needs_copy_helper),
                        'assign', klass_name))
    class_fields.append(
                _class_object_ref((spec.c_bindings or klass.needs_array_helper),
                        'array', klass_name))
    class_fields.append(
                _class_object_ref((spec.c_bindings or klass.needs_copy_helper),
                        'copy', klass_name))
    class_fields.append(
                _class_object_ref((not spec.c_bindings and klass.iface_file.type is not IfaceFileType.NAMESPACE),
                        'release', klass_name))
    class_fields.append(
            _class_object_ref((len(klass.superclasses) != 0), 'cast',
                    klass_name))
    class_fields.append(
                _class_object_ref((klass.convert_to_type_code is not None and klass.iface_file.type is not IfaceFileType.NAMESPACE),
                        'convertTo', klass_name))
    class_fields.append(
                _class_object_ref((klass.convert_from_type_code is not None and klass.iface_file.type is not IfaceFileType.NAMESPACE),
                        'convertFrom', klass_name))
    class_fields.append('SIP_NULLPTR')
    class_fields.append(
                _class_object_ref((klass.pickle_code is not None), 'pickle',
                        klass_name))
    class_fields.append(
                _class_object_ref((klass.finalisation_code is not None),
                        'final', klass_name))
    class_fields.append(_class_object_ref(klass.mixin, 'mixin', klass_name))

    if _abi_supports_array(spec):
        class_fields.append(
                    _class_object_ref((spec.c_bindings or klass.needs_array_helper),
                            'array_delete', klass_name))

        if klass.can_create:
            class_fields.append(f'sizeof ({_scoped_class_name(spec, klass)})')
        else:
            class_fields.append('0')

    base_fields = ',\n        '.join(base_fields)
    container_fields = ',\n        '.join(container_fields)
    class_fields = ',\n    '.join(class_fields)

    sf.write(
f'''

sipClassTypeDef sipTypeDef_{module.py_name}_{klass_name} = {{
    {{
        {base_fields},
    }},
    {{
        {container_fields},
    }},
    {class_fields},
}};
''')


def _class_object_ref(test, object_name, klass_name):
    """ Return an appropriate reference to a class-specific object. """

    return object_name + '_' + klass_name if test else 'SIP_NULLPTR'


def _can_set_variable(variable):
    """ Return True if a variable can be set. """

    if variable.no_setter:
        return False

    if len(variable.type.derefs) == 0 and variable.type.is_const:
        return False

    return True


def _pyqt_emitters(sf, spec, klass):
    """ Generate the PyQt emitters for a class. """

    klass_name = klass.iface_file.fq_cpp_name.as_word
    scope_s = _scoped_class_name(spec, klass)
    klass_name_ref = _cached_name_ref(klass.py_name)

    for member in klass.members:
        in_emitter = False

        for overload in klass.overloads:
            if not (overload.common is member and overload.pyqt_method_specifier is PyQtMethodSpecifier.SIGNAL and _has_optional_args(overload)):
                continue

            if not in_emitter:
                in_emitter = True

                sf.write('\n\n')

                if not spec.c_bindings:
                    sf.write(f'extern "C" {{static int emit_{klass_name}_{overload.cpp_name}(void *, PyObject *);}}\n\n')

                sf.write(
f'''static int emit_{klass_name}_{overload.cpp_name}(void *sipCppV, PyObject *sipArgs)
{{
    PyObject *sipParseErr = SIP_NULLPTR;
    {scope_s} *sipCpp = reinterpret_cast<{scope_s} *>(sipCppV);
''')

            # Generate the code that parses the args and emits the appropriate
            # overloaded signal.
            sf.write('\n    {\n')

            _arg_parser(sf, spec, klass, overload.py_signature)

            sf.write(
f'''        {{
            Py_BEGIN_ALLOW_THREADS
            sipCpp->{overload.cpp_name}(''')

            _call_args(sf, spec, overload.cpp_signature, overload.py_signature)

            sf.write(''');
            Py_END_ALLOW_THREADS

''')

            _delete_temporaries(sf, spec, overload.py_signature)

            sf.write(
'''
            return 0;
        }
    }
''')

        if in_emitter:
            member_name_ref = _cached_name_ref(member.py_name)

            sf.write(
f'''
    sipNoMethod(sipParseErr, {klass_name_ref}, {member_name_ref}, SIP_NULLPTR);

    return -1;
}}
''')


def _pyqt_signal_table_entry(sf, spec, bindings, klass, signal, member_nr):
    """ Generate an entry in the PyQt signal table. """

    klass_name = klass.iface_file.fq_cpp_name.as_word

    stripped = False
    signature_state = {}

    args = []

    for arg in signal.cpp_signature.args:
        # Do some signal argument normalisation so that Qt doesn't have to.
        if arg.is_const and (arg.is_reference or len(arg.derefs) == 0):
            signature_state[arg] = arg.is_reference

            arg.is_const = False
            arg.is_reference = False

        if arg.scopes_stripped != 0:
            strip = arg.scopes_stripped
            stripped = True
        else:
            strip = STRIP_GLOBAL

        args.append(
                fmt_argument_as_cpp_type(spec, arg, scope=klass.iface_file,
                        strip=strip))

    # Note the lack of a separating space.
    args = ','.join(args)

    sf.write(f'    {{"{signal.cpp_name}({args})')

    # If a scope was stripped then append an unstripped version which can be
    # parsed by PyQt.
    if stripped:
        args = []

        for arg in signal.cpp_signature.args:
            args.append(
                    fmt_argument_as_cpp_type(spec, arg, scope=klass.iface_file,
                            strip=STRIP_GLOBAL))

        # Note the lack of a separating space.
        args = ','.join(args)

        sf.write(f'|({args})')

    sf.write('", ')

    # Restore the signature state.
    for arg, is_reference in signature_state.items():
        arg.is_const = True
        arg.is_reference = is_reference

    if bindings.docstrings:
        sf.write('"')

        if signal.docstring is not None:
            if signal.docstring.signature is DocstringSignature.PREPENDED:
                _overload_auto_docstring(sf, spec, signal)
                sf.write('\\n')

            sf.write(_docstring_text(signal.docstring))

            if signal.docstring.signature is DocstringSignature.APPENDED:
                sf.write('\\n')
                _overload_auto_docstring(sf, spec, signal)
        else:
            sf.write('\\1')
            _overload_auto_docstring(sf, spec, signal)

        sf.write('", ')
    else:
        sf.write('SIP_NULLPTR, ')

    sf.write(f'&methods_{klass_name}[{member_nr}], ' if member_nr >= 0 else 'SIP_NULLPTR, ')

    sf.write(f'emit_{klass_name}_{signal.cpp_name}' if _has_optional_args(signal) else 'SIP_NULLPTR')

    sf.write('},\n')


def _get_slot_name(slot_type):
    """ Return the sip module's string equivalent of a slot. """

    return slot_type.name.lower() + '_slot'


def _type_init(sf, spec, bindings, klass):
    """ Generate the initialisation function for the type. """

    klass_name = klass.iface_file.fq_cpp_name.as_word

    # See if we need to name the self and owner arguments so that we can avoid
    # a compiler warning about an unused argument.
    need_self = (spec.c_bindings or klass.has_shadow)
    need_owner = spec.c_bindings

    for ctor in klass.ctors:
        if _is_used_in_code(ctor.method_code, 'sipSelf'):
            need_self = True

        if ctor.transfer is Transfer.TRANSFER:
            need_owner = True
        else:
            for arg in ctor.py_signature.args:
                if not arg.is_in:
                    continue

                if arg.key is not None:
                    need_self = True

                if arg.transfer is Transfer.TRANSFER:
                    need_self = True

                if arg.transfer is Transfer.TRANSFER_THIS:
                    need_owner = True

    sf.write('\n\n')

    if not spec.c_bindings:
        sf.write(f'extern "C" {{static void *init_type_{klass_name}(sipSimpleWrapper *, PyObject *, PyObject *, PyObject **, PyObject **, PyObject **);}}\n')

    sip_self = 'sipSelf' if need_self else ''
    sip_owner = 'sipOwner' if need_owner else ''
    sip_cpp_type = 'sip' + klass_name if klass.has_shadow else _scoped_class_name(spec, klass)

    sf.write(
f'''static void *init_type_{klass_name}(sipSimpleWrapper *{sip_self}, PyObject *sipArgs, PyObject *sipKwds, PyObject **sipUnused, PyObject **{sip_owner}, PyObject **sipParseErr)
{{
    {sip_cpp_type} *sipCpp = SIP_NULLPTR;
''')

    if bindings.tracing:
        sf.write(f'\n    sipTrace(SIP_TRACE_INITS, "init_type_{klass_name}()\\n");\n')

    # Generate the code that parses the Python arguments and calls the correct
    # constructor.
    for ctor in klass.ctors:
        if ctor.access_specifier is AccessSpecifier.PRIVATE:
            continue

        sf.write('\n    {\n')

        if ctor.method_code is not None:
            error_flag = _need_error_flag(ctor.method_code)
            old_error_flag = _need_old_error_flag(ctor.method_code)
        else:
            error_flag = old_error_flag = False

        _arg_parser(sf, spec, klass, ctor.py_signature, ctor=ctor)
        _constructor_call(sf, spec, bindings, klass, ctor, error_flag,
                old_error_flag)

        sf.write('    }\n')

    sf.write(
'''
    return SIP_NULLPTR;
}
''')


def _count_virtual_overloads(spec, klass):
    """ Return the number of virtual members in a class. """

    return len(list(_unique_class_virtual_overloads(spec, klass)))

 
def _handling_exceptions(bindings, throw_args):
    """ Return True if exceptions from a callable are being handled. """

    # Handle any exceptions if there was no throw specifier, or a non-empty
    # throw specifier.
    return bindings.exceptions and (throw_args is None or throw_args.arguments is not None)


def _try(sf, bindings, throw_args):
    """ Generate the try block for a call. """

    # Generate the block if there was no throw specifier, or a non-empty throw
    # specifier.
    if _handling_exceptions(bindings, throw_args):
        sf.write(
'''            try
            {
''')


def _catch(sf, spec, bindings, py_signature, throw_args, release_gil):
    """ Generate the catch blocks for a call. """

    if _handling_exceptions(bindings, throw_args):
        use_handler = _abi_has_next_exception_handler(spec)

        sf.write('            }\n')

        if not use_handler:
            if throw_args is not None:
                for exception in throw_args.arguments:
                    _catch_block(sf, spec, exception,
                            py_signature=py_signature, release_gil=release_gil)
            elif spec.module.default_exception is not None:
                _catch_block(sf, spec, spec.module.default_exception,
                        py_signature=py_signature, release_gil=release_gil)

        sf.write(
'''            catch (...)
            {
''')

        if release_gil:
            sf.write(
'''                Py_BLOCK_THREADS

''')

        _delete_outs(sf, spec, py_signature)
        _delete_temporaries(sf, spec, py_signature)

        if use_handler:
            sf.write(
'''                void *sipExcState = SIP_NULLPTR;
                sipExceptionHandler sipExcHandler;
                std::exception_ptr sipExcPtr = std::current_exception();

                while ((sipExcHandler = sipNextExceptionHandler(&sipExcState)) != SIP_NULLPTR)
                    if (sipExcHandler(sipExcPtr))
                        return SIP_NULLPTR;

''')

        sf.write(
'''                sipRaiseUnknownException();
                return SIP_NULLPTR;
            }
''')


def _catch_block(sf, spec, exception, py_signature=None, release_gil=False):
    """ Generate a single catch block. """

    exception_fq_cpp_name = exception.iface_file.fq_cpp_name

    # The global scope is stripped from the exception name to be consistent
    # with older versions of SIP.
    exception_cpp_stripped = exception_fq_cpp_name.cpp_stripped(STRIP_GLOBAL)

    sip_exception_ref = 'sipExceptionRef' if exception.class_exception is not None or _is_used_in_code(exception.raise_code, 'sipExceptionRef') else ''

    sf.write(
f'''            catch ({exception_cpp_stripped} &{sip_exception_ref})
            {{
''')

    if release_gil:
        sf.write(
'''
                Py_BLOCK_THREADS
''')

    if py_signature is not None:
        _delete_outs(sf, spec, py_signature)
        _delete_temporaries(sf, spec, py_signature)
        result = 'SIP_NULLPTR'
    else:
        result = 'true'

    # See if the exception is a wrapped class.
    if exception.class_exception is not None:
        exception_cpp = exception_fq_cpp_name.as_cpp

        sf.write(
f'''                /* Hope that there is a valid copy ctor. */
                {exception_cpp} *sipExceptionCopy = new {exception_cpp}(sipExceptionRef);

                sipRaiseTypeException({_gto_name(exception)}, sipExceptionCopy);
''')
    else:
        sf.write_code(exception.raise_code)

    sf.write(
f'''
                return {result};
            }}
''')


def _throw_specifier(bindings, throw_args):
    """ Return a throw specifier. """

    return ' noexcept' if bindings.exceptions and throw_args is not None and throw_args.arguments is None else ''


def _constructor_call(sf, spec, bindings, klass, ctor, error_flag,
        old_error_flag):
    """ Generate a single constructor call. """

    klass_name = klass.iface_file.fq_cpp_name.as_word
    scope_s = _scoped_class_name(spec, klass)

    sf.write('        {\n')

    if ctor.premethod_code is not None:
        sf.write('\n')
        sf.write_code(ctor.premethod_code)
        sf.write('\n')

    if error_flag:
        sf.write('            sipErrorState sipError = sipErrorNone;\n\n')
    elif old_error_flag:
        sf.write('            int sipIsErr = 0;\n\n')

    if ctor.deprecated is not None:
        # Note that any temporaries will leak if an exception is raised.

        if abi_has_deprecated_message(spec):
            str_deprecated_message = f'"{ctor.deprecated}"' if ctor.deprecated else 'SIP_NULLPTR'
            sf.write(f'            if (sipDeprecated({_cached_name_ref(klass.py_name)}, SIP_NULLPTR, {str_deprecated_message}) < 0)\n')
        else:
            sf.write(f'            if (sipDeprecated({_cached_name_ref(klass.py_name)}, SIP_NULLPTR) < 0)\n')
            
        sf.write(f'                return SIP_NULLPTR;\n\n')

    # Call any pre-hook.
    if ctor.prehook is not None:
        sf.write(f'            sipCallHook("{ctor.prehook}");\n\n')

    if ctor.method_code is not None:
        sf.write_code(ctor.method_code)
    elif spec.c_bindings:
        sf.write(f'            sipCpp = sipMalloc(sizeof ({scope_s}));\n')
    else:
        release_gil = _release_gil(ctor.gil_action, bindings)

        if ctor.raises_py_exception:
            sf.write('            PyErr_Clear();\n\n')

        if release_gil:
            sf.write('            Py_BEGIN_ALLOW_THREADS\n')

        _try(sf, bindings, ctor.throw_args)

        klass_type = 'sip' + klass_name if klass.has_shadow else scope_s
        sf.write(f'            sipCpp = new {klass_type}(')

        if ctor.is_cast:
            # We have to fiddle the type to generate the correct code.
            arg0 = ctor.py_signature.args[0]
            saved_definition = arg0.definition
            arg0.definition = klass
            cast_call = fmt_argument_as_cpp_type(spec, arg0)
            arg0.definition = saved_definition

            sf.write(f'a0->operator {cast_call}()')
        else:
            _call_args(sf, spec, ctor.cpp_signature, ctor.py_signature)

        sf.write(');\n')

        _catch(sf, spec, bindings, ctor.py_signature, ctor.throw_args,
                release_gil)

        if release_gil:
            sf.write('            Py_END_ALLOW_THREADS\n')

        # This is a bit of a hack to say we want the result transferred.  We
        # don't simply call sipTransferTo() because the wrapper object hasn't
        # been fully initialised yet.
        if ctor.transfer is Transfer.TRANSFER:
            sf.write('\n            *sipOwner = Py_None;\n')

    # Handle any /KeepReference/ arguments.
    for arg_nr, arg in enumerate(ctor.py_signature.args):
        if not arg.is_in:
            continue

        if arg.key is not None:
            arg_name = fmt_argument_as_name(spec, arg, arg_nr)
            suffix = 'Keep' if (arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING) and len(arg.derefs) == 1) or not arg.get_wrapper else 'Wrapper'

            sf.write(f'\n            sipKeepReference((PyObject *)sipSelf, {arg.key}, {arg_name}{suffix});\n')

    _gc_ellipsis(sf, ctor.py_signature)
    _delete_temporaries(sf, spec, ctor.py_signature)

    sf.write('\n')

    if ctor.raises_py_exception:
        sf.write(
'''            if (PyErr_Occurred())
            {
                delete sipCpp;
                return SIP_NULLPTR;
            }

''')

    if error_flag:
        sf.write('            if (sipError == sipErrorNone)\n')

        if klass.has_shadow or ctor.posthook is not None:
            sf.write('            {\n')

        if klass.has_shadow:
            sf.write('                sipCpp->sipPySelf = sipSelf;\n\n')

        # Call any post-hook.
        if ctor.posthook is not None:
            sf.write(f'            sipCallHook("{ctor.posthook}");\n\n')

        sf.write('                return sipCpp;\n')

        if klass.has_shadow or ctor.posthook is not None:
            sf.write('            }\n')

        sf.write(
'''
            if (sipUnused)
            {
                Py_XDECREF(*sipUnused);
            }

            sipAddException(sipError, sipParseErr);

            if (sipError == sipErrorFail)
                return SIP_NULLPTR;
''')
    else:
        if old_error_flag:
            sf.write(
'''            if (sipIsErr)
            {
                if (sipUnused)
                {
                    Py_XDECREF(*sipUnused);
                }

                sipAddException(sipErrorFail, sipParseErr);
                return SIP_NULLPTR;
            }

''')

        if klass.has_shadow:
            sf.write('            sipCpp->sipPySelf = sipSelf;\n\n')

        # Call any post-hook.
        if ctor.posthook is not None:
            sf.write(f'            sipCallHook("{ctor.posthook}");\n\n')

        sf.write('            return sipCpp;\n')

    sf.write('        }\n')


def _skip_overload(overload, member, klass, scope, want_local=True):
    """ See if a member overload should be skipped. """

    # Skip if it's not the right name.
    if overload.common is not member:
        return True

    # Skip if it's a signal.
    if overload.pyqt_method_specifier is PyQtMethodSpecifier.SIGNAL:
        return True

    # Skip if it's a private abstract.
    if overload.is_abstract and overload.access_specifier is AccessSpecifier.PRIVATE:
        return True

    # If we are disallowing them, skip if it's not in the current class unless
    # it is protected.
    if want_local and overload.access_specifier is not AccessSpecifier.PROTECTED and klass is not scope:
        return True

    return False


def _member_function(sf, spec, bindings, klass, member, original_klass):
    """ Generate a class member function. """

    # Check that there is at least one overload that needs to be handled.  See
    # if we can avoid naming the "self" argument (and suppress a compiler
    # warning).  See if we need to remember if "self" was explicitly passed as
    # an argument.  See if we need to handle keyword arguments.
    need_method = need_self = need_args = need_selfarg = need_orig_self = False

    for overload in original_klass.overloads:
        # Skip protected methods if we don't have the means to handle them.
        if overload.access_specifier is AccessSpecifier.PROTECTED and not klass.has_shadow:
            continue

        if not _skip_overload(overload, member, klass, original_klass):
            need_method = True

            if overload.access_specifier is not AccessSpecifier.PRIVATE:
                need_args = True

                if spec.abi_version >= (13, 0) or not overload.is_static:
                    need_self = True

                    if overload.is_abstract:
                        need_orig_self = True
                    elif overload.is_virtual or overload.is_virtual_reimplementation or _is_used_in_code(overload.method_code, 'sipSelfWasArg'):
                        need_selfarg = True

    # Handle the trivial case.
    if not need_method:
        return

    klass_name = klass.iface_file.fq_cpp_name.as_word
    member_py_name = member.py_name.name

    sf.write('\n\n')

    # Generate the docstrings.
    if _has_member_docstring(bindings, member, original_klass.overloads):
        sf.write(f'PyDoc_STRVAR(doc_{klass_name}_{member_py_name}, "')

        has_auto_docstring = _member_docstring(sf, spec, bindings, member,
                original_klass.overloads,
                is_method=not klass.is_hidden_namespace)

        sf.write('");\n\n')
    else:
        has_auto_docstring = False

    if member.no_arg_parser or member.allow_keyword_args:
        arg3_type = ', PyObject *'
        arg3_decl = ', PyObject *sipKwds'
    else:
        arg3_type = ''
        arg3_decl = ''

    sip_self = 'sipSelf' if need_self else ''
    sip_args = 'sipArgs' if need_args else ''

    if not spec.c_bindings:
        sf.write(f'extern "C" {{static PyObject *meth_{klass_name}_{member_py_name}(PyObject *, PyObject *{arg3_type});}}\n')

    sf.write(f'static PyObject *meth_{klass_name}_{member_py_name}(PyObject *{sip_self}, PyObject *{sip_args}{arg3_decl})\n{{\n')

    if bindings.tracing:
        sf.write(f'    sipTrace(SIP_TRACE_METHODS, "meth_{klass_name}_{member_py_name}()\\n");\n\n')

    if not member.no_arg_parser:
        if need_args:
            sf.write('    PyObject *sipParseErr = SIP_NULLPTR;\n')

        if need_selfarg:
            # This determines if we call the explicitly scoped version or the
            # unscoped version (which will then go via the vtable).
            #
            # - If the call was unbound and self was passed as the first
            #   argument (ie. Foo.meth(self)) then we always want to call the
            #   explicitly scoped version.
            #
            # - If the call was bound then we only call the unscoped version in
            #   case there is a C++ sub-class reimplementation that Python
            #   knows nothing about.  Otherwise, if the call was invoked by
            #   super() within a Python reimplementation then the Python
            #   reimplementation would be called recursively.
            #
            # In addition, if the type is a derived class then we know that
            # there can't be a C++ sub-class that we don't know about so we can
            # avoid the vtable.
            #
            # Note that we would like to rename 'sipSelfWasArg' to
            # 'sipExplicitScope' but it is part of the public API.
            if spec.abi_version >= (13, 0):
                sipself_test = f'!PyObject_TypeCheck(sipSelf, sipTypeAsPyTypeObject({_gto_name(klass)}))'
            else:
                sipself_test = '!sipSelf'

            sf.write(f'    bool sipSelfWasArg = ({sipself_test} || sipIsDerivedClass((sipSimpleWrapper *)sipSelf));\n')

        if need_orig_self:
            # This is similar to the above but for abstract methods.  We allow
            # the (potential) recursion because it means that the concrete
            # implementation can be put in a mixin and it will all work.
            sf.write('    PyObject *sipOrigSelf = sipSelf;\n')

    for overload in original_klass.overloads:
        # If we are handling one variant then we must handle them all.
        if _skip_overload(overload, member, klass, original_klass, want_local=False):
            continue

        if overload.access_specifier is AccessSpecifier.PRIVATE:
            continue

        if member.no_arg_parser:
            sf.write_code(overload.method_code)
            break

        _function_body(sf, spec, bindings, klass, overload,
                original_klass=original_klass)

    if not member.no_arg_parser:
        sip_parse_err = 'sipParseErr' if need_args else 'SIP_NULLPTR'
        klass_py_name_ref = _cached_name_ref(klass.py_name)
        member_py_name_ref = _cached_name_ref(member.py_name)
        docstring_ref = f'doc_{klass_name}_{member_py_name}' if has_auto_docstring else 'SIP_NULLPTR'

        sf.write(
f'''
    sipNoMethod({sip_parse_err}, {klass_py_name_ref}, {member_py_name_ref}, {docstring_ref});

    return SIP_NULLPTR;
''')

    sf.write('}\n')


def _function_body(sf, spec, bindings, scope, overload, original_klass=None,
        dereferenced=True):
    """ Generate the function calls for a particular overload. """

    if scope is None:
        original_scope = None
    elif isinstance(scope, WrappedClass):
        # If there was no original class (ie. where a virtual was first
        # defined) then use this class,
        if original_klass is None:
            original_klass = scope

        original_scope = original_klass
    else:
        original_scope = scope

    py_signature = overload.py_signature

    sf.write('\n    {\n')

    # In case we have to fiddle with it.
    py_signature_adjusted = False

    if is_number_slot(overload.common.py_slot):
        # Number slots must have two arguments because we parse them slightly
        # differently.
        if len(py_signature.args) == 1:
            py_signature.args.append(py_signature.args[0])

            # Insert self in the right place.
            py_signature.args[0] = Argument(ArgumentType.CLASS, is_in=True,
                    is_reference=True, definition=original_klass)

            py_signature_adjusted = True

        _arg_parser(sf, spec, scope, py_signature, overload=overload)
    elif not is_int_arg_slot(overload.common.py_slot) and not is_zero_arg_slot(overload.common.py_slot):
        _arg_parser(sf, spec, scope, py_signature, overload=overload)

    _function_call(sf, spec, bindings, scope, overload, dereferenced,
            original_scope)

    sf.write('    }\n')

    if py_signature_adjusted:
        del overload.py_signature.args[0]


def _handle_result(sf, spec, overload, is_new_instance, result_size_arg_nr,
        action):
    """ Generate the code to handle the result of a call to a member function.
    """

    result = overload.py_signature.result

    if result.type is ArgumentType.VOID and len(result.derefs) == 0:
        result = None

    # See if we are returning 0, 1 or more values.
    nr_return_values = 0

    if result is not None:
        only_out_arg_nr = -1
        nr_return_values += 1

    has_owner = False

    for arg_nr, arg in enumerate(overload.py_signature.args):
        if arg.is_out:
            only_out_arg_nr = arg_nr
            nr_return_values += 1

        if arg.transfer is Transfer.TRANSFER_THIS:
            has_owner = True

    # Handle the trivial case.
    if nr_return_values == 0:
        sf.write(
f'''            Py_INCREF(Py_None);
            {action} Py_None;
''')

        return

    # Handle results that are classes or mapped types separately.
    if result is not None and result.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        result_gto_name = _gto_name(result.definition)

        if overload.transfer is Transfer.TRANSFER_BACK:
            result_owner = 'Py_None'
        elif overload.transfer is Transfer.TRANSFER:
            result_owner = 'sipSelf'
        else:
            result_owner = 'SIP_NULLPTR'

        sip_res = _const_cast(spec, result, 'sipRes')

        if is_new_instance or overload.factory:
            this_action = action if nr_return_values == 1 else 'PyObject *sipResObj ='
            owner = '(PyObject *)sipOwner' if has_owner and overload.factory else result_owner

            sf.write(f'            {this_action} sipConvertFromNewType({sip_res}, {result_gto_name}, {owner});\n')

            # Shortcut if this is the only value returned.
            if nr_return_values == 1:
                return
        else:
            need_xfer = overload.transfer is Transfer.TRANSFER and overload.is_static

            this_action = 'PyObject *sipResObj =' if nr_return_values > 1 or need_xfer else action
            owner = 'SIP_NULLPTR' if need_xfer else result_owner

            sf.write(f'            {this_action} sipConvertFromType({sip_res}, {result_gto_name}, {owner});\n')

            # Transferring the result of a static overload needs an explicit
            # call to sipTransferTo().
            if need_xfer:
                sf.write('\n           sipTransferTo(sipResObj, Py_None);\n')

            # Shortcut if this is the only value returned.
            if nr_return_values == 1:
                if need_xfer:
                    sf.write('\n           return sipResObj;\n')

                return

    # If there are multiple values then build a tuple.
    if nr_return_values > 1:
        build_result_args = ['0']

        # Build the format string.
        format_s = ''

        if result is not None:
            format_s += 'R' if result.type in (ArgumentType.CLASS, ArgumentType.MAPPED) else _get_build_result_format(result)

        for arg in overload.py_signature.args:
            if arg.is_out:
                format_s += _get_build_result_format(arg)

        build_result_args.append('"(' + format_s + ')"')

        # Pass the values for conversion.
        if result is not None:
            build_result_args.append('sipResObj' if result.type in (ArgumentType.CLASS, ArgumentType.MAPPED) else 'sipRes')

            if result.type is ArgumentType.ENUM and result.definition.fq_cpp_name is not None:
                build_result_args.append(_gto_name(result.definition))

        for arg_nr, arg in enumerate(overload.py_signature.args):
            if arg.is_out:
                build_result_args.append(fmt_argument_as_name(spec, arg,
                        arg_nr))

                if arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
                    build_result_args.append(_gto_name(arg.definition))

                    transfer = 'Py_None' if arg.transfer is Transfer.TRANSFER_BACK else 'SIP_NULLPTR'
                    build_result_args.append(transfer)
                elif arg.type is ArgumentType.ENUM and arg.definition.fq_cpp_name is not None:
                    build_result_args.append(_gto_name(arg.definition))

        build_result_args = ', '.join(build_result_args)

        sf.write(f'            {action} sipBuildResult({build_result_args});\n')

        # All done for multiple values.
        return

    # Deal with the only returned value.
    if only_out_arg_nr < 0:
        value = result
        value_name = 'sipRes'
    else:
        value = overload.py_signature.args[only_out_arg_nr]
        value_name = fmt_argument_as_name(spec, value, only_out_arg_nr)

    if value.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        need_new_instance = _need_new_instance(value)

        convertor = 'sipConvertFromNewType' if need_new_instance else 'sipConvertFromType'
        value_name = _const_cast(spec, value, value_name)
        transfer = 'Py_None' if not need_new_instance and value.transfer is Transfer.TRANSFER_BACK else 'SIP_NULLPTR'

        sf.write(f'            {action} {convertor}({value_name}, {_gto_name(value.definition)}, {transfer});\n')

    elif value.type is ArgumentType.ENUM:
        if value.definition.fq_cpp_name is not None:
            if not spec.c_bindings:
                value_name = f'static_cast<int>({value_name})'

            sf.write(f'            {action} sipConvertFromEnum({value_name}, {_gto_name(value.definition)});\n')
        else:
            sf.write(f'            {action} PyLong_FromLong({value_name});\n')

    elif value.type is ArgumentType.ASCII_STRING:
        if len(value.derefs) == 0:
            sf.write(f'            {action} PyUnicode_DecodeASCII(&{value_name}, 1, SIP_NULLPTR);\n')
        else:
            sf.write(
f'''            if ({value_name} == SIP_NULLPTR)
            {{
                Py_INCREF(Py_None);
                return Py_None;
            }}

            {action} PyUnicode_DecodeASCII({value_name}, strlen({value_name}), SIP_NULLPTR);
''')

    elif value.type is ArgumentType.LATIN1_STRING:
        if len(value.derefs) == 0:
            sf.write(f'            {action} PyUnicode_DecodeLatin1(&{value_name}, 1, SIP_NULLPTR);\n')
        else:
            sf.write(
f'''            if ({value_name} == SIP_NULLPTR)
            {{
                Py_INCREF(Py_None);
                return Py_None;
            }}

            {action} PyUnicode_DecodeLatin1({value_name}, strlen({value_name}), SIP_NULLPTR);
''')

    elif value.type is ArgumentType.UTF8_STRING:
        if len(value.derefs) == 0:
            sf.write(f'            {action} PyUnicode_FromStringAndSize(&{value_name}, 1);\n')
        else:
            sf.write(
f'''            if ({value_name} == SIP_NULLPTR)
            {{
                Py_INCREF(Py_None);
                return Py_None;
            }}

            {action} PyUnicode_FromString({value_name});
''')

    elif value.type in (ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING):
        cast = '' if value.type is ArgumentType.STRING else '(char *)'

        if len(value.derefs) == 0:
            sf.write(f'            {action} PyBytes_FromStringAndSize({cast}&{value_name}, 1);\n')
        else:
            sf.write(
f'''            if ({value_name} == SIP_NULLPTR)
            {{
                Py_INCREF(Py_None);
                return Py_None;
            }}

            {action} PyBytes_FromString({cast}{value_name});
''')

    elif value.type is ArgumentType.WSTRING:
        if len(value.derefs) == 0:
            sf.write(f'            {action} PyUnicode_FromWideChar(&{value_name}, 1);\n')
        else:
            sf.write(
f'''            if ({value_name} == SIP_NULLPTR)
            {{
                Py_INCREF(Py_None);
                return Py_None;
            }}

            {action} PyUnicode_FromWideChar({value_name}, (Py_ssize_t)wcslen({value_name}));
''')

    elif value.type in (ArgumentType.BOOL, ArgumentType.CBOOL):
        sf.write(f'            {action} PyBool_FromLong({value_name});\n')

    elif value.type in (ArgumentType.BYTE, ArgumentType.SBYTE, ArgumentType.SHORT, ArgumentType.INT, ArgumentType.CINT, ArgumentType.LONG):
        sf.write(f'            {action} PyLong_FromLong({value_name});\n')

    elif value.type in (ArgumentType.UBYTE, ArgumentType.USHORT, ArgumentType.UINT, ArgumentType.ULONG, ArgumentType.SIZE):
        sf.write(f'            {action} PyLong_FromUnsignedLong({value_name});\n')

    elif value.type is ArgumentType.LONGLONG:
        sf.write(f'            {action} PyLong_FromLongLong({value_name});\n')

    elif value.type is ArgumentType.ULONGLONG:
        sf.write(f'            {action} PyLong_FromUnsignedLongLong({value_name});\n')

    elif value.type is ArgumentType.SSIZE:
        sf.write(f'            {action} PyLong_FromSsize_t({value_name});\n')

    elif value.type is ArgumentType.VOID:
        convertor = 'sipConvertFromConstVoidPtr' if value.is_const else 'sipConvertFromVoidPtr'
        if result_size_arg_nr >= 0:
            convertor += 'AndSize'

        sf.write(f'            {action} {convertor}({_get_void_ptr_cast(value)}{value_name}')

        if result_size_arg_nr >= 0:
            sf.write(', ' + fmt_argument_as_name(spec, overload.py_signature.args[result_size_arg_nr], result_size_arg_nr))

        sf.write(');\n')

    elif value.type is ArgumentType.CAPSULE:
        sf.write(f'            {action} PyCapsule_New({value_name}, "{value.definition.as_cpp}", SIP_NULLPTR);\n')

    elif value.type in (ArgumentType.STRUCT, ArgumentType.UNION):
        convertor = 'sipConvertFromConstVoidPtr' if value.is_const else 'sipConvertFromVoidPtr'

        sf.write(f'            {action} {convertor}({value_name});\n')

    elif value.type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
        sf.write(f'            {action} PyFloat_FromDouble((double){value_name});\n')

    elif value.type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
        sf.write(f'            {action} PyFloat_FromDouble({value_name});\n')

    elif value.type in (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE, ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER, ArgumentType.PYENUM):
        sf.write(f'            {action} {value_name};\n')


def _get_build_result_format(type):
    """ Return the format string used by sipBuildResult() for a particular
    type.
    """

    if type.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        return 'N' if _need_new_instance(type) else 'D'

    if type.type is ArgumentType.FAKE_VOID:
        return 'D'

    if type.type in (ArgumentType.BOOL, ArgumentType.CBOOL):
        return 'b'

    if type.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING):
        return 'A' if _is_string(type) else 'a'

    if type.type in (ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING):
        return 's' if _is_string(type) else 'c'

    if type.type is ArgumentType.WSTRING:
        return 'x' if _is_string(type) else 'w'

    if type.type is ArgumentType.ENUM:
        return 'F' if type.definition.fq_cpp_name is not None else 'e'

    if type.type in (ArgumentType.BYTE, ArgumentType.SBYTE):
        # Note that this is the correct thing to do even if char is unsigned.
        return 'L'

    if type.type is ArgumentType.UBYTE:
        return 'M'

    if type.type is ArgumentType.SHORT:
        return 'h'

    if type.type is ArgumentType.USHORT:
        return 't'

    if type.type in (ArgumentType.INT, ArgumentType.CINT):
        return 'i'

    if type.type is ArgumentType.UINT:
        return 'u'

    if type.type is ArgumentType.SIZE:
        return '='

    if type.type is ArgumentType.LONG:
        return 'l'

    if type.type is ArgumentType.ULONG:
        return 'm'

    if type.type is ArgumentType.LONGLONG:
        return 'n'

    if type.type is ArgumentType.ULONGLONG:
        return 'o'

    if type.type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
        return 'V'

    if type.type is ArgumentType.CAPSULE:
        return 'z'

    if type.type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
        return 'f'

    if type.type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
        return 'd'

    if type.type in (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE, ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER, ArgumentType.PYENUM):
        return 'R'

    # We should never get here.
    return ''


def _is_string(type):
    """ Check if a type is a string rather than a char type. """

    nr_derefs = len(type.derefs)

    if type.is_out and not type.is_reference:
        nr_derefs -= 1

    return nr_derefs > 0


def _needs_heap_copy(arg, using_copy_ctor=True):
    """ Return True if an argument (or result) needs to be copied to the heap.
    """

    # The type is a class or mapped type and not a pointer.
    if not arg.no_copy and arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and len(arg.derefs) == 0:
        # We need a copy unless it is a non-const reference.
        if not arg.is_reference or arg.is_const:
            # We assume we can copy a mapped type.
            if arg.type is ArgumentType.MAPPED:
                return True

            klass = arg.definition

            # We can't copy an abstract class.
            if klass.is_abstract:
                return False

            # We can copy if we have a public copy ctor.
            if not klass.cannot_copy:
                return True

            # We can't copy if we must use a copy ctor.
            if using_copy_ctor:
                return False

            # We can copy if we have a public assignment operator.
            return not klass.cannot_assign

    return False


def _function_call(sf, spec, bindings, scope, overload, dereferenced,
        original_scope):
    """ Generate a function call. """

    py_slot = overload.common.py_slot
    result = overload.py_signature.result
    result_cpp_type = fmt_argument_as_cpp_type(spec, result, plain=True,
            no_derefs=True)
    static_factory = (scope is None or overload.is_static) and overload.factory

    sf.write('        {\n')

    # If there is no shadow class then protected methods can never be called.
    if overload.access_specifier is AccessSpecifier.PROTECTED and not scope.has_shadow:
        sf.write(
'''            /* Never reached. */
        }
''')

        return

    # Save the full result type as we may want to fiddle with it.
    saved_result_is_const = result.is_const

    # See if we need to make a copy of the result on the heap.
    is_new_instance = _needs_heap_copy(result, using_copy_ctor=False)

    if is_new_instance:
        result.is_const = False

    result_decl = _get_result_decl(spec, scope, overload, result)
    if result_decl is not None:
        sf.write('            ' + result_decl + ';\n')
        separating_newline = True
    else:
        separating_newline = False

    # See if we want to keep a reference to the result.
    post_process = result.key is not None

    delete_temporaries = True
    result_size_arg_nr = -1

    for arg_nr, arg in enumerate(overload.py_signature.args):
        if arg.result_size:
            result_size_arg_nr = arg_nr

        if static_factory and arg.key is not None:
            post_process = True

        # If we have an In,Out argument that has conversion code then we delay
        # the destruction of any temporary variables until after we have
        # converted the outputs.
        if arg.is_in and arg.is_out and _get_convert_to_type_code(arg) is not None:
            delete_temporaries = False
            post_process = True

        # If we are returning a class via an output only reference or pointer
        # then we need an instance on the heap.
        if arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and _need_new_instance(arg):
            arg_name = fmt_argument_as_name(spec, arg, arg_nr)
            arg_cpp_type = fmt_argument_as_cpp_type(spec, arg, plain=True,
                    no_derefs=True)
            sf.write(f'            {arg_name} = new {arg_cpp_type}();\n')
            separating_newline = True

    if post_process:
        sf.write('            PyObject *sipResObj;\n')
        separating_newline = True

    if overload.premethod_code is not None:
        sf.write('\n')
        sf.write_code(overload.premethod_code)

    error_flag = old_error_flag = False

    if overload.method_code is not None:
        # See if the handwritten code seems to be using the error flag.
        if _need_error_flag(overload.method_code):
            sf.write('            sipErrorState sipError = sipErrorNone;\n')
            error_flag = True
            separating_newline = True
        elif _need_old_error_flag(overload.method_code):
            sf.write('            int sipIsErr = 0;\n')
            old_error_flag = True
            separating_newline = True

    if separating_newline:
        sf.write('\n')

    # If it is abstract make sure that self was bound.
    if overload.is_abstract:
        sf.write(
f'''            if (!sipOrigSelf)
            {{
                sipAbstractMethod({_cached_name_ref(scope.py_name)}, {_cached_name_ref(overload.common.py_name)});
                return SIP_NULLPTR;
            }}

''')

    if overload.deprecated is not None:
        scope_py_name_ref = _cached_name_ref(scope.py_name) if scope is not None and scope.py_name is not None else 'SIP_NULLPTR'
        error_return = '-1' if is_void_return_slot(py_slot) or is_int_return_slot(py_slot) or is_ssize_return_slot(py_slot) or is_hash_return_slot(py_slot) else 'SIP_NULLPTR'

        # Note that any temporaries will leak if an exception is raised.
        if abi_has_deprecated_message(spec):
            str_deprecated_message = f'"{overload.deprecated}"' if overload.deprecated else 'SIP_NULLPTR'
            sf.write(f'            if (sipDeprecated({scope_py_name_ref}, {_cached_name_ref(overload.common.py_name)}, {str_deprecated_message}) < 0)\n')
        else:
            sf.write(f'            if (sipDeprecated({scope_py_name_ref}, {_cached_name_ref(overload.common.py_name)}) < 0)\n')
        
        sf.write(f'                return {error_return};\n\n')

    # Call any pre-hook.
    if overload.prehook is not None:
        sf.write(f'            sipCallHook("{overload.prehook}");\n\n')

    if overload.method_code is not None:
        sf.write_code(overload.method_code)
    else:
        release_gil = _release_gil(overload.gil_action, bindings)
        needs_closing_paren = False

        if is_new_instance and spec.c_bindings:
            sf.write(
f'''            if ((sipRes = ({result_cpp_type} *)sipMalloc(sizeof ({result_cpp_type}))) == SIP_NULLPTR)
        {{
''')

            _gc_ellipsis(sf, overload.py_signature)

            sf.write(
'''                return SIP_NULLPTR;
            }

''')

        if overload.raises_py_exception:
            sf.write('            PyErr_Clear();\n\n')

        if isinstance(scope, WrappedClass) and scope.len_cpp_name is not None:
            _sequence_support(sf, spec, scope, overload)

        if release_gil:
            sf.write('            Py_BEGIN_ALLOW_THREADS\n')

        _try(sf, bindings, overload.throw_args)

        sf.write('            ')

        if result_decl is not None:
            # Construct a copy on the heap if needed.
            if is_new_instance:
                if spec.c_bindings:
                    sf.write('*sipRes = ')
                elif result.type is ArgumentType.CLASS and result.definition.cannot_copy:
                    sf.write(f'sipRes = reinterpret_cast<{result_cpp_type} *>(::operator new(sizeof ({result_cpp_type})));\n            *sipRes = ')
                else:
                    sf.write(f'sipRes = new {result_cpp_type}(')
                    needs_closing_paren = True
            else:
                sf.write('sipRes = ')

                # See if we need the address of the result.  Any reference will
                # be non-const.
                if result.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and (len(result.derefs) == 0 or result.is_reference):
                    sf.write('&')

        if py_slot is None:
            _cpp_function_call(sf, spec, scope, overload, original_scope)
        elif py_slot is PySlot.CALL:
            sf.write('(*sipCpp)(')
            _call_args(sf, spec, overload.cpp_signature, overload.py_signature)
            sf.write(')')
        else:
            sf.write(_get_slot_call(spec, scope, overload, dereferenced))

        if needs_closing_paren:
            sf.write(')')

        sf.write(';\n')

        _catch(sf, spec, bindings, overload.py_signature, overload.throw_args,
                release_gil)

        if release_gil:
            sf.write('            Py_END_ALLOW_THREADS\n')

    for arg_nr, arg in enumerate(overload.py_signature.args):
        if not arg.is_in:
            continue

        # Handle any /KeepReference/ arguments except for static factories.
        if not static_factory and arg.key is not None:
            sip_self = 'SIP_NULLPTR' if scope is None or overload.is_static else 'sipSelf'
            keep_reference_call = _get_keep_reference_call(spec, arg, arg_nr,
                    sip_self)

            sf.write(f'\n            {keep_reference_call};\n')

        # Handle /TransferThis/ for non-factory methods.
        if not overload.factory and arg.transfer is Transfer.TRANSFER_THIS:
            sf.write(
'''
            if (sipOwner)
                sipTransferTo(sipSelf, (PyObject *)sipOwner);
            else
                sipTransferBack(sipSelf);
''')

    if overload.transfer is Transfer.TRANSFER_THIS:
        sf.write('\n            sipTransferTo(sipSelf, SIP_NULLPTR);\n')

    _gc_ellipsis(sf, overload.py_signature)

    if delete_temporaries and not is_zero_arg_slot(py_slot):
        _delete_temporaries(sf, spec, overload.py_signature)

    sf.write('\n')

    # Handle the error flag if it was used.
    error_value = '-1' if is_void_return_slot(py_slot) or is_int_return_slot(py_slot) or is_ssize_return_slot(py_slot) or is_hash_return_slot(py_slot) else '0'

    if overload.raises_py_exception:
        sf.write(
f'''            if (PyErr_Occurred())
                return {error_value};

''')
    elif error_flag:
        if not is_zero_arg_slot(py_slot):
            sf.write(
f'''            if (sipError == sipErrorFail)
                return {error_value};

''')

        sf.write(
'''            if (sipError == sipErrorNone)
            {
''')
    elif old_error_flag:
        sf.write(
f'''            if (sipIsErr)
                return {error_value};

''')

    # Call any post-hook.
    if overload.posthook is not None:
        sf.write(f'\n            sipCallHook("{overload.posthook}");\n')

    if is_void_return_slot(py_slot):
        sf.write(
'''            return 0;
''')
    elif is_inplace_number_slot(py_slot) or is_inplace_sequence_slot(py_slot):
        sf.write(
'''            Py_INCREF(sipSelf);
            return sipSelf;
''')
    elif is_int_return_slot(py_slot) or is_ssize_return_slot(py_slot) or is_hash_return_slot(py_slot):
        sf.write(
'''            return sipRes;
''')
    else:
        action = 'sipResObj =' if post_process else 'return'
        _handle_result(sf, spec, overload, is_new_instance, result_size_arg_nr,
                action)

        # Delete the temporaries now if we haven't already done so.
        if not delete_temporaries:
            _delete_temporaries(sf, spec, overload.py_signature)

        # Keep a reference to a pointer to a class if it isn't owned by Python.
        if result.key is not None:
            sip_self = 'SIP_NULLPTR' if overload.is_static else 'sipSelf'
            sf.write(f'\n            sipKeepReference({sip_self}, {result.key}, sipResObj);\n')

        # Keep a reference to any argument with the result if the function is a
        # static factory.
        if static_factory:
            for arg_nr, arg in enumerate(overload.py_signature.args):
                if not arg.is_in:
                    continue

                if arg.key != None:
                    keep_reference_call = _get_keep_reference_call(spec, arg,
                            arg_nr, 'sipResObj')
                    sf.write(f'\n            {keep_reference_call};\n')

        if post_process:
            sf.write('\n            return sipResObj;\n')

    if error_flag:
        sf.write('            }\n')

        if not is_zero_arg_slot(py_slot):
            sf.write('\n            sipAddException(sipError, &sipParseErr);\n')

    sf.write('        }\n')

    # Restore the full state of the result.
    result.is_const = saved_result_is_const


def _get_keep_reference_call(spec, arg, arg_nr, object_name):
    """ Return a call to sipKeepReference() for an argument. """

    arg_name = fmt_argument_as_name(spec, arg, arg_nr)
    suffix = 'Wrapper' if arg.get_wrapper and (arg.type not in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING) or len(arg.derefs) != 1) else 'Keep'

    return f'sipKeepReference({object_name}, {arg.key}, {arg_name}{suffix})'


def _get_result_decl(spec, scope, overload, result):
    """ Return the declaration of a variable to hold the result of a function
    call if one is needed.
    """

    # See if sipRes is needed.
    no_result = (is_inplace_number_slot(overload.common.py_slot) or
             is_inplace_sequence_slot(overload.common.py_slot) or
             (result.type is ArgumentType.VOID and len(result.derefs) == 0))

    if no_result:
        return None

    result_decl = _get_named_value_decl(spec, scope, result, 'sipRes')

    # The typical %MethodCode usually causes a compiler warning, so we
    # initialise the result in that case to try and suppress it.
    initial_value = ' = ' + _cast_zero(spec, result) if overload.method_code is not None else ''

    return result_decl + initial_value


def _get_slot_call(spec, scope, overload, dereferenced):
    """ Return the call to a Python slot (except for PySlot.CALL which is
    handled separately).
    """

    py_slot = overload.common.py_slot

    if py_slot is PySlot.GETITEM:
        return f'(*sipCpp)[{_get_slot_arg(spec, overload, 0)}]'

    if py_slot in (PySlot.INT, PySlot.FLOAT):
        return '*sipCpp'

    if py_slot is PySlot.ADD:
        return _get_number_slot_call(spec, overload, '+')

    if py_slot is PySlot.CONCAT:
        return _get_binary_slot_call(spec, scope, overload, '+', dereferenced)

    if py_slot is PySlot.SUB:
        return _get_number_slot_call(spec, overload, '-')

    if py_slot in (PySlot.MUL, PySlot.MATMUL):
        return _get_number_slot_call(spec, overload, '*')

    if py_slot is PySlot.REPEAT:
        return _get_binary_slot_call(spec, scope, overload, '*', dereferenced)

    if py_slot is PySlot.TRUEDIV:
        return _get_number_slot_call(spec, overload, '/')

    if py_slot is PySlot.MOD:
        return _get_number_slot_call(spec, overload, '%')

    if py_slot is PySlot.AND:
        return _get_number_slot_call(spec, overload, '&')

    if py_slot is PySlot.OR:
        return _get_number_slot_call(spec, overload, '|')

    if py_slot is PySlot.XOR:
        return _get_number_slot_call(spec, overload, '^')

    if py_slot is PySlot.LSHIFT:
        return _get_number_slot_call(spec, overload, '<<')

    if py_slot is PySlot.RSHIFT:
        return _get_number_slot_call(spec, overload, '>>')

    if py_slot in (PySlot.IADD, PySlot.ICONCAT):
        return _get_binary_slot_call(spec, scope, overload, '+=', dereferenced)

    if py_slot is PySlot.ISUB:
        return _get_binary_slot_call(spec, scope, overload, '-=', dereferenced)

    if py_slot in (PySlot.IMUL, PySlot.IREPEAT, PySlot.IMATMUL):
        return _get_binary_slot_call(spec, scope, overload, '*=', dereferenced)

    if py_slot is PySlot.ITRUEDIV:
        return _get_binary_slot_call(spec, scope, overload, '/=', dereferenced)

    if py_slot is PySlot.IMOD:
        return _get_binary_slot_call(spec, scope, overload, '%=', dereferenced)

    if py_slot is PySlot.IAND:
        return _get_binary_slot_call(spec, scope, overload, '&=', dereferenced)

    if py_slot is PySlot.IOR:
        return _get_binary_slot_call(spec, scope, overload, '|=', dereferenced)

    if py_slot is PySlot.IXOR:
        return _get_binary_slot_call(spec, scope, overload, '^=', dereferenced)

    if py_slot is PySlot.ILSHIFT:
        return _get_binary_slot_call(spec, scope, overload, '<<=', dereferenced)

    if py_slot is PySlot.IRSHIFT:
        return _get_binary_slot_call(spec, scope, overload, '>>=', dereferenced)

    if py_slot is PySlot.INVERT:
        return '~(*sipCpp)'

    if py_slot is PySlot.LT:
        return _get_binary_slot_call(spec, scope, overload, '<', dereferenced)

    if py_slot is PySlot.LE:
        return _get_binary_slot_call(spec, scope, overload, '<=', dereferenced)

    if py_slot is PySlot.EQ:
        return _get_binary_slot_call(spec, scope, overload, '==', dereferenced)

    if py_slot is PySlot.NE:
        return _get_binary_slot_call(spec, scope, overload, '!=', dereferenced)

    if py_slot is PySlot.GT:
        return _get_binary_slot_call(spec, scope, overload, '>', dereferenced)

    if py_slot is PySlot.GE:
        return _get_binary_slot_call(spec, scope, overload, '>=', dereferenced)

    if py_slot is PySlot.NEG:
        return '-(*sipCpp)'

    if py_slot is PySlot.POS:
        return '+(*sipCpp)'

    # We should never get here.
    return ''


def _cpp_function_call(sf, spec, scope, overload, original_scope):
    """ Generate a call to a C++ function. """

    cpp_name = overload.cpp_name

    # If the function is protected then call the public wrapper.  If it is
    # virtual then call the explicit scoped function if "self" was passed as
    # the first argument.

    nr_parens = 1

    if scope is None:
        sf.write(cpp_name + '(')
    elif scope.iface_file.type is IfaceFileType.NAMESPACE:
        sf.write(f'{scope.iface_file.fq_cpp_name.as_cpp}::{cpp_name}(')
    elif overload.is_static:
        if overload.access_specifier is AccessSpecifier.PROTECTED:
            sf.write(f'sip{scope.iface_file.fq_cpp_name.as_word}::sipProtect_{cpp_name}(')
        else:
            sf.write(f'{original_scope.iface_file.fq_cpp_name.as_cpp}::{cpp_name}(')
    elif overload.access_specifier is AccessSpecifier.PROTECTED:
        if not overload.is_abstract and (overload.is_virtual or overload.is_virtual_reimplementation):
            sf.write(f'sipCpp->sipProtectVirt_{cpp_name}(sipSelfWasArg')

            if len(overload.cpp_signature.args) != 0:
                sf.write(', ')
        else:
            sf.write(f'sipCpp->sipProtect_{cpp_name}(')
    elif not overload.is_abstract and (overload.is_virtual or overload.is_virtual_reimplementation):
        sf.write(f'(sipSelfWasArg ? sipCpp->{original_scope.iface_file.fq_cpp_name.as_cpp}::{cpp_name}(')
        _call_args(sf, spec, overload.cpp_signature, overload.py_signature)
        sf.write(f') : sipCpp->{cpp_name}(')
        nr_parens += 1
    else:
        sf.write(f'sipCpp->{cpp_name}(')

    _call_args(sf, spec, overload.cpp_signature, overload.py_signature)

    sf.write(')' * nr_parens)


def _get_slot_arg(spec, overload, arg_nr):
    """ Return an argument to a slot call. """

    arg = overload.py_signature.args[arg_nr]

    dereference = '*' if arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and len(arg.derefs) == 0 else ''

    return dereference + fmt_argument_as_name(spec, arg, arg_nr)


# A map of operators and their complements.
_OPERATOR_COMPLEMENTS = {
    '<': '>=',
    '<=': '>',
    '==': '!=',
    '!=': '==',
    '>': '<=',
    '>=': '<',
}

def _get_binary_slot_call(spec, scope, overload, operator, dereferenced):
    """ Return the call to a binary (non-number) slot method. """

    slot_call = ''

    if overload.is_complementary:
        operator = _OPERATOR_COMPLEMENTS[operator]
        slot_call += '!'

    if overload.is_global:
        # If it has been moved from a namespace then get the C++ scope.
        if overload.common.namespace_iface_file is not None:
            slot_call += overload.common.namespace_iface_file.fq_cpp_name.as_cpp + '::'

        if dereferenced:
            slot_call += f'operator{operator}((*sipCpp), '
        else:
            slot_call += f'operator{operator}(sipCpp, '
    else:
        dereference = '->' if dereferenced else '.'

        if overload.is_abstract:
            slot_call += f'sipCpp{dereference}operator{operator}('
        else:
            slot_call += f'sipCpp{dereference}{scope.iface_file.fq_cpp_name.as_cpp}::operator{operator}('

    slot_call += _get_slot_arg(spec, overload, 0)
    slot_call += ')'

    return slot_call


def _get_number_slot_call(spec, overload, operator):
    """ Return the call to a binary number slot method. """

    arg0 = _get_slot_arg(spec, overload, 0)
    arg1 = _get_slot_arg(spec, overload, 1)

    return f'({arg0} {operator} {arg1})'


def _arg_parser(sf, spec, scope, py_signature, ctor=None, overload=None):
    """ Generate the argument variables for a member
    function/constructor/operator.
    """

    # If the scope is just a namespace, then ignore it.
    if isinstance(scope, WrappedClass) and scope.iface_file.type is IfaceFileType.NAMESPACE:
        scope = None

    # For ABI v13 and later static methods use self for the type object.
    if spec.abi_version >= (13, 0):
        handle_self = (scope is not None and overload is not None and overload.common.py_slot is None)
    else:
        handle_self = (scope is not None and overload is not None and overload.common.py_slot is None and not overload.is_static)

    # Generate the local variables that will hold the parsed arguments and
    # values returned via arguments.
    array_len_arg_nr = -1
    need_owner = False
    ctor_needs_self = False

    for arg_nr, arg in enumerate(py_signature.args):
        if arg.array is ArrayArgument.ARRAY_SIZE:
            array_len_arg_nr = arg_nr

        _argument_variable(sf, spec, scope, arg, arg_nr)

        if arg.transfer is Transfer.TRANSFER_THIS:
            need_owner = True

        if ctor is not None and arg.transfer is Transfer.TRANSFER:
            ctor_needs_self = True

    if overload is not None and need_owner:
        sf.write('        sipWrapper *sipOwner = SIP_NULLPTR;\n')

    if handle_self and not overload.is_static:
        cpp_type = 'const ' if overload.is_const else ''

        if overload.access_specifier is AccessSpecifier.PROTECTED and scope.has_shadow:
            cpp_type += 'sip' + scope.iface_file.fq_cpp_name.as_word
        else:
            cpp_type += _scoped_class_name(spec, scope)

        sf.write(f'        {cpp_type} *sipCpp;\n\n')
    elif len(py_signature.args) != 0:
        sf.write('\n')

    # Generate the call to the parser function.
    args = []
    single_arg = False

    if overload is not None and is_number_slot(overload.common.py_slot):
        parser_function = 'sipParsePair'
        args.append('&sipParseErr')
        args.append('sipArg0')
        args.append('sipArg1')

    elif overload is not None and overload.common.py_slot is PySlot.SETATTR:
        # We don't even try to invoke the parser if there is a value and there
        # shouldn't be (or vice versa) so that the list of errors doesn't get
        # polluted with signatures that can never apply.
        if overload.is_delattr:
            operator = '=='
            sip_value = 'SIP_NULLPTR'
        else:
            operator = '!='
            sip_value = 'sipValue'

        parser_function = f'sipValue {operator} SIP_NULLPTR && sipParsePair'
        args.append('&sipParseErr')
        args.append('sipName')
        args.append(sip_value)

    elif (overload is not None and overload.common.allow_keyword_args) or ctor is not None:
        # We handle keywords if we might have been passed some (because one of
        # the overloads uses them or we are a ctor).  However this particular
        # overload might not have any.
        if overload is not None:
            kw_args = overload.kw_args
        elif ctor is not None:
            kw_args = ctor.kw_args
        else:
            kw_args = KwArgs.NONE

        # The above test isn't good enough because when the flags were set in
        # the parser we couldn't know for sure if an argument was an output
        # pointer.  Therefore we check here.  The drawback is that we may
        # generate the name string for the argument but never use it, or we
        # might have an empty keyword name array or one that contains only
        # NULLs.
        is_ka_list = False

        if kw_args is not KwArgs.NONE:
            for arg in py_signature.args:
                if not arg.is_in:
                    continue

                if not is_ka_list:
                    sf.write('        static const char *sipKwdList[] = {\n')
                    is_ka_list = True

                if arg.name is not None and (kw_args is KwArgs.ALL or arg.default_value is not None):
                    arg_name_ref = _cached_name_ref(arg.name)
                else:
                    arg_name_ref = 'SIP_NULLPTR'

                sf.write(f'            {arg_name_ref},\n')

            if is_ka_list:
                sf.write('        };\n\n')

        parser_function = 'sipParseKwdArgs'
        args.append('sipParseErr' if ctor is not None else '&sipParseErr')
        args.append('sipArgs')
        args.append('sipKwds')
        args.append('sipKwdList' if is_ka_list else 'SIP_NULLPTR')
        args.append('sipUnused' if ctor is not None else 'SIP_NULLPTR')

    else:
        single_arg = not (overload is None or overload.common.py_slot is None or is_multi_arg_slot(overload.common.py_slot))
        plural = '' if single_arg else 's'

        parser_function = 'sipParseArgs'
        args.append('&sipParseErr')
        args.append('sipArg' + plural)

    # Generate the format string.
    format_s = '"'
    optional_args = False

    if single_arg:
        format_s += '1'

    if ctor_needs_self:
        format_s += '#'
    elif handle_self:
        if overload.is_static:
            format_s += 'C'
        elif overload.access_is_really_protected:
            format_s += 'p'
        else:
            format_s += 'B'

    for arg in py_signature.args:
        if not arg.is_in:
            continue

        if arg.default_value is not None and not optional_args:
            format_s += '|'
            optional_args = True

        # Get the wrapper if explicitly asked for or we are going to keep a
        # reference to.  However if it is an encoded string then we will get
        # the actual wrapper from the format character.
        if arg.get_wrapper:
            format_s += '@'
        elif arg.key is not None:
            if not (arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING) and len(arg.derefs) == 1):
                format_s += '@'

        if arg.type is ArgumentType.ASCII_STRING:
            format_s += 'AA' if _is_string(arg) else 'aA'

        elif arg.type is ArgumentType.LATIN1_STRING:
            format_s += 'AL' if _is_string(arg) else 'aL'

        elif arg.type is ArgumentType.UTF8_STRING:
            format_s += 'A8' if _is_string(arg) else 'a8'

        elif arg.type in (ArgumentType.SSTRING, ArgumentType.USTRING, ArgumentType.STRING):
            if arg.array is ArrayArgument.ARRAY:
                format_s += 'k'
            elif _is_string(arg):
                format_s += 's'
            else:
                format_s += 'c'

        elif arg.type is ArgumentType.WSTRING:
            if arg.array is ArrayArgument.ARRAY:
                format_s += 'K'
            elif _is_string(arg):
                format_s += 'x'
            else:
                format_s += 'w'

        elif arg.type is ArgumentType.ENUM:
            if arg.definition.fq_cpp_name is None:
                format_s += 'e'
            elif arg.is_constrained:
                format_s += 'XE'
            else:
                format_s += 'E'

        elif arg.type is ArgumentType.BOOL:
            format_s += 'b'

        elif arg.type is ArgumentType.CBOOL:
            format_s += 'Xb'

        elif arg.type is ArgumentType.INT:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'i'

        elif arg.type is ArgumentType.UINT:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'u'

        elif arg.type is ArgumentType.SIZE:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += '='

        elif arg.type is ArgumentType.CINT:
            format_s += 'Xi'

        elif arg.type is ArgumentType.BYTE:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'I' if _abi_has_working_char_conversion(spec) else 'L'

        elif arg.type is ArgumentType.SBYTE:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'L'

        elif arg.type is ArgumentType.UBYTE:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'M'

        elif arg.type is ArgumentType.SHORT:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'h'

        elif arg.type is ArgumentType.USHORT:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 't'

        elif arg.type is ArgumentType.LONG:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'l'

        elif arg.type is ArgumentType.ULONG:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'm'

        elif arg.type is ArgumentType.LONGLONG:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'n'

        elif arg.type is ArgumentType.ULONGLONG:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                format_s += 'o'

        elif arg.type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
            format_s += 'v'

        elif arg.type is ArgumentType.CAPSULE:
            format_s += 'z'

        elif arg.type is ArgumentType.FLOAT:
            format_s += 'f'

        elif arg.type is ArgumentType.CFLOAT:
            format_s += 'Xf'

        elif arg.type is ArgumentType.DOUBLE:
            format_s += 'd'

        elif arg.type is ArgumentType.CDOUBLE:
            format_s += 'Xd'

        elif arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            if arg.array is ArrayArgument.ARRAY:
                format_s += '>' if arg.type is ArgumentType.CLASS and _abi_supports_array(spec) else 'r'
            else:
                format_s += 'J' + _get_subformat_char(arg)

        elif arg.type is ArgumentType.PYOBJECT:
            format_s += 'P' + _get_subformat_char(arg)

        elif arg.type in (ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYSLICE, ArgumentType.PYTYPE):
            format_s += 'N' if arg.allow_none else 'T'

        elif arg.type is ArgumentType.PYCALLABLE:
            format_s += 'H' if arg.allow_none else 'F'

        elif arg.type is ArgumentType.PYBUFFER:
            format_s += '$' if arg.allow_none else '!'

        elif arg.type is ArgumentType.PYENUM:
            format_s += '^' if arg.allow_none else '&'

        elif arg.type is ArgumentType.ELLIPSIS:
            format_s += 'W'

    format_s += '"'
    args.append(format_s)

    # Generate the parameters corresponding to the format string.
    if ctor_needs_self:
        args.append('sipSelf')
    elif handle_self:
        args.append('&sipSelf')

        if not overload.is_static:
            args.append(_gto_name(scope))
            args.append('&sipCpp')

    for arg_nr, arg in enumerate(py_signature.args):
        if not arg.is_in:
            continue

        arg_name = fmt_argument_as_name(spec, arg, arg_nr)
        arg_name_ref = '&' + arg_name

        # Use the wrapper name if it was explicitly asked for.
        if arg.get_wrapper:
            args.append(f'&{arg_name}Wrapper')
        elif arg.key is not None:
            args.append(f'&{arg_name}Keep')

        if arg.type is ArgumentType.MAPPED:
            mapped_type = arg.definition

            args.append(_gto_name(mapped_type))
            args.append(arg_name_ref)

            if arg.array is ArrayArgument.ARRAY:
                array_len_arg_name = fmt_argument_as_name(spec,
                        py_signature.args[array_len_arg_nr], array_len_arg_nr)
                args.append('&' + array_len_arg_name)
            elif mapped_type.convert_to_type_code is not None and not arg.is_constrained:
                args.append('SIP_NULLPTR' if mapped_type.no_release else f'&{arg_name}State')

                if mapped_type.needs_user_state:
                    args.append(f'&{arg_name}UserState')

        elif arg.type is ArgumentType.CLASS:
            klass = arg.definition

            args.append(_gto_name(klass))
            args.append(arg_name_ref)

            if arg.array is ArrayArgument.ARRAY:
                array_len_arg_name = fmt_argument_as_name(spec,
                        py_signature.args[array_len_arg_nr], array_len_arg_nr)
                args.append('&' + array_len_arg_name)

                if _abi_supports_array(spec):
                    args.append(f'&{arg_name}IsTemp')
            else:
                if arg.transfer is Transfer.TRANSFER_THIS:
                    args.append('sipOwner' if ctor is not None else '&sipOwner')

                if klass.convert_to_type_code is not None and not arg.is_constrained:
                    args.append(f'&{arg_name}State')

        elif arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING):
            if arg.key is None and len(arg.derefs) == 1:
                args.append(f'&{arg_name}Keep')

            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYTUPLE:
            args.append('&PyTuple_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYLIST:
            args.append('&PyList_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYDICT:
            args.append('&PyDict_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYSLICE:
            args.append('&PySlice_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.PYTYPE:
            args.append('&PyType_Type')
            args.append(arg_name_ref)

        elif arg.type is ArgumentType.ENUM:
            if arg.definition.fq_cpp_name is not None:
                args.append(_gto_name(arg.definition))

            args.append(arg_name_ref)

        elif arg.type is ArgumentType.CAPSULE:
            args.append('"' + arg.definition.as_cpp + '"')
            args.append(arg_name_ref)

        else:
            if arg.array is not ArrayArgument.ARRAY_SIZE:
                args.append(arg_name_ref)

            if arg.array is ArrayArgument.ARRAY:
                array_len_arg_name = fmt_argument_as_name(spec,
                        py_signature.args[array_len_arg_nr], array_len_arg_nr)
                args.append('&' + array_len_arg_name)

    args = ', '.join(args)

    sf.write(f'        if ({parser_function}({args}))\n')


def _get_subformat_char(arg):
    """ Return the sub-format character for an argument. """

    flags = 0

    if arg.transfer is Transfer.TRANSFER:
        flags |= 0x02

    if arg.transfer is Transfer.TRANSFER_BACK:
        flags |= 0x04

    if arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
        if len(arg.derefs) == 0 or arg.disallow_none:
            flags |= 0x01

        if arg.transfer is Transfer.TRANSFER_THIS:
            flags |= 0x10

        if arg.is_constrained or (arg.type is ArgumentType.CLASS and arg.definition.convert_to_type_code is None):
            flags |= 0x08

    return chr(ord('0') + flags)


def _get_convert_to_type_code(type):
    """ Return a type's %ConvertToTypeCode. """

    if type.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and not type.is_constrained:
        return type.definition.convert_to_type_code

    return None


def _gc_ellipsis(sf, signature):
    """ Generate the code to garbage collect any ellipsis argument. """

    last = len(signature.args) - 1

    if last >= 0 and signature.args[last].type is ArgumentType.ELLIPSIS:
        sf.write(f'\n            Py_DECREF(a{last});\n')


def _delete_outs(sf, spec, py_signature):
    """ Generate the code to delete any instances created to hold /Out/
    arguments.
    """

    for arg_nr, arg in enumerate(py_signature.args):
        if arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and _need_new_instance(arg):
            sf.write(f'                delete {fmt_argument_as_name(spec, arg, arg_nr)};\n')



def _delete_temporaries(sf, spec, py_signature):
    """ Generate the code to delete any temporary variables on the heap created
    by type convertors.
    """

    for arg_nr, arg in enumerate(py_signature.args):
        arg_name = fmt_argument_as_name(spec, arg, arg_nr)

        if arg.array is ArrayArgument.ARRAY and arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED):
            if arg.transfer is not Transfer.TRANSFER:
                extra_indent = ''

                if arg.type is ArgumentType.CLASS and _abi_supports_array(spec):
                    sf.write(f'            if ({arg_name}IsTemp)\n')
                    extra_indent = '    '

                if spec.c_bindings:
                    sf.write(f'            {extra_indent}sipFree({arg_name});\n')
                else:
                    sf.write(f'            {extra_indent}delete[] {arg_name};\n')

            continue

        if not arg.is_in:
            continue

        if arg.type in (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING) and len(arg.derefs) == 1:
            decref = 'Py_XDECREF' if arg.default_value is not None else 'Py_DECREF'

            sf.write(f'            {decref}({arg_name}Keep);\n')

        elif arg.type is ArgumentType.WSTRING and len(arg.derefs) == 1:
            if spec.c_bindings or not arg.is_const:
                sf.write(f'            sipFree({arg_name});\n')
            else:
                sf.write(f'            sipFree(const_cast<wchar_t *>({arg_name}));\n')

        else:
            convert_to_type_code = _get_convert_to_type_code(arg)

            if convert_to_type_code is not None:
                if arg.type is ArgumentType.MAPPED and arg.definition.no_release:
                    continue

                sf.write(f'            sipReleaseType{_user_state_suffix(spec, arg)}(')

                if spec.c_bindings or not arg.is_const:
                    sf.write(arg_name)
                else:
                    arg_cpp_plain = fmt_argument_as_cpp_type(spec, arg,
                            plain=True, no_derefs=True)
                    sf.write(f'const_cast<{arg_cpp_plain} *>({arg_name})')

                sf.write(f', {_gto_name(arg.definition)}, {arg_name}State')

                if _type_needs_user_state(arg):
                    sf.write(f', {arg_name}UserState')

                sf.write(');\n')


def _get_normalised_cached_name(cached_name):
    """ Return the normalised form of a cached name. """

    # If the name seems to be a template then just use the offset to ensure
    # that it is unique.
    if '<' in cached_name.name:
        return str(cached_name.offset)

    # Handle C++ and Python scopes.
    return cached_name.name.replace(':', '_').replace('.', '_')


def _scoped_class_name(spec, klass):
    """ Return a scoped class name as a string.  Protected classes have to be
    explicitly scoped.
    """

    return fmt_class_as_scoped_name(spec, klass, scope=klass.iface_file)


def _need_error_flag(code):
    """ Return True if handwritten code uses the error flag. """

    return _is_used_in_code(code, 'sipError')


def _need_old_error_flag(code):
    """ Return True if handwritten code uses the deprecated error flag. """

    return _is_used_in_code(code, 'sipIsErr')


def _need_new_instance(arg):
    """ Return True if the argument type means an instance needs to be created
    on the heap to pass back to Python.
    """

    if not arg.is_in and arg.is_out:
        if arg.is_reference and len(arg.derefs) == 0:
            return True

        if not arg.is_reference and len(arg.derefs) == 1:
            return True

    return False


def _fake_protected_args(signature):
    """ Convert any protected arguments (ie. those whose type is unavailable
    outside of a shadow class) to a fundamental type to be used instead (with
    suitable casts).
    """

    protection_state = []

    for arg in signature.args:
        if arg.type is ArgumentType.ENUM and arg.definition.is_protected:
            protection_state.append(arg)
            arg.type = ArgumentType.INT
        elif arg.type is ArgumentType.CLASS and arg.definition.is_protected:
            protection_state.append((arg, arg.derefs, arg.is_reference))
            arg.type = ArgumentType.FAKE_VOID
            arg.derefs = [False]
            arg.is_reference = False

    return protection_state


def _restore_protected_args(protection_state):
    """ Restore any protected arguments faked by _fake_protected_args(). """

    for protected in protection_state:
        if isinstance(protected, Argument):
            protected.type = ArgumentType.ENUM
        else:
            protected, derefs, is_reference = protected
            protected.type = ArgumentType.CLASS
            protected.derefs = derefs
            protected.is_reference = is_reference


def _remove_protection(arg, protection_state):
    """ Reset and save any protections so that the argument will be rendered
    exactly as defined in C++.
    """

    if arg.type in (ArgumentType.CLASS, ArgumentType.ENUM) and arg.definition.is_protected:
        arg.definition.is_protected = False
        protection_state.add(arg.definition)


def _remove_protections(signature, protection_state):
    """ Reset and save any protections so that the signature will be rendered
    exactly as defined in C++.
    """

    _remove_protection(signature.result, protection_state)

    for arg in signature.args:
        _remove_protection(arg, protection_state)


def _restore_protections(protection_state):
    """ Restore any protections removed by _remove_protection(). """

    for protected in protection_state:
        protected.is_protected = True


def _need_dealloc(spec, bindings, klass):
    """ Return True if a dealloc function is needed for a class. """

    if klass.iface_file.type is IfaceFileType.NAMESPACE:
        return False

    # Each of these conditions cause some code to be generated.

    if bindings.tracing:
        return True

    if spec.c_bindings:
        return True

    if len(klass.dealloc_code) != 0:
        return True

    if klass.dtor is AccessSpecifier.PUBLIC:
        return True

    if klass.has_shadow:
        return True

    return False


def _arg_name(spec, name, code):
    """ Return the argument name to use in a function definition for
    handwritten code.
    """

    # Always use the name in C code.
    if spec.c_bindings:
        return name

    # Use the name if it is used in the handwritten code.
    if _is_used_in_code(code, name):
        return name

    # Don't use the name.
    return ''


def _use_in_code(code, s, spec=None):
    """ Return the string to use depending on whether it is used in some code
    and optionally if the bindings are for C.
    """

    # Always use the string for C bindings.
    if spec is not None and spec.c_bindings:
        return s

    return s if _is_used_in_code(code, s) else ''


def _is_used_in_code(code, s):
    """ Return True if a string is used in code. """

    # The code may be a list of code blocks or an optional code block.
    if code is None:
        return False

    if isinstance(code, CodeBlock):
        code = [code]

    for cb in code:
        if s in cb.text:
            return True

    return False


def _class_from_void(spec, klass):
    """ Return an assignment statement from a void * variable to a class
    instance variable.
    """

    klass_type = _scoped_class_name(spec, klass)

    if spec.c_bindings:
        return f'{klass_type} *sipCpp = ({klass_type} *)sipCppV'

    return f'{klass_type} *sipCpp = reinterpret_cast<{klass_type} *>(sipCppV)'


def _mapped_type_from_void(spec, mapped_type_type):
    """ Return an assignment statement from a void * variable to a mapped type
    variable.
    """

    if spec.c_bindings:
        return f'{mapped_type_type} *sipCpp = ({mapped_type_type} *)sipCppV'

    return f'{mapped_type_type} *sipCpp = reinterpret_cast<{mapped_type_type} *>(sipCppV)'


# The types that need a Python reference.
_PY_REF_TYPES = (ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING,
    ArgumentType.UTF8_STRING, ArgumentType.USTRING, ArgumentType.SSTRING,
    ArgumentType.STRING)

def _keep_py_reference(arg):
    """ Return True if the argument has a type that requires an extra reference
    to the originating object to be kept.
    """

    return (arg.type in _PY_REF_TYPES and not arg.is_reference and len(arg.derefs) > 0)


def _get_encoding(type):
    """ Return the encoding character for the given type. """

    if type.type is ArgumentType.ASCII_STRING:
        encoding = 'A'
    elif type.type is ArgumentType.LATIN1_STRING:
        encoding = 'L'
    elif type.type is ArgumentType.UTF8_STRING:
        encoding = '8'
    elif type.type is ArgumentType.WSTRING:
        encoding = 'w' if len(type.derefs) == 0 else 'W'
    else:
        encoding = 'N'

    return encoding


def _has_member_docstring(bindings, member, overloads):
    """ Return True if a function/method has a docstring. """

    auto_docstring = False

    # Check for any explicit docstrings and remember if there were any that
    # could be automatically generated.
    for overload in _callable_overloads(member, overloads):
        if overload.docstring is not None:
            return True

        if bindings.docstrings:
            auto_docstring = True

    if member.no_arg_parser:
        return False

    return auto_docstring


def _member_docstring(sf, spec, bindings, member, overloads, is_method=False):
    """ Generate the docstring for all overloads of a function/method.  Return
    True if the docstring was entirely automatically generated.
    """

    NEWLINE = '\\n"\n"'

    auto_docstring = True

    # See if all the docstrings are automatically generated.
    all_auto = True
    any_implied = False

    for overload in _callable_overloads(member, overloads):
        if overload.docstring is not None:
            all_auto = False

            if overload.docstring.signature is not DocstringSignature.DISCARDED:
                any_implied = True

    # Generate the docstring.
    is_first = True

    for overload in _callable_overloads(member, overloads):
        if not is_first:
            sf.write(NEWLINE)

            # Insert a blank line if any explicit docstring wants to include a
            # signature.  This maintains compatibility with previous versions.
            if any_implied:
                sf.write(NEWLINE)

        if overload.docstring is not None:
            if overload.docstring.signature is DocstringSignature.PREPENDED:
                _member_auto_docstring(sf, spec, bindings, overload, is_method)
                sf.write(NEWLINE)

            sf.write(_docstring_text(overload.docstring))

            if overload.docstring.signature is DocstringSignature.APPENDED:
                sf.write(NEWLINE)
                _member_auto_docstring(sf, spec, bindings, overload, is_method)

            auto_docstring = False
        elif all_auto or any_implied:
            _member_auto_docstring(sf, spec, bindings, overload, is_method)

        is_first = False

    return auto_docstring


def _member_auto_docstring(sf, spec, bindings, overload, is_method):
    """ Generate the automatic docstring for a function/method. """

    if bindings.docstrings:
        _overload_auto_docstring(sf, spec, overload, is_method=is_method)


def _has_class_docstring(bindings, klass):
    """ Return True if a class has a docstring. """

    auto_docstring = False

    # Check for any explicit docstrings and remember if there were any that
    # could be automatically generated.
    if klass.docstring is not None:
        return True

    for ctor in klass.ctors:
        if ctor.access_specifier is AccessSpecifier.PRIVATE:
            continue

        if ctor.docstring is not None:
            return True

        if bindings.docstrings:
            auto_docstring = True

    if not klass.can_create:
        return False

    return auto_docstring


def _class_docstring(sf, spec, bindings, klass):
    """ Generate the docstring for a class. """

    NEWLINE = '\\n"\n"'

    # See if all the docstrings are automatically generated.
    all_auto = (klass.docstring is None)
    any_implied = False

    for ctor in klass.ctors:
        if ctor.access_specifier is AccessSpecifier.PRIVATE:
            continue

        if ctor.docstring is not None:
            all_auto = False

            if ctor.docstring.signature is not DocstringSignature.DISCARDED:
                any_implied = True

    # Generate the docstring.
    if all_auto:
        sf.write('\\1')

    if klass.docstring is not None and klass.docstring.signature is not DocstringSignature.PREPENDED:
        sf.write(_docstring_text(klass.docstring))
        is_first = False
    else:
        is_first = True

    if klass.docstring is None or klass.docstring.signature is not DocstringSignature.DISCARDED:
        for ctor in klass.ctors:
            if ctor.access_specifier is AccessSpecifier.PRIVATE:
                continue

            if not is_first:
                sf.write(NEWLINE)

                # Insert a blank line if any explicit docstring wants to
                # include a signature.  This maintains compatibility with
                # previous versions.
                if any_implied:
                    sf.write(NEWLINE)

            if ctor.docstring is not None:
                if ctor.docstring.signature is DocstringSignature.PREPENDED:
                    _ctor_auto_docstring(sf, spec, bindings, klass, ctor)
                    sf.write(NEWLINE)

                sf.write(_docstring_text(ctor.docstring))

                if ctor.docstring.signature is DocstringSignature.APPENDED:
                    sf.write(NEWLINE)
                    _ctor_auto_docstring(sf, spec, bindings, klass, ctor)
            elif all_auto or any_implied:
                _ctor_auto_docstring(sf, spec, bindings, klass, ctor)

            is_first = False

    if klass.docstring is not None and klass.docstring.signature is DocstringSignature.PREPENDED:
        if not is_first:
            sf.write(NEWLINE)
            sf.write(NEWLINE)

        sf.write(_docstring_text(klass.docstring))


def _ctor_auto_docstring(sf, spec, bindings, klass, ctor):
    """ Generate the automatic docstring for a ctor. """

    if bindings.docstrings:
        py_name = fmt_scoped_py_name(klass.scope, klass.py_name.name)
        signature = fmt_signature_as_type_hint(spec, ctor.py_signature,
                need_self=False, exclude_result=True)
        sf.write(py_name + signature)


def _docstring_text(docstring):
    """ Return the text of a docstring. """

    text = docstring.text

    # Remove any single trailing newline.
    if text.endswith('\n'):
        text = text[:-1]

    s = ''

    for ch in text:
        if ch == '\n':
            # Let the compiler concatanate lines.
            s += '\\n"\n"'
        elif ch in r'\"':
            s += '\\'
            s += ch
        elif ch.isprintable():
            s += ch
        else:
            s += f'\\{ord(ch):03o}'

    return s


def _module_docstring(sf, module):
    """ Generate the definition of a module's optional docstring. """

    if module.docstring is not None:
        sf.write(
f'''
"PyDoc_STRVAR(doc_mod_{module.py_name}, "{_docstring_text(module.docstring)}");
''')


def _get_void_ptr_cast(type):
    """ Return a void* cast for an argument if needed. """

    # Generate a cast if the argument's type was a typedef.  This allows us to
    # use typedef's to void* to hide something more complex that we don't
    # handle.
    if type.original_typedef is None:
        return ''

    return '(const void *)' if type.is_const else '(void *)'


def _declare_limited_api(sf, py_debug, module=None):
    """ Declare the use of the limited API. """

    if py_debug:
        return

    if module is None or module.use_limited_api:
        sf.write(
'''
#if !defined(Py_LIMITED_API)
#define Py_LIMITED_API
#endif
''')


def _plugin_signals_table(sf, spec, bindings, klass):
    """ Generate the PyQt signals table and return True if anything was
    generated.
    """

    # Handle the trivial case.
    if not klass.is_qobject:
        return False

    is_signals = False

    # The signals must be grouped by name.
    for member in klass.members:
        member_nr = member.member_nr

        for overload in klass.overloads:
            if overload.common is not member or overload.pyqt_method_specifier is not PyQtMethodSpecifier.SIGNAL:
                continue

            if member_nr >= 0:
                # See if there is a non-signal overload.
                for non_sig in klass.overloads:
                    if non_sig is not overload and non_sig.common is member and non_sig.pyqt_method_specifier is not PyQtMethodSpecifier.SIGNAL:
                        break
                else:
                    member_nr = -1

            if not is_signals:
                is_signals = True

                _pyqt_emitters(sf, spec, klass)

                pyqt_version = '5' if _pyqt5(spec) else '6'
                sf.write(
f'''

/* Define this type's signals. */
static const pyqt{pyqt_version}QtSignal signals_{klass.iface_file.fq_cpp_name.as_word}[] = {{
''')

            # We enable a hack that supplies any missing optional arguments.
            # We only include the version with all arguments and provide an
            # emitter function which handles the optional arguments.
            _pyqt_signal_table_entry(sf, spec, bindings, klass, overload,
                    member_nr)

            member_nr = -1

    if is_signals:
        sf.write('    {SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR, SIP_NULLPTR}\n};\n')

    return is_signals


def _has_optional_args(overload):
    """ Return True if an overload has optional arguments. """

    args = overload.cpp_signature.args

    return len(args) != 0 and args[-1].default_value is not None


def _pyqt_class_plugin(sf, spec, bindings, klass):
    """ Generate any extended class definition data for PyQt.  Return True if
    anything was generated.
    """

    is_signals = _plugin_signals_table(sf, spec, bindings, klass)

    # The PyQt6 support code doesn't assume the structure is generated.
    if _pyqt6(spec):
        generated = is_signals

        if klass.is_qobject and not klass.pyqt_no_qmetaobject:
            generated = True

        if klass.pyqt_interface is not None:
            generated = True

        if not generated:
            return False

    klass_name = klass.iface_file.fq_cpp_name.as_word

    pyqt_version = '5' if _pyqt5(spec) else '6'
    sf.write(f'\n\nstatic pyqt{pyqt_version}ClassPluginDef plugin_{klass_name} = {{\n')

    mo_ref = f'&{_scoped_class_name(spec, klass)}::staticMetaObject' if klass.is_qobject and not klass.pyqt_no_qmetaobject else 'SIP_NULLPTR'
    sf.write(f'    {mo_ref},\n')

    if _pyqt5(spec):
        sf.write(f'    {klass.pyqt_flags},\n')

    signals_ref = f'signals_{klass_name}' if is_signals else 'SIP_NULLPTR'
    sf.write(f'    {signals_ref},\n')

    interface_ref = f'"{klass.pyqt_interface}"' if klass.pyqt_interface is not None else 'SIP_NULLPTR'
    sf.write(f'    {interface_ref}\n')

    sf.write('};\n')

    return True


def _global_function_table_entries(sf, spec, bindings, members):
    """ Generate the entries in a table of PyMethodDef for global functions.
    """

    for member in members:
        if member.py_slot is None:
            py_name = _get_normalised_cached_name(member.py_name)
            sf.write(f'        {{sipName_{py_name}, ')

            if member.no_arg_parser or member.allow_keyword_args:
                sf.write(f'SIP_MLMETH_CAST(func_{member.py_name.name}), METH_VARARGS|METH_KEYWORDS')
            else:
                sf.write(f'func_{member.py_name.name}, METH_VARARGS')

            docstring = _optional_ptr(
                    _has_member_docstring(bindings, member,
                            spec.module.overloads),
                    'doc_' + member.py_name.name)
            sf.write(f', {docstring}}},\n')


def _enum_class_scope(spec, enum):
    """ Return the scope of an unscoped enum as a string. """

    if enum.is_protected:
        scope_s = 'sip' + enum.scope.iface_file.fq_cpp_name.as_word
    elif enum.scope.is_protected:
        scope_s = _scoped_class_name(spec, enum.scope)
    else:
        scope_s = enum.scope.iface_file.fq_cpp_name.as_cpp

    return scope_s


def _include_sip_h(sf, module):
    """ Generate the inclusion of sip.h. """

    if module.py_ssize_t_clean:
        sf.write(
'''
#define PY_SSIZE_T_CLEAN
''')

    sf.write(
'''
#include "sip.h"
''')


def _enum_member(spec, enum_member):
    """ Return an enum member as a string. """

    if spec.c_bindings:
        return enum_member.cpp_name

    enum = enum_member.scope

    if enum.no_scope:
        scope_s = ''
    else:
        if enum.is_scoped:
            scope_s = '::' + enum.cached_fq_cpp_name.name
        elif isinstance(enum.scope, WrappedClass):
            scope_s = _enum_class_scope(spec, enum)
        elif isinstance(enum.scope, MappedType):
            scope_s = enum.scope.iface_file.fq_cpp_name.as_cpp
        else:
            # This can't happen.
            scope_s = ''

        scope_s += '::'

    return f'static_cast<int>({scope_s}{enum_member.cpp_name})'


def _type_needs_user_state(type):
    """ Return True if a type needs user state to be provided. """

    return type.type is ArgumentType.MAPPED and type.definition.needs_user_state


def _user_state_suffix(spec, type):
    """ Return the suffix for functions that have a variant that supports a
    user state.
    """

    return 'US' if spec.abi_version >= (13, 0) and _type_needs_user_state(type) else ''


def _exception_handler(sf, spec):
    """ Generate the exception handler for a module. """

    need_decl = True

    for exception in spec.exceptions:
        if exception.iface_file.module is spec.module:
            if need_decl:
                sf.write(
f'''

/* Handle the exceptions defined in this module. */
bool sipExceptionHandler_{spec.module.py_name}(std::exception_ptr sipExcPtr)
{{
    try {{
        std::rethrow_exception(sipExcPtr);
    }}
''')

                need_decl = False

            _catch_block(sf, spec, exception)

    if not need_decl:
        sf.write(
'''    catch (...) {}

    return false;
}
''')
 
 
def _pyqt5(spec):
    """ Return True if the PyQt5 plugin was specified. """

    return 'PyQt5' in spec.plugins


def _pyqt6(spec):
    """ Return True if the PyQt6 plugin was specified. """

    return 'PyQt6' in spec.plugins


def _append_qualifier_defines(module, bindings, qualifier_defines):
    """ Append the #defines for each feature defined in a module to a list of
    them.
    """

    for qualifier in module.qualifiers:
        qualifier_type_name = None

        if qualifier.type is QualifierType.TIME:
            if _qualifier_enabled(qualifier, bindings):
                qualifier_type_name = 'TIMELINE'

        elif qualifier.type is QualifierType.PLATFORM:
            if _qualifier_enabled(qualifier, bindings):
                qualifier_type_name = 'PLATFORM'

        elif qualifier.type is QualifierType.FEATURE:
            if qualifier.name not in bindings.disabled_features and qualifier.enabled_by_default:
                qualifier_type_name = 'FEATURE'

        if qualifier_type_name is not None:
            qualifier_defines.append(f'#define SIP_{qualifier_type_name}_{qualifier.name}')


def _qualifier_enabled(qualifier, bindings):
    """ Return True if a qualifier is enabled. """

    for tag in bindings.tags:
        if qualifier.name == tag:
            return qualifier.enabled_by_default

    return False


def _overload_auto_docstring(sf, spec, overload, is_method=True):
    """ Generate the docstring for a single API overload. """

    need_self = is_method and not overload.is_static
    signature = fmt_signature_as_type_hint(spec, overload.py_signature,
            need_self=need_self)
    sf.write(overload.common.py_name.name + signature)


def _sequence_support(sf, spec, klass, overload):
    """ Generate extra support for sequences because the class has an overload
    that has been annotated with __len__.
    """

    # We require a single int argument.
    if len(overload.py_signature.args) != 1:
        return

    arg0 = overload.py_signature.args[0]

    if not py_as_int(arg0):
        return

    # At the moment all we do is check that an index to __getitem__ is within
    # range so that the class supports Python iteration.  In the future we
    # should add support for negative indices, slices, __setitem__ and
    # __delitem__ (which will require enhancements to the sip module ABI).
    if overload.common.py_slot is PySlot.GETITEM:
        index_arg = fmt_argument_as_name(spec, arg0, 0)

        sf.write(
f'''            if ({index_arg} < 0 || {index_arg} >= sipCpp->{klass.len_cpp_name}())
            {{
                PyErr_SetNone(PyExc_IndexError);
                return SIP_NULLPTR;
            }}

''')


def _abi_has_next_exception_handler(spec):
    """ Return True if the ABI implements sipNextExceptionHandler(). """

    return abi_version_check(spec, (12, 9), (13, 1))

def _abi_has_working_char_conversion(spec):
    """ Return True if the ABI has working char to/from a Python integer
    converters (ie. char is not assumed to be signed).
    """

    return abi_version_check(spec, (12, 15), (13, 8))

def _abi_supports_array(spec):
    """ Return True if the ABI supports sip.array. """

    return abi_version_check(spec, (12, 11), (13, 4))

def _cached_name_ref(cached_name, as_nr=False):
    """ Return a reference to a cached name. """

    prefix = 'sipNameNr_' if as_nr else 'sipName_'

    return prefix + _get_normalised_cached_name(cached_name)


def _const_cast(spec, type, value):
    """ Return a value with an appropriate const_cast to a type. """

    if type.is_const:
        cpp_type = fmt_argument_as_cpp_type(spec, type, plain=True,
                no_derefs=True)

        return f'const_cast<{cpp_type} *>({value})'

    return value


def _callable_overloads(member, overloads):
    """ An iterator over the non-private and non-signal overloads. """

    for overload in overloads:
        if overload.common is member and overload.access_specifier is not AccessSpecifier.PRIVATE and overload.pyqt_method_specifier is not PyQtMethodSpecifier.SIGNAL:
            yield overload


def _gto_name(wrapped_object):
    """ Return the name of the generated type object for a wrapped object. """

    fq_cpp_name = wrapped_object.fq_cpp_name if isinstance(wrapped_object, WrappedEnum) else wrapped_object.iface_file.fq_cpp_name

    return 'sipType_' + fq_cpp_name.as_word


def _unique_class_ctors(spec, klass):
    """ An iterator over non-private ctors that have a unique C++ signature.
    """

    for ctor in klass.ctors:
        if ctor.access_specifier is AccessSpecifier.PRIVATE:
            continue

        if ctor.cpp_signature is None:
            continue

        for do_ctor in klass.ctors:
            if do_ctor is ctor:
                yield ctor
                break

            if do_ctor.cpp_signature is not None and same_signature(spec, do_ctor.cpp_signature, ctor.cpp_signature):
                break


def _unique_class_virtual_overloads(spec, klass):
    """ An iterator over non-private virtual overloads that have a unique C++
    signature.
    """

    for virtual_overload in klass.virtual_overloads:
        overload = virtual_overload.overload

        if overload.access_specifier is AccessSpecifier.PRIVATE:
            continue

        for do_virtual_overload in klass.virtual_overloads:
            if do_virtual_overload is virtual_overload:
                yield virtual_overload
                break

            do_overload = do_virtual_overload.overload

            if do_overload.cpp_name == overload.cpp_name and same_signature(spec, do_overload.cpp_signature, overload.cpp_signature):
                break


def _module_supports_qt(spec):
    """ Return True if the module implements Qt support. """

    return spec.pyqt_qobject is not None and spec.pyqt_qobject.iface_file.module is spec.module


def _optional_ptr(is_ptr, name):
    """ Return an appropriate reference to an optional pointer. """

    return name if is_ptr else 'SIP_NULLPTR'


# The map of slots to C++ names.
_SLOT_NAME_MAP = {
    PySlot.ADD:         'operator+',
    PySlot.SUB:         'operator-',
    PySlot.MUL:         'operator*',
    PySlot.TRUEDIV:     'operator/',
    PySlot.MOD:         'operator%',
    PySlot.AND:         'operator&',
    PySlot.OR:          'operator|',
    PySlot.XOR:         'operator^',
    PySlot.LSHIFT:      'operator<<',
    PySlot.RSHIFT:      'operator>>',
    PySlot.IADD:        'operator+=',
    PySlot.ISUB:        'operator-=',
    PySlot.IMUL:        'operator*=',
    PySlot.ITRUEDIV:    'operator/=',
    PySlot.IMOD:        'operator%=',
    PySlot.IAND:        'operator&=',
    PySlot.IOR:         'operator|=',
    PySlot.IXOR:        'operator^=',
    PySlot.ILSHIFT:     'operator<<=',
    PySlot.IRSHIFT:     'operator>>=',
    PySlot.INVERT:      'operator~',
    PySlot.CALL:        'operator()',
    PySlot.GETITEM:     'operator[]',
    PySlot.LT:          'operator<',
    PySlot.LE:          'operator<=',
    PySlot.EQ:          'operator==',
    PySlot.NE:          'operator!=',
    PySlot.GT:          'operator>',
    PySlot.GE:          'operator>=',
}

def _overload_cpp_name(overload):
    """ Return the C++ name of an overload. """

    py_slot = overload.common.py_slot

    return overload.cpp_name if py_slot is None else _SLOT_NAME_MAP[py_slot]


def _py_scope(scope):
    """ Return the Python scope by accounting for hidden C++ namespaces. """

    return None if isinstance(scope, WrappedClass) and scope.is_hidden_namespace else scope


def _release_gil(gil_action, bindings):
    """ Return True if the GIL is to be released. """

    return bindings.release_gil if gil_action is GILAction.DEFAULT else gil_action is GILAction.RELEASE


def _variables_in_scope(spec, scope, check_handler=True):
    """ An iterator over the variables in a scope. """

    for variable in spec.variables:
        if _py_scope(variable.scope) is scope and variable.module is spec.module:
            if check_handler and variable.needs_handler:
                continue

            yield variable


def _write_instances_table(sf, scope, instances, declaration_template):
    """ Write a table of instances.  Return True if there was a table written.
    """

    if len(instances) == 0:
        return False

    if scope is None:
        dict_type = 'module'
        suffix = ''
    else:
        dict_type = 'type'
        suffix = '_' + scope.iface_file.fq_cpp_name.as_word

    declaration = declaration_template.format(dict_type=dict_type,
            suffix=suffix)
    sf.write(f'\n\n{declaration} = {{\n')

    for instance in instances:
        entry = ', '.join(instance)
        sf.write(f'    {{{entry}}},\n')

    sentinals = ', '.join('0' * len(instances[0]))
    sf.write(f'    {{{sentinals}}}\n}};\n')

    return True


class SourceFile:
    """ The encapsulation of a source file. """

    def __init__(self, source_name, description, module, project, generated):
        """ Initialise the object. """

        self._description = description
        self._module = module

        self.open(source_name, project)

        generated.append(source_name)

    def __enter__(self):
        """ Implement a context manager for the file. """

        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """ Implement a context manager for the file. """

        self.close()

    def close(self):
        """ Close the source file. """

        self._f.close()

    def open(self, source_name, project):
        """ Open a source file and make it current. """

        self._f = open(source_name, 'w', encoding='UTF-8')

        self._line_nr = 1

        self._write_header_comments(self._description, self._module,
                project.version_info)

    def write(self, s):
        """ Write a string while tracking the current line number. """

        self._f.write(s)
        self._line_nr += s.count('\n')

    def write_code(self, code):
        """ Write some handwritten code. """

        # A trivial case.
        if code is None:
            return

        # The code may be a single code block or a list of them.
        code_blocks = code if isinstance(code, list) else [code]

        # Another trivial case.
        if not code_blocks:
            return

        for code_block in code_blocks:
            self.write(f'#line {code_block.line_nr} "{self._posix_path(code_block.sip_file)}"\n')
            self.write(code_block.text)

        self.write(f'#line {self._line_nr + 1} "{self._posix_path(self._f.name)}"\n')

    @staticmethod
    def _posix_path(path):
        """ Return the POSIX format of a path. """

        return path.replace('\\', '/')

    def _write_header_comments(self, description, module, version_info):
        """ Write the comments at the start of the file. """

        version_info_s = f' *\n * Generated by SIP {SIP_VERSION_STR}\n' if version_info else ''

        copying_s = fmt_copying(module.copying, ' *')

        self.write(
f'''/*
 * {description}
{version_info_s}{copying_s} */

''')


class CompilationUnit(SourceFile):
    """ Encapsulate a compilation unit, ie. a C or C++ source file. """

    def __init__(self, source_name, description, module, project, buildable,
            sip_api_file=True):
        """ Initialise the object. """

        super().__init__(source_name, description, module, project,
                buildable.sources)

        self.write_code(module.unit_code)

        if sip_api_file:
            self.write(f'#include "sipAPI{module.py_name}.h"\n')
