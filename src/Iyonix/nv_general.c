/* Authors:
   Mark Watson 12/1999,
   Apsed,
   Rudolf Cornelissen 10/2002-2/2005
*/

//#define MODULE_BIT 0x00008000

#include "nv_std.h"
#include <kernel.h>
#include <swis.h>

#define PCI_ConfigurationRead 0x50388

#define false 0
#define true 1

typedef unsigned char vuint8;
typedef unsigned short vuint16;

static status_t test_ram(void);
static status_t nvxx_general_powerup (void);
static status_t nv_general_bios_to_powergraphics(void);

#if 0
static void nv_dump_configuration_space (void)
{
#define DUMP_CFG(reg, type) if (si->ps.card_type >= type) do { \
	uint32 value = CFGR(reg); \
	MSG(("configuration_space 0x%02x %20s 0x%08x\n", \
		NVCFG_##reg, #reg, value)); \
} while (0)
	DUMP_CFG (DEVID,	0);
	DUMP_CFG (DEVCTRL,	0);
	DUMP_CFG (CLASS,	0);
	DUMP_CFG (HEADER,	0);
	DUMP_CFG (BASE1REGS,0);
	DUMP_CFG (BASE2FB,	0);
	DUMP_CFG (BASE3,	0);
	DUMP_CFG (BASE4,	0);
	DUMP_CFG (BASE5,	0);
	DUMP_CFG (BASE6,	0);
	DUMP_CFG (BASE7,	0);
	DUMP_CFG (SUBSYSID1,0);
	DUMP_CFG (ROMBASE,	0);
	DUMP_CFG (CAPPTR,	0);
	DUMP_CFG (CFG_1,	0);
	DUMP_CFG (INTERRUPT,0);
	DUMP_CFG (SUBSYSID2,0);
	DUMP_CFG (AGPREF,	0);
	DUMP_CFG (AGPSTAT,	0);
	DUMP_CFG (AGPCMD,	0);
	DUMP_CFG (ROMSHADOW,0);
	DUMP_CFG (VGA,		0);
	DUMP_CFG (SCHRATCH,	0);
	DUMP_CFG (CFG_10,	0);
	DUMP_CFG (CFG_11,	0);
	DUMP_CFG (CFG_12,	0);
	DUMP_CFG (CFG_13,	0);
	DUMP_CFG (CFG_14,	0);
	DUMP_CFG (CFG_15,	0);
	DUMP_CFG (CFG_16,	0);
	DUMP_CFG (PCIEREF,	0);
	DUMP_CFG (PCIEDCAP,	0);
	DUMP_CFG (PCIEDCTST,0);
	DUMP_CFG (PCIELCAP,	0);
	DUMP_CFG (PCIELCTST,0);
	DUMP_CFG (CFG_22,	0);
	DUMP_CFG (CFG_23,	0);
	DUMP_CFG (CFG_24,	0);
	DUMP_CFG (CFG_25,	0);
	DUMP_CFG (CFG_26,	0);
	DUMP_CFG (CFG_27,	0);
	DUMP_CFG (CFG_28,	0);
	DUMP_CFG (CFG_29,	0);
	DUMP_CFG (CFG_30,	0);
	DUMP_CFG (CFG_31,	0);
	DUMP_CFG (CFG_32,	0);
	DUMP_CFG (CFG_33,	0);
	DUMP_CFG (CFG_34,	0);
	DUMP_CFG (CFG_35,	0);
	DUMP_CFG (CFG_36,	0);
	DUMP_CFG (CFG_37,	0);
	DUMP_CFG (CFG_38,	0);
	DUMP_CFG (CFG_39,	0);
	DUMP_CFG (CFG_40,	0);
	DUMP_CFG (CFG_41,	0);
	DUMP_CFG (CFG_42,	0);
	DUMP_CFG (CFG_43,	0);
	DUMP_CFG (CFG_44,	0);
	DUMP_CFG (CFG_45,	0);
	DUMP_CFG (CFG_46,	0);
	DUMP_CFG (CFG_47,	0);
	DUMP_CFG (CFG_48,	0);
	DUMP_CFG (CFG_49,	0);
	DUMP_CFG (CFG_50,	0);
#undef DUMP_CFG
}
#endif

status_t nv_general_powerup()
{
	status_t status;
	_kernel_swi_regs r;
	unsigned int dev_id;

	r.r[0] = 0;
	r.r[2] = 4;
	r.r[3] = handle;
	_kernel_swi(PCI_ConfigurationRead, &r, &r);
	dev_id = r.r[1];

	LOG(1,("POWERUP: Haiku nVidia Accelerant 0.43 running.\n"));

	/* preset no laptop */
	si->ps.laptop = false;

	/* detect card type and power it up */
	switch(dev_id)
	{
	/* Vendor Nvidia */
	case 0x002010de: /* Nvidia TNT1 */
		si->ps.card_type = NV04;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia TNT1 (NV04)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x002810de: /* Nvidia TNT2 (pro) */
	case 0x002910de: /* Nvidia TNT2 Ultra */
	case 0x002a10de: /* Nvidia TNT2 */
	case 0x002b10de: /* Nvidia TNT2 */
		si->ps.card_type = NV05;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia TNT2 (NV05)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x002c10de: /* Nvidia Vanta (Lt) */
		si->ps.card_type = NV05;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia Vanta (Lt) (NV05)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x002d10de: /* Nvidia TNT2-M64 (Pro) */
		si->ps.card_type = NV05M64;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia TNT2-M64 (Pro) (NV05M64)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x002e10de: /* Nvidia NV06 Vanta */
	case 0x002f10de: /* Nvidia NV06 Vanta */
		si->ps.card_type = NV06;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia Vanta (NV06)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x004010de: /* Nvidia GeForce FX 6800 Ultra */
	case 0x004110de: /* Nvidia GeForce FX 6800 */
	case 0x004210de: /* Nvidia GeForce FX 6800LE */
		si->ps.card_type = NV40;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 6800 (NV40)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x004310de: /* Nvidia unknown FX */
		si->ps.card_type = NV40;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia unknown FX (NV40)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x004510de: /* Nvidia GeForce FX 6800 GT */
		si->ps.card_type = NV40;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 6800 GT (NV40)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x004d10de: /* Nvidia Quadro FX 4400 */
	case 0x004e10de: /* Nvidia Quadro FX 4000 */
		si->ps.card_type = NV40;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 4000/4400 (NV40)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00a010de: /* Nvidia Aladdin TNT2 */
		si->ps.card_type = NV05;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia Aladdin TNT2 (NV05)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00c010de: /* Nvidia unknown FX */
	case 0x00c110de: /* Nvidia unknown FX */
	case 0x00c210de: /* Nvidia unknown FX */ //fixme? Xorg says: GeForce 6800 LE
		si->ps.card_type = NV41;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia unknown FX (NV41)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00f010de: /* Nvidia GeForce FX 6800 (Ultra) AGP(?) */
		si->ps.card_type = NV40;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 6800 AGP(?) (NV40(?))\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00f110de: /* Nvidia GeForce FX 6600 GT AGP */
	case 0x00f210de: /* Nvidia GeForce FX 6600 GT AGP */
		si->ps.card_type = NV43;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 6600 GT AGP (NV43)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00f810de: /* Nvidia Quadro FX 3400 PCIe(?) */
		si->ps.card_type = NV35;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 3400 PCIe(?) (NV35(?))\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00f910de: /* Nvidia GeForce PCX 6800 PCIe */
		si->ps.card_type = NV45;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia GeForce PCX 6800 PCIe (NV45)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00fa10de: /* Nvidia GeForce PCX 5750 PCIe */
		si->ps.card_type = NV36;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce PCX 5750 PCIe (NV36)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00fb10de: /* Nvidia GeForce PCX 5900 PCIe */
		si->ps.card_type = NV35;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce PCX 5900 PCIe (NV35(?))\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00fc10de: /* Nvidia GeForce PCX 5300 PCIe */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce PCX 5300 PCIe (NV34(?))\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00fd10de: /* Nvidia Quadro PCX PCIe */
		si->ps.card_type = NV45;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia Quadro PCX PCIe (NV45)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00fe10de: /* Nvidia Quadro FX 1300 PCIe(?) */
		si->ps.card_type = NV36;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 1300 PCIe(?) (NV36(?))\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00ff10de: /* Nvidia GeForce PCX 4300 PCIe */
		si->ps.card_type = NV18;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia GeForce PCX 4300 PCIe (NV18)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x010010de: /* Nvidia GeForce256 SDR */
	case 0x010110de: /* Nvidia GeForce256 DDR */
	case 0x010210de: /* Nvidia GeForce256 Ultra */
		si->ps.card_type = NV10;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia GeForce256 (NV10)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x010310de: /* Nvidia Quadro */
		si->ps.card_type = NV10;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia Quadro (NV10)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x011010de: /* Nvidia GeForce2 MX/MX400 */
	case 0x011110de: /* Nvidia GeForce2 MX100/MX200 DDR */
		si->ps.card_type = NV11;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia GeForce2 MX (NV11)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x011210de: /* Nvidia GeForce2 Go */
		si->ps.card_type = NV11;
		si->ps.card_arch = NV10A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce2 Go (NV11)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x011310de: /* Nvidia Quadro2 MXR/EX/Go */
		si->ps.card_type = NV11;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia Quadro2 MXR/EX/Go (NV11)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x014010de: /* Nvidia GeForce FX 6600 GT */
	case 0x014110de: /* Nvidia GeForce FX 6600 */
		si->ps.card_type = NV43;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 6600 (NV43)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x014410de: /* Nvidia GeForce FX 6600 Go */
		si->ps.card_type = NV43;
		si->ps.card_arch = NV40A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 6600 Go (NV43)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x014510de: /* Nvidia GeForce FX 6610 XL */
		si->ps.card_type = NV43;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 6610 XL (NV43)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x014610de: /* Nvidia GeForce FX 6600 TE Go / 6200 TE Go */
	case 0x014810de: /* Nvidia GeForce FX 6600 Go */
		si->ps.card_type = NV43;
		si->ps.card_arch = NV40A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 6600 Go / 6200 Go (NV43)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x014e10de: /* Nvidia Quadro FX 540 */
		si->ps.card_type = NV43;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 540 (NV43)\n"));
		status = nvxx_general_powerup();
		break;
	//fixme? Xorg says: 0x014f == GeForce 6200
	case 0x015010de: /* Nvidia GeForce2 GTS/Pro */
	case 0x015110de: /* Nvidia GeForce2 Ti DDR */
	case 0x015210de: /* Nvidia GeForce2 Ultra */
		si->ps.card_type = NV15;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia GeForce2 (NV15)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x015310de: /* Nvidia Quadro2 Pro */
		si->ps.card_type = NV15;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia Quadro2 Pro (NV15)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x016010de: /* Nvidia unknown FX Go */
		si->ps.card_type = NV44;
		si->ps.card_arch = NV40A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia unknown FX Go (NV44)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x016110de: /* Nvidia GeForce 6200 TurboCache */
		si->ps.card_type = NV44;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia GeForce 6200 TurboCache (NV44)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x016210de: /* Nvidia unknown FX */
	case 0x016310de: /* Nvidia unknown FX */
	case 0x016410de: /* Nvidia unknown FX */
	case 0x016510de: /* Nvidia unknown FX */
		si->ps.card_type = NV44;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia unknown FX (NV44)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x016610de: /* Nvidia unknown FX Go */
		si->ps.card_type = NV44;
		si->ps.card_arch = NV40A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia unknown FX Go (NV44)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x016710de: /* Nvidia GeForce 6200 Go */
		si->ps.card_type = NV44;
		si->ps.card_arch = NV40A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce 6200 Go (NV44)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x016810de: /* Nvidia GeForce 6250 Go */
		si->ps.card_type = NV44;
		si->ps.card_arch = NV40A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce 6250 Go (NV44)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x016e10de: /* Nvidia unknown FX */
		si->ps.card_type = NV44;
		si->ps.card_arch = NV40A;
		LOG(4,("POWERUP: Detected Nvidia unknown FX (NV44)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x017010de: /* Nvidia GeForce4 MX 460 */
	case 0x017110de: /* Nvidia GeForce4 MX 440 */
	case 0x017210de: /* Nvidia GeForce4 MX 420 */
	case 0x017310de: /* Nvidia GeForce4 MX 440SE */
		si->ps.card_type = NV17;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 MX (NV17)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x017410de: /* Nvidia GeForce4 440 Go */
	case 0x017510de: /* Nvidia GeForce4 420 Go */
	case 0x017610de: /* Nvidia GeForce4 420 Go 32M */
	case 0x017710de: /* Nvidia GeForce4 460 Go */
	case 0x017910de: /* Nvidia GeForce4 440 Go 64M (on PPC GeForce4 MX) */
		si->ps.card_type = NV17;
		si->ps.card_arch = NV10A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 Go (NV17)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x017810de: /* Nvidia Quadro4 500 XGL/550 XGL */
	case 0x017a10de: /* Nvidia Quadro4 200 NVS/400 NVS */
		si->ps.card_type = NV17;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia Quadro4 (NV17)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x017c10de: /* Nvidia Quadro4 500 GoGL */
		si->ps.card_type = NV17;
		si->ps.card_arch = NV10A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia Quadro4 500 GoGL (NV17)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x017d10de: /* Nvidia GeForce4 410 Go 16M*/
		si->ps.card_type = NV17;
		si->ps.card_arch = NV10A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 410 Go (NV17)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x018110de: /* Nvidia GeForce4 MX 440 AGP8X */
	case 0x018210de: /* Nvidia GeForce4 MX 440SE AGP8X */
	case 0x018310de: /* Nvidia GeForce4 MX 420 AGP8X */
	case 0x018510de: /* Nvidia GeForce4 MX 4000 AGP8X */
		si->ps.card_type = NV18;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 MX AGP8X (NV18)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x018610de: /* Nvidia GeForce4 448 Go */
	case 0x018710de: /* Nvidia GeForce4 488 Go */
		si->ps.card_type = NV18;
		si->ps.card_arch = NV10A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 Go (NV18)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x018810de: /* Nvidia Quadro4 580 XGL */
		si->ps.card_type = NV18;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia Quadro4 (NV18)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x018910de: /* Nvidia GeForce4 MX AGP8X */
		si->ps.card_type = NV18;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 MX AGP8X (NV18)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x018a10de: /* Nvidia Quadro4 280 NVS AGP8X */
	case 0x018b10de: /* Nvidia Quadro4 380 XGL */
	case 0x018c10de: /* Nvidia Quadro4 NVS 50 PCI */
		si->ps.card_type = NV18;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia Quadro4 (NV18)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x01a010de: /* Nvidia GeForce2 Integrated GPU */
		si->ps.card_type = NV11;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia GeForce2 Integrated GPU (CRUSH, NV11)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x01f010de: /* Nvidia GeForce4 MX Integrated GPU */
		si->ps.card_type = NV17;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 MX Integrated GPU (NFORCE2, NV17)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x020010de: /* Nvidia GeForce3 */
	case 0x020110de: /* Nvidia GeForce3 Ti 200 */
	case 0x020210de: /* Nvidia GeForce3 Ti 500 */
		si->ps.card_type = NV20;
		si->ps.card_arch = NV20A;
		LOG(4,("POWERUP: Detected Nvidia GeForce3 (NV20)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x020310de: /* Nvidia Quadro DCC */
		si->ps.card_type = NV20;
		si->ps.card_arch = NV20A;
		LOG(4,("POWERUP: Detected Nvidia Quadro DCC (NV20)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x025010de: /* Nvidia GeForce4 Ti 4600 */
	case 0x025110de: /* Nvidia GeForce4 Ti 4400 */
	case 0x025210de: /* Nvidia GeForce4 Ti 4600 */
	case 0x025310de: /* Nvidia GeForce4 Ti 4200 */
		si->ps.card_type = NV25;
		si->ps.card_arch = NV20A;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 Ti (NV25)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x025810de: /* Nvidia Quadro4 900 XGL */
	case 0x025910de: /* Nvidia Quadro4 750 XGL */
	case 0x025b10de: /* Nvidia Quadro4 700 XGL */
		si->ps.card_type = NV25;
		si->ps.card_arch = NV20A;
		LOG(4,("POWERUP: Detected Nvidia Quadro4 XGL (NV25)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x028010de: /* Nvidia GeForce4 Ti 4800 AGP8X */
	case 0x028110de: /* Nvidia GeForce4 Ti 4200 AGP8X */
		si->ps.card_type = NV28;
		si->ps.card_arch = NV20A;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 Ti AGP8X (NV28)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x028210de: /* Nvidia GeForce4 Ti 4800SE */
		si->ps.card_type = NV28;
		si->ps.card_arch = NV20A;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 Ti 4800SE (NV28)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x028610de: /* Nvidia GeForce4 4200 Go */
		si->ps.card_type = NV28;
		si->ps.card_arch = NV20A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce4 4200 Go (NV28)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x028810de: /* Nvidia Quadro4 980 XGL */
	case 0x028910de: /* Nvidia Quadro4 780 XGL */
		si->ps.card_type = NV28;
		si->ps.card_arch = NV20A;
		LOG(4,("POWERUP: Detected Nvidia Quadro4 XGL (NV28)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x028c10de: /* Nvidia Quadro4 700 GoGL */
		si->ps.card_type = NV28;
		si->ps.card_arch = NV20A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia Quadro4 700 GoGL (NV28)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x02a010de: /* Nvidia GeForce3 Integrated GPU */
		si->ps.card_type = NV20;
		si->ps.card_arch = NV20A;
		LOG(4,("POWERUP: Detected Nvidia GeForce3 Integrated GPU (XBOX, NV20)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x030110de: /* Nvidia GeForce FX 5800 Ultra */
	case 0x030210de: /* Nvidia GeForce FX 5800 */
		si->ps.card_type = NV30;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5800 (NV30)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x030810de: /* Nvidia Quadro FX 2000 */
	case 0x030910de: /* Nvidia Quadro FX 1000 */
		si->ps.card_type = NV30;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX (NV30)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x031110de: /* Nvidia GeForce FX 5600 Ultra */
	case 0x031210de: /* Nvidia GeForce FX 5600 */
		si->ps.card_type = NV31;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5600 (NV31)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x031310de: /* Nvidia unknown FX */
		si->ps.card_type = NV31;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia unknown FX (NV31)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x031410de: /* Nvidia GeForce FX 5600XT */
		si->ps.card_type = NV31;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5600XT (NV31)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x031610de: /* Nvidia unknown FX Go */
	case 0x031710de: /* Nvidia unknown FX Go */
		si->ps.card_type = NV31;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia unknown FX Go (NV31)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x031a10de: /* Nvidia GeForce FX 5600 Go */
		si->ps.card_type = NV31;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5600 Go (NV31)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x031b10de: /* Nvidia GeForce FX 5650 Go */
		si->ps.card_type = NV31;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5650 Go (NV31)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x031c10de: /* Nvidia Quadro FX 700 Go */
		si->ps.card_type = NV31;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 700 Go (NV31)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x031d10de: /* Nvidia unknown FX Go */
	case 0x031e10de: /* Nvidia unknown FX Go */
	case 0x031f10de: /* Nvidia unknown FX Go */
		si->ps.card_type = NV31;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia unknown FX Go (NV31)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032010de: /* Nvidia GeForce FX 5200 */
	case 0x032110de: /* Nvidia GeForce FX 5200 Ultra */
	case 0x032210de: /* Nvidia GeForce FX 5200 */
	case 0x032310de: /* Nvidia GeForce FX 5200SE */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5200 (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032410de: /* Nvidia GeForce FX 5200 Go */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5200 Go (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032510de: /* Nvidia GeForce FX 5250 Go */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5250 Go (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032610de: /* Nvidia GeForce FX 5500 */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5500 (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032710de: /* Nvidia GeForce FX 5100 */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5100 (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032810de: /* Nvidia GeForce FX 5200 Go 32M/64M */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5200 Go (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032910de: /* Nvidia GeForce FX 5200 (PPC) */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5200 (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032a10de: /* Nvidia Quadro NVS 280 PCI */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia Quadro NVS 280 PCI (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032b10de: /* Nvidia Quadro FX 500/600 PCI */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 500/600 PCI (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032c10de: /* Nvidia GeForce FX 5300 Go */
	case 0x032d10de: /* Nvidia GeForce FX 5100 Go */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX Go (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x032e10de: /* Nvidia unknown FX Go */
	case 0x032f10de: /* Nvidia unknown FX Go */
		si->ps.card_type = NV34;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia unknown FX Go (NV34)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x033010de: /* Nvidia GeForce FX 5900 Ultra */
	case 0x033110de: /* Nvidia GeForce FX 5900 */
		si->ps.card_type = NV35;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5900 (NV35)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x033210de: /* Nvidia GeForce FX 5900 XT */
		si->ps.card_type = NV35;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5900 XT (NV35)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x033310de: /* Nvidia GeForce FX 5950 Ultra */
		si->ps.card_type = NV38;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5950 Ultra (NV38)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x033410de: /* Nvidia GeForce FX 5900 ZT */
		si->ps.card_type = NV38;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5900 ZT (NV38(?))\n"));
		status = nvxx_general_powerup();
		break;
	case 0x033810de: /* Nvidia Quadro FX 3000 */
		si->ps.card_type = NV35;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 3000 (NV35)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x033f10de: /* Nvidia Quadro FX 700 */
		si->ps.card_type = NV35;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 700 (NV35)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x034110de: /* Nvidia GeForce FX 5700 Ultra */
	case 0x034210de: /* Nvidia GeForce FX 5700 */
	case 0x034310de: /* Nvidia GeForce FX 5700LE */
	case 0x034410de: /* Nvidia GeForce FX 5700VE */
		si->ps.card_type = NV36;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5700 (NV36)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x034710de: /* Nvidia GeForce FX 5700 Go */
	case 0x034810de: /* Nvidia GeForce FX 5700 Go */
		si->ps.card_type = NV36;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia GeForce FX 5700 Go (NV36)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x034c10de: /* Nvidia Quadro FX 1000 Go */
		si->ps.card_type = NV36;
		si->ps.card_arch = NV30A;
		si->ps.laptop = true;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 1000 Go (NV36)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x034e10de: /* Nvidia Quadro FX 1100 */
		si->ps.card_type = NV36;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia Quadro FX 1100 (NV36)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x034f10de: /* Nvidia unknown FX */
		si->ps.card_type = NV36;
		si->ps.card_arch = NV30A;
		LOG(4,("POWERUP: Detected Nvidia unknown FX (NV36(?))\n"));
		status = nvxx_general_powerup();
		break;
	/* Vendor Elsa GmbH */
	case 0x0c601048: /* Elsa Gladiac Geforce2 MX */
		si->ps.card_type = NV11;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Elsa Gladiac Geforce2 MX (NV11)\n"));
		status = nvxx_general_powerup();
		break;
	/* Vendor Nvidia STB/SGS-Thompson */
	case 0x002012d2: /* Nvidia STB/SGS-Thompson TNT1 */
		si->ps.card_type = NV04;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia STB/SGS-Thompson TNT1 (NV04)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x002812d2: /* Nvidia STB/SGS-Thompson TNT2 (pro) */
	case 0x002912d2: /* Nvidia STB/SGS-Thompson TNT2 Ultra */
	case 0x002a12d2: /* Nvidia STB/SGS-Thompson TNT2 */
	case 0x002b12d2: /* Nvidia STB/SGS-Thompson TNT2 */
		si->ps.card_type = NV05;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia STB/SGS-Thompson TNT2 (NV05)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x002c12d2: /* Nvidia STB/SGS-Thompson Vanta (Lt) */
		si->ps.card_type = NV05;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia STB/SGS-Thompson Vanta (Lt) (NV05)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x002d12d2: /* Nvidia STB/SGS-Thompson TNT2-M64 (Pro) */
		si->ps.card_type = NV05M64;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia STB/SGS-Thompson TNT2-M64 (Pro) (NV05M64)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x002e12d2: /* Nvidia STB/SGS-Thompson NV06 Vanta */
	case 0x002f12d2: /* Nvidia STB/SGS-Thompson NV06 Vanta */
		si->ps.card_type = NV06;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia STB/SGS-Thompson Vanta (NV06)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x00a012d2: /* Nvidia STB/SGS-Thompson Aladdin TNT2 */
		si->ps.card_type = NV05;
		si->ps.card_arch = NV04A;
		LOG(4,("POWERUP: Detected Nvidia STB/SGS-Thompson Aladdin TNT2 (NV05)\n"));
		status = nvxx_general_powerup();
		break;
	/* Vendor Varisys Limited */
	case 0x35031888: /* Varisys GeForce4 MX440 */
		si->ps.card_type = NV17;
		si->ps.card_arch = NV10A;
		LOG(4,("POWERUP: Detected Varisys GeForce4 MX440 (NV17)\n"));
		status = nvxx_general_powerup();
		break;
	case 0x35051888: /* Varisys GeForce4 Ti 4200 */
		si->ps.card_type = NV25;
		si->ps.card_arch = NV20A;
		LOG(4,("POWERUP: Detected Varisys GeForce4 Ti 4200 (NV25)\n"));
		status = nvxx_general_powerup();
		break;
	default:
		LOG(8,("POWERUP: Failed to detect valid card 0x%08x\n",CFGR(DEVID)));
		return B_ERROR;
	}

	return status;
}

static status_t test_ram()
{
	uint32 value, offset;
	status_t result = B_OK;

	/* make sure we don't corrupt the hardware cursor by using fbc.frame_buffer. */
	if (si->fbc.frame_buffer == NULL)
	{
		LOG(8,("INIT: test_ram detected NULL pointer.\n"));
		return B_ERROR;
	}

	for (offset = 0, value = 0x55aa55aa; offset < 256; offset++)
	{
		/* write testpattern to cardRAM */
		((uint32 *)si->fbc.frame_buffer)[offset] = value;
		/* toggle testpattern */
		value = 0xffffffff - value;
	}

	for (offset = 0, value = 0x55aa55aa; offset < 256; offset++)
	{
		/* readback and verify testpattern from cardRAM */
		if (((uint32 *)si->fbc.frame_buffer)[offset] != value) result = B_ERROR;
		/* toggle testpattern */
		value = 0xffffffff - value;
	}
	return result;
}

/* NOTE:
 * This routine *has* to be done *after* SetDispplayMode has been executed,
 * or test results will not be representative!
 * (CAS latency is dependant on NV setup on some (DRAM) boards) */
status_t nv_set_cas_latency()
{
	status_t result = B_ERROR;
	uint8 latency = 0;

	/* check current RAM access to see if we need to change anything */
	if (test_ram() == B_OK)
	{
		LOG(4,("INIT: RAM access OK.\n"));
		return B_OK;
	}

	/* check if we read PINS at starttime so we have valid registersettings at our disposal */
	if (si->ps.pins_status != B_OK)
	{
		LOG(4,("INIT: RAM access errors; not fixable: PINS was not read from cardBIOS.\n"));
		return B_ERROR;
	}

	/* OK. We might have a problem, try to fix it now.. */
	LOG(4,("INIT: RAM access errors; tuning CAS latency if prudent...\n"));

	switch(si->ps.card_type)
	{
	default:
			LOG(4,("INIT: RAM CAS tuning not implemented for this card, aborting.\n"));
			return B_OK;
			break;
	}
	if (result == B_OK)
		LOG(4,("INIT: RAM access OK. CAS latency set to %d cycles.\n", latency));
	else
		LOG(4,("INIT: RAM access not fixable. CAS latency set to %d cycles.\n", latency));

	return result;
}

#if 0
void setup_virtualized_heads(bool cross)
{
	if (cross)
	{
		head1_validate_timing	= (crtc_validate_timing)	nv_crtc2_validate_timing;
		head1_set_timing		= (crtc_set_timing)			nv_crtc2_set_timing;
		head1_depth				= (crtc_depth)				nv_crtc2_depth;
		head1_dpms				= (crtc_dpms)				nv_crtc2_dpms;
		head1_dpms_fetch		= (crtc_dpms_fetch)			nv_crtc2_dpms_fetch;
		head1_set_display_pitch	= (crtc_set_display_pitch)	nv_crtc2_set_display_pitch;
		head1_set_display_start	= (crtc_set_display_start)	nv_crtc2_set_display_start;
		head1_cursor_init		= (crtc_cursor_init)		nv_crtc2_cursor_init;
		head1_cursor_show		= (crtc_cursor_show)		nv_crtc2_cursor_show;
		head1_cursor_hide		= (crtc_cursor_hide)		nv_crtc2_cursor_hide;
		head1_cursor_define		= (crtc_cursor_define)		nv_crtc2_cursor_define;
		head1_cursor_position	= (crtc_cursor_position)	nv_crtc2_cursor_position;

		head1_mode				= (dac_mode)				nv_dac2_mode;
		head1_palette			= (dac_palette)				nv_dac2_palette;
		head1_set_pix_pll		= (dac_set_pix_pll)			nv_dac2_set_pix_pll;
		head1_pix_pll_find		= (dac_pix_pll_find)		nv_dac2_pix_pll_find;

		head2_validate_timing	= (crtc_validate_timing)	nv_crtc_validate_timing;
		head2_set_timing		= (crtc_set_timing)			nv_crtc_set_timing;
		head2_depth				= (crtc_depth)				nv_crtc_depth;
		head2_dpms				= (crtc_dpms)				nv_crtc_dpms;
		head2_dpms_fetch		= (crtc_dpms_fetch)			nv_crtc_dpms_fetch;
		head2_set_display_pitch	= (crtc_set_display_pitch)	nv_crtc_set_display_pitch;
		head2_set_display_start	= (crtc_set_display_start)	nv_crtc_set_display_start;
		head2_cursor_init		= (crtc_cursor_init)		nv_crtc_cursor_init;
		head2_cursor_show		= (crtc_cursor_show)		nv_crtc_cursor_show;
		head2_cursor_hide		= (crtc_cursor_hide)		nv_crtc_cursor_hide;
		head2_cursor_define		= (crtc_cursor_define)		nv_crtc_cursor_define;
		head2_cursor_position	= (crtc_cursor_position)	nv_crtc_cursor_position;

		head2_mode				= (dac_mode)				nv_dac_mode;
		head2_palette			= (dac_palette)				nv_dac_palette;
		head2_set_pix_pll		= (dac_set_pix_pll)			nv_dac_set_pix_pll;
		head2_pix_pll_find		= (dac_pix_pll_find)		nv_dac_pix_pll_find;
	}
	else
	{
		head1_validate_timing	= (crtc_validate_timing)	nv_crtc_validate_timing;
		head1_set_timing		= (crtc_set_timing)			nv_crtc_set_timing;
		head1_depth				= (crtc_depth)				nv_crtc_depth;
		head1_dpms				= (crtc_dpms)				nv_crtc_dpms;
		head1_dpms_fetch		= (crtc_dpms_fetch)			nv_crtc_dpms_fetch;
		head1_set_display_pitch	= (crtc_set_display_pitch)	nv_crtc_set_display_pitch;
		head1_set_display_start	= (crtc_set_display_start)	nv_crtc_set_display_start;
		head1_cursor_init		= (crtc_cursor_init)		nv_crtc_cursor_init;
		head1_cursor_show		= (crtc_cursor_show)		nv_crtc_cursor_show;
		head1_cursor_hide		= (crtc_cursor_hide)		nv_crtc_cursor_hide;
		head1_cursor_define		= (crtc_cursor_define)		nv_crtc_cursor_define;
		head1_cursor_position	= (crtc_cursor_position)	nv_crtc_cursor_position;

		head1_mode				= (dac_mode)				nv_dac_mode;
		head1_palette			= (dac_palette)				nv_dac_palette;
		head1_set_pix_pll		= (dac_set_pix_pll)			nv_dac_set_pix_pll;
		head1_pix_pll_find		= (dac_pix_pll_find)		nv_dac_pix_pll_find;

		head2_validate_timing	= (crtc_validate_timing)	nv_crtc2_validate_timing;
		head2_set_timing		= (crtc_set_timing)			nv_crtc2_set_timing;
		head2_depth				= (crtc_depth)				nv_crtc2_depth;
		head2_dpms				= (crtc_dpms)				nv_crtc2_dpms;
		head2_dpms_fetch		= (crtc_dpms_fetch)			nv_crtc2_dpms_fetch;
		head2_set_display_pitch	= (crtc_set_display_pitch)	nv_crtc2_set_display_pitch;
		head2_set_display_start	= (crtc_set_display_start)	nv_crtc2_set_display_start;
		head2_cursor_init		= (crtc_cursor_init)		nv_crtc2_cursor_init;
		head2_cursor_show		= (crtc_cursor_show)		nv_crtc2_cursor_show;
		head2_cursor_hide		= (crtc_cursor_hide)		nv_crtc2_cursor_hide;
		head2_cursor_define		= (crtc_cursor_define)		nv_crtc2_cursor_define;
		head2_cursor_position	= (crtc_cursor_position)	nv_crtc2_cursor_position;

		head2_mode				= (dac_mode)				nv_dac2_mode;
		head2_palette			= (dac_palette)				nv_dac2_palette;
		head2_set_pix_pll		= (dac_set_pix_pll)			nv_dac2_set_pix_pll;
		head2_pix_pll_find		= (dac_pix_pll_find)		nv_dac2_pix_pll_find;
	}
}
#endif

void set_crtc_owner(bool head)
{
	if (si->ps.secondary_head)
	{
		if (!head)
		{
			/* note: 'OWNER' is a non-standard register in behaviour(!) on NV11's,
			 * while non-NV11 cards behave normally.
			 *
			 * Double-write action needed on those strange NV11 cards: */
			/* RESET: needed on NV11 */
			CRTCW(OWNER, 0xff);
			/* enable access to CRTC1, SEQ1, GRPH1, ATB1, ??? */
			CRTCW(OWNER, 0x00);
		}
		else
		{
			/* note: 'OWNER' is a non-standard register in behaviour(!) on NV11's,
			 * while non-NV11 cards behave normally.
			 *
			 * Double-write action needed on those strange NV11 cards: */
			/* RESET: needed on NV11 */
			CRTC2W(OWNER, 0xff);
			/* enable access to CRTC2, SEQ2, GRPH2, ATB2, ??? */
			CRTC2W(OWNER, 0x03);
		}
	}
}

static status_t nvxx_general_powerup()
{
#if 0
	LOG(4, ("INIT: NV powerup\n"));

	/* setup cardspecs */
	/* note:
	 * this MUST be done before the driver attempts a card coldstart */
	set_specs();

	/* only process BIOS for finetuning specs and coldstarting card if requested
	 * by the user;
	 * note:
	 * this in fact frees the driver from relying on the BIOS to be executed
	 * at system power-up POST time. */
	if (!si->settings.usebios)
	{
		LOG(2, ("INIT: Attempting card coldstart!\n"));
		/* update the cardspecs in the shared_info PINS struct according to reported
		 * specs as much as is possible;
		 * this also coldstarts the card if possible (executes BIOS CMD script(s)) */
		parse_pins();
	}
	else
	{
		LOG(2, ("INIT: Skipping card coldstart!\n"));
	}

	/* get RAM size and fake panel startup (panel init code is still missing) */
	fake_panel_start();

	/* log the final card specifications */
	dump_pins();

	/* dump config space as it is after a possible coldstart attempt */
	if (si->settings.logmask & 0x80000000) nv_dump_configuration_space();

	/* setup CRTC and DAC functions access: determined in fake_panel_start */
	setup_virtualized_heads(si->ps.crtc2_prim);

	/* do powerup needed from pre-inited card state as done by system POST cardBIOS
	 * execution or driver coldstart above */
	return nv_general_bios_to_powergraphics();
#endif
   return 0;
}

/* this routine switches the CRTC/DAC sets to 'connectors', but only for analog
 * outputs. We need this to make sure the analog 'switch' is set in the same way the
 * digital 'switch' is set by the BIOS or we might not be able to use dualhead. */
status_t nv_general_output_select(bool cross)
{
	/* make sure this call is warranted */
	if (si->ps.secondary_head)
	{
		/* NV11 cards can't switch heads (confirmed) */
		if (si->ps.card_type != NV11)
		{
			if (cross)
			{
				LOG(4,("INIT: switching analog outputs to be cross-connected\n"));

				/* enable head 2 on connector 1 */
				/* (b8 = select CRTC (head) for output,
				 *  b4 = ??? (confirmed not to be a FP switch),
				 *  b0 = enable CRT) */
				DACW(OUTPUT, 0x00000101);
				/* enable head 1 on connector 2 */
				DAC2W(OUTPUT, 0x00000001);
			}
			else
			{
				LOG(4,("INIT: switching analog outputs to be straight-through\n"));

				/* enable head 1 on connector 1 */
				DACW(OUTPUT, 0x00000001);
				/* enable head 2 on connector 2 */
				DAC2W(OUTPUT, 0x00000101);
			}
		}
		else
		{
			LOG(4,("INIT: NV11 analog outputs are hardwired to be straight-through\n"));
		}
		return B_OK;
	}
	else
	{
		return B_ERROR;
	}
}

/* this routine switches CRTC/DAC set use. We need this because it's unknown howto
 * switch digital panels to/from a specific CRTC/DAC set. */
#if 0
status_t nv_general_head_select(bool cross)
{
	/* make sure this call is warranted */
	if (si->ps.secondary_head)
	{
		/* invert CRTC/DAC use to do switching */
		if (cross)
		{
			LOG(4,("INIT: switching CRTC/DAC use to be cross-connected\n"));
			si->crtc_switch_mode = !si->ps.crtc2_prim;
		}
		else
		{
			LOG(4,("INIT: switching CRTC/DAC use to be straight-through\n"));
			si->crtc_switch_mode = si->ps.crtc2_prim;
		}
		/* update CRTC and DAC functions access */
		setup_virtualized_heads(si->crtc_switch_mode);

		return B_OK;
	}
	else
	{
		return B_ERROR;
	}
}

/* basic change of card state from VGA to enhanced mode:
 * Should work from VGA BIOS POST init state. */
static status_t nv_general_bios_to_powergraphics()
{
	/* let acc engine make power off/power on cycle to start 'fresh' */
	NV_REG32(NV32_PWRUPCTRL) = 0x13110011;
	snooze(1000);

	/* power-up all nvidia hardware function blocks */
	/* bit 28: OVERLAY ENGINE (BES),
	 * bit 25: CRTC2, (> NV04A)
	 * bit 24: CRTC1,
	 * bit 20: framebuffer,
	 * bit 16: PPMI,
	 * bit 12: PGRAPH,
	 * bit  8: PFIFO,
	 * bit  4: PMEDIA,
	 * bit  0: TVOUT. (> NV04A) */
	NV_REG32(NV32_PWRUPCTRL) = 0x13111111;

	/* select colormode CRTC registers base adresses */
	NV_REG8(NV8_MISCW) = 0xcb;

	/* enable access to primary head */
	set_crtc_owner(0);
	/* unlock head's registers for R/W access */
	CRTCW(LOCK, 0x57);
	CRTCW(VSYNCE ,(CRTCR(VSYNCE) & 0x7f));
	if (si->ps.secondary_head)
	{
		/* enable access to secondary head */
		set_crtc_owner(1);
		/* unlock head's registers for R/W access */
		CRTC2W(LOCK, 0x57);
		CRTC2W(VSYNCE ,(CRTCR(VSYNCE) & 0x7f));
	}

	/* turn off both displays and the hardcursors (also disables transfers) */
	head1_dpms(false, false, false);
	head1_cursor_hide();
	if (si->ps.secondary_head)
	{
		head2_dpms(false, false, false);
		head2_cursor_hide();
	}

	if (si->ps.secondary_head)
	{
		/* switch overlay engine to CRTC1 */
		/* bit 17: GPU FP port #1	(confirmed NV25, NV28, confirmed not on NV34),
		 * bit 16: GPU FP port #2	(confirmed NV25, NV28, NV34),
		 * bit 12: overlay engine	(all cards),
		 * bit  9: TVout chip #2	(confirmed on NV18, NV25, NV28),
		 * bit  8: TVout chip #1	(all cards),
		 * bit  4: both I2C busses	(all cards) */
		NV_REG32(NV32_2FUNCSEL) &= ~0x00001000;
		NV_REG32(NV32_FUNCSEL) |= 0x00001000;
	}
	si->overlay.crtc = false;

	/* enable 'enhanced' mode on primary head: */
	/* enable access to primary head */
	set_crtc_owner(0);
	/* note: 'BUFFER' is a non-standard register in behaviour(!) on most
	 * NV11's like the GeForce2 MX200, while the MX400 and non-NV11 cards
	 * behave normally.
	 * Also readback is not nessesarily what was written before!
	 *
	 * Double-write action needed on those strange NV11 cards: */
	/* RESET: don't doublebuffer CRTC access: set programmed values immediately... */
	CRTCW(BUFFER, 0xff);
	/* ... and use fine pitched CRTC granularity on > NV4 cards (b2 = 0) */
	/* note: this has no effect on possible bandwidth issues. */
	CRTCW(BUFFER, 0xfb);
	/* select VGA mode (old VGA register) */
	CRTCW(MODECTL, 0xc3);
	/* select graphics mode (old VGA register) */
	SEQW(MEMMODE, 0x0e);
	/* select 8 dots character clocks (old VGA register) */
	SEQW(CLKMODE, 0x21);
	/* select VGA mode (old VGA register) */
	GRPHW(MODE, 0x00);
	/* select graphics mode (old VGA register) */
	GRPHW(MISC, 0x01);
	/* select graphics mode (old VGA register) */
	ATBW(MODECTL, 0x01);
	/* enable 'enhanced mode', enable Vsync & Hsync,
	 * set DAC palette to 8-bit width, disable large screen */
	CRTCW(REPAINT1, 0x04);

	/* enable 'enhanced' mode on secondary head: */
	if (si->ps.secondary_head)
	{
		/* enable access to secondary head */
		set_crtc_owner(1);
		/* select colormode CRTC2 registers base adresses */
		NV_REG8(NV8_MISCW) = 0xcb;
		/* note: 'BUFFER' is a non-standard register in behaviour(!) on most
		 * NV11's like the GeForce2 MX200, while the MX400 and non-NV11 cards
		 * behave normally.
		 * Also readback is not nessesarily what was written before!
		 *
		 * Double-write action needed on those strange NV11 cards: */
		/* RESET: don't doublebuffer CRTC2 access: set programmed values immediately... */
		CRTC2W(BUFFER, 0xff);
		/* ... and use fine pitched CRTC granularity on > NV4 cards (b2 = 0) */
		/* note: this has no effect on possible bandwidth issues. */
		CRTC2W(BUFFER, 0xfb);
		/* select VGA mode (old VGA register) */
		CRTC2W(MODECTL, 0xc3);
		/* select graphics mode (old VGA register) */
		SEQW(MEMMODE, 0x0e);
		/* select 8 dots character clocks (old VGA register) */
		SEQW(CLKMODE, 0x21);
		/* select VGA mode (old VGA register) */
		GRPHW(MODE, 0x00);
		/* select graphics mode (old VGA register) */
		GRPHW(MISC, 0x01);
		/* select graphics mode (old VGA register) */
		ATB2W(MODECTL, 0x01);
		/* enable 'enhanced mode', enable Vsync & Hsync,
		 * set DAC palette to 8-bit width, disable large screen */
		CRTC2W(REPAINT1, 0x04);
	}

	/* enable palettes */
	DACW(GENCTRL, 0x00100100);
	if (si->ps.secondary_head) DAC2W(GENCTRL, 0x00100100);

	/* enable programmable PLLs */
	DACW(PLLSEL, 0x10000700);
	if (si->ps.secondary_head) DACW(PLLSEL, (DACR(PLLSEL) | 0x20000800));

	/* turn on DAC and make sure detection testsignal routing is disabled
	 * (b16 = disable DAC,
	 *  b12 = enable testsignal output */
	//fixme note: b20 ('DACTM_TEST') when set apparantly blocks a DAC's video output
	//(confirmed NV43), while it's timing remains operational (black screen).
	//It feels like in some screen configurations it can move the output to the other
	//output connector as well...
	DACW(TSTCTRL, (DACR(TSTCTRL) & 0xfffeefff));
	/* turn on DAC2 if it exists
	 * (NOTE: testsignal function block resides in DAC1 only (!)) */
	if (si->ps.secondary_head) DAC2W(TSTCTRL, (DAC2R(TSTCTRL) & 0xfffeefff));

	/* NV40 and NV45 need a 'tweak' to make sure the CRTC FIFO's/shiftregisters get
	 * their data in time (otherwise momentarily ghost images of windows or such
	 * may appear on heavy acceleration engine use for instance, especially in 32-bit
	 * colordepth) */
	if ((si->ps.card_type == NV40) || (si->ps.card_type == NV45))
	{
		/* clear b15: some framebuffer config item (unknown) */
		NV_REG32(NV32_PFB_CLS_PAGE2) &= 0xffff7fff;
	}

	/* setup AGP:
	 * Note:
	 * This may only be done when no transfers are in progress on the bus, so now
	 * is probably a good time.. */
	nv_agp_setup();

	/* turn screen one on */
	head1_dpms(true, true, true);

	return B_OK;
}
#endif

/* Check if mode virtual_size adheres to the cards _maximum_ contraints, and modify
 * virtual_size to the nearest valid maximum for the mode on the card if not so.
 * Also: check if virtual_width adheres to the cards granularity constraints, and
 * create mode slopspace if not so.
 * We use acc or crtc granularity constraints based on the 'worst case' scenario.
 *
 * Mode slopspace is reflected in fbc->bytes_per_row BTW. */
#if 0
status_t nv_general_validate_pic_size (display_mode *target, uint32 *bytes_per_row, bool *acc_mode)
{
	uint32 video_pitch;
	uint32 acc_mask, crtc_mask;
	uint32 max_crtc_width, max_acc_width;
	uint8 depth = 8;

	/* determine pixel multiple based on acceleration engine constraints */
	/* note:
	 * because of the seemingly 'random' variations in these constraints we take
	 * a reasonable 'lowest common denominator' instead of always true constraints. */
	switch (si->ps.card_arch)
	{
	case NV04A:
		/* confirmed for:
		 * TNT1 (NV04), TNT2 (NV05), TNT2-M64 (NV05M64), GeForce2 MX400 (NV11),
		 * GeForce4 MX440 (NV18), GeForceFX 5200 (NV34) in PIO acc mode;
		 * confirmed for:
		 * TNT1 (NV04), TNT2 (NV05), TNT2-M64 (NV05M64), GeForce4 Ti4200 (NV28),
		 * GeForceFX 5200 (NV34) in DMA acc mode. */
		switch (target->space)
		{
			case B_CMAP8: acc_mask = 0x0f; depth =  8; break;
			case B_RGB15: acc_mask = 0x07; depth = 16; break;
			case B_RGB16: acc_mask = 0x07; depth = 16; break;
			case B_RGB24: acc_mask = 0x0f; depth = 24; break;
			case B_RGB32: acc_mask = 0x03; depth = 32; break;
			default:
				LOG(8,("INIT: unknown color space: 0x%08x\n", target->space));
				return B_ERROR;
		}
		break;
	default:
		/* confirmed for:
		 * GeForce4 Ti4200 (NV28), GeForceFX 5600 (NV31) in PIO acc mode;
		 * confirmed for:
		 * GeForce2 MX400 (NV11), GeForce4 MX440 (NV18), GeForcePCX 5750 (NV36),
		 * GeForcePCX 6600 GT (NV43) in DMA acc mode. */
		switch (target->space)
		{
			case B_CMAP8: acc_mask = 0x3f; depth =  8; break;
			case B_RGB15: acc_mask = 0x1f; depth = 16; break;
			case B_RGB16: acc_mask = 0x1f; depth = 16; break;
			case B_RGB24: acc_mask = 0x3f; depth = 24; break;
			case B_RGB32: acc_mask = 0x0f; depth = 32; break;
			default:
				LOG(8,("INIT: unknown color space: 0x%08x\n", target->space));
				return B_ERROR;
		}
		break;
	}

	/* determine pixel multiple based on CRTC memory pitch constraints:
	 * -> all NV cards have same granularity constraints on CRTC1 and CRTC2,
	 *    provided that the CRTC1 and CRTC2 BUFFER register b2 = 0;
	 *
	 * (Note: Don't mix this up with CRTC timing contraints! Those are
	 *        multiples of 8 for horizontal, 1 for vertical timing.) */
	switch (si->ps.card_type)
	{
	default:
//	case NV04:
		/* confirmed for:
		 * TNT1 always;
		 * TNT2, TNT2-M64, GeForce2 MX400, GeForce4 MX440, GeForce4 Ti4200,
		 * GeForceFX 5200: if the CRTC1 (and CRTC2) BUFFER register b2 = 0 */
		/* NOTE:
		 * Unfortunately older cards have a hardware fault that prevents use.
		 * We need doubled granularity on those to prevent the single top line
		 * from shifting to the left!
		 * This is confirmed for TNT2, GeForce2 MX200, GeForce2 MX400.
		 * Confirmed OK are:
		 * GeForce4 MX440, GeForce4 Ti4200, GeForceFX 5200. */
		switch (target->space)
		{
			case B_CMAP8: crtc_mask = 0x0f; break; /* 0x07 */
			case B_RGB15: crtc_mask = 0x07; break; /* 0x03 */
			case B_RGB16: crtc_mask = 0x07; break; /* 0x03 */
			case B_RGB24: crtc_mask = 0x0f; break; /* 0x07 */
			case B_RGB32: crtc_mask = 0x03; break; /* 0x01 */
			default:
				LOG(8,("INIT: unknown color space: 0x%08x\n", target->space));
				return B_ERROR;
		}
		break;
//	default:
		/* confirmed for:
		 * TNT2, TNT2-M64, GeForce2 MX400, GeForce4 MX440, GeForce4 Ti4200,
		 * GeForceFX 5200: if the CRTC1 (and CRTC2) BUFFER register b2 = 1 */
/*		switch (target->space)
		{
			case B_CMAP8: crtc_mask = 0x1f; break;
			case B_RGB15: crtc_mask = 0x0f; break;
			case B_RGB16: crtc_mask = 0x0f; break;
			case B_RGB24: crtc_mask = 0x1f; break;
			case B_RGB32: crtc_mask = 0x07; break;
			default:
				LOG(8,("INIT: unknown color space: 0x%08x\n", target->space));
				return B_ERROR;
		}
		break;
*/	}

	/* set virtual_width limit for accelerated modes */
	/* note:
	 * because of the seemingly 'random' variations in these constraints we take
	 * a reasonable 'lowest common denominator' instead of always true constraints. */
	switch (si->ps.card_arch)
	{
	case NV04A:
		/* confirmed for:
		 * TNT1 (NV04), TNT2 (NV05), TNT2-M64 (NV05M64) in both PIO and DMA acc mode. */
		switch(target->space)
		{
			case B_CMAP8: max_acc_width = 8176; break;
			case B_RGB15: max_acc_width = 4088; break;
			case B_RGB16: max_acc_width = 4088; break;
			case B_RGB24: max_acc_width = 2720; break;
			case B_RGB32: max_acc_width = 2044; break;
			default:
				LOG(8,("INIT: unknown color space: 0x%08x\n", target->space));
				return B_ERROR;
		}
		break;
	default:
		/* confirmed for:
		 * GeForce4 Ti4200 (NV28), GeForceFX 5600 (NV31) in PIO acc mode;
		 * GeForce2 MX400 (NV11), GeForce4 MX440 (NV18), GeForceFX 5200 (NV34) can do
		 * 16368/8184/8184/5456/4092, so a bit better in PIO acc mode;
		 * confirmed for:
		 * GeForce2 MX400 (NV11), GeForce4 MX440 (NV18), GeForcePCX 5750 (NV36),
		 * GeForcePCX 6600 GT (NV43) in DMA acc mode;
		 * GeForce4 Ti4200 (NV28), GeForceFX 5200 (NV34) can do
		 * 16368/8184/8184/5456/4092, so a bit better in DMA acc mode. */
		switch(target->space)
		{
			case B_CMAP8: max_acc_width = 16320; break;
			case B_RGB15: max_acc_width =  8160; break;
			case B_RGB16: max_acc_width =  8160; break;
			case B_RGB24: max_acc_width =  5440; break;
			case B_RGB32: max_acc_width =  4080; break;
			default:
				LOG(8,("INIT: unknown color space: 0x%08x\n", target->space));
				return B_ERROR;
		}
		break;
	}

	/* set virtual_width limit for unaccelerated modes */
	switch (si->ps.card_type)
	{
	default:
//	case NV04:
		/* confirmed for:
		 * TNT1 always;
		 * TNT2, TNT2-M64, GeForce2 MX400, GeForce4 MX440, GeForce4 Ti4200,
		 * GeForceFX 5200: if the CRTC1 (and CRTC2) BUFFER register b2 = 0 */
		/* NOTE:
		 * Unfortunately older cards have a hardware fault that prevents use.
		 * We need doubled granularity on those to prevent the single top line
		 * from shifting to the left!
		 * This is confirmed for TNT2, GeForce2 MX200, GeForce2 MX400.
		 * Confirmed OK are:
		 * GeForce4 MX440, GeForce4 Ti4200, GeForceFX 5200. */
		switch(target->space)
		{
			case B_CMAP8: max_crtc_width = 16368; break; /* 16376 */
			case B_RGB15: max_crtc_width =  8184; break; /*  8188 */
			case B_RGB16: max_crtc_width =  8184; break; /*  8188 */
			case B_RGB24: max_crtc_width =  5456; break; /*  5456 */
			case B_RGB32: max_crtc_width =  4092; break; /*  4094 */
			default:
				LOG(8,("INIT: unknown color space: 0x%08x\n", target->space));
				return B_ERROR;
		}
		break;
//	default:
		/* confirmed for:
		 * TNT2, TNT2-M64, GeForce2 MX400, GeForce4 MX440, GeForce4 Ti4200,
		 * GeForceFX 5200: if the CRTC1 (and CRTC2) BUFFER register b2 = 1 */
/*		switch(target->space)
		{
			case B_CMAP8: max_crtc_width = 16352; break;
			case B_RGB15: max_crtc_width =  8176; break;
			case B_RGB16: max_crtc_width =  8176; break;
			case B_RGB24: max_crtc_width =  5440; break;
			case B_RGB32: max_crtc_width =  4088; break;
			default:
				LOG(8,("INIT: unknown color space: 0x%08x\n", target->space));
				return B_ERROR;
		}
		break;
*/	}

	/* check for acc capability, and adjust mode to adhere to hardware constraints */
	if (max_acc_width <= max_crtc_width)
	{
		/* check if we can setup this mode with acceleration */
		*acc_mode = true;
		/* virtual_width */
		if (target->virtual_width > max_acc_width) *acc_mode = false;
		/* virtual_height */
		/* (NV cards can even do more than this(?)...
		 *  but 4096 is confirmed on all cards at max. accelerated width.) */
		if (target->virtual_height > 4096) *acc_mode = false;

		/* now check virtual_size based on CRTC constraints */
		if (target->virtual_width > max_crtc_width) target->virtual_width = max_crtc_width;
		/* virtual_height: The only constraint here is the cards memory size which is
		 * checked later on in ProposeMode: virtual_height is adjusted then if needed.
		 * 'Limiting here' to the variable size that's at least available (uint16). */
		if (target->virtual_height > 65535) target->virtual_height = 65535;

		/* OK, now we know that virtual_width is valid, and it's needing no slopspace if
		 * it was confined above, so we can finally calculate safely if we need slopspace
		 * for this mode... */
		if (*acc_mode)
		{
			/* the mode needs to adhere to the largest granularity imposed... */
			if (acc_mask < crtc_mask)
				video_pitch = ((target->virtual_width + crtc_mask) & ~crtc_mask);
			else
				video_pitch = ((target->virtual_width + acc_mask) & ~acc_mask);
		}
		else /* unaccelerated mode */
			video_pitch = ((target->virtual_width + crtc_mask) & ~crtc_mask);
	}
	else /* max_acc_width > max_crtc_width */
	{
		/* check if we can setup this mode with acceleration */
		*acc_mode = true;
		/* (we already know virtual_width will be no problem) */
		/* virtual_height */
		/* (NV cards can even do more than this(?)...
		 *  but 4096 is confirmed on all cards at max. accelerated width.) */
		if (target->virtual_height > 4096) *acc_mode = false;

		/* now check virtual_size based on CRTC constraints */
		if (*acc_mode)
		{
			/* note that max_crtc_width already adheres to crtc_mask */
			if (target->virtual_width > (max_crtc_width & ~acc_mask))
					target->virtual_width = (max_crtc_width & ~acc_mask);
		}
		else /* unaccelerated mode */
		{
			if (target->virtual_width > max_crtc_width)
					target->virtual_width = max_crtc_width;
		}
		/* virtual_height: The only constraint here is the cards memory size which is
		 * checked later on in ProposeMode: virtual_height is adjusted then if needed.
		 * 'Limiting here' to the variable size that's at least available (uint16). */
		if (target->virtual_height > 65535) target->virtual_height = 65535;

		/* OK, now we know that virtual_width is valid, and it's needing no slopspace if
		 * it was confined above, so we can finally calculate safely if we need slopspace
		 * for this mode... */
		if (*acc_mode)
		{
			/* the mode needs to adhere to the largest granularity imposed... */
			if (acc_mask < crtc_mask)
				video_pitch = ((target->virtual_width + crtc_mask) & ~crtc_mask);
			else
				video_pitch = ((target->virtual_width + acc_mask) & ~acc_mask);
		}
		else /* unaccelerated mode */
			video_pitch = ((target->virtual_width + crtc_mask) & ~crtc_mask);
	}

	LOG(2,("INIT: memory pitch will be set to %d pixels for colorspace 0x%08x\n",
														video_pitch, target->space));
	if (target->virtual_width != video_pitch)
		LOG(2,("INIT: effective mode slopspace is %d pixels\n",
											(video_pitch - target->virtual_width)));

	/* now calculate bytes_per_row for this mode */
	*bytes_per_row = video_pitch * (depth >> 3);

	return B_OK;
}
#endif
