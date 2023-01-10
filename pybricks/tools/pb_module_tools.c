// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include "py/mpconfig.h"

#if PYBRICKS_PY_TOOLS

#include "py/mphal.h"
#include "py/runtime.h"

#include <pybricks/tools.h>

#include <pybricks/util_mp/pb_kwarg_helper.h>
#include <pybricks/util_mp/pb_obj_helper.h>
#include <pybricks/util_pb/pb_error.h>

STATIC mp_obj_t tools_wait(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_FUNCTION(n_args, pos_args, kw_args,
        PB_ARG_REQUIRED(time));

    mp_int_t time = pb_obj_get_int(time_in);
    if (time > 0) {
        mp_hal_delay_ms(time);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(tools_wait_obj, 0, tools_wait);

STATIC bool _pb_module_tools_run_loop_is_active;

bool pb_module_tools_run_loop_is_active() {
    return _pb_module_tools_run_loop_is_active;
}

STATIC mp_obj_t pb_module_tools___init__(void) {
    _pb_module_tools_run_loop_is_active = false;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(pb_module_tools___init___obj, pb_module_tools___init__);

STATIC mp_obj_t pb_module_tools_set_run_loop_active(mp_obj_t self_in) {
    _pb_module_tools_run_loop_is_active = mp_obj_is_true(self_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pb_module_tools_set_run_loop_active_obj, pb_module_tools_set_run_loop_active);

STATIC const mp_rom_map_elem_t tools_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_tools)      },
    { MP_ROM_QSTR(MP_QSTR___init__),    MP_ROM_PTR(&pb_module_tools___init___obj)},
    { MP_ROM_QSTR(MP_QSTR_wait),        MP_ROM_PTR(&tools_wait_obj)     },
    { MP_ROM_QSTR(MP_QSTR_StopWatch),   MP_ROM_PTR(&pb_type_StopWatch)  },
    { MP_ROM_QSTR(MP_QSTR__set_run_loop_active), MP_ROM_PTR(&pb_module_tools_set_run_loop_active_obj)},
};
STATIC MP_DEFINE_CONST_DICT(pb_module_tools_globals, tools_globals_table);

const mp_obj_module_t pb_module_tools = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pb_module_tools_globals,
};

#if PYBRICKS_RUNS_ON_EV3DEV
// ev3dev extends the C module in Python
MP_REGISTER_MODULE(MP_QSTR__tools, pb_module_tools);
#else
MP_REGISTER_MODULE(MP_QSTR_pybricks_dot_tools, pb_module_tools);
#endif

#endif // PYBRICKS_PY_TOOLS
