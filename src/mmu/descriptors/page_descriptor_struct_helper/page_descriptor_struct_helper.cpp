/* src/mmu/descriptors/page_descriptor_struct_helper/page_descriptor_struct_helper.cpp */

#include <utils.hpp>
#include <mmu.hpp>
#include <page_descriptor_helper.hpp>
#include <terminal.h>
#include <page_descriptor_helper.hpp>

using PE = help_me_build_page_entry;

void PE::set_default_values()
{
    descriptor.mair_index_info = ATTR_IDX_0;
    descriptor.rw_info         = EL2_RO;
    descriptor.share_info      = INNER_S;
    descriptor.access_info     = NORM;
    descriptor.ng_info         = GLOBAL;
    descriptor.gp_info         = GUARD_UNSET;
    descriptor.dbm_info        = DBM_UNSET;
    descriptor.contiguous_info = CONT_UNSET;
    descriptor.priv_exec_info  = PX_ALLOW;
    descriptor.exec_info       = EXEC_UNAVAIL;
};

void PE::set_mair(enum MAIR_INDEX index)
{
    descriptor.mair_index_info = index;
};

void PE::set_rw_perms(enum CHMOD_LEVEL chmod)
{
    descriptor.rw_info = chmod;
};

void PE::set_shareability(enum SHAREABILITY info)
{
    descriptor.share_info = info;
};

void PE::set_accessability(enum ACCESS_LEVEL info)
{
    descriptor.access_info = info;
};

void PE::set_global(enum NG info)
{
    descriptor.ng_info = info;
};

void PE::set_guarded(enum GUARD_LEVEL info)
{
    descriptor.gp_info = info;
};

void PE::set_dirty(enum DBM info)
{
    descriptor.dbm_info = info;
};

void PE::set_contiguous(enum CONTIGUOUS info)
{
    descriptor.contiguous_info = info;
};

void PE::set_pxn(enum PRIVLGD_EXEC_PERM info)
{
    descriptor.priv_exec_info = info;
};

void PE::set_exec(enum EXECUTE_PERMISSION info)
{
    descriptor.exec_info = info;
};
