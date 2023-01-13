// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#ifndef PYBRICKS_INCLUDED_PYBRICKS_TOOLS_H
#define PYBRICKS_INCLUDED_PYBRICKS_TOOLS_H

#include "py/mpconfig.h"

#if PYBRICKS_PY_TOOLS

#include "py/obj.h"

bool pb_module_tools_run_loop_is_active();

void pb_type_tools_wait_reset(void);

mp_obj_t pb_type_tools_wait_new(mp_int_t duration);

extern const mp_obj_type_t pb_type_StopWatch;

#endif // PYBRICKS_PY_TOOLS

#endif // PYBRICKS_INCLUDED_PYBRICKS_TOOLS_H
