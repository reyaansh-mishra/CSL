/* includes/CSL/specific-includes/page_descriptor_helper.hpp */

#include <csl.h>

/** INFO
 * Standard EL2 (HCR_EL2.E2H = 0) Stage 1 Page Descriptor (4KB Granule)
 *
 * Bit Layout Summary:
 * [0:1]    -> Descriptor Type (0b11 = Valid L3 Page Entry)
 * [4:2]    -> AttrIndx[2:0] (Indexes MAIR_EL2)
 * [5]      -> NS / Security state (0 = Non-Secure, 1 = Secure)
 * [7:6]    -> AP[2] Access Permission (0 = EL2 R/W, 1 = EL2 RO)
 * [9:8]    -> SH[1:0] Shareability
 * [10]     -> AF (Access Flag)
 * [11]     -> nG (non-Global)
 * [47:12]  -> Physical Page Base Address [47:12] (or [49:12] for 50-bit PA)
 * [49:48]  -> High PA bits (if 50-bit PA) / RES0
 * [50]     -> GP (Guarded Page / BTI)
 * [51]     -> DBM (Dirty Bit Management)
 * [52]     -> Contiguous Hint
 * [53]     -> PXN
 * [54]     -> XN (Execute-Never at EL2)
 * [58:55]  -> Software Defined Bits (Ignored by hardware)
 * [63:59]  -> PBHA / Page-Based Hardware Attributes
 */


/* Bits [0:1]: Not covered here. */

/* Bits [4:2]: AttrIndex, Indexes one of eight Attributes in MAIR_EL2 */
enum MAIR_INDEX : uint64_t {
    ATTR_IDX_0 = 0b000,
    ATTR_IDX_1 = 0b001,
    ATTR_IDX_2 = 0b010,
    ATTR_IDX_3 = 0b011,
    ATTR_IDX_4 = 0b100,
    ATTR_IDX_5 = 0b101,
    ATTR_IDX_6 = 0b110,
    ATTR_IDX_7 = 0b111
};

/* Bit [5]: Security: RES0! */

/* Bits [7:6] (Access Permission): Controls data access permissions
 * 00 = EL2 Read/Write
 * 10 = EL2 Read only
 */

enum CHMOD_LEVEL : uint64_t {
    EL2_RW  = 0b00,
    EL2_RO  = 0b10,
};

/* Bits [9:8] (SH[1:0] - Shareability): Defines shareability domain 
 * 00 = Non-shareable
 * 10 = Outer Shareable
 * 11 = Inner Shareable
 */

enum SHAREABILITY : uint64_t {
    NON_S   = 0b00,
    OUTER_S = 0b10,
    INNER_S = 0b11
};

/* Bit [10] (AF - Access Flag): 0: Failt On Page Access 
 * 0: Fault
 * 1: Normal Page
 */

enum ACCESS_LEVEL : uint64_t {
    FAULT   = 0b0,
    NORM    = 0b1
};

/* Bit [11] (nG - Not Global)
 * 0: This mapping is valid for all addr spaces
 * 1: This mapping is ONLY valid for THIS addr space
 */

enum NG : uint64_t {
    GLOBAL      = 0b0,
    SPECIFIC    = 0b1
};

/* Bit [50]: Guarded Page 
 * 0:
 * 1:
 */

enum GUARD_LEVEL : uint64_t {
    GUARD_UNSET = 0b0,
    GUARD_SET   = 0b1
};

/* Bit [51]: Dirty Bit */
enum DBM : uint64_t {
    DBM_UNSET   = 0b0,
    DBM_SET     = 0b1
};

/* Bit [52]: Contiguous - Indicates Next 15 **ENTRIES** can be merged & cached with this */
enum CONTIGUOUS : uint64_t {
    CONT_UNSET    = 0b0,
    CONT_SET      = 0b1,
};

/* Bit [53]: Privilaged Execute Never */

enum PRIVLGD_EXEC_PERM : uint64_t {
    PX_ALLOW   = 0b0,
    PX_BLOCK   = 0b1,
};

/* Bit [54]: Execute Never:
 * 0: Executable
 * 1: Not executable
 */

enum EXECUTE_PERMISSION : uint64_t {
    EXEC_AVAIL      = 0b0,
    EXEC_UNAVAIL    = 0b1
};


struct L3_Page_Descriptor_Info {
    MAIR_INDEX          mair_index_info;
    CHMOD_LEVEL         rw_info;
    SHAREABILITY        share_info;
    ACCESS_LEVEL        access_info;
    NG                  ng_info;
    GUARD_LEVEL         gp_info;
    DBM                 dbm_info;
    CONTIGUOUS          contiguous_info;
    PRIVLGD_EXEC_PERM   priv_exec_info;
    EXECUTE_PERMISSION  exec_info;
};

class help_me_build_page_entry {
    public:
        void set_default_values();
        void set_mair(enum MAIR_INDEX);
        void set_rw_perms(enum CHMOD_LEVEL EL2_rw_perms);
        void set_shareability(enum SHAREABILITY);
        void set_accessability(enum ACCESS_LEVEL);
        void set_global(enum NG gobal_avail);
        void set_guarded(enum GUARD_LEVEL);
        void set_dirty(enum DBM);
        void set_contiguous(enum CONTIGUOUS);
        void set_pxn(enum PRIVLGD_EXEC_PERM);
        void set_exec(enum EXECUTE_PERMISSION);

        inline struct L3_Page_Descriptor_Info get() { return descriptor; };

    private:
        struct L3_Page_Descriptor_Info descriptor;
};
