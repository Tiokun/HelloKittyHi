#pragma once

#include "../Main/Main.h"

#define FIX_MIN_FOV_HEAD	30
#define FIX_MIN_FOV_CHEST	50
#define FOV_BASE_DISTANCE	200

namespace Settings
{
	void LoadSettings( string szIniFile );
	void SaveSettings( string szIniFile );
	extern float hitmarkerAlpha;
	int TriggerCharToKey( const char* Key );

	namespace InventoryChanger {
		extern bool enabled;
		extern std::vector<uint32_t> medals;
		extern bool equipped_medal_override;
		extern uint32_t equipped_medal;
	}

	namespace Aimbot
	{
		struct weapon_aim_s
		{
			//[swap_lines]
			bool aim_Active;
			int aim_Smooth;
			int aim_Fov;
			int aim_FovType;
			int aim_BestHit;
			int aim_Spot;
			int aim_Delay;
			int aim_Rcs;
			int aim_RcsFov;
			int aim_RcsSmooth;
			int aim_RcsClampType;
			bool aim_AutoPistol;
			//[/swap_lines]
		};
		//[swap_lines]
		extern int	aim_Backtracktickrate;
		extern bool aim_Backtrack;
		extern bool aim_DrawBacktrack;
		extern bool aim_Deathmatch;
		extern bool aim_WallAttack;
		extern bool aim_CheckSmoke;
		extern bool aim_AntiJump;
		extern int	aim_RcsType;
		extern bool aim_DrawFov;
		extern bool aim_DrawSpot;
		
		extern weapon_aim_s weapon_aim_settings[33];
		//[/swap_lines]
	}

	/*namespace Triggerbot
	{
		struct weapon_trigger_s
		{
			//[swap_lines]
			int trigger_DistanceMin;
			int trigger_DistanceMax;
			int trigger_Fov;
			int trigger_DelayBefore;
			int trigger_DelayAfter;
			bool trigger_HeadOnly;
			int trigger_HitGroup;
			int trigger_Assist;
			int trigger_AssistRcs;
			int trigger_AssistFov;
			int trigger_AssistFovType;
			int trigger_AssistSmooth;
			//[/swap_lines]
		};
		//[swap_lines]
		extern int trigger_Enable;
		extern bool trigger_Deathmatch;
		extern bool trigger_WallAttack;
		extern bool trigger_FastZoom;
		extern int trigger_Key;
		extern int trigger_KeyMode;
		extern bool trigger_SmokCheck;
		extern bool trigger_DrawFov;
		extern bool trigger_DrawSpot;
		extern bool trigger_DrawFovAssist;
		
		extern weapon_trigger_s weapon_trigger_settings[33];
		//[/swap_lines]
	}*/

	namespace Esp
	{
		//[swap_lines]
		extern int esp_Style; // 0 - Box 1 - CoalBox
		extern int esp_Size;
		extern bool esp_Line;
		extern bool esp_Outline; // Box ( 0 - Box 1 - OutlineBox ) ,
						  // CoalBox ( 0 - CoalBox 1 - OutlineCoalBox )	

		extern bool esp_Time;
		extern bool esp_Watermark;
		extern bool esp_Cheatbuild;
		extern bool esp_Name;
		extern bool esp_Rank;
		extern int esp_Health;
		extern int esp_Armor;
		extern bool esp_Weapon;
		extern bool esp_Ammo;
		extern bool esp_Distance;
		extern bool esp_Sound;

		extern bool esp_HitMarker;
		extern int esp_HitMarkerSound;
		extern float esp_HitMarkerColor[3];
		extern float GrenadeHelper[3];
		extern bool esp_ghelper;
		extern bool esp_logs;

		extern bool esp_Skeleton;
		extern int esp_BulletTrace;
		extern bool esp_Team;
		extern bool esp_Enemy;
		extern int esp_Visible;
		extern int esp_ChamsVisible;

		extern int esp_Chams;
		extern bool esp_Bomb;
		extern int esp_BombTimer;
		extern bool esp_WorldWeapons;
		extern bool esp_WorldGrenade;
		extern bool esp_Defusing;

		extern float esp_Color_CT[3];
		extern float esp_Color_TT[3];
		extern float esp_Color_VCT[3];
		extern float esp_Color_VTT[3];

		extern float chams_Color_CT[3];
		extern float chams_Color_TT[3];
		extern float chams_Color_VCT[3];
		extern float chams_Color_VTT[3];
		//[/swap_lines]
	}

	namespace Radar
	{
		//[swap_lines]
		extern bool rad_Active;
		extern bool rad_Team;
		extern bool rad_Enemy;
		extern bool rad_Sound;
		extern bool rad_InGame;
		extern int rad_Range;
		extern int rad_Alpha;
		extern float rad_Color_CT[3];
		extern float rad_Color_TT[3];
		extern float rad_Color_VCT[3];
		extern float rad_Color_VTT[3];
		//[/swap_lines]
	}

	namespace Skin
	{
		//[swap_lines]
		extern int knf_ct_model;
		extern int knf_ct_skin;
		extern int knf_tt_model;
		extern int knf_tt_skin;
		extern int gloves_skin;
		//[/swap_lines]
	}

	namespace Misc
	{
		//[swap_lines]
		extern QAngle qLastTickAngle;
		extern bool misc_ThirdPerson;
		extern float misc_ThirdPersonRange;
		extern const char* misc_SkyName;
		extern bool misc_ChamsMaterials;
		extern int misc_ChamsMaterialsList;
		extern bool misc_ArmMaterials;
		extern int misc_ArmMaterialsList;
		extern int misc_ArmMaterialsType;
		extern int misc_CurrentSky;
		extern bool misc_KnifeChanger;
		extern bool misc_NoSky;
		extern bool misc_Snow;
		extern bool misc_EPostprocess;
		extern bool misc_Postprocess;
		extern bool misc_Bhop;
		extern int misc_Clan;
		extern bool misc_spamregular;
		extern bool misc_spamrandom;
		extern bool misc_Punch;
		extern bool misc_AwpAim;
		extern bool misc_NoFlash;	
		extern bool misc_WireHands;
		extern bool misc_NoHands;
		extern bool misc_AutoStrafe;
		extern bool misc_AutoAccept;
		extern bool misc_Spectators;
		extern bool misc_RainbowMenu;
		extern float misc_RainbowSpeed;
		extern bool misc_FovChanger;
		extern int misc_FovView;
		extern int misc_FovModelView;
		extern float misc_AwpAimColor[3];
		extern float hitmarkerAlpha;

		extern bool misc_ranktoggle;
		extern int misc_Rank;
		extern int misc_XP;
		extern int misc_Wins;
		extern int misc_Level;
		extern int misc_Friendly;
		extern int misc_Leader;
		extern int misc_Teacher;
		extern int misc_Rank_Combo; //Add this for Rank Box
	}
}

#define CHEAT_NAME "LegitWare - Counter-Strike: Global Offensive (BETA)"
#define WATER_MARK "Welcome to LegitWare"
#define AIMBOT_TEXT "Aim"
#define COLORS_TEXT "Colors"
#define CONFIG_TEXT "Config"
//#define TRIGGER_TEXT "Trigger"
#define KNIFEBOT_TEXT "Knifebot"
#define VISUAL_TEXT "ESP"
#define RADAR_TEXT "Radar"
#define SKIN_TEXT "Skin"
#define MISC_TEXT "Misc"

#define CVAR_AIMBOT_DEATHMATCH "aim_Deathmatch"
#define CVAR_AIMBOT_WALLATTACK "aim_WallAttack"
#define CVAR_AIMBOT_CHECKSMOKE "aim_CheckSmoke"
#define CVAR_AIMBOT_ANTIJUMP	"aim_IgnoreJump"
#define CVAR_AIMBOT_RCSTYPE "aim_RcsType"
#define CVAR_AIMBOT_DRAWFOV "aim_DrawFov"
#define CVAR_AIMBOT_DRAWSPOT "aim_DrawSpot"
#define CVAR_AIMBOT_BACKTRACK "aim_Backtrack"
#define CVAR_AIMBOT_DRAWBACKTRACK "aim_DrawBacktrack"
#define CVAR_AIMBOT_BACKTRACK_TICK "aim_Backtracktickrate"

#define CVAR_AIMBOT_ACTIVE "aim_Active"
#define CVAR_AIMBOT_SMOOTH "aim_Smooth"
#define CVAR_AIMBOT_FOV "aim_Fov"
#define CVAR_AIMBOT_FOVTYPE "aim_FovType"
#define CVAR_AIMBOT_BESTHIT "aim_BestHit"
#define CVAR_AIMBOT_SPOT "aim_Spot"
#define CVAR_AIMBOT_DELAY "aim_Delay"

#define CVAR_AIMBOT_RCS "aim_Rcs"
#define CVAR_AIMBOT_RCSFOV "aim_RcsFov"
#define CVAR_AIMBOT_RCSSMOOTH "aim_RcsSmooth"
#define CVAR_AIMBOT_RCSCLAMPTYPE "aim_RcsClampType"
#define CVAR_AIMBOT_AUTOPISTOL "aim_AutoPistol"

/*#define CVAR_TRIGGER_ENABLED "triggerbot: enabled"
#define CVAR_TRIGGER_DISABLED "triggerbot: disabled"

#define CVAR_TRIGGER_ENABLE "trigger_Enable"
#define CVAR_TRIGGER_DEATHMATCH "trigger_Deathmatch"
#define CVAR_TRIGGER_WALLATTACK "trigger_WallAttack"
#define CVAR_TRIGGER_FASTZOOM "trigger_FastZoom"
#define CVAR_TRIGGER_KEY "trigger_Key"
#define CVAR_TRIGGER_KEYMODE "trigger_KeyMode"
#define CVAR_TRIGGER_SMOKCHECK "trigger_SmokCheck"
#define CVAR_TRIGGER_DRAWFOV "trigger_DrawFov"
#define CVAR_TRIGGER_DRAWSPOT "trigger_DrawSpot"
#define CVAR_TRIGGER_DRAWFOVASSIST "trigger_DrawFovAssist"

#define CVAR_TRIGGER_DISTANCEMIN "trigger_DistanceMin"
#define CVAR_TRIGGER_DISTANCEMAX "trigger_DistanceMax"
#define CVAR_TRIGGER_FOV "trigger_Fov"
#define CVAR_TRIGGER_DELAYBEFORE "trigger_DelayBefore"
#define CVAR_TRIGGER_DELAYAFTER "trigger_DelayAfter"
#define CVAR_TRIGGER_HEADONLY "trigger_HeadOnly"
#define CVAR_TRIGGER_HITGROUP "trigger_HitGroup"
#define CVAR_TRIGGER_ASSIST "trigger_Assist"
#define CVAR_TRIGGER_ASSISTRCS "trigger_AssistRcs"
#define CVAR_TRIGGER_ASSISTFOV "trigger_AssistFov"
#define CVAR_TRIGGER_ASSISTFOVTYPE "trigger_AssistFovType"
#define CVAR_TRIGGER_ASSISTSMOOTH "trigger_AssistSmooth"*/

#define CVAR_KEY_MOUSE3 "MOUSE3"
#define CVAR_KEY_MOUSE4 "MOUSE4"
#define CVAR_KEY_MOUSE5 "MOUSE5"

#define CVAR_ESP_STYLE "esp_Style"
#define CVAR_ESP_SIZE "esp_Size"
#define CVAR_ESP_LINE "esp_Line"
#define CVAR_ESP_OUTLINE "esp_Outline"
#define CVAR_ESP_NAME "esp_Name"
#define CVAR_ESP_RANK "esp_Rank"
#define CVAR_ESP_TIME "esp_Time"
#define CVAR_ESP_WATER "esp_Watermark"
#define CVAR_ESP_CHEATBUILD "esp_Cheatbuild"

#define CVAR_ESP_HITMARKER "esp_HitMarker"
#define CVAR_ESP_HITMARKER_SOUND "esp_HitMarkerSound"
#define CVAR_ESP_HITMARKER_COLOR "esp_HitMarkerColor"

#define CVAR_ESP_HEALTH "esp_Health"
#define CVAR_ESP_ARMOR "esp_Armor"
#define CVAR_ESP_WEAPON "esp_Weapon"
#define CVAR_ESP_AMMO "esp_Ammo"
#define CVAR_ESP_DISTANCE "esp_Distance"
#define CVAR_ESP_SOUND "esp_Sound"


#define CVAR_ESP_SKELETON "esp_Skeleton"
#define CVAR_ESP_BULLETTRACE "esp_BulletTrace"
#define CVAR_ESP_TEAM "esp_Team"
#define CVAR_ESP_ENEMY "esp_Enemy"
#define CVAR_ESP_VISIBLE "esp_Visible"
#define CVAR_ESP_CHAMSVISIBLE "ESP_ChamsVisible"

#define CVAR_ESP_CHAMS "esp_Chams"
#define CVAR_ESP_BOMB "esp_Bomb"
#define CVAR_ESP_ISDEFUSING "esp_Defusing"
#define CVAR_ESP_BOMBTIMER "esp_BombTimer"
#define CVAR_ESP_WORLDWEAPONS "esp_WorldWeapons"
#define CVAR_ESP_WORLDGRENADE "esp_WorldGrenade"
#define CVAR_ESP_LOGS "esp_logs"
#define CVAR_ESP_GHELPER "esp_ghelper"

#define CVAR_ESP_COLOR_CT "esp_Color_CT"
#define CVAR_ESP_COLOR_TT "esp_Color_TT"
#define CVAR_ESP_COLOR_VCT "esp_Color_VCT"
#define CVAR_ESP_COLOR_VTT "esp_Color_VTT"

#define CVAR_CHAMS_COLOR_CT "chams_Color_CT"
#define CVAR_CHAMS_COLOR_TT "chams_Color_TT"
#define CVAR_CHAMS_COLOR_VCT "chams_Color_VCT"
#define CVAR_CHAMS_COLOR_VTT "chams_Color_VTT"

#define C4_TIMER_STRING "C4 Timer: %0.2f"

#define CVAR_RAD_ACTIVE "rad_Active"
#define CVAR_RAD_TEAM "rad_Team"
#define CVAR_RAD_ENEMY "rad_Enemy"
#define CVAR_RAD_SOUND "rad_Sound"
#define CVAR_RAD_INGAME "rad_InGame"
#define CVAR_RAD_RANGE "rad_Range"
#define CVAR_RAD_ALPHA "rad_Alpha"

#define CVAR_RAD_COLOR_CT "rad_Color_CT"
#define CVAR_RAD_COLOR_TT "rad_Color_TT"
#define CVAR_RAD_COLOR_VCT "rad_Color_VCT"
#define CVAR_RAD_COLOR_VTT "rad_Color_VTT"

#define CVAR_RAD_COLOR_CT "rad_Color_CT"
#define CVAR_RAD_COLOR_TT "rad_Color_TT"
#define CVAR_RAD_COLOR_VCT "rad_Color_VCT"
#define CVAR_RAD_COLOR_VTT "rad_Color_VTT"

#define CVAR_KNIFEBOT_ACTIVE "knf_Active"
#define CVAR_KNIFEBOT_TEAM "knf_Team"
#define CVAR_KNIFEBOT_ATTACK "knf_Attack"
#define CVAR_KNIFEBOT_DISTATTACK "knf_DistAttack"
#define CVAR_KNIFEBOT_DISTATTACK2 "knf_DistAttack2"

#define CVAR_SKIN_CT_MODEL "knf_ct_model"
#define CVAR_SKIN_CT_SKIN "knf_ct_skin"
#define CVAR_SKIN_TT_MODEL "knf_tt_model"
#define CVAR_SKIN_TT_SKIN "knf_tt_skin"
#define CVAR_SKIN_GLOVES "gloves_skin"

#define CVAR_MISC_EPOSTPROCESS "misc_EPostprocess"
#define CVAR_MISC_POSTPROCESS "misc_Postprocess"
#define CVAR_MISC_BHOP "misc_Bhop"
#define CVAR_MISC_PUNCH "misc_Punch"
#define CVAR_MISC_AWPAIM "misc_AwpAim"
#define CVAR_MISC_NOFLASH "misc_NoFlash"
#define CVAR_MISC_NOHANDS "misc_NoHands"
#define CVAR_MISC_WIREHANDS "misc_WireHands"
#define CVAR_MISC_AUTOSTRAFE "misc_AutoStrafe"
#define CVAR_MISC_AUTOACCEPT "misc_AutoAccept"
#define CVAR_MISC_SPECTATORS "misc_Spectators"
#define CVAR_MISC_FOV_CHANGER "misc_FovChanger"
#define CVAR_MISC_FOV_VIEW "misc_FovView"
#define CVAR_MISC_FOV_MDL_VIEW "misc_FovModelView"
#define CVAR_MISC_AWPAIM_COLOR "misc_AwpAimColor"
#define CVAR_MISC_RANKTOGGLE "misc_ranktoggle"
#define CVAR_MISC_RANK_COMBO "misc_Rank_Combo"
#define CVAR_MISC_LEVEL "misc_Level"
#define CVAR_MISC_XP "misc_XP"
#define CVAR_MISC_WINS "misc_Wins"
#define CVAR_MISC_FRIENDLY "misc_Friendly"
#define CVAR_MISC_LEADER "misc_Leader"
#define CVAR_MISC_TEACHER "misc_Teacher"