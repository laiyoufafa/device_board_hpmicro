/*
 * Copyright (c) 2022 HPMicro
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HPMICRO_ETHERNETIF_H
#define HPMICRO_ETHERNETIF_H


#include "lwip/err.h"
#include "lwip/netif.h"
#include "netif/etharp.h"

#ifdef __cplusplus
extern "C" {
#endif

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif