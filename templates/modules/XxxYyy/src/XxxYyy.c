// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "module.h"
#include "azure_c_shared_utility/xlogging.h"

#include "XxxYyy.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/lock.h"

typedef struct XXX_YYY_HANDLE_DATA_TAG
{
    // TODO: Add specific members 
    THREAD_HANDLE threadHandle;
    LOCK_HANDLE lockHandle;
    BROKER_HANDLE broker;
    int sendCycle;
}XXX_YYY_HANDLE_DATA;

// Data structure for JSON configuration
typedef struct XXX_YYY_CONFIG_TAG
{
    int sendCycle;
} XXX_YYY_CONFIG;

int xxxYyyThread(void *param)
{
    XXX_YYY_HANDLE_DATA* handleData = param;

    MESSAGE_CONFIG msgConfig;
    MAP_HANDLE propertiesMap = Map_Create(NULL);
    if(propertiesMap == NULL)
    {
        LogError("unable to create a Map");
    }
    else
    {
        if (Map_AddOrUpdate(propertiesMap, "xxx_yyy", "from Azure IoT Gateway SDK simple sample!") != MAP_OK)
        {
            LogError("unable to Map_AddOrUpdate");
        }
        else
        {
            msgConfig.size = (size_t)strlen(HELLOWORLD_MESSAGE);
            msgConfig.source = (unsigned char*)HELLOWORLD_MESSAGE;
    
            msgConfig.sourceProperties = propertiesMap;

            MESSAGE_HANDLE xxxYyyMessage = Message_Create(&msgConfig);
            if (xxxYyyMessage == NULL)
            {
                LogError("unable to create \"xxx yyy\" message");
            }
            else
            {
                while (1)
                {
                    if (Lock(handleData->lockHandle) == LOCK_OK)
                    {
                        if (handleData->stopThread)
                        {
                            (void)Unlock(handleData->lockHandle);
                            break; /*gets out of the thread*/
                        }
                        else
                        {
                            (void)Broker_Publish(handleData->broker, (MODULE_HANDLE)handleData, xxxYyyMessage);
                            (void)Unlock(handleData->lockHandle);
                        }
                    }
                    else
                    {
                        /*shall retry*/
                    }
                    (void)ThreadAPI_Sleep(handleData->sendCycle); /*every 5 seconds*/
                }
                Message_Destroy(xxxYyyMessage);
            }
        }
    }
    return 0;
}


static MODULE_HANDLE XxxYyy_Create(BROKER_HANDLE broker, const void* configuration)
{
    XXX_YYY_HANDLE_DATA* result;
    XXX_YYY_CONFIG* moduleConfig;
    
	if (broker == NULL || configuration == NULL)
    {
        LogError("invalid arg broker=%p or configuration=%p", broker, configuration);
        result = NULL;
    }
    else
    {
        moduleConfig = (XXX_YYY_CONFIG*)configuration;
        result = malloc(sizeof(XXX_YYY_HANDLE_DATA));
        if(result == NULL)
        {
            LogError("unable to malloc");
        }
        else
        {
            result->lockHandle = Lock_Init();
            if(result->lockHandle == NULL)
            {
                LogError("unable to Lock_Init");
                free(result);
                result = NULL;
            }
            else
            {
                result->broker = broker;
                result->sendCycle = moduleConfig->sendCycle;
                // TODO: Add specific logic 
            }
        }
    }
    return result;
}

static void* XxxYyy_ParseConfigurationFromJson(const char* configuration)
{
    XXX_YYY_CONFIG* result = NULL;
    if (configuration != NULL)
    {
        JSON_Value* json = json_parse_string((const char*)configuration);
        if (json == NULL)
        {
            LogError("Failed to parse json configuration!");
        }
        else
        {
            result = (XXX_YYY_CONFIG*)malloc(sizeof(XXX_YYY_CONFIG));
            result->sendCycle = (int)json_object_get_number(json, SEND_CYCLE);
        }
    }
    // TODO: Add specific parse logic 
    return result;
}

static void XxxYyy_FreeConfiguration(void* configuration)
{
    if (configuration != NULL)
    {
        free(configuration);
    }
    // TODO: Add sepcific free logic if necessary 
}

static void XxxYyy_Start(MODULE_HANDLE module)
{
    XXX_YYY_HANDLE_DATA* handleData = module;
    if (handleData != NULL)
    {
        if (Lock(handleData->lockHandle) != LOCK_OK)
        {
            LogError("not able to Lock, still setting the thread to finish");
            handleData->stopThread = 1;
        }
        else
        {
            if (ThreadAPI_Create(&handleData->threadHandle, xxxYyyThread, handleData) != THREADAPI_OK)
            {
                LogError("failed to spawn a thread");
                handleData->threadHandle = NULL;
            }
            (void)Unlock(handleData->lockHandle);
        }
        // if you don't need Lock capability you can delete them.
    }
}

static void XxxYyy_Destroy(MODULE_HANDLE module)
{
    XXX_YYY_HANDLE_DATA* handleData = module;
    // TODO: Add specific destory logic 
    int notUsed;
    if (Lock(handleData->lockHandle) != LOCK_OK)
    {
        LogError("not able to Lock, still setting the thread to finish");
        handleData->stopThread = 1;
    }
    else
    {
        handleData->stopThread = 1;
        Unlock(handleData->lockHandle);
    }

    if (handleData->threadHandle != NULL &&
        ThreadAPI_Join(handleData->threadHandle, &notUsed) != THREADAPI_OK)
    {
        LogError("unable to ThreadAPI_Join, still proceeding in _Destroy");
    }
    
    (void)Lock_Deinit(handleData->lockHandle);
    free(handleData);
}

static void XxxYyy_Receive(MODULE_HANDLE moduleHandle, MESSAGE_HANDLE messageHandle)
{
    (void)moduleHandle;
    (void)messageHandle;

    // TODO: Add specific action logic when this module receives message
}

static const MODULE_API_1 XxxYyy_API_all =
{
    {MODULE_API_VERSION_1},
    XxxYyy_ParseConfigurationFromJson,
	XxxYyy_FreeConfiguration,
    XxxYyy_Create,
    XxxYyy_Destroy,
    XxxYyy_Receive,
    XxxYyy_Start
};

#ifdef BUILD_MODULE_TYPE_STATIC
MODULE_EXPORT const MODULE_API* MODULE_STATIC_GETAPI(XXX_YYY_MODULE)(MODULE_API_VERSION gateway_api_version)
#else
MODULE_EXPORT const MODULE_API* Module_GetApi(MODULE_API_VERSION gateway_api_version)
#endif
{
    const MODULE_API * api;
    if (gateway_api_version >= XxxYyy_API_all.base.version)
    {
        api= (const MODULE_API*)&XxxYyy_API_all;
    }
    else
    {
        api = NULL;
    }
    return api;
}
