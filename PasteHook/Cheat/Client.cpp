#include "Client.h"
#include <ctime>
#include "../Gui/memoryfonts.h"
#include "Backtrack/LagComp.h"
#include "GrenadeHelper/CGrenadeAPI.h"


//[enc_string_enable /]
//[junk_enable /]


ImFont* imgfont;
ImFont* font;


namespace Client
{
	//[swap_lines]
	int	iScreenWidth = 0;
	int	iScreenHeight = 0;

	string BaseDir;
	string LogFile;
	string GuiFile;
	string IniFile;

	vector<string> ConfigList;

	Vector2D	g_vCenterScreen = Vector2D(0.f, 0.f);

	CPlayers*	g_pPlayers = nullptr;
	CRender*	g_pRender = nullptr;
	CGui*		g_pGui = nullptr;

	CAimbot*	g_pAimbot = nullptr;
	CTriggerbot* g_pTriggerbot = nullptr;
	CEsp*		g_pEsp = nullptr;
	CRadar*		g_pRadar = nullptr;
	CSkin*		g_pSkin = nullptr;
	CMisc*		g_pMisc = nullptr;
	CInventoryChanger* g_pInventoryChanger = nullptr;
	CInventoryChanger1* g_pInventoryChanger1 = nullptr;

	bool		bC4Timer = false;
	int			iC4Timer = 40;

	int			iWeaponID = 0;
	int			iWeaponSelectIndex = WEAPON_DEAGLE;
	int			iWeaponSelectSkinIndex = -1;
	//[/swap_lines]

	void ReadConfigs(LPCTSTR lpszFileName)
	{
		if (!strstr(lpszFileName, "gui.ini"))
		{
			ConfigList.push_back(lpszFileName);
		}
	}

	void RefreshConfigs()
	{
		ConfigList.clear();
		string ConfigDir = "C:/LegitWare/*.ini";
		SearchFiles(ConfigDir.c_str(), ReadConfigs, FALSE);
	}

	bool SendClientHello()
	{
		CMsgClientHello Message;

		Message.set_client_session_need(1);
		Message.clear_socache_have_versions();

		void* ptr = malloc(Message.ByteSize() + 8);

		if (!ptr)
			return false;

		((uint32_t*)ptr)[0] = k_EMsgGCClientHello | ((DWORD)1 << 31);
		((uint32_t*)ptr)[1] = 0;

		Message.SerializeToArray((void*)((DWORD)ptr + 8), Message.ByteSize());

		bool result = Interfaces::SteamGameCoordinator()->SendMessage(k_EMsgGCClientHello | ((DWORD)1 << 31), ptr, Message.ByteSize() + 8) == k_EGCResultOK;

		free(ptr);

		return result;
	}

	bool SendMMHello()
	{
		CMsgGCCStrike15_v2_MatchmakingClient2GCHello Message;
		void* ptr = malloc(Message.ByteSize() + 8);
		if (!ptr)
			return false;

		auto unMsgType = k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31);
		((uint32_t*)ptr)[0] = unMsgType;
		((uint32_t*)ptr)[1] = 0;

		Message.SerializeToArray((void*)((DWORD)ptr + 8), Message.ByteSize());

		bool result = Interfaces::SteamGameCoordinator()->SendMessage(k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31), ptr, Message.ByteSize() + 8) == k_EGCResultOK;

		free(ptr);
		return result;
	}

	bool Initialize(IDirect3DDevice9* pDevice)
	{
		g_pPlayers = new CPlayers();
		g_pRender = new CRender(pDevice);
		g_pGui = new CGui();

		g_pAimbot = new CAimbot();
		//g_pTriggerbot = new CTriggerbot();
		g_pEsp = new CEsp();
		g_pSkin = new CSkin();
		g_pRadar = new CRadar();
		g_pMisc = new CMisc();
		g_pInventoryChanger = new CInventoryChanger();
		g_pInventoryChanger1 = new CInventoryChanger1();

		CreateDirectoryW(L"C:\\LegitWare", NULL);
		GuiFile = "C:/LegitWare/gui.ini";
		IniFile = "C:/LegitWare/default.ini";

		g_pSkin->InitalizeSkins();

		Settings::LoadSettings(IniFile);

		iWeaponSelectSkinIndex = GetWeaponSkinIndexFromPaintKit(g_SkinChangerCfg[iWeaponSelectIndex].nFallbackPaintKit);

		g_pGui->GUI_Init(pDevice);

		RefreshConfigs();

		return true;
	}

	void Shutdown()
	{
		DELETE_MOD(g_pPlayers);
		DELETE_MOD(g_pRender);
		DELETE_MOD(g_pGui);

		DELETE_MOD(g_pAimbot);
		//DELETE_MOD(g_pTriggerbot);
		DELETE_MOD(g_pEsp);
		DELETE_MOD(g_pRadar);
		DELETE_MOD(g_pSkin);
		DELETE_MOD(g_pMisc);
	}


	void OnRender()
	{
		if (g_pRender && !Interfaces::Engine()->IsTakingScreenshot() && Interfaces::Engine()->IsActiveApp())
		{
			g_pRender->BeginRender();

			if (g_pGui)
				g_pGui->GUI_Draw_Elements();

			Interfaces::Engine()->GetScreenSize(iScreenWidth, iScreenHeight);

			g_vCenterScreen.x = iScreenWidth / 2.f;
			g_vCenterScreen.y = iScreenHeight / 2.f;


			
			
			if (Settings::Esp::esp_Watermark)
			{
				g_pRender->Text(2, 2, false, true, Color::LegitWare(), WATER_MARK);
			}

			if (Settings::Esp::esp_Cheatbuild)
				g_pRender->Text(15, 45, false, true, Color::White(), "Last cheat compile: %s : %s", __DATE__, __TIME__);
			
			{
				if (g_pEsp)
					g_pEsp->OnRender();

				if (g_pMisc)
				{
					g_pMisc->OnRender();
					g_pMisc->OnRenderSpectatorList();
				}
			}

			std::time_t result = std::time(nullptr);

			if (Settings::Esp::esp_Time)
				g_pRender->Text(15, 30, false, true, Color::White(), std::asctime(std::localtime(&result)));

			g_pRender->EndRender();
		}
	}

	void OnLostDevice()
	{
		if (g_pRender)
			g_pRender->OnLostDevice();

		if (g_pGui)
			ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	void OnResetDevice()
	{
		if (g_pRender)
			g_pRender->OnResetDevice();

		if (g_pGui)
			ImGui_ImplDX9_CreateDeviceObjects();
	}

	void OnRetrieveMessage(void* ecx, void* edx, uint32_t *punMsgType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize)
	{
		g_pInventoryChanger->PostRetrieveMessage(punMsgType, pubDest, cubDest, pcubMsgSize);
		g_pInventoryChanger1->PostRetrieveMessage(punMsgType, pubDest, cubDest, pcubMsgSize);
	}

	void OnSendMessage(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData)
	{

		void* pubDataMutable = const_cast<void*>(pubData);
		g_pInventoryChanger->PreSendMessage(unMsgType, pubDataMutable, cubData);
		g_pInventoryChanger1->PreSendMessage(unMsgType, pubDataMutable, cubData);
	}

	void OnCreateMove(CUserCmd* pCmd)
	{
		if (g_pPlayers && Interfaces::Engine()->IsInGame())
		{
			g_pPlayers->Update();

			if (g_pEsp)
				g_pEsp->OnCreateMove(pCmd);

			if (IsLocalAlive())
			{
				if (!bIsGuiVisible)
				{
					int iWeaponSettingsSelectID = GetWeaponSettingsSelectID();

					if (iWeaponSettingsSelectID >= 0)
						iWeaponID = iWeaponSettingsSelectID;
				}

				if (g_pAimbot)
					g_pAimbot->OnCreateMove(pCmd, g_pPlayers->GetLocal());

				if (g_pMisc)
					g_pMisc->OnCreateMove(pCmd);

				backtracking->legitBackTrack(pCmd);
			}
		}
	}

	void OnFireEventClientSideThink(IGameEvent* pEvent)
	{
		if (!strcmp(pEvent->GetName(), "player_connect_full") ||
			!strcmp(pEvent->GetName(), "round_start") ||
			!strcmp(pEvent->GetName(), "cs_game_disconnected"))
		{
			if (g_pPlayers)
				g_pPlayers->Clear();

			if (g_pEsp)
				g_pEsp->OnReset();
		}

		if (Interfaces::Engine()->IsConnected())
		{
			hitmarker::singleton()->initialize();

			if (g_pEsp)
				g_pEsp->OnEvents(pEvent);

			if (g_pSkin)
				g_pSkin->OnEvents(pEvent);

		}
	}

	void OnFrameStageNotify(ClientFrameStage_t Stage)
	{
		if (Interfaces::Engine()->IsInGame() && Interfaces::Engine()->IsConnected())
		{
			ConVar* sv_cheats = Interfaces::GetConVar()->FindVar("sv_cheats");
			SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
			sv_cheats_spoofed->SetInt(1);

			if (g_pMisc)
				g_pMisc->FrameStageNotify(Stage);

			Skin_OnFrameStageNotify(Stage);
			Gloves_OnFrameStageNotify(Stage);
		}
	}

	void OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
	{
		if (Interfaces::Engine()->IsInGame() && ctx && pCustomBoneToWorld)
		{
			if (g_pEsp)
				g_pEsp->OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

			if (g_pMisc)
				g_pMisc->OnDrawModelExecute();
		}
	}

	void OnPlaySound(const Vector* pOrigin, const char* pszSoundName)
	{
		if (!pszSoundName || !Interfaces::Engine()->IsInGame())
			return;

		if (!strstr(pszSoundName, "bulletLtoR") &&
			!strstr(pszSoundName, "rics/ric") &&
			!strstr(pszSoundName, "impact_bullet"))
		{
			if (g_pEsp && IsLocalAlive() && Settings::Esp::esp_Sound && pOrigin)
			{
				if (!GetVisibleOrigin(*pOrigin))
					g_pEsp->SoundEsp.AddSound(*pOrigin);
			}
		}
	}

	void OnPlaySound(const char* pszSoundName)
	{
		if (g_pMisc)
			g_pMisc->OnPlaySound(pszSoundName);
	}

	void OnOverrideView(CViewSetup* pSetup)
	{
		if (g_pMisc)
			g_pMisc->OnOverrideView(pSetup);
	}

	void OnGetViewModelFOV(float& fov)
	{
		if (g_pMisc)
			g_pMisc->OnGetViewModelFOV(fov);
	}

	void OnRenderGUI()
	{
		ImGui::SetNextWindowSize(ImVec2(628.f, 400.f));

		if (ImGui::Begin(CHEAT_NAME, &bIsGuiVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			ImGui::PushFont(imgfont);

			if (Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_FovType > 1)
				Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_FovType = 1;

			if (Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_BestHit > 1)
				Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_BestHit = 1;

			if (Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Spot > 5)
				Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Spot = 5;

			/*
			const char* tabNames[] = {
				AIMBOT_TEXT , TRIGGER_TEXT , VISUAL_TEXT , RADAR_TEXT , 
				MISC_TEXT , COLORS_TEXT , CONFIG_TEXT };
			*/
			

			const char* tabNames[] = {
				"A",	// Aim Tab
              //"F",	// Trigger Tab
				"D",	// Visuals
				"G",	// Skin Tab
				"E",	// Misc Tab	
				"C",	// Color
				"B"		// Config Tab
			};
			

			

			static int tabOrder[] = { 0 , 1 , 2 , 3 , 4 , 5};
			static int tabSelected = 0;
			const bool tabChanged = ImGui::TabLabels(tabNames,
				sizeof(tabNames) / sizeof(tabNames[0]),
				tabSelected, tabOrder);

			ImGui::PopFont();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			float SpaceLineOne = 120.f;
			float SpaceLineTwo = 220.f;
			float SpaceLineThr = 320.f;


				if (tabSelected == 0) // Aimbot
				{
					ImGui::BeginGroup();
					ImGui::BeginChild(1, ImVec2(0, 0), true);
					{
					ImGui::Text("Aimbot");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::PushItemWidth(110.f);
					ImGui::Text("Current Weapon: ");
					ImGui::SameLine();
					ImGui::Combo("##AimWeapon", &iWeaponID, pWeaponData, IM_ARRAYSIZE(pWeaponData));
					ImGui::PopItemWidth();

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox("Deathmatch", &Settings::Aimbot::aim_Deathmatch);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("WallAttack", &Settings::Aimbot::aim_WallAttack);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("CheckSmoke", &Settings::Aimbot::aim_CheckSmoke);

					ImGui::Checkbox("AntiJump", &Settings::Aimbot::aim_AntiJump);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Draw Fov", &Settings::Aimbot::aim_DrawFov);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("DrawSpot", &Settings::Aimbot::aim_DrawSpot);

					ImGui::Checkbox("Backtrack", &Settings::Aimbot::aim_Backtrack);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Draw Ticks", &Settings::Aimbot::aim_DrawBacktrack);
					ImGui::SliderInt("Ticks", &Settings::Aimbot::aim_Backtracktickrate, 1, 12);

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox("Active", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Active);

					if (iWeaponID <= 9)
					{
						ImGui::SameLine();
						ImGui::Checkbox("Autopistol", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_AutoPistol);
					}
					ImGui::PushItemWidth(362.f);
					ImGui::SliderInt("Smooth", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Smooth, 1, 300);
					ImGui::SliderInt("Fov", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Fov, 1, 300);
					ImGui::PopItemWidth();

					const char* AimFovType[] = { "Dynamic" , "Static" };
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("Fov Type", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_FovType, AimFovType, IM_ARRAYSIZE(AimFovType));
					ImGui::PopItemWidth();

					const char* BestHit[] = { "Disable" , "Enable" };
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("BestHit", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_BestHit, BestHit, IM_ARRAYSIZE(BestHit));

					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("if disabled then used Aimspot");

					ImGui::PopItemWidth();

					const char* Aimspot[] = { "Head" , "Neck" , "Low Neck" , "Body" , "Thorax" , "Chest" };
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("Aimspot", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Spot, Aimspot, IM_ARRAYSIZE(Aimspot));
					ImGui::PopItemWidth();

					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::PushItemWidth(362.f);
					ImGui::SliderInt("ShotDelay", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Delay, 0, 200);
					ImGui::SliderInt("Rcs", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_Rcs, 0, 100);
					ImGui::PopItemWidth();

					if (iWeaponID >= 10 && iWeaponID <= 30)
					{
						ImGui::PushItemWidth(362.f);
						ImGui::SliderInt("Rcs Fov", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_RcsFov, 1, 300);
						ImGui::SliderInt("Rcs Smooth", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_RcsSmooth, 1, 300);
						ImGui::PopItemWidth();

						const char* ClampType[] = { "All Target" , "Shot" , "Shot + Target" };
						ImGui::PushItemWidth(362.f);
						ImGui::Combo("Rcs Clamp", &Settings::Aimbot::weapon_aim_settings[iWeaponID].aim_RcsClampType, ClampType, IM_ARRAYSIZE(ClampType));
						ImGui::PopItemWidth();
					}
					}
					ImGui::EndChild();
					ImGui::EndGroup();
				}

				/*else if (tabSelected == 1) // Trigger
				{
					const char* TriggerEnable[] = { "Disable" , "Fov" , "Trace" };
					ImGui::PushItemWidth(80.f);
					ImGui::Combo("Enable", &Settings::Triggerbot::trigger_Enable, TriggerEnable, IM_ARRAYSIZE(TriggerEnable));
					ImGui::PopItemWidth();
					ImGui::SameLine();

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox("Deathmatch", &Settings::Triggerbot::trigger_Deathmatch);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("WallAttack", &Settings::Triggerbot::trigger_WallAttack);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("FastZoom", &Settings::Triggerbot::trigger_FastZoom);

					ImGui::Checkbox("SmokCheck", &Settings::Triggerbot::trigger_SmokCheck);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("DrawFov", &Settings::Triggerbot::trigger_DrawFov);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("DrawSpot", &Settings::Triggerbot::trigger_DrawSpot);
					ImGui::SameLine(SpaceLineThr);
					ImGui::Checkbox("DrawFovAssist", &Settings::Triggerbot::trigger_DrawFovAssist);

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					const char* items1[] = { CVAR_KEY_MOUSE3 , CVAR_KEY_MOUSE4 , CVAR_KEY_MOUSE5 };
					ImGui::PushItemWidth(80.f);
					ImGui::Combo("Key", &Settings::Triggerbot::trigger_Key, items1, IM_ARRAYSIZE(items1));
					ImGui::PopItemWidth();
					ImGui::SameLine();

					const char* items2[] = { "Hold" , "Press" };
					ImGui::PushItemWidth(80.f);
					ImGui::Combo("Key Mode", &Settings::Triggerbot::trigger_KeyMode, items2, IM_ARRAYSIZE(items2));
					ImGui::PopItemWidth();
					ImGui::SameLine();

					ImGui::PushItemWidth(110.f);
					ImGui::Combo("Weapon", &iWeaponID, pWeaponData, IM_ARRAYSIZE(pWeaponData));
					ImGui::PopItemWidth();

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::PushItemWidth(362.f);
					ImGui::SliderInt("Min Disstance", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_DistanceMin, 0, 5000);
					ImGui::SliderInt("Max Disstance", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_DistanceMax, 0, 5000);
					ImGui::SliderInt("Fov", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_Fov, 1, 100);
					ImGui::SliderInt("Delay Before", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_DelayBefore, 0, 200);
					ImGui::SliderInt("Delay After", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_DelayAfter, 0, 1000);
					ImGui::PopItemWidth();

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox("HeadOnly", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_HeadOnly);
					ImGui::SameLine();

					const char* AssistMode[] = { "Disable" , "One Shot" , "Auto" };
					ImGui::PushItemWidth(80.f);
					ImGui::Combo("Assist", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_Assist, AssistMode, IM_ARRAYSIZE(AssistMode));
					ImGui::PopItemWidth();
					ImGui::SameLine();

					const char* AssistFovType[] = { "Dynamic" , "Static" };
					ImGui::PushItemWidth(80.f);
					ImGui::Combo("Assist Fov Type", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_AssistFovType, AssistFovType, IM_ARRAYSIZE(AssistFovType));
					ImGui::PopItemWidth();

					const char* HitGroup[] = { "All" , "Head + Body" , "Head" };
					ImGui::Combo("HitGroup", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_HitGroup, HitGroup, IM_ARRAYSIZE(HitGroup));
					//ImGui::PopItemWidth();

					ImGui::PushItemWidth(362.f);
					ImGui::SliderInt("Assist Rcs", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_AssistRcs, 0, 100);
					ImGui::SliderInt("Assist Fov", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_AssistFov, 1, 300);
					ImGui::SliderInt("Assist Smooth", &Settings::Triggerbot::weapon_trigger_settings[iWeaponID].trigger_AssistSmooth, 1, 300);
					ImGui::PopItemWidth();

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
				}*/

				else if (tabSelected == 1) // Visuals
				{
					ImGui::BeginGroup();
					ImGui::BeginChild(1, ImVec2(0, 0), true); // the 2 "0, 0" are the size of the childframe, change those to fit your menu.
					{
					ImGui::Text("Generell");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::Checkbox("Show Spectators", &Settings::Misc::misc_Spectators);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Watermark", &Settings::Esp::esp_Watermark);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Time", &Settings::Esp::esp_Time);
					ImGui::SameLine(SpaceLineThr);
					ImGui::Checkbox("Cheat Build", &Settings::Esp::esp_Cheatbuild);


					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Text("Esp");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					string style_1 = "Box";
					string style_2 = "Corner Box";

					const char* items1[] = { style_1.c_str() , style_2.c_str() };

					ImGui::PushItemWidth(339.f);
					ImGui::Combo("Type", &Settings::Esp::esp_Style, items1, IM_ARRAYSIZE(items1));
					ImGui::PopItemWidth();

					ImGui::Checkbox("Team", &Settings::Esp::esp_Team);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Enemy", &Settings::Esp::esp_Enemy);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Bomb", &Settings::Esp::esp_Bomb);
					ImGui::SameLine(SpaceLineThr);
					ImGui::Checkbox("Sound", &Settings::Esp::esp_Sound);

					ImGui::Checkbox("Line", &Settings::Esp::esp_Line);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("OutLine", &Settings::Esp::esp_Outline);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Name", &Settings::Esp::esp_Name);
					ImGui::SameLine(SpaceLineThr);
					ImGui::Checkbox("Rank", &Settings::Esp::esp_Rank);

					ImGui::Checkbox("Weapon", &Settings::Esp::esp_Weapon);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Ammo", &Settings::Esp::esp_Ammo);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Distance", &Settings::Esp::esp_Distance);
					ImGui::SameLine(SpaceLineThr);
					ImGui::Checkbox("Skeleton", &Settings::Esp::esp_Skeleton);

					ImGui::Checkbox("Defusing", &Settings::Esp::esp_Defusing);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("World Weapon", &Settings::Esp::esp_WorldWeapons);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("World Grenade", &Settings::Esp::esp_WorldGrenade);
					ImGui::SameLine(SpaceLineThr);
					ImGui::Checkbox("Buy Logs", &Settings::Esp::esp_logs);

					ImGui::Checkbox("Grenade Helper", &Settings::Esp::esp_ghelper); 
					ImGui::SameLine(100, 0);
					ImGui::SameLine();
					if (ImGui::Button("Update Map"))
						cGrenade.bUpdateGrenadeInfo(Interfaces::Engine()->GetLevelNameShort());

					string visible_1 = "Enemy";
					string visible_2 = "Team";
					string visible_3 = "All";
					string visible_4 = "Only Visible";

					const char* items2[] = { visible_1.c_str() , visible_2.c_str() , visible_3.c_str() , visible_4.c_str() };

					ImGui::PushItemWidth(339.f);
					ImGui::Combo("Visible", &Settings::Esp::esp_Visible, items2, IM_ARRAYSIZE(items2));


					ImGui::SliderInt("Size", &Settings::Esp::esp_Size, 0, 10);
					ImGui::SliderInt("BombTimer", &Settings::Esp::esp_BombTimer, 0, 65);
					ImGui::SliderInt("BulletTrace", &Settings::Esp::esp_BulletTrace, 0, 3000);


					string hpbar_1 = "None";
					string hpbar_2 = "Number";
					string hpbar_3 = "Bottom";
					string hpbar_4 = "Left";

					const char* items3[] = { hpbar_1.c_str() , hpbar_2.c_str() , hpbar_3.c_str() , hpbar_4.c_str() };
					ImGui::Combo("Health", &Settings::Esp::esp_Health, items3, IM_ARRAYSIZE(items3));

					string arbar_1 = "None";
					string arbar_2 = "Number";
					string arbar_3 = "Bottom";
					string arbar_4 = "Right";

					const char* items4[] = { arbar_1.c_str() , arbar_2.c_str() , arbar_3.c_str() , arbar_4.c_str() };
					ImGui::Combo("Armor", &Settings::Esp::esp_Armor, items4, IM_ARRAYSIZE(items4));

					

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Text("Hitmarker");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					const char* iHitSound[] =
					{
						"Off",
						"Default",
						"Anime",
						"Roblox",
						"German"
					};

					ImGui::Checkbox("Hit Marker", &Settings::Esp::esp_HitMarker);
					ImGui::Text("Hit Marker Sound");
					ImGui::SameLine();
					ImGui::Combo("##HITSOUND", &Settings::Esp::esp_HitMarkerSound, iHitSound, ARRAYSIZE(iHitSound));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Text("Chams");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();


					string chams_1 = "None";
					string chams_2 = "Flat";
					string chams_3 = "Texture";

					const char* items5[] = { chams_1.c_str() , chams_2.c_str() , chams_3.c_str() };
					ImGui::Combo("Chams", &Settings::Esp::esp_Chams, items5, IM_ARRAYSIZE(items5));
					ImGui::Combo("Chams Visible", &Settings::Esp::esp_ChamsVisible, items2, IM_ARRAYSIZE(items2));


					const char* material_items[] =
					{
						"Glass",
						"Crystal",
						"Gold",
						"Dark Chrome",
						"Plastic Glass",
						"Velvet",
						"Crystal Blue",
						"Detailed Gold"
					};

					ImGui::Checkbox("Chams Materials", &Settings::Misc::misc_ChamsMaterials);
					ImGui::PushItemWidth(362.f);
					ImGui::Combo("##CHAMSMATERIALS", &Settings::Misc::misc_ChamsMaterialsList, material_items, ARRAYSIZE(material_items));


					const char* armtype_items[] =
					{
						"Arms Only",
						"Arms + Weapon"
					};

					ImGui::Checkbox("Arm Materials", &Settings::Misc::misc_ArmMaterials);
					ImGui::PushItemWidth(362.f);
					ImGui::Text("Type");
					ImGui::SameLine();
					ImGui::Combo("##ARMMATERIALTYPE", &Settings::Misc::misc_ArmMaterialsType, armtype_items, ARRAYSIZE(armtype_items));
					ImGui::PushItemWidth(362.f);
					ImGui::Text("Material");
					ImGui::SameLine();
					ImGui::Combo("##ARMMATERIALS", &Settings::Misc::misc_ArmMaterialsList, material_items, ARRAYSIZE(material_items));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Text("Radar");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::Checkbox("Active", &Settings::Radar::rad_Active);
					ImGui::SameLine();
					ImGui::Checkbox("InGame", &Settings::Radar::rad_InGame);

						}
				ImGui::EndChild();
				ImGui::EndGroup();
				}

				else if (tabSelected == 2) // Skinchanger
				{
					ImGui::BeginGroup();
					ImGui::BeginChild(1, ImVec2(0, 0), true); 
					{
					const char* quality_items[] =
					{
						"Normal","Genuine","Vintage","Unusual","Community","Developer",
						"Self-Made","Customized","Strange","Completed","Tournament"
					};

					const char* gloves_listbox_items[49] =
					{
						"default",
						"Sport Gloves | Superconductor",
						"Sport Gloves | Pandora's Box",
						"Sport Gloves | Hedge Maze",
						"Sport Gloves | Arid",
						"Sport Gloves | Vice",
						"Sport Gloves | Omega",
						"Sport Gloves | Bronze Morph",
						"Sport Gloves | Amphibious",
						"Moto Gloves | Eclipse",
						"Moto Gloves | Spearmint",
						"Moto Gloves | Boom!",
						"Moto Gloves | Cool Mint",
						"Moto Gloves | Polygon",
						"Moto Gloves | Transport",
						"Moto Gloves | Turtle",
						"Moto Gloves | Pow",
						"Specialist Gloves | Crimson Kimono",
						"Specialist Gloves | Emerald Web",
						"Specialist Gloves | Foundation",
						"Specialist Gloves | Forest DDPAT",
						"Specialist Gloves | Mogul",
						"Specialist Gloves | Fade",
						"Specialist Gloves | Buckshot",
						"Specialist Gloves | Crimson Web",
						"Driver Gloves | Lunar Weave",
						"Driver Gloves | Convoy",
						"Driver Gloves | Crimson Weave",
						"Driver Gloves | Diamondback",
						"Driver Gloves | Racing Green",
						"Driver Gloves | Overtake",
						"Driver Gloves | Imperial Plad",
						"Driver Gloves | King Snake",
						"Hand Wraps | Leather",
						"Hand Wraps | Spruce DDPAT",
						"Hand Wraps | Badlands",
						"Hand Wraps | Slaughter",
						"Hand Wraps | Aboreal",
						"Hand Wraps | Duct Tape",
						"Hand Wraps | Overprint",
						"Hand Wraps | Cobalt Skulls",
						"Bloodhound Gloves | Charred",
						"Bloodhound Gloves | Snakebite",
						"Bloodhound Gloves | Bronzed",
						"Bloodhound Gloves | Guerrilla",
						"Hydra Gloves | Case Hardened",
						"Hydra Gloves | Rattler",
						"Hydra Gloves | Mangrove",
						"Hydra Gloves | Emerald",
					};

					const char* knife_models_items[] =
					{
						"Default","Bayonet","Flip","Gut","Karambit" ,"M9 Bayonet",
						"Huntsman","Falchion","Bowie","Butterfly","Shadow Daggers"
					};

					const char* quality_items2[] =
					{
						"Normal","Genuine","Vintage","Unusual","Community","Developer",
						"Self-Made","Customized","Strange","Completed","Tournament"
					};

					static int iSelectKnifeCTSkinIndex = -1;
					static int iSelectKnifeTTSkinIndex = -1;

					int iKnifeCTModelIndex = Settings::Skin::knf_ct_model;
					int iKnifeTTModelIndex = Settings::Skin::knf_tt_model;

					static int iOldKnifeCTModelIndex = -1;
					static int iOldKnifeTTModelIndex = -1;


					//[enc_string_enable /]

					ImGui::Text("Skin Changer");

					ImGui::Separator();

					ImGui::Text("Current Weapon: %s", pWeaponData[iWeaponID]);

					ImGui::PushItemWidth(362.f);

					static int iOldWeaponID = -1;

					ImGui::Combo("Weapon##WeaponSelect", &iWeaponID, pWeaponData, IM_ARRAYSIZE(pWeaponData));

					iWeaponSelectIndex = pWeaponItemIndexData[iWeaponID];

					if (iOldWeaponID != iWeaponID)
						iWeaponSelectSkinIndex = GetWeaponSkinIndexFromPaintKit(g_SkinChangerCfg[iWeaponSelectIndex].nFallbackPaintKit);

					iOldWeaponID = iWeaponID;

					string WeaponSkin = pWeaponData[iWeaponID];
					WeaponSkin += " Skin";

					ImGui::ComboBoxArray(WeaponSkin.c_str(), &iWeaponSelectSkinIndex, WeaponSkins[iWeaponID].SkinNames);

					ImGui::Combo("Weapon Quality", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].iEntityQuality, quality_items, IM_ARRAYSIZE(quality_items));
					ImGui::SliderFloat("Weapon Wear", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].flFallbackWear, 0.001f, 1.f);
					ImGui::InputInt("Weapon StatTrak", &g_SkinChangerCfg[pWeaponItemIndexData[iWeaponID]].nFallbackStatTrak, 1, 100, ImGuiInputTextFlags_CharsDecimal);

					ImGui::Combo("Gloves Skin", &Settings::Skin::gloves_skin, gloves_listbox_items,
						IM_ARRAYSIZE(gloves_listbox_items));

					ImGui::Separator();
					ImGui::Text("Knife Changer");
					ImGui::Separator();

					ImGui::Combo("Knife CT Model", &Settings::Skin::knf_ct_model, knife_models_items, IM_ARRAYSIZE(knife_models_items));
					ImGui::Combo("Knife TT Model", &Settings::Skin::knf_tt_model, knife_models_items, IM_ARRAYSIZE(knife_models_items));	

					if (iOldKnifeCTModelIndex != iKnifeCTModelIndex && Settings::Skin::knf_ct_model)
						iSelectKnifeCTSkinIndex = GetKnifeSkinIndexFromPaintKit(Settings::Skin::knf_ct_skin, false);

					if (iOldKnifeTTModelIndex != iKnifeTTModelIndex && Settings::Skin::knf_tt_model)
						iSelectKnifeTTSkinIndex = GetKnifeSkinIndexFromPaintKit(Settings::Skin::knf_ct_skin, true);

					iOldKnifeCTModelIndex = iKnifeCTModelIndex;
					iOldKnifeTTModelIndex = iKnifeTTModelIndex;

					string KnifeCTModel = knife_models_items[Settings::Skin::knf_ct_model];
					string KnifeTTModel = knife_models_items[Settings::Skin::knf_tt_model];

					KnifeCTModel += " Skin##KCT";
					KnifeTTModel += " Skin##KTT";

					ImGui::SliderFloat("Knife CT Wear", &g_SkinChangerCfg[WEAPON_KNIFE].flFallbackWear, 0.001f, 1.f);
					ImGui::Combo("Knife CT Quality", &g_SkinChangerCfg[WEAPON_KNIFE].iEntityQuality, quality_items2, IM_ARRAYSIZE(quality_items2));
					ImGui::ComboBoxArray(KnifeCTModel.c_str(), &iSelectKnifeCTSkinIndex, KnifeSkins[iKnifeCTModelIndex].SkinNames);

					ImGui::SliderFloat("Knife TT Wear", &g_SkinChangerCfg[WEAPON_KNIFE_T].flFallbackWear, 0.001f, 1.f);
					ImGui::Combo("Knife TT Qality", &g_SkinChangerCfg[WEAPON_KNIFE_T].iEntityQuality, quality_items2, IM_ARRAYSIZE(quality_items2));
					ImGui::ComboBoxArray(KnifeTTModel.c_str(), &iSelectKnifeTTSkinIndex, KnifeSkins[iKnifeTTModelIndex].SkinNames);

					ImGui::PopItemWidth();

					if (ImGui::Button("Apply##Skin"))
					{
						if (iWeaponSelectSkinIndex >= 0) {
							g_SkinChangerCfg[iWeaponSelectIndex].nFallbackPaintKit = WeaponSkins[iWeaponID].SkinPaintKit[iWeaponSelectSkinIndex];
						}
						ForceFullUpdate();

						if (iSelectKnifeCTSkinIndex > 0) {
							Settings::Skin::knf_ct_skin = KnifeSkins[iKnifeCTModelIndex].SkinPaintKit[iSelectKnifeCTSkinIndex];
						}
						if (iSelectKnifeTTSkinIndex > 0) {
							Settings::Skin::knf_tt_skin = KnifeSkins[iKnifeTTModelIndex].SkinPaintKit[iSelectKnifeTTSkinIndex];
						}
						ForceFullUpdate();
					}

					}
					ImGui::EndChild();
					ImGui::EndGroup();
				}

				else if (tabSelected == 3) // Misc
				{
					ImGui::BeginGroup();
					ImGui::BeginChild(1, ImVec2(0, 0), true);
					{
					ImGui::Text("Misc");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					const char* skybox_items[] =
					{
						"cs_baggage_skybox_",
						"cs_tibet",
						"embassy",
						"italy",
						"jungle",
						"nukeblank",
						"office",
						"sky_cs15_daylight01_hdr",
						"sky_cs15_daylight02_hdr",
						"sky_cs15_daylight03_hdr",
						"sky_cs15_daylight04_hdr",
						"sky_csgo_cloudy01",
						"sky_csgo_night02",
						"sky_csgo_night02b",
						"sky_csgo_night_flat",
						"sky_day02_05_hdr",
						"sky_day02_05",
						"sky_dust",
						"sky_l4d_rural02_ldr",
						"sky_venice",
						"vertigo_hdr",
						"vertigoblue_hdr",
						"vertigo",
						"vietnam"
					};

					
					ImGui::Checkbox("Third Person", &Settings::Misc::misc_ThirdPerson);
					ImGui::SameLine(SpaceLineOne);
					ImGui::SliderFloat("##THIRDPERSONRANGE", &Settings::Misc::misc_ThirdPersonRange, 0.f, 500.f, "Third Person Range: %0.f");

					ImGui::Checkbox("AutoAccept", &Settings::Misc::misc_AutoAccept);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Bhop", &Settings::Misc::misc_Bhop);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Punch", &Settings::Misc::misc_Punch);
					ImGui::SameLine(SpaceLineThr);
					ImGui::Checkbox("Trashtalk", &Settings::Misc::misc_spamrandom);

					ImGui::Checkbox("NoFlash", &Settings::Misc::misc_NoFlash);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("AutoStrafe", &Settings::Misc::misc_AutoStrafe);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("NoHands", &Settings::Misc::misc_NoHands);
					ImGui::SameLine(SpaceLineThr);
					
					ImGui::Checkbox("Chat Spam", &Settings::Misc::misc_spamregular);


					ImGui::Checkbox("WireHands", &Settings::Misc::misc_WireHands);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("Fov Changer", &Settings::Misc::misc_FovChanger);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Sniper Crosshair", &Settings::Misc::misc_AwpAim);
					ImGui::PushItemWidth(362.f);
					ImGui::SliderInt("Fov View", &Settings::Misc::misc_FovView, 1, 170);
					ImGui::SliderInt("Fov Model View", &Settings::Misc::misc_FovModelView, 1, 190);

					ImGui::PopItemWidth();

					string clan_1 = "None";
					string clan_2 = "Clear";
					string clan_3 = "LegitWare";
					string clan_4 = "LegitWare No-name";
					string clan_5 = "Valve";
					string clan_6 = "Valve No-name";
					string clan_7 = "LegitWare Animation";
					const char* items5[] = { clan_1.c_str() , clan_2.c_str() , clan_3.c_str() , clan_4.c_str() , clan_5.c_str() , clan_6.c_str() , clan_7.c_str() };
					ImGui::Combo("Clan Changer", &Settings::Misc::misc_Clan, items5, IM_ARRAYSIZE(items5));
					ImGui::Spacing();
					

					if (ImGui::Combo("", &Settings::Misc::misc_CurrentSky, skybox_items, IM_ARRAYSIZE(skybox_items)))
					{
						Settings::Misc::misc_SkyName = skybox_items[Settings::Misc::misc_CurrentSky];
					}
					ImGui::SameLine();
					ImGui::Text("Skybox (Spoof)");
					ImGui::Checkbox("Snow (Spoof)", &Settings::Misc::misc_Snow);
					ImGui::SameLine(SpaceLineOne);
					ImGui::Checkbox("NoSky (Spoof)", &Settings::Misc::misc_NoSky);
					ImGui::SameLine(SpaceLineTwo);
					ImGui::Checkbox("Disable Postprocess (Spoof)", &Settings::Misc::misc_EPostprocess);

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Text("Ranks/Levels/Commends");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					const char* ranklist[] =
					{
						"OFF",
						"Silver I",
						"Silver II",
						"Silver III",
						"Silver IV",
						"Silver Elite",
						"Silver Elite Master",

						"Gold Nova I",
						"Gold Nova II",
						"Gold Nova III",
						"Gold Nova Master",
						"Master Guardian I",
						"Master Guardian II",

						"Master Guardian Elite",
						"Distinguished Master Guardian",
						"Legendary Eagle",
						"Legendary Eagle Master",
						"Supreme Master First Class",
						"Global Elite"

					};

					ImGui::Checkbox("Enable", &Settings::Misc::misc_ranktoggle);
					ImGui::SliderInt("Level (1-40)", &Settings::Misc::misc_Level, 1, 40);
					ImGui::Combo(("Rank"), &Settings::Misc::misc_Rank_Combo, ranklist, ARRAYSIZE(ranklist));
					if (Settings::Misc::misc_Rank_Combo >= 1)
					{
						Settings::Misc::misc_Rank = Settings::Misc::misc_Rank_Combo;
					}
					else
					{
						Settings::Misc::misc_Rank = 0;
					}
					ImGui::InputInt("XP", &Settings::Misc::misc_XP);
					ImGui::InputInt("Wins", &Settings::Misc::misc_Wins);
					ImGui::InputInt("Friendly", &Settings::Misc::misc_Friendly);
					ImGui::InputInt("Teacher", &Settings::Misc::misc_Teacher);
					ImGui::InputInt("Leader", &Settings::Misc::misc_Leader);

					ImGui::Spacing();
					ImGui::Spacing();

					if (ImGui::Button("Apply"))
					{
						SendMMHello();
					}

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Text("Medals");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					static int medal_id = 0;
					if (ImGui::Button("Add") && medal_id != 0) {
						Settings::InventoryChanger::medals.insert(Settings::InventoryChanger::medals.end(), medal_id);
						medal_id = 0;
					}
					ImGui::InputInt("Medal id", &medal_id);

					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
					ImGui::ListBoxHeader("Medal list");
					for (size_t m = 0; m < Settings::InventoryChanger::medals.size(); m++) {
						if (ImGui::Selectable(std::to_string(Settings::InventoryChanger::medals[m]).c_str())) {
							if (Settings::InventoryChanger::equipped_medal == Settings::InventoryChanger::medals[m]) {
								Settings::InventoryChanger::equipped_medal = 0;
								Settings::InventoryChanger::equipped_medal_override = false;
							}
							Settings::InventoryChanger::medals.erase(Settings::InventoryChanger::medals.begin() + m);
						}
					}
					ImGui::ListBoxFooter();
					ImGui::PopStyleColor();

					ImGui::Checkbox("Equipped Medal", &Settings::InventoryChanger::equipped_medal_override);
					if (Settings::InventoryChanger::equipped_medal_override) {
						static int equipped_medal = 0;
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
						if (ImGui::Combo("Equipped Medal", &equipped_medal, [](void* data, int idx, const char** out_text)
						{
							*out_text = std::to_string(Settings::InventoryChanger::medals[idx]).c_str();
							return true;
						}, nullptr, Settings::InventoryChanger::medals.size(), 5)) {
							Settings::InventoryChanger::equipped_medal = Settings::InventoryChanger::medals[equipped_medal];
						}
						ImGui::PopStyleColor();
					}
					ImGui::Spacing();
					if (ImGui::Button("Apply##Medals")) {
						SendClientHello();

					}
					}
					ImGui::EndChild();
					ImGui::EndGroup();
				}

				else if (tabSelected == 4) // Color
				{
					ImGui::BeginGroup();
					ImGui::BeginChild(1, ImVec2(0, 0), true);
					{
					ImGui::Text("Color's");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::Text("Hit Marker");
					ImGui::Separator();
					ImGui::ColorEdit3("Hit Marker Color", Settings::Esp::esp_HitMarkerColor);
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Text("Sniper Crosshair");
					ImGui::Separator();
					ImGui::ColorEdit3("Sniper Crosshair Color", Settings::Misc::misc_AwpAimColor);
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Text("ESP");
					ImGui::Separator();
					ImGui::ColorEdit3("Esp Color Hit Marker", Settings::Esp::esp_HitMarkerColor);
					ImGui::ColorEdit3("Esp Color CT", Settings::Esp::esp_Color_CT);
					ImGui::ColorEdit3("Esp Color TT", Settings::Esp::esp_Color_TT);
					ImGui::ColorEdit3("Esp Color Visible CT", Settings::Esp::esp_Color_VCT);
					ImGui::ColorEdit3("Esp Color Visible TT", Settings::Esp::esp_Color_VTT);
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Text("Chams");
					ImGui::Separator();

					ImGui::ColorEdit3("Chams Color CT", Settings::Esp::chams_Color_CT);
					ImGui::ColorEdit3("Chams Color TT", Settings::Esp::chams_Color_TT);
					ImGui::ColorEdit3("Chams Color Visible CT", Settings::Esp::chams_Color_VCT);
					ImGui::ColorEdit3("Chams Color Visible TT", Settings::Esp::chams_Color_VTT);
					}
					ImGui::EndChild();
					ImGui::EndGroup();
				}

				else if (tabSelected == 5) // Config
				{
					ImGui::BeginGroup();
					ImGui::BeginChild(1, ImVec2(0, 0), true);
					{
					ImGui::Text("Config");

					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Spacing();

					static int iConfigSelect = 0;
					static int iMenuSheme = 1;
					static char ConfigName[64] = { 0 };

					ImGui::ComboBoxArray("Select Config", &iConfigSelect, ConfigList);

					ImGui::Separator();

					if (ImGui::Button("Load Config"))
					{
						Settings::LoadSettings("C:/LegitWare/" + ConfigList[iConfigSelect]);
						SendMMHello();
					}
					ImGui::SameLine();
					if (ImGui::Button("Save Config"))
					{
						Settings::SaveSettings("C:/LegitWare/" + ConfigList[iConfigSelect]);
					}
					ImGui::SameLine();
					if (ImGui::Button("Refresh Config List"))
					{
						RefreshConfigs();
					}

					ImGui::Separator();

					ImGui::InputText("Config Name", ConfigName, 64);

					if (ImGui::Button("Create & Save new Config"))
					{
						string ConfigFileName = ConfigName;

						if (ConfigFileName.size() < 1)
						{
							ConfigFileName = "settings";
						}

						Settings::SaveSettings("C:/LegitWare/" + ConfigFileName + ".ini");
						RefreshConfigs();
					}
					}
					ImGui::EndChild();
					ImGui::EndGroup();
				}
			}
			ImGui::End();
		}
}




	//GUI.cpp


	using namespace Client;

	//[enc_string_enable /]
	//[junk_enable /]

	bool bIsGuiInitalize = false;
	bool bIsGuiVisible = false;
	WNDPROC WndProc_o = nullptr;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

	LRESULT WINAPI GUI_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CGui::CGui() {}

	CGui::~CGui()
	{
		ImGui_ImplDX9_Shutdown();
	}

	void CGui::GUI_Init(IDirect3DDevice9 * pDevice)
	{
		HWND hWindow = FindWindowA("Valve001", 0);

		ImGui_ImplDX9_Init(hWindow, pDevice);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();


		io.IniFilename = GuiFile.c_str();
		font = io.Fonts->AddFontFromMemoryCompressedTTF(menutext_compressed_data, menutext_compressed_size, 12.f);


		imgfont = io.Fonts->AddFontFromMemoryCompressedTTF(Pastehook_Regular_compressed_data, Pastehook_Regular_compressed_size, 32.f);

		style.Alpha = 1.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMinSize = ImVec2(200, 200);
		style.FramePadding = ImVec2(4, 2);
		style.ItemSpacing = ImVec2(6, 3);
		style.ItemInnerSpacing = ImVec2(8, 8);
		style.Alpha = 1.f;
		style.WindowRounding = 0.f;
		style.FrameRounding = 0.f;
		style.IndentSpacing = 6.0f;
		style.ColumnsMinSpacing = 50.0f;
		style.GrabMinSize = 14.0f;
		style.GrabRounding = 16.0f;
		style.ScrollbarSize = 12.0f;
		style.ScrollbarRounding = 16.0f;
		style.ChildWindowRounding = 0.f;
		style.ScrollbarRounding = 0.f;
		style.GrabRounding = 0.f;
		style.AntiAliasedLines = true;
		style.AntiAliasedShapes = true;

		BlueSheme();

		ImGui_ImplDX9_CreateDeviceObjects();

		WndProc_o = (WNDPROC)SetWindowLongA(hWindow, GWL_WNDPROC, (LONG)(LONG_PTR)GUI_WndProc);

		bIsGuiInitalize = true;
	}

	void CGui::GUI_Begin_Render()
	{
		ImGui_ImplDX9_NewFrame();
	}

	void CGui::GUI_End_Render()
	{
		ImGui::Render();
	}

	LRESULT WINAPI GUI_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static bool is_down = false;
		static bool is_clicked = false;
		static bool check_closed = false;

		if (GUI_KEY_DOWN(VK_INSERT))
		{
			is_clicked = false;
			is_down = true;
		}
		else if (!GUI_KEY_DOWN(VK_INSERT) && is_down)
		{
			is_clicked = true;
			is_down = false;
		}
		else
		{
			is_clicked = false;
			is_down = false;
		}

		if (!bIsGuiVisible && !is_clicked && check_closed)
		{
			string msg = "cl_mouseenable " + to_string(!bIsGuiVisible);
			Interfaces::Engine()->ClientCmd_Unrestricted2(msg.c_str());
			check_closed = false;
		}

		if (is_clicked)
		{
			bIsGuiVisible = !bIsGuiVisible;

			string msg = "cl_mouseenable " + to_string(!bIsGuiVisible);
			Interfaces::Engine()->ClientCmd_Unrestricted2(msg.c_str());

			if (!check_closed)
				check_closed = true;
		}

		if (bIsGuiVisible && ImGui_ImplDX9_WndProcHandler(hwnd, uMsg, wParam, lParam))
			return true;

		return CallWindowProcA(WndProc_o, hwnd, uMsg, wParam, lParam);
	}

	void CGui::GUI_Draw_Elements()
	{
		if (!bIsGuiInitalize || Interfaces::Engine()->IsTakingScreenshot() || !Interfaces::Engine()->IsActiveApp())
			return;

		g_pGui->GUI_Begin_Render();

		ImGui::GetIO().MouseDrawCursor = bIsGuiVisible;

		bool bOpenTimer = (bIsGuiVisible || (bC4Timer && iC4Timer));

		if (Settings::Misc::misc_Spectators) g_pMisc->OnRenderSpectatorList();

		if (g_pEsp && Settings::Esp::esp_BombTimer && bOpenTimer)
		{
			ImVec2 OldMinSize = ImGui::GetStyle().WindowMinSize;

			ImGui::GetStyle().WindowMinSize = ImVec2(0.f, 0.f);

			ImGui::SetNextWindowSize(ImVec2(125.f, 30.f));

			if (ImGui::Begin("Bomb Timer", &bOpenTimer,
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
			{
				// BELOW 5 SEC
				if (g_pEsp->fC4Timer < 5) { 
					ImGui::TextColored(ImColor(255, 0, 0), C4_TIMER_STRING, g_pEsp->fC4Timer);
				}
				// BELOW 10 SEC
				else if (g_pEsp->fC4Timer < 10) { 
					ImGui::TextColored(ImColor(255, 102, 0), C4_TIMER_STRING, g_pEsp->fC4Timer);
				}
				// ABOVE 10 SEC
				else if (g_pEsp->fC4Timer > 10)
				{
					ImGui::TextColored(ImColor(0, 255, 0), C4_TIMER_STRING, g_pEsp->fC4Timer);
				}
				ImGui::End();
			}

			ImGui::GetStyle().WindowMinSize = OldMinSize;
		}

		if (bIsGuiVisible)
		{
			int pX, pY;
			Interfaces::InputSystem()->GetCursorPosition(&pX, &pY);

			ImGui::GetIO().MousePos.x = (float)pX;
			ImGui::GetIO().MousePos.y = (float)pY;

			Client::OnRenderGUI();
		}

		g_pGui->GUI_End_Render();
	}

	void CGui::BlueSheme()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.49f, 0.82f, 0.39f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.49f, 0.82f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.49f, 0.82f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.49f, 0.82f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.49f, 0.82f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.49f, 0.82f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.49f, 0.82f, 0.98f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.20f, 0.49f, 0.82f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}


	namespace ImGui
	{
		static auto vector_getter = [](void* vec, int idx, const char** out_text)
		{
			auto& vector = *static_cast<std::vector<std::string>*>(vec);
			if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
			*out_text = vector.at(idx).c_str();
			return true;
		};

		IMGUI_API bool ComboBoxArray(const char* label, int* currIndex, std::vector<std::string>& values)
		{
			if (values.empty()) { return false; }
			return Combo(label, currIndex, vector_getter,
				static_cast<void*>(&values), values.size());
		}

		IMGUI_API bool TabLabels(const char **tabLabels, int tabSize, int &tabIndex, int *tabOrder)
		{
			ImGuiStyle& style = ImGui::GetStyle();

			const ImVec2 itemSpacing = style.ItemSpacing;
			const ImVec4 color = style.Colors[ImGuiCol_Button];
			const ImVec4 colorActive = style.Colors[ImGuiCol_ButtonActive];
			const ImVec4 colorHover = style.Colors[ImGuiCol_ButtonHovered];
			const ImVec4 colorText = style.Colors[ImGuiCol_Text];
			style.ItemSpacing.x = 2.5;
			style.ItemSpacing.y = 1;
			const ImVec4 colorSelectedTab = ImVec4(color.x, color.y, color.z, color.w*0.5f);
			const ImVec4 colorSelectedTabHovered = ImVec4(colorHover.x, colorHover.y, colorHover.z, colorHover.w*0.5f);
			const ImVec4 colorSelectedTabText = ImVec4(colorText.x*0.8f, colorText.y*0.8f, colorText.z*0.8f, colorText.w*0.8f);

			if (tabSize>0 && (tabIndex<0 || tabIndex >= tabSize))
			{
				if (!tabOrder)
				{
					tabIndex = 0;
				}
				else
				{
					tabIndex = -1;
				}
			}

			float windowWidth = 0.f, sumX = 0.f;
			windowWidth = ImGui::GetWindowWidth() - style.WindowPadding.x - (ImGui::GetScrollMaxY()>0 ? style.ScrollbarSize : 0.f);

			const bool isMMBreleased = ImGui::IsMouseReleased(2);
			int justClosedTabIndex = -1, newtabIndex = tabIndex;

			bool selection_changed = false; bool noButtonDrawn = true;

			for (int j = 0, i; j < tabSize; j++)
			{
				i = tabOrder ? tabOrder[j] : j;
				if (i == -1) continue;

				if (sumX > 0.f)
				{
					sumX += style.ItemSpacing.x;
					sumX += ImGui::CalcTextSize(tabLabels[i]).x + 2.f*style.FramePadding.x;

					if (sumX>windowWidth)
					{
						sumX = 0.f;
					}
					else
					{
						ImGui::SameLine();
					}
				}

				if (i != tabIndex)
				{
					// Push the style
					style.Colors[ImGuiCol_Button] = colorSelectedTab;
					style.Colors[ImGuiCol_ButtonActive] = colorSelectedTab;
					style.Colors[ImGuiCol_ButtonHovered] = colorSelectedTabHovered;
					style.Colors[ImGuiCol_Text] = colorSelectedTabText;
				}
				// Draw the button
				ImGui::PushID(i);   // otherwise two tabs with the same name would clash.
				if (ImGui::Button(tabLabels[i], ImVec2(100.f, 50.f))) { selection_changed = (tabIndex != i); newtabIndex = i; }
				ImGui::PopID();
				if (i != tabIndex)
				{
					// Reset the style
					style.Colors[ImGuiCol_Button] = color;
					style.Colors[ImGuiCol_ButtonActive] = colorActive;
					style.Colors[ImGuiCol_ButtonHovered] = colorHover;
					style.Colors[ImGuiCol_Text] = colorText;
				}
				noButtonDrawn = false;

				if (sumX == 0.f) sumX = style.WindowPadding.x + ImGui::GetItemRectSize().x; // First element of a line

			}

			tabIndex = newtabIndex;

			// Change selected tab when user closes the selected tab
			if (tabIndex == justClosedTabIndex && tabIndex >= 0)
			{
				tabIndex = -1;
				for (int j = 0, i; j < tabSize; j++)
				{
					i = tabOrder ? tabOrder[j] : j;
					if (i == -1)
					{
						continue;
					}
					tabIndex = i;
					break;
				}
			}

			// Restore the style
			style.Colors[ImGuiCol_Button] = color;
			style.Colors[ImGuiCol_ButtonActive] = colorActive;
			style.Colors[ImGuiCol_ButtonHovered] = colorHover;
			style.Colors[ImGuiCol_Text] = colorText;
			style.ItemSpacing = itemSpacing;

			return selection_changed;
		}
	}