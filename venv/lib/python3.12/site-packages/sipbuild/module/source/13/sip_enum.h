/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This file defines the API for the enum support.
 *
 * Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>
 */


#ifndef _SIP_ENUM_H
#define _SIP_ENUM_H


/* Remove when Python v3.12 is no longer supported. */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "sip.h"


#ifdef __cplusplus
extern "C" {
#endif


PyObject *sip_api_convert_from_enum(int member, const sipTypeDef *td);
int sip_api_convert_to_enum(PyObject *obj, const sipTypeDef *td);
int sip_api_is_enum_flag(PyObject *obj);

int sip_enum_create(sipExportedModuleDef *client, sipEnumTypeDef *etd,
        sipIntInstanceDef **next_int_p, PyObject *dict);
const sipTypeDef *sip_enum_get_generated_type(PyObject *obj);
int sip_enum_init(void);
int sip_enum_is_enum(PyObject *obj);


#ifdef __cplusplus
}
#endif

#endif
