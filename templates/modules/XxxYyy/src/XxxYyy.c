// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "module.h"
#include "azure_c_shared_utility/xlogging.h"

#include "XxxYyy.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/lock.h"

typedef struct XXX_YYY_HANDLE_DATA_TAG
{
    // TODO: Add specific members 
    LOCK_HANDLE lockHandle;
    BROKER_HANDLE broker;

}XXX_YYY_HANDLE_DATA;

static MODULE_HANDLE XxxYyy_Create(BROKER_HANDLE broker, const void* configuration)
{
    XXX_YYY_HANDLE_DATA* result;
    (void)configuration;
    
	if (broker == NULL) /*configuration is not used*/
    {
        LogError("invalid arg broker=%p", broker);
        result = NULL;
    }
    else
    {
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
                // TODO: Add specific logic 
            }
        }
    }
    return result;
}

static void* XxxYyy_ParseConfigurationFromJson(const char* configuration)
{
	(void)configuration;
    // TODO: Add specific parse logic 
    return NULL;
}

static void XxxYyy_FreeConfiguration(void* configuration)
{
	(void)configuration;
    // TODO: Add sepcific free logic if necessary 
}

static void XxxYyy_Start(MODULE_HANDLE module)
{
    XxxYyy_HANDLE_DATA* handleData = module;
    if (handleData != NULL)
    {
        if (Lock(handleData->lockHandle) != LOCK_OK)
        {
            LogError("not able to Lock, still setting the thread to finish");
        }
        else
        {
            (void)Unlock(handleData->lockHandle);
        }
        // TODO: Add specific start logic 
        // if you don't need Lock capability you can delete them.
    }
}

static void XxxYyy_Destroy(MODULE_HANDLE module)
{
    XxxYyy_HANDLE_DATA* handleData = module;
    // TODO: Add specific destory logic 
    if (Lock(handleData->lockHandle) != LOCK_OK)
    {
        LogError("not able to Lock, still setting the thread to finish");
    }
    else
    {
        Unlock(handleData->lockHandle);
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
