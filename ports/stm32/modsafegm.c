#include "py/mpconfig.h"
#include "py/runtime.h"

mp_obj_t nonsecure_task(int n);

mp_obj_t __attribute__((weak)) nonsecure_task(int n)
{
    return mp_const_none;
}

STATIC mp_obj_t mod_safegm_call_asp3(void)
{
    static int n = 0;
    n += 1;
    nonsecure_task(n);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_safegm_call_asp3_obj, mod_safegm_call_asp3);

STATIC const mp_rom_map_elem_t mp_module_safegm_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_safegm) },
    { MP_ROM_QSTR(MP_QSTR_call_asp3), MP_ROM_PTR(&mod_safegm_call_asp3_obj) }
};

STATIC MP_DEFINE_CONST_DICT(mp_module_safegm_globals, mp_module_safegm_globals_table);

const mp_obj_module_t mp_module_safegm = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_safegm_globals
};
