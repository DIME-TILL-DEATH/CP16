#ifndef __HW_AT45DB321_H__
#define __HW_AT45DB321_H__

//! Continuous Array Read(Legacy Command)
//
#define AT45DB321_CMD_CARL       0xE8

//
//! Continuous Array Read(High Frequency Mode)
//
#define AT45DB321_CMD_CARH       0x0B

//
//! Continuous Array Read (Low Frequency Mode)
//
#define AT45DB321_CMD_RDID       0x03

//
//! Main Memory Page Read
//
#define AT45DB321_CMD_MMPR       0xD2

//
//! Buffer1 Read
//
#define AT45DB321_CMD_BF1R       0xD4
//
//! Buffer2 Read
//
#define AT45DB321_CMD_BF2R       0xD6

//
//! Buffer1 Write
//
#define AT45DB321_CMD_BF1W       0x84
//
//! Buffer2 Write
//
#define AT45DB321_CMD_BF2W       0x87

//
//! Buffer1 to Main Memory Page Program with Built-in Erase
//
#define AT45DB321_CMD_B1TMW      0x83
//
//! Buffer2 to Main Memory Page Program with Built-in Erase
//
#define AT45DB321_CMD_B2TMW      0x86

//
//! Buffer1 to Main Memory Page Program without Built-in Erase
//
#define AT45DB321_CMD_B1TMO      0x88
//
//! Buffer2 to Main Memory Page Program without Built-in Erase
//
#define AT45DB321_CMD_B2TMO      0x89

//
//! Page Erase
//
#define AT45DB321_CMD_ERPG       0x81

//
//! Block Erase
//
#define AT45DB321_CMD_ERBL       0x50

//
//! Sector Erase
//
#define AT45DB321_CMD_ERSC       0x7C

//
//! Main Memory Page to Buffer1 Transfer
//
#define AT45DB321_CMD_MTB1       0x53
//
//! Main Memory Page to Buffer2 Transfer
//
#define AT45DB321_CMD_MTB2       0x55

//
//! Auto Page Rewrite with Buffer1
//
#define AT45DB321_CMD_APR1       0x58
//
//! Auto Page Rewrite with Buffer2
//
#define AT45DB321_CMD_APR2       0x59

//
//! Status Register Read
//
#define AT45DB321_CMD_SRRD       0xD7

//
//! Manufacturer and Device ID Read
//
#define AT45DB321_CMD_MDID       0x9F

//
//! Chip Erase
//
const uint8_t AT45DB321_CMD_CPER[4] = { 0xC7, 0x94, 0x80, 0x9A};

//
//! Enable Software Sector Protection Command
//! Notice:
//!     If the device is power cycled, then the software controlled protection will be disabled.
//! Once the device is powered up, the Enable Sector Protection command  should be reissued
//! if sector protection is desired and if the WP pin is not used.
//
const uint8_t AT45DB321_CMD_ESSP[4] = { 0x3D, 0x2A, 0x7F, 0xA9};
//
//! Disable Software Sector Protection Command
//
const uint8_t AT45DB321_CMD_DSSP[4] = { 0x3D, 0x2A, 0x7F, 0x9A};

//
//! Erase Sector Protection Register Command
//
const uint8_t AT45DB321_CMD_ESPR[4] = { 0x3D, 0x2A, 0x7F, 0xCF};
//
//! Program Sector Protection Register Command
//
const uint8_t AT45DB321_CMD_PSPR[4] = { 0x3D, 0x2A, 0x7F, 0xFC};
//
//! Read Sector Protection Register Command
//
const uint8_t AT45DB321_CMD_RSPR[4] = { 0x32, 0xFF, 0xFF, 0xFF};
//
//! Sector Lockdown
//! Attention:
//!     Once a sector is locked down, it can never be erased or programmed, and it can never be unlocked.
//
const uint8_t AT45DB321_CMD_SCLD[4] = { 0x3D, 0x2A, 0x7F, 0x30};
//
//! Programming the Configuration Register
//! Attention:
//!     This ��power of 2�� Command changes the page size from initial 528 bytes to 512 bytes.
//! The ��power of 2�� page size is a onetime programmable configuration register and once the device is
//! configured for ��power of 2�� page size, it cannot be reconfigured again.
//
const uint8_t AT45DB321_CMD_PGCR[4] = {0x3D, 0x2A, 0x80, 0xA6};

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup AT45DB321_Attribute
//! AT45DB321 SPI Flash attribute.
//! @{
//
//*****************************************************************************

//
//! Status Register Bits
//
#define AT45DB321_DATA_MATCH     0x40
#define AT45DB321_IDLE           0x80
#define AT45DB321_SCPT_EN        0x20
#define AT45DB321_PGSZ           0x01

//
//! AT45DB321 default page size
//
#define AT45DB321_PAGE_SIZE_DEFAULT       528UL

//
//! AT45DB321 binary page size
//
#define AT45DB321_PAGE_SIZE_BINARY        512UL

//
//! AT45DB321 total pages
//
#define AT45DB321_PAGES                   8192UL

//
//! AT45DB321 total blocks
//
#define AT45DB321_BLOCKS                  1024

//
//! AT45DB321 total sectors
//
#define AT45DB321_SECTORS                 64

//
//! AT45DB321 buffers define
//
#define AT45DB321_BUF1                    1
#define AT45DB321_BUF2                    2
//
//! AT45DB321 max SPI clock rate
//
#define AT45DB321_MAX_CLK  66000000   //unit: Hz

#endif // __HW_AT45DB321_H__
