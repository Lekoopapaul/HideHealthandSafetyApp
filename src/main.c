#include "utils/logger.h"
#include <coreinit/filesystem.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <wups.h>
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemMultipleValues.h>
#include <wups/config/WUPSConfigItemStub.h>
#include <coreinit/mcp.h>
#include <coreinit/title.h>
#include <sysapp/launch.h>

WUPS_PLUGIN_NAME("HideHealthandSafetyApp");
WUPS_PLUGIN_DESCRIPTION("This will hide the Health and Safety App");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("Lekoopapaul");
WUPS_PLUGIN_LICENSE("MIT");

//Health and Safety App IDs
#define EUR_ID 0x000500101004E200
#define USA_ID 0x000500101004E100
#define JPN_ID 0x000500101004E000

#define ENABLE_PLUGIN_CONFIG_ID "Enable"
#define ENABLE_PLUGIN_CONFIG_NAME_STRING "Enable"

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE("hide_health_and_safety_app");

bool Enable = true;
bool prevEnableValue = true;

void EnableOpenChanged(ConfigItemBoolean *item, bool newValue) {
    DEBUG_FUNCTION_LINE_VERBOSE("New value in EnableOpenChanged: %d", newValue);
    Enable = newValue;
    
    WUPSStorageAPI_StoreBool(NULL, ENABLE_PLUGIN_CONFIG_ID, Enable);
}

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle root) {
    {
        // Let's create a new category called "Settings"
        WUPSConfigCategoryHandle settingsCategory;
        WUPSConfigAPICreateCategoryOptionsV1 settingsCategoryOptions = {.name = "Settings"};
        if (WUPSConfigAPI_Category_Create(settingsCategoryOptions, &settingsCategory) != WUPSCONFIG_API_RESULT_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to create settings category");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }

        // Add a new item to this settings category
        if (WUPSConfigItemBoolean_AddToCategory(settingsCategory, ENABLE_PLUGIN_CONFIG_ID, ENABLE_PLUGIN_CONFIG_NAME_STRING, true, Enable, &EnableOpenChanged) != WUPSCONFIG_API_RESULT_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to add item to category");
            return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
        }

        // Add the category to the root.
        if (WUPSConfigAPI_Category_AddCategory(root, settingsCategory) != WUPSCONFIG_API_RESULT_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to add category to root item");
        }
    }

    return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
}

void ConfigMenuClosedCallback() {
    WUPSStorageAPI_SaveStorage(false);

    if(Enable != prevEnableValue){
        
        _SYSLaunchTitleWithStdArgsInNoSplash(OSGetTitleID(), NULL);
    }
    prevEnableValue = Enable;
}

INITIALIZE_PLUGIN() {
    initLogging();
    DEBUG_FUNCTION_LINE("INITIALIZE_PLUGIN of Hide Health and Safety App!");

    WUPSConfigAPIOptionsV1 configOptions = {.name = "Hide Health and Safety App"};

    if (WUPSConfigAPI_Init(configOptions, ConfigMenuOpenedCallback, ConfigMenuClosedCallback) != WUPSCONFIG_API_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to init config api");
    }

    WUPSStorageError storageRes;
    // Try to get value from storage
    if ((storageRes = WUPSStorageAPI_GetBool(NULL, ENABLE_PLUGIN_CONFIG_ID, &Enable)) == WUPS_STORAGE_ERROR_NOT_FOUND) {

        // Add the value to the storage if it's missing.
        if (WUPSStorageAPI_StoreBool(NULL, ENABLE_PLUGIN_CONFIG_ID, Enable) != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to store bool");
        }
    } else if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to get bool %s (%d)", WUPSConfigAPI_GetStatusStr(storageRes), storageRes);
    } else {
        DEBUG_FUNCTION_LINE_INFO("Successfully read the value from storage: %d %s (%d)", Enable, WUPSConfigAPI_GetStatusStr(storageRes), storageRes);
    }
    WUPSStorageAPI_SaveStorage(false);

    prevEnableValue = Enable;
    deinitLogging();
}

DECL_FUNCTION(int32_t, MCP_TitleList, uint32_t handle, uint32_t *outTitleCount, MCPTitleListType *titleList, uint32_t size) {
    int32_t result = real_MCP_TitleList(handle, outTitleCount, titleList, size);
    if(Enable){
        MCPTitleListType newtitelist[(*outTitleCount)-1];

        for(int i = 0;i < *outTitleCount; i++){
            if(titleList[i].titleId != EUR_ID || titleList[i].titleId != USA_ID || titleList[i].titleId != JPN_ID ){
                memcpy(&(newtitelist[i]),&(titleList[i]),sizeof(MCPTitleListType));
            }
        }
        *outTitleCount=(*outTitleCount)-1;
        size=size-sizeof(MCPTitleListType);
    }
    return result;
}

WUPS_MUST_REPLACE_FOR_PROCESS(MCP_TitleList, WUPS_LOADER_LIBRARY_COREINIT, MCP_TitleList,WUPS_FP_TARGET_PROCESS_WII_U_MENU);