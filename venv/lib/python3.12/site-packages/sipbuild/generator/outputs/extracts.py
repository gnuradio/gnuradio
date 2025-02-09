# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ...exceptions import UserException


def output_extract(spec, extract_ref):
    """ Output an extract. """

    # Get the id and file name from the reference.
    extract_id, _, extract_file = extract_ref.partition(':')
    if not extract_file:
        raise UserException(
                "an extract must be in the form 'id:file', not '{0}'".format(
                        extract_ref))

    # Get all the parts of this extract.
    ordered_parts = []
    appended_parts = []

    for extract in spec.extracts:
        if extract.id == extract_id:
            if extract.order < 0:
                appended_parts.append(extract)
            else:
                ordered_parts.append(extract)

    # Put the parts in the correct order.
    ordered_parts.sort(key=lambda e: e.order)
    ordered_parts.extend(appended_parts)

    if len(ordered_parts) == 0:
        raise UserException(
                "there is no extract defined with the identifier '{0}'".format(
                        extract_id))

    # Write the parts to the file.
    with open(extract_file, 'w', encoding='UTF-8') as ef:
        for extract_part in ordered_parts:
            ef.write(extract_part.text)
