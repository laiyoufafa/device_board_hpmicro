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
#include <stdio.h>
#include <stdlib.h>
#include "lwip/tcpip.h"
#include "ohos_init.h"
#include "hpm_lwip.h"
#include "ethernetif.h"
#include "lwip/tcpip.h"


static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t rxDescTab0[ENET_RX_BUFF_COUNT] ; /* Ethernet Rx DMA Descriptor */

static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t txDescTab0[ENET_TX_BUFF_COUNT] ; /* Ethernet Tx DMA Descriptor */

static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t rxBuff0[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t txBuff0[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */

static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t rxDescTab1[ENET_RX_BUFF_COUNT] ; /* Ethernet Rx DMA Descriptor */

static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t txDescTab1[ENET_TX_BUFF_COUNT] ; /* Ethernet Tx DMA Descriptor */

static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t rxBuff1[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t txBuff1[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */


struct HpmEnetDevice enetDev[2] = {
    [0] = {
        .isEnable = 1,
        .isDefault = 1,
        .name = "geth",
        .base = BOARD_ENET_RGMII,
        .irqNum = IRQn_ENET0,
        .infType = enet_inf_rgmii,
        .macAddr = {0x98, 0x2C, 0xBC, 0xB1, 0x9F, 0x15},
        .ip = {192, 168, 2, 35},
        .netmask = {255, 255, 255, 0},
        .gw = {192, 168, 1, 1},
        .desc = {
            .tx_desc_list_head = txDescTab0,
            .rx_desc_list_head = rxDescTab0,
            .tx_buff_cfg = {
                .buffer = (uint32_t)txBuff0,
                .count = ENET_TX_BUFF_COUNT,
                .size = ENET_TX_BUFF_SIZE,
            },
             .rx_buff_cfg = {
                .buffer = (uint32_t)rxBuff0,
                .count = ENET_RX_BUFF_COUNT,
                .size = ENET_RX_BUFF_SIZE,
            },

        },
    },
    [1] = {
        .isEnable = 1,
        .isDefault = 0,
        .name = "eth",
        .base = BOARD_ENET_RMII,
        .irqNum = IRQn_ENET1,
        .infType = enet_inf_rmii,
        .macAddr = {0x98, 0x2C, 0xBC, 0xB1, 0x9F, 0x17},
        .ip = {192, 168, 1, 88},
        .netmask = {255, 255, 255, 0},
        .gw = {192, 168, 1, 1},
        .desc = {
            .tx_desc_list_head = txDescTab1,
            .rx_desc_list_head = rxDescTab1,
            .tx_buff_cfg = {
                .buffer = (uint32_t)txBuff1,
                .count = ENET_TX_BUFF_COUNT,
                .size = ENET_TX_BUFF_SIZE,
            },
             .rx_buff_cfg = {
                .buffer = (uint32_t)rxBuff1,
                .count = ENET_RX_BUFF_COUNT,
                .size = ENET_RX_BUFF_SIZE,
            },
        },
    },
};


void enetDevInit(struct HpmEnetDevice *dev)
{
    if (!dev->isEnable) {
        return 0;
    }

    board_init_enet_pins(dev->base);
    board_reset_enet_phy(dev->base);

    if (dev->infType == enet_inf_rmii) {
        board_init_enet_rmii_reference_clock(dev->base, BOARD_ENET_RMII_INT_REF_CLK);
    }

    if (dev->infType == enet_inf_rgmii) {
        board_init_enet_rgmii_clock_delay(dev->base);
    }

    memset(dev->desc.rx_desc_list_head, 0x00, sizeof(enet_rx_desc_t) * dev->desc.rx_buff_cfg.count);
    memset(dev->desc.tx_desc_list_head, 0x00, sizeof(enet_tx_desc_t) * dev->desc.tx_buff_cfg.count);
    
    enet_mac_config_t macCfg;
    macCfg.mac_addr_high[0] = dev->macAddr[5];
    macCfg.mac_addr_high[0] <<= 8;
    macCfg.mac_addr_high[0] |= dev->macAddr[4];

    macCfg.mac_addr_low[0]  = dev->macAddr[3];
    macCfg.mac_addr_low[0] <<= 8;
    macCfg.mac_addr_low[0] |= dev->macAddr[2];
    macCfg.mac_addr_low[0] <<= 8;
    macCfg.mac_addr_low[0] |= dev->macAddr[1];
    macCfg.mac_addr_low[0] <<= 8;
    macCfg.mac_addr_low[0] |= dev->macAddr[0];
    macCfg.valid_max_count  = 1;

    uint32_t dmaIntEnable = ENET_DMA_INTR_EN_NIE_SET(1)   /* Enable normal interrupt summary */
                            | ENET_DMA_INTR_EN_RIE_SET(1);  /* Enable receive interrupt */ 
    enet_controller_init(dev->base, dev->infType, &dev->desc, &macCfg, dmaIntEnable);
    dev->base->INTR_MASK |= 0xFFFFFFFF;
    dev->base->MMC_INTR_MASK_RX |= 0xFFFFFFFF;
    dev->base->MMC_INTR_MASK_TX |= 0xFFFFFFFF;
    dev->base->MMC_IPC_INTR_MASK_RX |= 0xFFFFFFFF;
    enet_disable_lpi_interrupt(dev->base);

    if (dev->infType == enet_inf_rgmii) {
        rtl8211_config_t phyConfig;
        rtl8211_reset(dev->base);
        rtl8211_basic_mode_default_config(dev->base, &phyConfig);
        rtl8211_basic_mode_init(dev->base, &phyConfig);
    }
    
    if (dev->infType == enet_inf_rmii) {
        rtl8201_config_t phyConfig;
        rtl8201_reset(dev->base);
        rtl8201_basic_mode_default_config(dev->base, &phyConfig);
        rtl8201_basic_mode_init(dev->base, &phyConfig);
    }

    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP_ADDR4(&ipaddr, dev->ip[0], dev->ip[1], dev->ip[2], dev->ip[3]);
    IP_ADDR4(&netmask, dev->netmask[0], dev->netmask[1], dev->netmask[2], dev->netmask[3]);
    IP_ADDR4(&gw, dev->gw[0], dev->gw[1], dev->gw[2], dev->gw[3]);

    netif_add(&dev->netif, &ipaddr, &netmask, &gw, dev, ethernetif_init, tcpip_input);

    if (dev->isDefault) {
        netif_set_default(&dev->netif);
    }

    netif_set_up(&dev->netif);
}


void HpmLwipInit(void)
{
    printf("HpmLwipInit...\n");

    tcpip_init(NULL, NULL);

    enetDevInit(&enetDev[0]);
    enetDevInit(&enetDev[1]);
}


APP_SERVICE_INIT(HpmLwipInit);
