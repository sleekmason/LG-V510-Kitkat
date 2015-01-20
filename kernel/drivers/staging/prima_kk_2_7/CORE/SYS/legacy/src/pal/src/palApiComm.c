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

#include <palApi.h>
#include <sirTypes.h>   //                         
#include <vos_api.h>

#include <sirParams.h>  //                     
#include "wlan_qct_wda.h"

#ifdef MEMORY_DEBUG
eHalStatus palAllocateMemory_debug( tHddHandle hHdd, void **ppMemory, tANI_U32 numBytes, char* fileName, tANI_U32 lineNum )
{
   eHalStatus halStatus = eHAL_STATUS_SUCCESS;
   
   *ppMemory = vos_mem_malloc_debug( numBytes, fileName, lineNum );
   
   if ( NULL == *ppMemory ) 
   {
      halStatus = eHAL_STATUS_FAILURE;
   }
   
   return( halStatus );
}
#else
eHalStatus palAllocateMemory( tHddHandle hHdd, void **ppMemory, tANI_U32 numBytes )
{
   eHalStatus halStatus = eHAL_STATUS_SUCCESS;
   
   *ppMemory = vos_mem_malloc( numBytes );
   
   if ( NULL == *ppMemory ) 
   {
      halStatus = eHAL_STATUS_FAILURE;
   }
   
   return( halStatus );
}
#endif


eHalStatus palFreeMemory( tHddHandle hHdd, void *pMemory )
{
   vos_mem_free( pMemory );
   
   return( eHAL_STATUS_SUCCESS );
}

eHalStatus palFillMemory( tHddHandle hHdd, void *pMemory, tANI_U32 numBytes, tANI_BYTE fillValue )
{
   vos_mem_set( pMemory, numBytes, fillValue );
   
   return( eHAL_STATUS_SUCCESS );
}


eHalStatus palCopyMemory( tHddHandle hHdd, void *pDst, const void *pSrc, tANI_U32 numBytes )
{
   vos_mem_copy( pDst, pSrc, numBytes );
   
   return( eHAL_STATUS_SUCCESS );
}



tANI_BOOLEAN palEqualMemory( tHddHandle hHdd, void *pMemory1, void *pMemory2, tANI_U32 numBytes )
{
   return( vos_mem_compare( pMemory1, pMemory2, numBytes ) );
}   


eHalStatus palPktAlloc(tHddHandle hHdd, eFrameType frmType, tANI_U16 size, void **data, void **ppPacket)
{
   eHalStatus halStatus = eHAL_STATUS_FAILURE;
   VOS_STATUS vosStatus;
   
   vos_pkt_t *pVosPacket;
   
   do 
   {
      //                                                                                              
      //                                                                                   
      VOS_ASSERT( HAL_TXRX_FRM_802_11_MGMT == frmType );
    
      if ( HAL_TXRX_FRM_802_11_MGMT != frmType ) break;
   
      //                                                                                                
      //                                        
      vosStatus = vos_pkt_get_packet( &pVosPacket, VOS_PKT_TYPE_TX_802_11_MGMT, size, 1, VOS_TRUE, NULL, NULL );
      if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) ) break;
      
      //                                                                                            
      //                                                        
      vosStatus = vos_pkt_reserve_head( pVosPacket, data, size );
      if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) ) 
      {
         vos_pkt_return_packet( pVosPacket );
         break;
      }
      
      //                                                                                           
      //                       
      *ppPacket = (void *)pVosPacket;
      
      halStatus = eHAL_STATUS_SUCCESS;
   
   } while( 0 );
   
   return( halStatus );
}



void palPktFree( tHddHandle hHdd, eFrameType frmType, void* buf, void *pPacket)
{
   vos_pkt_t *pVosPacket = (vos_pkt_t *)pPacket;
   VOS_STATUS vosStatus;
      
   do 
   {
      VOS_ASSERT( pVosPacket );
      
      if ( !pVosPacket ) break;
      
      //                                                                                              
      //                                                                                   
      VOS_ASSERT( HAL_TXRX_FRM_802_11_MGMT == frmType );
      if ( HAL_TXRX_FRM_802_11_MGMT != frmType ) break;
      
      //                                                                                           
      //                    
      vosStatus = vos_pkt_return_packet( pVosPacket );
      VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
      
   } while( 0 );
   
   return;
}
   


tANI_U32 palGetTickCount(tHddHandle hHdd)
{
   return( vos_timer_get_system_ticks() );
}


tANI_U32 pal_be32_to_cpu(tANI_U32 x)
{
   return( x );
}

tANI_U32 pal_cpu_to_be32(tANI_U32 x)
{
   return(( x ) );
}   

tANI_U16 pal_be16_to_cpu(tANI_U16 x)
{
   return( ( x ) );
}   
   
tANI_U16 pal_cpu_to_be16(tANI_U16 x)
{
   return( ( x ) );
}   



eHalStatus palSpinLockAlloc( tHddHandle hHdd, tPalSpinLockHandle *pHandle )
{
   eHalStatus halStatus = eHAL_STATUS_FAILURE;
   VOS_STATUS vosStatus;
   vos_lock_t *pLock;
   
   do
   {
      pLock = vos_mem_malloc( sizeof( vos_lock_t ) );
   
      if ( NULL == pLock ) break;
      
      vosStatus = vos_lock_init( pLock );
      if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
      {
         vos_mem_free( pLock );
         break;
      }
      
      *pHandle = (tPalSpinLockHandle)pLock;
      halStatus = eHAL_STATUS_SUCCESS;
      
   } while( 0 );
   
   return( halStatus );   
}


eHalStatus palSpinLockFree( tHddHandle hHdd, tPalSpinLockHandle hSpinLock )
{
   eHalStatus halStatus = eHAL_STATUS_FAILURE;
   vos_lock_t *pLock = (vos_lock_t *)hSpinLock;
   VOS_STATUS vosStatus;
   
   vosStatus = vos_lock_destroy( pLock );
   if ( VOS_IS_STATUS_SUCCESS( vosStatus ) )
   {
      //                                          
      //                                               
      vos_mem_free( pLock );
      
      halStatus = eHAL_STATUS_SUCCESS;
   }
   return( halStatus );
}


eHalStatus palSpinLockTake( tHddHandle hHdd, tPalSpinLockHandle hSpinLock )
{
   eHalStatus halStatus = eHAL_STATUS_FAILURE;
   vos_lock_t *pLock = (vos_lock_t *)hSpinLock;
   VOS_STATUS vosStatus;
   
   vosStatus = vos_lock_acquire( pLock );
   if ( VOS_IS_STATUS_SUCCESS( vosStatus ) )
   {
      //                                                   
      //               
      halStatus = eHAL_STATUS_SUCCESS;
   }
   
   return( halStatus );
}   
   




eHalStatus palSpinLockGive( tHddHandle hHdd, tPalSpinLockHandle hSpinLock )
{
   eHalStatus halStatus = eHAL_STATUS_FAILURE;
   vos_lock_t *pLock = (vos_lock_t *)hSpinLock;
   VOS_STATUS vosStatus;
   
   vosStatus = vos_lock_release( pLock );
   if ( VOS_IS_STATUS_SUCCESS( vosStatus ) )
   {
      //                                                   
      //               
      halStatus = eHAL_STATUS_SUCCESS;
   }

   return( halStatus );
} 


//                                                                  
//                                            
eHalStatus palSendMBMessage(tHddHandle hHdd, void *pBuf)
{
   eHalStatus halStatus = eHAL_STATUS_FAILURE;
   tSirRetStatus sirStatus;
   v_CONTEXT_t vosContext;
   v_VOID_t *hHal;

   vosContext = vos_get_global_context( VOS_MODULE_ID_HDD, hHdd );
   if (NULL == vosContext)
   {
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                "%s: invalid vosContext", __func__);
   }
   else
   {
      hHal = vos_get_context( VOS_MODULE_ID_SME, vosContext );
      if (NULL == hHal)
      {
         VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                   "%s: invalid hHal", __func__);
      }
      else
      {
         sirStatus = uMacPostCtrlMsg( hHal, pBuf );
         if ( eSIR_SUCCESS == sirStatus )
         {
            halStatus = eHAL_STATUS_SUCCESS;
         }
      }
   }

   palFreeMemory( hHdd, pBuf );

   return( halStatus );
}
  


//                                      
//                       
//                                                                                              
eHalStatus palSemaphoreAlloc( tHddHandle hHdd, tPalSemaphoreHandle *pHandle, tANI_S32 count )
{
    (void)hHdd;
    (void)pHandle;
    (void)count;
    if(pHandle)
    {
        *pHandle = NULL;
    }

    return (eHAL_STATUS_SUCCESS);
}

eHalStatus palSemaphoreFree( tHddHandle hHdd, tPalSemaphoreHandle hSemaphore )
{
    (void)hHdd;
    (void)hSemaphore;

    return (eHAL_STATUS_SUCCESS);
}

eHalStatus palSemaphoreTake( tHddHandle hHdd, tPalSemaphoreHandle hSemaphore )
{
    (void)hHdd;
    (void)hSemaphore;

    return (eHAL_STATUS_SUCCESS);
}

eHalStatus palSemaphoreGive( tHddHandle hHdd, tPalSemaphoreHandle hSemaphore )
{
    (void)hHdd;
    (void)hSemaphore;

    return (eHAL_STATUS_SUCCESS);
}

eHalStatus palMutexAlloc( tHddHandle hHdd, tPalSemaphoreHandle *pHandle) 
{
    (void)hHdd;
    (void)pHandle;

    if(pHandle)
    {
        *pHandle = NULL;
    }
    return (eHAL_STATUS_SUCCESS);
}

eHalStatus palMutexAllocLocked( tHddHandle hHdd, tPalSemaphoreHandle *pHandle)
{
    (void)hHdd;
    (void)pHandle;

    if(pHandle)
    {
        *pHandle = NULL;
    }
    return (eHAL_STATUS_SUCCESS);
}


eAniBoolean pal_in_interrupt(void)
{
    return (eANI_BOOLEAN_FALSE);
}

void pal_local_bh_disable(void)
{
}

void pal_local_bh_enable(void)
{
}

