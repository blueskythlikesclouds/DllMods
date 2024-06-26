#include "CubeMapFix.h"

#include "Configuration.h"

void CubeMapFix::init(ModInfo_t* modInfo)
{
	if (!Configuration::correctCubemaps)
		return;

	modInfo->API->BindFile(modInfo->CurrentMod, "w1a01/+w1a01_trr_cmn.pac", "disk/sonic2013_patch_0/w1a01/w1a01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w1a03/+w1a03_trr_cmn.pac", "disk/sonic2013_patch_0/w1a03/w1a03_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w1a04/+w1a04_trr_cmn.pac", "disk/sonic2013_patch_0/w1a04/w1a04_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w1a06/+w1a06_trr_cmn.pac", "disk/sonic2013_patch_0/w1a06/w1a06_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w2a01/+w2a01_obj.pac", "disk/sonic2013_patch_0/w2a01/w2a01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w2c01/+w2c01_obj.pac", "disk/sonic2013_patch_0/w2c01/w2c01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w3a01/+w3a01_obj.pac", "disk/sonic2013_patch_0/w3a01/w3a01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w3a03/+w3a03_obj.pac", "disk/sonic2013_patch_0/w3a03/w3a03_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w3a03/+w3a03_trr_cmn.pac", "disk/sonic2013_patch_0/w3a03/w3a03_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w3a06/+w3a06_obj.pac", "disk/sonic2013_patch_0/w3a06/w3a06_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w3a06/+w3a06_trr_cmn.pac", "disk/sonic2013_patch_0/w3a06/w3a06_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w4a01/+w4a01_obj.pac", "disk/sonic2013_patch_0/w4a01/w4a01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w4a01/+w4a01_trr_cmn.pac", "disk/sonic2013_patch_0/w4a01/w4a01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w4a04/+w4a04_obj.pac", "disk/sonic2013_patch_0/w4a04/w4a04_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w4a04/+w4a04_trr_cmn.pac", "disk/sonic2013_patch_0/w4a04/w4a04_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w4a06/+w4a06_far.pac", "disk/sonic2013_patch_0/w4a06/w4a06_far_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w4a06/+w4a06_obj.pac", "disk/sonic2013_patch_0/w4a06/w4a06_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w4a06/+w4a06_trr_cmn.pac", "disk/sonic2013_patch_0/w4a06/w4a06_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w5a01/+w5a01_obj.pac", "disk/sonic2013_patch_0/w5a01/w5a01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w5a01/+w5a01_trr_cmn.pac", "disk/sonic2013_patch_0/w5a01/w5a01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w5a02/+w5a02_trr_cmn.pac", "disk/sonic2013_patch_0/w5a02/w5a02_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w5a03/+w5a03_far.pac", "disk/sonic2013_patch_0/w5a03/w5a03_far_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w5a03/+w5a03_trr_cmn.pac", "disk/sonic2013_patch_0/w5a03/w5a03_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w6a03/+w6a03_obj.pac", "disk/sonic2013_patch_0/w6a03/w6a03_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "w6a05/+w6a05_obj.pac", "disk/sonic2013_patch_0/w6a05/w6a05_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+diorama_secret.pac", "disk/sonic2013_patch_0/worldmap/diorama_secret_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+diorama_w4a01.pac", "disk/sonic2013_patch_0/worldmap/diorama_w4a01_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+diorama_w7a06.pac", "disk/sonic2013_patch_0/worldmap/diorama_w7a06_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+diorama_xcub01.pac", "disk/sonic2013_patch_0/worldmap/diorama_xcub01_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+diorama_xind02.pac", "disk/sonic2013_patch_0/worldmap/diorama_xind02_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+diorama_xnim02.pac", "disk/sonic2013_patch_0/worldmap/diorama_xnim02_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+diorama_xsea01.pac", "disk/sonic2013_patch_0/worldmap/diorama_xsea01_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+diorama_xsea02.pac", "disk/sonic2013_patch_0/worldmap/diorama_xsea02_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+diorama_xsky01.pac", "disk/sonic2013_patch_0/worldmap/diorama_xsky01_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+worldmap_modeselect.pac", "disk/sonic2013_patch_0/worldmap/worldmap_modeselect_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "worldmap/+worldmap_trr_cmn.pac", "disk/sonic2013_patch_0/worldmap/worldmap_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xbee01/+xbee01_obj.pac", "disk/sonic2013_patch_0/xbee01/xbee01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xcas01/+xcas01_far.pac", "disk/sonic2013_patch_0/xcas01/xcas01_far_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xcas01/+xcas01_trr_cmn.pac", "disk/sonic2013_patch_0/xcas01/xcas01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xcub01/+xcub01_obj.pac", "disk/sonic2013_patch_0/xcub01/xcub01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xcub01/+xcub01_trr_cmn.pac", "disk/sonic2013_patch_0/xcub01/xcub01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xind01/+xind01_obj.pac", "disk/sonic2013_patch_0/xind01/xind01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xind01/+xind01_far.pac", "disk/sonic2013_patch_0/xind02/xind02_far_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xind02/+xind02_obj.pac", "disk/sonic2013_patch_0/xind02/xind02_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xind02/+xind02_trr_cmn.pac", "disk/sonic2013_patch_0/xind02/xind02_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xmoa01/+xmoa01_trr_cmn.pac", "disk/sonic2013_patch_0/xmoa01/xmoa01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xnim01/+xnim01_obj.pac", "disk/sonic2013_patch_0/xnim01/xnim01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xnim01/+xnim01_trr_cmn.pac", "disk/sonic2013_patch_0/xnim01/xnim01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xnim02/+xnim02_obj.pac", "disk/sonic2013_patch_0/xnim02/xnim02_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xnim02/+xnim02_trr_cmn.pac", "disk/sonic2013_patch_0/xnim02/xnim02_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xsea01/+xsea01_obj.pac", "disk/sonic2013_patch_0/xsea01/xsea01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xsea01/+xsea01_trr_cmn.pac", "disk/sonic2013_patch_0/xsea01/xsea01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xsea02/+xsea02_obj.pac", "disk/sonic2013_patch_0/xsea02/xsea02_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xsea02/+xsea02_trr_cmn.pac", "disk/sonic2013_patch_0/xsea02/xsea02_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xsky01/+xsky01_obj.pac", "disk/sonic2013_patch_0/xsky01/xsky01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xsky01/+xsky01_trr_cmn.pac", "disk/sonic2013_patch_0/xsky01/xsky01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "xswe01/+xswe01_trr_cmn.pac", "disk/sonic2013_patch_0/xswe01/xswe01_trr_cmn_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "zdlc01/+zdlc01_obj.pac", "dlc/0010/sonic2013_dlc_0010/zdlc01/zdlc01_obj_fix.pac", 1);
	modInfo->API->BindFile(modInfo->CurrentMod, "zdlc01/+zdlc01_trr_cmn.pac", "dlc/0010/sonic2013_dlc_0010/zdlc01/zdlc01_trr_cmn_fix.pac", 1);
}
