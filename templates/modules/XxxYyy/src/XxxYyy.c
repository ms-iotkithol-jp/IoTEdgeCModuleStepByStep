// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include "module.h"
#include "azure_c_shared_utility/xlogging.h"

#include "XxxYyy.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/constmap.h"
#include "azure_c_shared_utility/constbuffer.h"

#include <parson.h>

typedef struct XXX_YYY_HANDLE_DATA_TAG
{
    // TODO: Add specific members 
    THREAD_HANDLE threadHandle;
    LOCK_HANDLE lockHandle;
    int stopThread;
    BROKER_HANDLE broker;
    int sendCycle;
} XXX_YYY_HANDLE_DATA;

// Data structure for JSON configuration
typedef struct XXX_YYY_CONFIG_TAG
{
    int sendCycle;
} XXX_YYY_CONFIG;

#define XXX_YYY_MESSAGE "Message From XxxYyy module"
#define GW_SOURCE_PROPERTY "source"

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
            msgConfig.size = (size_t)strlen(XXX_YYY_MESSAGE);
            msgConfig.source = (unsigned char*)XXX_YYY_MESSAGE;
    
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
                result->stopThread = 0;
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
            JSON_Object* obj = json_value_get_object(json);
            result = (XXX_YYY_CONFIG*)malloc(sizeof(XXX_YYY_CONFIG));
            result->sendCycle = (int)json_object_get_number(obj, SEND_CYCLE);
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
    XXX_YYY_HANDLE_DATA* xxxYyyHandle = (XXX_YYY_HANDLE_DATA*)moduleHandle;
    if (xxxYyyHandle!=NULL)
    {
        LogInfo("Test SendCycle=%d", xxxYyyHandle->sendCycle);
    }
    CONSTMAP_HANDLE props = Message_GetProperties(messageHandle);
    if (props != NULL)
    {
        const char* source = ConstMap_GetValue(props, GW_SOURCE_PROPERTY);
        if (source != NULL)
        {
            LogInfo("Recieved mseesage from %s", source);
        }
        const char* xxxYyy = ConstMap_GetValue(props, "xxx-yyy");
        if (xxxYyy != NULL)
        {
            LogInfo("xxx-yyy:%s", xxxYyy);
        }
    }
    const CONSTBUFFER* buffer = Message_GetContent(messageHandle);
    if (buffer != NULL)
    {
        size_t size = buffer->size;
        const unsigned char* content = buffer->buffer;
        char* messageContent = (char*)malloc(size*2+1);
        for(size_t i=0;i<size;i++)
        {
            sprintf(&messageContent[i*2],"%02x", content[i]);
        }
        messageContent[size*2] = '\0';
        LogInfo("Received Content:size=%d,%s", size, messageContent);
        free(messageContent);
    }

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
