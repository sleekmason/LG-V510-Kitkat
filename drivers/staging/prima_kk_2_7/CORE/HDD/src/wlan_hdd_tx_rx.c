/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/**===========================================================================
  
  \file  wlan_hdd_tx_rx.c
  
  \brief Linux HDD Tx/RX APIs
         Copyright 2008 (c) Qualcomm, Incorporated.
         All Rights Reserved.
         Qualcomm Confidential and Proprietary.
  
  ==========================================================================*/
  
/*                                                                            
               
                                                                           */ 
#include <wlan_hdd_tx_rx.h>
#include <wlan_hdd_softap_tx_rx.h>
#include <wlan_hdd_dp_utils.h>
#include <wlan_qct_tl.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>

#include <wlan_hdd_p2p.h>
#include <linux/wireless.h>
#include <net/cfg80211.h>
#include <net/ieee80211_radiotap.h>
#include "sapApi.h"

#ifdef FEATURE_WLAN_TDLS
#include "wlan_hdd_tdls.h"
#endif

/*                                                                            
                                        
                                                                           */ 

const v_U8_t hddWmmAcToHighestUp[] = {
   SME_QOS_WMM_UP_RESV,
   SME_QOS_WMM_UP_EE,
   SME_QOS_WMM_UP_VI,
   SME_QOS_WMM_UP_NC
};

//                                         
const v_U8_t hdd_QdiscAcToTlAC[] = {
   WLANTL_AC_VO,
   WLANTL_AC_VI,
   WLANTL_AC_BE,
   WLANTL_AC_BK,
};

static struct sk_buff* hdd_mon_tx_fetch_pkt(hdd_adapter_t* pAdapter);

/*                                                                            
                   
                                                                           */ 
  
/*                                                                            
                                       
                                                                           */ 

#ifdef DATA_PATH_UNIT_TEST
//                                   
static void dump_sk_buff(struct sk_buff * skb)
{
  VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: head = %p ", __func__, skb->head);
  VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: data = %p ", __func__, skb->data);
  VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: tail = %p ", __func__, skb->tail);
  VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: end = %p ", __func__, skb->end);
  VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: len = %d ", __func__, skb->len);
  VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: data_len = %d ", __func__, skb->data_len);
  VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: mac_len = %d\n", __func__, skb->mac_len);

  VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x ", 
     skb->data[0], skb->data[1], skb->data[2], skb->data[3], skb->data[4], 
     skb->data[5], skb->data[6], skb->data[7]); 
  VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", 
     skb->data[8], skb->data[9], skb->data[10], skb->data[11], skb->data[12],
     skb->data[13], skb->data[14], skb->data[15]); 
}

//                           
static void transport_thread(hdd_adapter_t *pAdapter)
{
   v_U8_t staId;
   WLANTL_ACEnumType ac = WLANTL_AC_BE;
   vos_pkt_t *pVosPacket = NULL ;
   vos_pkt_t dummyPacket;
   WLANTL_MetaInfoType pktMetaInfo;
   WLANTL_RxMetaInfoType pktRxMetaInfo;
   VOS_STATUS status = VOS_STATUS_E_FAILURE;

   status = hdd_tx_fetch_packet_cbk( pAdapter->pvosContext,
                                     &staId,
                                     &ac,
                                     &pVosPacket,
                                     &pktMetaInfo );
  if (status != VOS_STATUS_SUCCESS && status != VOS_STATUS_E_EMPTY)
     VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Test FAIL hdd_tx_fetch_packet_cbk", __func__);
  else
     VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Test PASS hdd_tx_fetch_packet_cbk", __func__);

  status = hdd_tx_complete_cbk(pAdapter->pvosContext, &dummyPacket, VOS_STATUS_SUCCESS);
  if (status != VOS_STATUS_SUCCESS)
     VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Test FAIL hdd_tx_complete_cbk", __func__);
  else
     VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Test PASS hdd_tx_complete_cbk", __func__);

  status = hdd_tx_low_resource_cbk(pVosPacket, pAdapter);
  if (status != VOS_STATUS_SUCCESS)
     VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Test FAIL hdd_tx_low_resource_cbk", __func__);
  else
     VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Test PASS hdd_tx_low_resource_cbk", __func__);
  
  status = hdd_rx_packet_cbk( pAdapter->pvosContext,
                              &dummyPacket,
                              staId,
                              &pktRxMetaInfo);
  if (status != VOS_STATUS_SUCCESS)
     VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Test FAIL hdd_rx_packet_cbk", __func__);
  else
     VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Test PASS hdd_rx_packet_cbk", __func__);

}
#endif


/*                                                                             
                                                                        

                                                     
                                                                   
                                                
                                                                             */
static VOS_STATUS hdd_flush_tx_queues( hdd_adapter_t *pAdapter )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   v_SINT_t i = -1;
   hdd_list_node_t *anchor = NULL;
   skb_list_node_t *pktNode = NULL;
   struct sk_buff *skb = NULL;

   pAdapter->isVosLowResource = VOS_FALSE;

   while (++i != NUM_TX_QUEUES) 
   {
      //                                   
      spin_lock_bh(&pAdapter->wmm_tx_queue[i].lock);
      while (true) 
      {
         status = hdd_list_remove_front( &pAdapter->wmm_tx_queue[i], &anchor );
         if(VOS_STATUS_E_EMPTY != status)
         {
            pktNode = list_entry(anchor, skb_list_node_t, anchor);
            skb = pktNode->skb;
            //    
            //                              
            ++pAdapter->hdd_stats.hddTxRxStats.txFlushed;
            ++pAdapter->hdd_stats.hddTxRxStats.txFlushedAC[i];
            kfree_skb(skb);
            continue;
         }
         break;
      }
      spin_unlock_bh(&pAdapter->wmm_tx_queue[i].lock);
      //                                    
      pAdapter->isTxSuspended[i] = VOS_FALSE;
   }

   return status;
}

static struct sk_buff* hdd_mon_tx_fetch_pkt(hdd_adapter_t* pAdapter)
{
   skb_list_node_t *pktNode = NULL;
   struct sk_buff *skb = NULL;
   v_SIZE_t size = 0;
   WLANTL_ACEnumType ac = 0;
   VOS_STATUS status = VOS_STATUS_E_FAILURE;
   hdd_list_node_t *anchor = NULL;

   if( NULL == pAdapter )
   {
      VOS_ASSERT(0);
      return NULL;
   }

   //                                           
   hdd_list_size( &pAdapter->wmm_tx_queue[ac], &size ); 
   if( size == 0 )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                 "%s: NO Packet Pending", __func__);
      return NULL;
   }

   //                                
   spin_lock_bh(&pAdapter->wmm_tx_queue[ac].lock);
   status = hdd_list_remove_front( &pAdapter->wmm_tx_queue[ac], &anchor );
   spin_unlock_bh(&pAdapter->wmm_tx_queue[ac].lock);

   if(VOS_STATUS_SUCCESS == status)
   {
      //                                                  
      pktNode = list_entry(anchor, skb_list_node_t, anchor);
      skb = pktNode->skb;
   }
   else
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: Not able to remove Packet from the list",
                  __func__);

      return NULL;
   }

   //                                                                          
   if ( (pAdapter->isTxSuspended[ac]) &&
        (size <= HDD_TX_QUEUE_LOW_WATER_MARK) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
                 "%s: TX queue[%d] re-enabled", __func__, ac);
      pAdapter->isTxSuspended[ac] = VOS_FALSE;      
      /*                                              */
      netif_tx_start_all_queues( pAdapter->dev ); 
   }

   return skb;
}

void hdd_mon_tx_mgmt_pkt(hdd_adapter_t* pAdapter)
{
   hdd_cfg80211_state_t *cfgState;
   struct sk_buff* skb;
   hdd_adapter_t* pMonAdapter = NULL;
   struct ieee80211_hdr *hdr;

   if (pAdapter == NULL )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
       "%s: pAdapter is NULL", __func__);
      return;
   }

   pMonAdapter = hdd_get_adapter( pAdapter->pHddCtx, WLAN_HDD_MONITOR );

   cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );

   if( NULL != cfgState->buf )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
          "%s: Already one MGMT packet Tx going on", __func__);
      return;
   }

   skb = hdd_mon_tx_fetch_pkt(pMonAdapter);

   if (NULL == skb)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
       "%s: No Packet Pending", __func__);
      return;
   }

   cfgState->buf = vos_mem_malloc( skb->len ); //    
   if( cfgState->buf == NULL )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
          "%s: Failed to Allocate memory", __func__);
      goto fail;
   }

   cfgState->len = skb->len;

   vos_mem_copy( cfgState->buf, skb->data, skb->len);

   cfgState->skb = skb; //    
   cfgState->action_cookie = (tANI_U32)cfgState->buf;

   hdr = (struct ieee80211_hdr *)skb->data;
   if( (hdr->frame_control & HDD_FRAME_TYPE_MASK)
                                       == HDD_FRAME_TYPE_MGMT )
   {
       if( (hdr->frame_control & HDD_FRAME_SUBTYPE_MASK)
                                       == HDD_FRAME_SUBTYPE_DEAUTH )
       {
          hdd_softap_sta_deauth( pAdapter, hdr->addr1 ); 
          goto mgmt_handled;
       }
       else if( (hdr->frame_control & HDD_FRAME_SUBTYPE_MASK) 
                                      == HDD_FRAME_SUBTYPE_DISASSOC )
       {
          hdd_softap_sta_disassoc( pAdapter, hdr->addr1 ); 
          goto mgmt_handled;
       }
   }
   VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
      "%s: Sending action frame to SAP to TX, Len %d", __func__, skb->len);

   if (VOS_STATUS_SUCCESS != 
      WLANSAP_SendAction( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                           skb->data, skb->len, 0) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
          "%s: WLANSAP_SendAction returned fail", __func__);
      hdd_sendActionCnf( pAdapter, FALSE );
   }
   return;

mgmt_handled:
   hdd_sendActionCnf( pAdapter, TRUE );
   return;
fail:
   kfree_skb(pAdapter->skb_to_tx);
   pAdapter->skb_to_tx = NULL;
   return;
}

void hdd_mon_tx_work_queue(struct work_struct *work)
{
   hdd_adapter_t* pAdapter = container_of(work, hdd_adapter_t, monTxWorkQueue);
   hdd_mon_tx_mgmt_pkt(pAdapter);
}

int hdd_mon_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
   v_U16_t rt_hdr_len;
   struct ieee80211_hdr *hdr;
   hdd_adapter_t *pPgBkAdapter, *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);
   struct ieee80211_radiotap_header *rtap_hdr =
                        (struct ieee80211_radiotap_header *)skb->data;

   /*                                                      */
   pPgBkAdapter = pAdapter->sessionCtx.monitor.pAdapterForTx;    
   if(pPgBkAdapter == NULL)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
           "%s: No Adapter to piggy back. Dropping the pkt on monitor inf",
                                                                 __func__);
      goto fail; /*                                */
   }
 
   /*                                                                         */
   if (unlikely(skb->len < sizeof(struct ieee80211_radiotap_header)))
      goto fail; /*                                */
   
   /*                                                     */
   if (unlikely(rtap_hdr->it_version))
      goto fail; /*                             */
 
   /*                              */
   rt_hdr_len = ieee80211_get_radiotap_len(skb->data);
 
   /*                                                                         */
   if (unlikely(skb->len < rt_hdr_len))
      goto fail;
 
   /*                                        */  
   dev->trans_start = jiffies;
   /*
                                                     
                                  
    */
   skb_set_mac_header(skb, rt_hdr_len);
   skb_set_network_header(skb, rt_hdr_len);
   skb_set_transport_header(skb, rt_hdr_len); 

   /*                                 */
   skb_pull(skb, rt_hdr_len);
  
   /*                                                                       
                                                
    */
   hdr = (struct ieee80211_hdr *)skb->data;
 
   /*                                                                     
                                                */
   if ( (hdr->frame_control & HDD_FRAME_TYPE_MASK)  == HDD_FRAME_TYPE_DATA)
   { 
      v_U8_t da[6];
      v_U8_t sa[6];

      memcpy (da, hdr->addr1, VOS_MAC_ADDR_SIZE);
      memcpy (sa, hdr->addr2, VOS_MAC_ADDR_SIZE);
 
      /*                        */ 
      skb_pull(skb, HDD_80211_HEADER_LEN);
 
      if ( HDD_FRAME_SUBTYPE_QOSDATA == 
          (hdr->frame_control & HDD_FRAME_SUBTYPE_MASK))
      {
         skb_pull(skb, HDD_80211_HEADER_QOS_CTL);
      }

      /*                 */ 
      skb_pull(skb, HDD_LLC_HDR_LEN);

      /*                                  */ 
      skb_push(skb, HDD_MAC_HDR_SIZE*2);
      memcpy(&skb->data[0], da, HDD_MAC_HDR_SIZE);
      memcpy(&skb->data[HDD_DEST_ADDR_OFFSET], sa, HDD_MAC_HDR_SIZE);

      /*                                                               */ 
      if (vos_be16_to_cpu(
         (*(unsigned short*)&skb->data[HDD_ETHERTYPE_802_1_X_FRAME_OFFSET]) ) 
                                                     != HDD_ETHERTYPE_802_1_X)
      {
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
           "%s: Not a Eapol packet. Drop this frame", __func__);
         //                               
         kfree_skb(skb);
         return NETDEV_TX_OK;
      }

      skb->protocol = htons(HDD_ETHERTYPE_802_1_X);
 
      hdd_hostapd_select_queue(pPgBkAdapter->dev, skb);
      return hdd_softap_hard_start_xmit( skb, pPgBkAdapter->dev );
   }
   else
   {
      VOS_STATUS status;
      WLANTL_ACEnumType ac = 0;
      skb_list_node_t *pktNode = NULL;
      v_SIZE_t pktListSize = 0;

      spin_lock(&pAdapter->wmm_tx_queue[ac].lock);
      //                                                                   
      if ( pAdapter->wmm_tx_queue[ac].count == pAdapter->wmm_tx_queue[ac].max_size)
      {
         /*                                                                       
                                                                              */
         netif_tx_stop_all_queues(pAdapter->dev);
         pAdapter->isTxSuspended[ac] = VOS_TRUE;
         spin_unlock(&pAdapter->wmm_tx_queue[ac].lock);      
         return NETDEV_TX_BUSY;   
      }
      spin_unlock(&pAdapter->wmm_tx_queue[ac].lock);      

      //                                                       
      pktNode = (skb_list_node_t *)&skb->cb;

      //                                     
      pktNode->skb = skb;

      INIT_LIST_HEAD(&pktNode->anchor);

      //                                                  
      spin_lock(&pAdapter->wmm_tx_queue[ac].lock);
      status = hdd_list_insert_back_size( &pAdapter->wmm_tx_queue[ac],
                                          &pktNode->anchor, &pktListSize );
      spin_unlock(&pAdapter->wmm_tx_queue[ac].lock);

      if ( !VOS_IS_STATUS_SUCCESS( status ) )
      {
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s:Insert Tx queue failed. Pkt dropped", __func__);
         kfree_skb(skb);
         return NETDEV_TX_OK;
      }

      if ( pktListSize == 1 )
      {
         /*                                                                  
                                                                                
                                 
          */
         schedule_work(&pPgBkAdapter->monTxWorkQueue);
      }
      return NETDEV_TX_OK;
   }
 
fail:
   VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
           "%s: Packet Rcvd at Monitor interface is not proper,"
           " Dropping the packet",
            __func__);
   kfree_skb(skb);
   return NETDEV_TX_OK;
}
/*                                                                             
                                                                          
                                                                            
                                                                              
                             

                                                        
                                                        

                                                        
                                                                      
                                                                             */
int hdd_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
   VOS_STATUS status;
   WLANTL_ACEnumType ac;
   sme_QosWmmUpType up;
   skb_list_node_t *pktNode = NULL;
   hdd_list_node_t *anchor = NULL;
   v_SIZE_t pktListSize = 0;
   hdd_adapter_t *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);
   v_BOOL_t granted;

   hdd_station_ctx_t *pHddStaCtx = &pAdapter->sessionCtx.station;
   
   v_BOOL_t txSuspended = VOS_FALSE;

   ++pAdapter->hdd_stats.hddTxRxStats.txXmitCalled;

   if (unlikely(netif_queue_stopped(dev))) {
       VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
                  "%s is called when netif TX is disabled", __func__);
       return NETDEV_TX_BUSY;
   }

   //                                                
   ac = hdd_QdiscAcToTlAC[skb->queue_mapping];

   //                                                                     
   //                               
   up = skb->priority;

   ++pAdapter->hdd_stats.hddTxRxStats.txXmitClassifiedAC[ac];

#ifdef HDD_WMM_DEBUG
   VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
              "%s: Classified as ac %d up %d", __func__, ac, up);
#endif //              

   spin_lock(&pAdapter->wmm_tx_queue[ac].lock);
   /*                */
   /*                                                                             
                                                                  
                                                 */
   if((pAdapter->wmm_tx_queue[ac].count + 1) % 10 == 0)
   {
           /*                                                                                                                       
                                                                                                    
                                                                                                                                        */
           WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext, pHddStaCtx->conn_info.staId[0], ac );
   }
   //                                                                   
   if ( pAdapter->wmm_tx_queue[ac].count == pAdapter->wmm_tx_queue[ac].max_size)
   {
      ++pAdapter->hdd_stats.hddTxRxStats.txXmitBackPressured;
      ++pAdapter->hdd_stats.hddTxRxStats.txXmitBackPressuredAC[ac];

      netif_tx_stop_queue(netdev_get_tx_queue(dev, skb_get_queue_mapping(skb)));
      pAdapter->isTxSuspended[ac] = VOS_TRUE;
      txSuspended = VOS_TRUE;
   }

   /*                                                             
                                                                      */
   if (WLANTL_AC_BE == ac)
   {
      if (pAdapter->wmm_tx_queue[ac].count >= HDD_TX_QUEUE_LOW_WATER_MARK)
      {
          VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
                     "%s: Best Effort AC Tx queue is 3/4th full", __func__);
          pAdapter->isVosLowResource = VOS_TRUE;
      }
      else
      {
          pAdapter->isVosLowResource = VOS_FALSE;
      }
   }

   spin_unlock(&pAdapter->wmm_tx_queue[ac].lock);
   if (VOS_TRUE == txSuspended)
   {
       VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                  "%s: TX queue full for AC=%d Disable OS TX queue",
                  __func__, ac );
      return NETDEV_TX_BUSY;
   }

   //                                                       
   pktNode = (skb_list_node_t *)&skb->cb;

   //                                     
   pktNode->skb = skb;

   //                                        
   pktNode->userPriority = up;


   INIT_LIST_HEAD(&pktNode->anchor);

   //                                                  
   spin_lock(&pAdapter->wmm_tx_queue[ac].lock);
   status = hdd_list_insert_back_size( &pAdapter->wmm_tx_queue[ac], &pktNode->anchor, &pktListSize );
   spin_unlock(&pAdapter->wmm_tx_queue[ac].lock);

   if ( !VOS_IS_STATUS_SUCCESS( status ) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,"%s:Insert Tx queue failed. Pkt dropped", __func__);
      ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
      ++pAdapter->hdd_stats.hddTxRxStats.txXmitDroppedAC[ac];
      ++pAdapter->stats.tx_dropped;
      kfree_skb(skb);
      return NETDEV_TX_OK;
   }

   ++pAdapter->hdd_stats.hddTxRxStats.txXmitQueued;
   ++pAdapter->hdd_stats.hddTxRxStats.txXmitQueuedAC[ac];

   //                                                
   if (likely(pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcAccessAllowed) ||
           ( pHddStaCtx->conn_info.uIsAuthenticated == VOS_FALSE))
   {
      granted = VOS_TRUE;
   }
   else
   {
      status = hdd_wmm_acquire_access( pAdapter, ac, &granted );
   }

   if ( granted && ( pktListSize == 1 ))
   {
      //                                                
      //                                                                                            
      status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext, pHddStaCtx->conn_info.staId[0], ac );      

      if ( !VOS_IS_STATUS_SUCCESS( status ) )
      {
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN, "%s: Failed to signal TL for AC=%d", __func__, ac );

         //                                                                                                               
         //                                                                                        
         spin_lock(&pAdapter->wmm_tx_queue[ac].lock);
         status = hdd_list_remove_back( &pAdapter->wmm_tx_queue[ac], &anchor );
         spin_unlock(&pAdapter->wmm_tx_queue[ac].lock);
         ++pAdapter->stats.tx_dropped;
         ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
         ++pAdapter->hdd_stats.hddTxRxStats.txXmitDroppedAC[ac];
         kfree_skb(skb);
         return NETDEV_TX_OK;
      }
   }

   dev->trans_start = jiffies;

   return NETDEV_TX_OK;
}

/*                                                                             
                                                                 
                                                               
                                                                   

                                                   
                   
                                                                             */
void hdd_tx_timeout(struct net_device *dev)
{
   VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
      "%s: Transmission timeout occurred", __func__);
   //                                                                        
   //                                                                       
   //                                                                          
   //                         
} 


/*                                                                             
                                                                 
                        

                                                        
  
                                                         
                                                                             */
struct net_device_stats* hdd_stats(struct net_device *dev)
{
   hdd_adapter_t *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);
   
   return &pAdapter->stats;
}


/*                                                                             
                                                             
                

                                                     
                                                                   
                                                
                                                                             */
VOS_STATUS hdd_init_tx_rx( hdd_adapter_t *pAdapter )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   v_SINT_t i = -1;

   pAdapter->isVosOutOfResource = VOS_FALSE;
   pAdapter->isVosLowResource = VOS_FALSE;

   //                                                                
   //                               

   while (++i != NUM_TX_QUEUES)
   { 
      pAdapter->isTxSuspended[i] = VOS_FALSE; 
      hdd_list_init( &pAdapter->wmm_tx_queue[i], HDD_TX_QUEUE_MAX_LEN);
   }

   return status;
}


/*                                                                             
                                                               
                

                                                     
                                                                   
                                                
                                                                             */
VOS_STATUS hdd_deinit_tx_rx( hdd_adapter_t *pAdapter )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   v_SINT_t i = -1;

   status = hdd_flush_tx_queues(pAdapter);
   while (++i != NUM_TX_QUEUES) 
   {
      //                                            
      hdd_list_destroy( &pAdapter->wmm_tx_queue[i] );
   }

   return status;
}


/*                                                                             
                                                                       
                

                                                     
                                                                   
                                                
                                                                             */
VOS_STATUS hdd_disconnect_tx_rx( hdd_adapter_t *pAdapter )
{
   return hdd_flush_tx_queues(pAdapter);
}


/*                                                                             
                                                                 

                                                  
                                                    
                                       
                                                                             */

v_BOOL_t hdd_IsEAPOLPacket( vos_pkt_t *pVosPacket )
{
    VOS_STATUS vosStatus  = VOS_STATUS_SUCCESS;
    v_BOOL_t   fEAPOL     = VOS_FALSE; 
    void       *pBuffer   = NULL;

    
    vosStatus = vos_pkt_peek_data( pVosPacket, (v_SIZE_t)HDD_ETHERTYPE_802_1_X_FRAME_OFFSET,
                          &pBuffer, HDD_ETHERTYPE_802_1_X_SIZE );
    if (VOS_IS_STATUS_SUCCESS( vosStatus ) )
    {
       if ( vos_be16_to_cpu( *(unsigned short*)pBuffer ) == HDD_ETHERTYPE_802_1_X )
       {
          fEAPOL = VOS_TRUE;
       }
    }  
    
   return fEAPOL;
}


#ifdef FEATURE_WLAN_WAPI //                             
/*                                                                             
                                                             

                                                
                                                 
                                       
                                                                             */

v_BOOL_t hdd_IsWAIPacket( vos_pkt_t *pVosPacket )
{
    VOS_STATUS vosStatus  = VOS_STATUS_SUCCESS;
    v_BOOL_t   fIsWAI     = VOS_FALSE;
    void       *pBuffer   = NULL;

    //                             
    vosStatus = vos_pkt_peek_data( pVosPacket, (v_SIZE_t)HDD_ETHERTYPE_802_1_X_FRAME_OFFSET,
                          &pBuffer, HDD_ETHERTYPE_802_1_X_SIZE );

    if (VOS_IS_STATUS_SUCCESS( vosStatus ) )
    {
       if ( vos_be16_to_cpu( *(unsigned short*)pBuffer ) == HDD_ETHERTYPE_WAI)
       {
          fIsWAI = VOS_TRUE;
       }
    }

   return fIsWAI;
}
#endif /*                   */

/*                                                                             
                                                                
                                                                    
                                                                  

                                                      
                                                                    
                                                        

                                                                       
                                                    
                                                                             */
VOS_STATUS hdd_tx_complete_cbk( v_VOID_t *vosContext, 
                                vos_pkt_t *pVosPacket, 
                                VOS_STATUS vosStatusIn )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   hdd_adapter_t *pAdapter = NULL;   
   hdd_context_t *pHddCtx = NULL;
   void* pOsPkt = NULL;
   
   if( ( NULL == vosContext ) || ( NULL == pVosPacket )  )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Null params being passed", __func__);
      return VOS_STATUS_E_FAILURE; 
   }

   //                        
   status = vos_pkt_get_os_packet( pVosPacket, &pOsPkt, VOS_TRUE );
   if(!VOS_IS_STATUS_SUCCESS( status ))
   {
      //                                                              
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Failure extracting skb from vos pkt", __func__);
      vos_pkt_return_packet( pVosPacket );
      return VOS_STATUS_E_FAILURE;
   }
   
   //                    
   pHddCtx = (hdd_context_t *)vos_get_context( VOS_MODULE_ID_HDD, vosContext );
   //                        
   pAdapter = hdd_get_adapter(pHddCtx,WLAN_HDD_INFRA_STATION);
   if(pAdapter == NULL)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,"%s: HDD adapter context is Null", __func__);
   }
   else
   {
      ++pAdapter->hdd_stats.hddTxRxStats.txCompleted;
   }

   kfree_skb((struct sk_buff *)pOsPkt); 

   //                                
   status = vos_pkt_return_packet( pVosPacket );
   if(!VOS_IS_STATUS_SUCCESS( status ))
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Could not return VOS packet to the pool", __func__);
   }

   return status;
}


/*                                                                             
                                                                        
                                  

                                                     
                                                                     
                                                            
                                                                  
                                                               
  
                                                                    
                                                                       
                                                    
                                                                             */
VOS_STATUS hdd_tx_fetch_packet_cbk( v_VOID_t *vosContext,
                                    v_U8_t *pStaId,
                                    WLANTL_ACEnumType  ac,
                                    vos_pkt_t **ppVosPacket,
                                    WLANTL_MetaInfoType *pPktMetaInfo )
{
   VOS_STATUS status = VOS_STATUS_E_FAILURE;
   hdd_adapter_t *pAdapter = NULL;
   hdd_context_t *pHddCtx = NULL;
   hdd_list_node_t *anchor = NULL;
   skb_list_node_t *pktNode = NULL;
   struct sk_buff *skb = NULL;
   vos_pkt_t *pVosPacket = NULL;
   v_MACADDR_t* pDestMacAddress = NULL;
   v_TIME_t timestamp;
   WLANTL_ACEnumType newAc;
   v_SIZE_t size = 0;
   tANI_U8   acAdmitted, i;

   //                      
   if ( ( NULL == vosContext ) || 
        ( NULL == pStaId ) || 
        ( NULL == ppVosPacket ) ||
        ( NULL == pPktMetaInfo ) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Null Params being passed", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   //                    
   pHddCtx = (hdd_context_t *)vos_get_context( VOS_MODULE_ID_HDD, vosContext );
   if(pHddCtx == NULL)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: HDD adapter context is Null", __func__);
      return VOS_STATUS_E_FAILURE;
   }
 
   pAdapter = pHddCtx->sta_to_adapter[*pStaId];
   if( NULL == pAdapter )
   {
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAILURE;
   }

   ++pAdapter->hdd_stats.hddTxRxStats.txFetched;

   *ppVosPacket = NULL;

   //                                        
   if( ac >= WLANTL_MAX_AC || ac < 0)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Invalid AC %d passed by TL", __func__, ac);
      return VOS_STATUS_E_FAILURE;
   }

   ++pAdapter->hdd_stats.hddTxRxStats.txFetchedAC[ac];

#ifdef HDD_WMM_DEBUG
   VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,"%s: AC %d passed by TL", __func__, ac);
#endif //              

   //                           
   //                                                
   //                                 

   //                               
   //                                              
   if (unlikely((0==pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcAccessAllowed) &&
                (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.uIsAuthenticated))
   {
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchEmpty;
#ifdef HDD_WMM_DEBUG
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                 "%s: no packets pending", __func__);
#endif //              
      return VOS_STATUS_E_FAILURE;
   }
      
   //                                           
   hdd_list_size( &pAdapter->wmm_tx_queue[ac], &size ); 
   if( size >  0 )
   {
       //                   
#ifdef HDD_WMM_DEBUG
       VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                       "%s: AC %d has packets pending", __func__, ac);
#endif //              
   }
   else
   {
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchEmpty;
#ifdef HDD_WMM_DEBUG
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: no packets pending", __func__);
#endif //              
      return VOS_STATUS_E_FAILURE;
   }

   //                                                                                             
   //                                                     
   status = vos_pkt_wrap_data_packet( &pVosPacket, 
                                      VOS_PKT_TYPE_TX_802_3_DATA, 
                                      NULL, //                          
                                      hdd_tx_low_resource_cbk, 
                                      pAdapter );

   if (status == VOS_STATUS_E_ALREADY || status == VOS_STATUS_E_RESOURCES)
   {
      //                                           
      pAdapter->isVosOutOfResource = VOS_TRUE;
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchLowResources;
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,"%s: VOSS in Low Resource scenario", __func__);
      //                                                    
      return VOS_STATUS_E_FAILURE;
   }

   //                                
   spin_lock_bh(&pAdapter->wmm_tx_queue[ac].lock);
   status = hdd_list_remove_front( &pAdapter->wmm_tx_queue[ac], &anchor );
   spin_unlock_bh(&pAdapter->wmm_tx_queue[ac].lock);

   if(VOS_STATUS_SUCCESS == status)
   {
      //                                                  
      pktNode = list_entry(anchor, skb_list_node_t, anchor);
      skb = pktNode->skb;
   }
   else
   {
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeueError;
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN, "%s: Error in de-queuing "
         "skb from Tx queue status = %d", __func__, status );
      vos_pkt_return_packet(pVosPacket);
      return VOS_STATUS_E_FAILURE;
   }

   //                         
   status = vos_pkt_set_os_packet( pVosPacket, skb );
   if (status != VOS_STATUS_SUCCESS)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,"%s: Error attaching skb", __func__);
      vos_pkt_return_packet(pVosPacket);
      ++pAdapter->stats.tx_dropped;
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeueError;
      kfree_skb(skb);
      return VOS_STATUS_E_FAILURE;
   }

   //                                        
   if(pVosPacket == NULL)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,"%s: VOS packet returned by VOSS is NULL", __func__);
      ++pAdapter->stats.tx_dropped;
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeueError;
      kfree_skb(skb);
      return VOS_STATUS_E_FAILURE;
   }

#ifdef FEATURE_WLAN_TDLS
    if (eTDLS_SUPPORT_ENABLED == pHddCtx->tdls_mode)
    {
        hdd_station_ctx_t *pHddStaCtx = &pAdapter->sessionCtx.station;
        u8 mac[6];

        wlan_hdd_tdls_extract_da(skb, mac);

        if (vos_is_macaddr_group((v_MACADDR_t *)mac)) {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_MED,
                      "broadcast packet, not adding to peer list");
        } else if (memcmp(pHddStaCtx->conn_info.bssId,
                            mac, 6) != 0) {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_MED,
                      "extract mac: " MAC_ADDRESS_STR,
                      MAC_ADDR_ARRAY(mac) );

            wlan_hdd_tdls_increment_pkt_count(pAdapter, mac, 1);
        } else {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_MED,
                       "packet da is bssid, not adding to peer list");
        }
    }
#endif

   //                        
   *ppVosPacket = pVosPacket;

   //                                          
   //                                                                 
   vos_pkt_get_timestamp( pVosPacket, &timestamp );
   pPktMetaInfo->usTimeStamp = (v_U16_t)timestamp;
   
   if(pAdapter->sessionCtx.station.conn_info.uIsAuthenticated == VOS_TRUE)
      pPktMetaInfo->ucIsEapol = 0;       
   else 
      pPktMetaInfo->ucIsEapol = hdd_IsEAPOLPacket( pVosPacket ) ? 1 : 0;

#ifdef FEATURE_WLAN_WAPI
   //                                                     
      pPktMetaInfo->ucIsWai = hdd_IsWAIPacket( pVosPacket ) ? 1 : 0;
#endif /*                   */

   if ((HDD_WMM_USER_MODE_NO_QOS == pHddCtx->cfg_ini->WmmMode) ||
       (!pAdapter->hddWmmStatus.wmmQap))
   {
      //                                                       
      pPktMetaInfo->ucUP = 0;
      pPktMetaInfo->ucTID = 0;
   }
   else
   {
      /*                                    
                                                          
                                                                      */
      if(!pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcAccessRequired ||
         pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcTspecValid)
      {
        pPktMetaInfo->ucUP = pktNode->userPriority;
        pPktMetaInfo->ucTID = pPktMetaInfo->ucUP;
      }
      else
      {
        //                
        acAdmitted = pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcTspecValid;
        newAc = WLANTL_AC_BK;
        for (i=ac-1; i>0; i--)
        {
            if (pAdapter->hddWmmStatus.wmmAcStatus[i].wmmAcAccessRequired == 0)
            {
                newAc = i;
                break;
            }
        }
        pPktMetaInfo->ucUP = hddWmmAcToHighestUp[newAc];
        pPktMetaInfo->ucTID = pPktMetaInfo->ucUP;
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,"Downgrading UP %d to UP %d ", pktNode->userPriority, pPktMetaInfo->ucUP);
      }
   }

   pPktMetaInfo->ucType = 0;          //                             
   pPktMetaInfo->ucDisableFrmXtl = 0; //                               
   if ( 1 < size )
   {
       pPktMetaInfo->bMorePackets = 1; //                            
   }
   else
   {
       pPktMetaInfo->bMorePackets = 0;
   }

   //                                                   
   pDestMacAddress = (v_MACADDR_t*)skb->data;
   pPktMetaInfo->ucBcast = vos_is_macaddr_broadcast( pDestMacAddress ) ? 1 : 0;
   pPktMetaInfo->ucMcast = vos_is_macaddr_group( pDestMacAddress ) ? 1 : 0;

   

   //                                                                          
   if ( (pAdapter->isTxSuspended[ac]) &&
        (size <= HDD_TX_QUEUE_LOW_WATER_MARK) )
   {
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchDePressured;
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchDePressuredAC[ac];
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                 "%s: TX queue[%d] re-enabled", __func__, ac);
      pAdapter->isTxSuspended[ac] = VOS_FALSE;      
      netif_tx_wake_queue(netdev_get_tx_queue(pAdapter->dev, 
                                        skb_get_queue_mapping(skb) ));
   }


   //                                                              
   //                                                             
   //                                                             
   //                                                           
   //                                                              
   //                                                          
   //                                                              
   //                                                             
   //                 
   pAdapter->stats.tx_bytes += skb->len;
   ++pAdapter->stats.tx_packets;
   ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeued;
   ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeuedAC[ac];

   if(pHddCtx->cfg_ini->thermalMitigationEnable)
   {
      if(mutex_lock_interruptible(&pHddCtx->tmInfo.tmOperationLock))
      {
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                    "%s: Tm Lock fail", __func__);
         return VOS_STATUS_E_FAILURE;
      }
      if(WLAN_HDD_TM_LEVEL_1 < pHddCtx->tmInfo.currentTmLevel)
      {
         if(0 == pHddCtx->tmInfo.txFrameCount)
         {
            /*                                   */
            pHddCtx->tmInfo.lastOpenTs = timestamp;
         }

         if(((timestamp - pHddCtx->tmInfo.lastOpenTs) > (pHddCtx->tmInfo.tmAction.txOperationDuration / 10)) &&
            (pHddCtx->tmInfo.txFrameCount >= pHddCtx->tmInfo.tmAction.txBlockFrameCountThreshold))
         {
            spin_lock(&pAdapter->wmm_tx_queue[ac].lock);
            /*                                                                 
                                          */
            netif_tx_stop_all_queues(pAdapter->dev);
            spin_unlock(&pAdapter->wmm_tx_queue[ac].lock);
            pHddCtx->tmInfo.lastblockTs = timestamp;
            if(VOS_TIMER_STATE_STOPPED == vos_timer_getCurrentState(&pHddCtx->tmInfo.txSleepTimer))
            {
               vos_timer_start(&pHddCtx->tmInfo.txSleepTimer, pHddCtx->tmInfo.tmAction.txSleepDuration);
            }
         }
         else if(((timestamp - pHddCtx->tmInfo.lastOpenTs) > (pHddCtx->tmInfo.tmAction.txOperationDuration / 10)) &&
                 (pHddCtx->tmInfo.txFrameCount < pHddCtx->tmInfo.tmAction.txBlockFrameCountThreshold))
         {
            /*                                                               
                                                               */
            pHddCtx->tmInfo.lastOpenTs = timestamp;
            pHddCtx->tmInfo.txFrameCount = 0;
         }
         else
         {
            /*            */
         }
         pHddCtx->tmInfo.txFrameCount++;
      }
      mutex_unlock(&pHddCtx->tmInfo.tmOperationLock);
   }


#ifdef HDD_WMM_DEBUG
   VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,"%s: Valid VOS PKT returned to TL", __func__);
#endif //              

   return status;
}


/*                                                                             
                                                                      
                                                                          
                                                                     
            

                                                  
                                                                       
  
                                                                      
                                                  
                                                                               */
VOS_STATUS hdd_tx_low_resource_cbk( vos_pkt_t *pVosPacket, 
                                    v_VOID_t *userData )
{
   VOS_STATUS status;
   v_SINT_t i = 0;
   v_SIZE_t size = 0;
   hdd_adapter_t* pAdapter = (hdd_adapter_t *)userData;
   
   if(pAdapter == NULL)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: HDD adapter context is Null", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   //                                                                                
   //                                                                               
   //                                                                          
   vos_pkt_return_packet( pVosPacket );

   pAdapter->isVosOutOfResource = VOS_FALSE;

   //                                                                 
   for( i=NUM_TX_QUEUES-1; i>=0; --i )
   {
      size = 0;
      hdd_list_size( &pAdapter->wmm_tx_queue[i], &size );
      if ( size > 0 )
      {
         status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext, 
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId [0], 
                                        (WLANTL_ACEnumType)i );
         if( !VOS_IS_STATUS_SUCCESS( status ) )
         {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Failure in indicating pkt to TL for ac=%d", __func__,i); 
         }
      }
   }

   return VOS_STATUS_SUCCESS;
}


/*                                                                             
                                                                   
                                                                   
                                

                                                        
                                                           
                                          
                                                                             

                                                                           
                                                       
                                                                             */
VOS_STATUS hdd_rx_packet_cbk( v_VOID_t *vosContext, 
                              vos_pkt_t *pVosPacketChain,
                              v_U8_t staId,
                              WLANTL_RxMetaInfoType* pRxMetaInfo )
{
   hdd_adapter_t *pAdapter = NULL;
   hdd_context_t *pHddCtx = NULL;
   VOS_STATUS status = VOS_STATUS_E_FAILURE;
   int rxstat;
   struct sk_buff *skb = NULL;
   vos_pkt_t* pVosPacket;
   vos_pkt_t* pNextVosPacket;

   //                      
   if ( ( NULL == vosContext ) || 
        ( NULL == pVosPacketChain ) ||
        ( NULL == pRxMetaInfo ) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Null params being passed", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   pHddCtx = (hdd_context_t *)vos_get_context( VOS_MODULE_ID_HDD, vosContext );
   if ( NULL == pHddCtx )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: HDD adapter context is Null", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   pAdapter = pHddCtx->sta_to_adapter[staId];
   if( NULL == pAdapter )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: pAdapter is Null for staId %u",
                 __func__, staId);
      return VOS_STATUS_E_FAILURE;
   }

   ++pAdapter->hdd_stats.hddTxRxStats.rxChains;

   //                                       
   pVosPacket = pVosPacketChain;
   do
   {
      //                                                
      //                                                                   
      status = vos_pkt_walk_packet_chain( pVosPacket, &pNextVosPacket, VOS_FALSE);

      //                                                  
      if (!((status == VOS_STATUS_SUCCESS) || (status == VOS_STATUS_E_EMPTY)))
      {
         ++pAdapter->hdd_stats.hddTxRxStats.rxDropped;
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Failure walking packet chain", __func__);
         return VOS_STATUS_E_FAILURE;
      }

      //                             
      //                                                     
      status = vos_pkt_get_os_packet( pVosPacket, (v_VOID_t **)&skb, VOS_TRUE );
      if(!VOS_IS_STATUS_SUCCESS( status ))
      {
         ++pAdapter->hdd_stats.hddTxRxStats.rxDropped;
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Failure extracting skb from vos pkt", __func__);
         return VOS_STATUS_E_FAILURE;
      }

      if (WLAN_HDD_ADAPTER_MAGIC != pAdapter->magic)
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
           "Magic cookie(%x) for adapter sanity verification is invalid", pAdapter->magic);
         return eHAL_STATUS_FAILURE;
      }

#ifdef FEATURE_WLAN_TDLS
    if ((eTDLS_SUPPORT_ENABLED == pHddCtx->tdls_mode) &&
         0 != pHddCtx->connected_peer_count)
    {
        hdd_station_ctx_t *pHddStaCtx = &pAdapter->sessionCtx.station;
        u8 mac[6];

        wlan_hdd_tdls_extract_sa(skb, mac);

        if (vos_is_macaddr_group((v_MACADDR_t *)mac)) {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_MED,
                      "rx broadcast packet, not adding to peer list");
        } else if (memcmp(pHddStaCtx->conn_info.bssId,
                            mac, 6) != 0) {
            hddTdlsPeer_t *curr_peer;
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_MED,
                      "rx extract mac:" MAC_ADDRESS_STR,
                      MAC_ADDR_ARRAY(mac) );
            curr_peer = wlan_hdd_tdls_find_peer(pAdapter, mac, TRUE);
            if ((NULL != curr_peer) && (eTDLS_LINK_CONNECTED == curr_peer->link_status)
                 && (TRUE == pRxMetaInfo->isStaTdls))
            {
                wlan_hdd_tdls_increment_pkt_count(pAdapter, mac, 0);
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,"rssi is %d", pRxMetaInfo->rssiAvg);
                wlan_hdd_tdls_set_rssi (pAdapter, mac, pRxMetaInfo->rssiAvg);
            }
        } else {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_MED,
                       "rx packet sa is bssid, not adding to peer list");
        }
    }
#endif

      skb->dev = pAdapter->dev;
      skb->protocol = eth_type_trans(skb, skb->dev);
      skb->ip_summed = CHECKSUM_NONE;
      ++pAdapter->hdd_stats.hddTxRxStats.rxPackets;
      ++pAdapter->stats.rx_packets;
      pAdapter->stats.rx_bytes += skb->len;
#ifdef WLAN_OPEN_SOURCE
#ifdef WLAN_FEATURE_HOLD_RX_WAKELOCK
      wake_lock_timeout(&pHddCtx->rx_wake_lock, msecs_to_jiffies(HDD_WAKE_LOCK_DURATION));
#endif
#endif
      rxstat = netif_rx_ni(skb);
      if (NET_RX_SUCCESS == rxstat)
      {
         ++pAdapter->hdd_stats.hddTxRxStats.rxDelivered;
      }
      else
      {
         ++pAdapter->hdd_stats.hddTxRxStats.rxRefused;
      }
      //                                         
      pVosPacket = pNextVosPacket;

   } while (pVosPacket);

   //                                                       
   status = vos_pkt_return_packet( pVosPacketChain );
   if(!VOS_IS_STATUS_SUCCESS( status ))
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Failure returning vos pkt", __func__);
   }
   
   pAdapter->dev->last_rx = jiffies;

   return status;   
}

