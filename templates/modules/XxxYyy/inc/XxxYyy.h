// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef XXX_YYY_H
#define XXX_YYY_H

#include "module.h"

#ifdef __cplusplus
extern "C"
{
#endif

MODULE_EXPORT const MODULE_API* MODULE_STATIC_GETAPI(XXX_YYY_MODULE)(MODULE_API_VERSION gateway_api_version);

#define SEND_CYCLE "send-cycle-sec"

#ifdef __cplusplus
}
#endif


#endif /*XXX_YYY_H*/