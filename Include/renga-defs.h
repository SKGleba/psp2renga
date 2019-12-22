#define RENGA_ENTRIES_MAX (13)
#define RENGA_BLOCK_SZ (0x10000)
#define RENGA_MAX_SM_SZ (0xE000)
#define RENGA_BLOCKS_START (0x500)
#define RENGA_END_OFF (0xF0000)

#define RENGA_MAGIC_MASTER (0)
#define RENGA_MAGIC_UPDATE (0xbc68)
#define RENGA_MAGIC_373UPDATE (0xbd40)
#define RENGA_MAGIC_AIGMR (0x42f8)
#define RENGA_MAGIC_KPRX (0xb080)
#define RENGA_MAGIC_COMPAT (0x5238)
#define RENGA_MAGIC_QAF (0x6ee8)
#define RENGA_MAGIC_MGKM (0x4160)
#define RENGA_MAGIC_PM (0x71a8)
#define RENGA_MAGIC_SPKG_BLAHBLAH_2 (0x77e0)
#define RENGA_MAGIC_UTOKEN (0x4bf8)
#define RENGA_MAGIC_ENCDEC (0x3c58)
#define RENGA_MAGIC_GCAUTH (0xb308)
#define RENGA_MAGIC_RMAUTH (0x3cc0)
#define RENGA_MAGIC_ACT (0x50a8)

#define RENGA_TYPE_CODE (1)
#define RENGA_TYPE_SM (0x10)

typedef unsigned char u8_t;             ///< Unsigned 8-bit type
typedef unsigned short int u16_t;       ///< Unsigned 16-bit type
typedef unsigned int u32_t;             ///< Unsigned 32-bit type
typedef unsigned long long u64_t;        ///< Unsigned 64-bit type
typedef unsigned size_t;

typedef struct {
  u32_t unused;
  u32_t paddr;
  u32_t resp;
} __attribute__((packed)) NMFcuc_nfo;

typedef struct {
  u16_t magic;
  u8_t do_use;
  u8_t status;
  u32_t smsz;
} __attribute__((packed)) NMFsm_nfo;

typedef struct {
  u16_t magic;
  u8_t do_use;
  u8_t status;
  u32_t mgrpaddr;
  NMFcuc_nfo cucnfo;
  NMFsm_nfo entries[RENGA_ENTRIES_MAX];
} __attribute__((packed)) NMFfm_nfo;