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

/*============================================================================
  FILE:         vos_nvitem.c
  OVERVIEW:     This source file contains definitions for vOS NV Item APIs
  DEPENDENCIES: NV, remote API client, WinCE REX
                Copyright (c) 2008 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*                                                                            
                         
                                                                            */
//                                                                       
//                         

/*                                                                            
                
                                                                            */
#include "vos_types.h"
#include "aniGlobal.h"
#include "vos_nvitem.h"
#include "vos_trace.h"
#include "vos_api.h"
#include "wlan_hdd_misc.h"
#include "vos_sched.h"
#include "wlan_hdd_main.h"
#include <net/cfg80211.h>
static char crda_alpha2[2] = {0, 0}; /*                                    */
static char run_time_alpha2[2] = {0, 0}; /*                                            */
static v_BOOL_t crda_regulatory_entry_valid = VOS_FALSE;
static v_BOOL_t crda_regulatory_run_time_entry_valid = VOS_FALSE;

/*                                                                            
                                         
                                                                            */
#define VALIDITY_BITMAP_NV_ID    NV_WLAN_VALIDITY_BITMAP_I
#define VALIDITY_BITMAP_SIZE     32
#define MAX_COUNTRY_COUNT        300
//                                                 
#define VOS_HARD_CODED_MAC    {0, 0x0a, 0xf5, 4, 5, 6}

#define DEFAULT_NV_VALIDITY_BITMAP 0xFFFFFFFF

/*                                                                            
                    
                                                                            */
//                                                              
//                                                                        
//                                                                      

//                                                             
typedef struct
{
   v_U8_t            regDomain;
   v_COUNTRYCODE_t   countryCode;
} CountryInfo_t;
//                                                             
typedef struct
{
   v_U16_t           countryCount;
   CountryInfo_t     countryInfo[MAX_COUNTRY_COUNT];
} CountryInfoTable_t;
/*                                                                            
                          
                                                                            */
/*                                                                            
                              
                                                                            */
//                             
//                                                
//                                             
static CountryInfoTable_t countryInfoTable =
{
    254,
    {
        { REGDOMAIN_FCC,     {'U', 'S'}},  //                                    
        { REGDOMAIN_ETSI,    {'A', 'D'}},  //       
        { REGDOMAIN_ETSI,    {'A', 'E'}},  //   
        { REGDOMAIN_N_AMER_EXC_FCC, {'A', 'F'}},  //           
        { REGDOMAIN_WORLD,   {'A', 'G'}},  //                   
        { REGDOMAIN_FCC,     {'A', 'I'}},  //        
        { REGDOMAIN_ETSI,    {'A', 'L'}},  //       
        { REGDOMAIN_N_AMER_EXC_FCC, {'A', 'M'}},  //       
        { REGDOMAIN_ETSI,    {'A', 'N'}},  //                    
        { REGDOMAIN_NO_5GHZ, {'A', 'O'}},  //      
        { REGDOMAIN_WORLD,   {'A', 'Q'}},  //          
        { REGDOMAIN_WORLD,   {'A', 'R'}},  //         
        { REGDOMAIN_FCC,     {'A', 'S'}},  //              
        { REGDOMAIN_ETSI,    {'A', 'T'}},  //       
        { REGDOMAIN_WORLD,   {'A', 'U'}},  //         
        { REGDOMAIN_ETSI,    {'A', 'W'}},  //     
        { REGDOMAIN_WORLD,   {'A', 'X'}},  //             
        { REGDOMAIN_N_AMER_EXC_FCC, {'A', 'Z'}},  //          
        { REGDOMAIN_ETSI,    {'B', 'A'}},  //                      
        { REGDOMAIN_APAC,    {'B', 'B'}},  //        
        { REGDOMAIN_HI_5GHZ, {'B', 'D'}},  //          
        { REGDOMAIN_ETSI,    {'B', 'E'}},  //       
        { REGDOMAIN_HI_5GHZ, {'B', 'F'}},  //            
        { REGDOMAIN_ETSI,    {'B', 'G'}},  //        
        { REGDOMAIN_APAC,    {'B', 'H'}},  //       
        { REGDOMAIN_NO_5GHZ, {'B', 'I'}},  //       
        { REGDOMAIN_NO_5GHZ, {'B', 'J'}},  //     
        { REGDOMAIN_FCC,     {'B', 'M'}},  //       
        { REGDOMAIN_APAC,    {'B', 'N'}},  //                 
        { REGDOMAIN_HI_5GHZ, {'B', 'O'}},  //       
        { REGDOMAIN_WORLD,   {'B', 'R'}},  //      
        { REGDOMAIN_APAC,    {'B', 'S'}},  //       
        { REGDOMAIN_NO_5GHZ, {'B', 'T'}},  //      
        { REGDOMAIN_WORLD,   {'B', 'V'}},  //             
        { REGDOMAIN_ETSI,    {'B', 'W'}},  //        
        { REGDOMAIN_ETSI,    {'B', 'Y'}},  //       
        { REGDOMAIN_HI_5GHZ, {'B', 'Z'}},  //      
        { REGDOMAIN_FCC,     {'C', 'A'}},  //      
        { REGDOMAIN_WORLD,   {'C', 'C'}},  //                       
        { REGDOMAIN_NO_5GHZ, {'C', 'D'}},  //                                     
        { REGDOMAIN_NO_5GHZ, {'C', 'F'}},  //                        
        { REGDOMAIN_NO_5GHZ, {'C', 'G'}},  //     
        { REGDOMAIN_ETSI,    {'C', 'H'}},  //           
        { REGDOMAIN_NO_5GHZ, {'C', 'I'}},  //             
        { REGDOMAIN_WORLD,   {'C', 'K'}},  //            
        { REGDOMAIN_APAC,    {'C', 'L'}},  //     
        { REGDOMAIN_NO_5GHZ, {'C', 'M'}},  //        
        { REGDOMAIN_APAC,    {'C', 'N'}},  //     
        { REGDOMAIN_APAC,    {'C', 'O'}},  //        
        { REGDOMAIN_APAC,    {'C', 'R'}},  //          
        { REGDOMAIN_NO_5GHZ, {'C', 'U'}},  //    
        { REGDOMAIN_ETSI,    {'C', 'V'}},  //          
        { REGDOMAIN_WORLD,   {'C', 'X'}},  //                
        { REGDOMAIN_ETSI,    {'C', 'Y'}},  //      
        { REGDOMAIN_ETSI,    {'C', 'Z'}},  //              
        { REGDOMAIN_ETSI,    {'D', 'E'}},  //       
        { REGDOMAIN_NO_5GHZ, {'D', 'J'}},  //        
        { REGDOMAIN_ETSI,    {'D', 'K'}},  //       
        { REGDOMAIN_WORLD,   {'D', 'M'}},  //        
        { REGDOMAIN_APAC,    {'D', 'O'}},  //                  
        { REGDOMAIN_ETSI,    {'D', 'Z'}},  //       
        { REGDOMAIN_APAC,    {'E', 'C'}},  //       
        { REGDOMAIN_ETSI,    {'E', 'E'}},  //       
        { REGDOMAIN_N_AMER_EXC_FCC, {'E', 'G'}},  //     
        { REGDOMAIN_WORLD,   {'E', 'H'}},  //              
        { REGDOMAIN_NO_5GHZ, {'E', 'R'}},  //       
        { REGDOMAIN_ETSI,    {'E', 'S'}},  //     
        { REGDOMAIN_ETSI,    {'E', 'T'}},  //        
        { REGDOMAIN_ETSI,    {'E', 'U'}},  //              
        { REGDOMAIN_ETSI,    {'F', 'I'}},  //       
        { REGDOMAIN_NO_5GHZ, {'F', 'J'}},  //    
        { REGDOMAIN_WORLD,   {'F', 'K'}},  //                           
        { REGDOMAIN_WORLD,   {'F', 'M'}},  //                               
        { REGDOMAIN_WORLD,   {'F', 'O'}},  //             
        { REGDOMAIN_ETSI,    {'F', 'R'}},  //      
        { REGDOMAIN_NO_5GHZ, {'G', 'A'}},  //     
        { REGDOMAIN_ETSI,    {'G', 'B'}},  //              
        { REGDOMAIN_WORLD,   {'G', 'D'}},  //       
        { REGDOMAIN_ETSI,    {'G', 'E'}},  //       
        { REGDOMAIN_ETSI,    {'G', 'F'}},  //             
        { REGDOMAIN_WORLD,   {'G', 'G'}},  //        
        { REGDOMAIN_WORLD,   {'G', 'H'}},  //     
        { REGDOMAIN_WORLD,   {'G', 'I'}},  //         
        { REGDOMAIN_ETSI,    {'G', 'L'}},  //         
        { REGDOMAIN_NO_5GHZ, {'G', 'M'}},  //      
        { REGDOMAIN_NO_5GHZ, {'G', 'N'}},  //      
        { REGDOMAIN_ETSI,    {'G', 'P'}},  //          
        { REGDOMAIN_NO_5GHZ, {'G', 'Q'}},  //                 
        { REGDOMAIN_ETSI,    {'G', 'R'}},  //      
        { REGDOMAIN_WORLD,   {'G', 'S'}},  //                                            
        { REGDOMAIN_APAC,    {'G', 'T'}},  //         
        { REGDOMAIN_FCC,     {'G', 'U'}},  //    
        { REGDOMAIN_NO_5GHZ, {'G', 'W'}},  //             
        { REGDOMAIN_HI_5GHZ, {'G', 'Y'}},  //      
        { REGDOMAIN_WORLD,   {'H', 'K'}},  //        
        { REGDOMAIN_WORLD,   {'H', 'M'}},  //                                 
        { REGDOMAIN_WORLD,   {'H', 'N'}},  //        
        { REGDOMAIN_ETSI,    {'H', 'R'}},  //       
        { REGDOMAIN_ETSI,    {'H', 'T'}},  //     
        { REGDOMAIN_ETSI,    {'H', 'U'}},  //       
        { REGDOMAIN_HI_5GHZ, {'I', 'D'}},  //         
        { REGDOMAIN_ETSI,    {'I', 'E'}},  //       
        { REGDOMAIN_N_AMER_EXC_FCC, {'I', 'L'}},  //      
        { REGDOMAIN_WORLD,   {'I', 'M'}},  //           
        { REGDOMAIN_APAC,    {'I', 'N'}},  //     
        { REGDOMAIN_WORLD,   {'I', 'O'}},  //                              
        { REGDOMAIN_NO_5GHZ, {'I', 'Q'}},  //    
        { REGDOMAIN_HI_5GHZ, {'I', 'R'}},  //                         
        { REGDOMAIN_ETSI,    {'I', 'S'}},  //       
        { REGDOMAIN_ETSI,    {'I', 'T'}},  //     
        { REGDOMAIN_JAPAN,   {'J', '1'}},  //                 
        { REGDOMAIN_JAPAN,   {'J', '2'}},  //                 
        { REGDOMAIN_JAPAN,   {'J', '3'}},  //                 
        { REGDOMAIN_JAPAN,   {'J', '4'}},  //                 
        { REGDOMAIN_JAPAN,   {'J', '5'}},  //                 
        { REGDOMAIN_WORLD,   {'J', 'E'}},  //      
        { REGDOMAIN_WORLD,   {'J', 'M'}},  //       
        { REGDOMAIN_APAC,    {'J', 'O'}},  //      
        { REGDOMAIN_JAPAN,   {'J', 'P'}},  //     
        { REGDOMAIN_KOREA,   {'K', '1'}},  //                 
        { REGDOMAIN_KOREA,   {'K', '2'}},  //                 
        { REGDOMAIN_KOREA,   {'K', '3'}},  //                 
        { REGDOMAIN_KOREA,   {'K', '4'}},  //                 
        { REGDOMAIN_APAC,    {'K', 'E'}},  //     
        { REGDOMAIN_NO_5GHZ, {'K', 'G'}},  //          
        { REGDOMAIN_ETSI,    {'K', 'H'}},  //        
        { REGDOMAIN_WORLD,   {'K', 'I'}},  //        
        { REGDOMAIN_NO_5GHZ, {'K', 'M'}},  //       
        { REGDOMAIN_WORLD,   {'K', 'N'}},  //                     
        { REGDOMAIN_WORLD,   {'K', 'P'}},  //                                      
        { REGDOMAIN_KOREA,   {'K', 'R'}},  //                  
        { REGDOMAIN_N_AMER_EXC_FCC, {'K', 'W'}},  //      
        { REGDOMAIN_FCC,     {'K', 'Y'}},  //              
        { REGDOMAIN_WORLD,   {'K', 'Z'}},  //          
        { REGDOMAIN_WORLD,   {'L', 'A'}},  //                                
        { REGDOMAIN_WORLD,   {'L', 'B'}},  //       
        { REGDOMAIN_WORLD,   {'L', 'C'}},  //           
        { REGDOMAIN_ETSI,    {'L', 'I'}},  //             
        { REGDOMAIN_WORLD,   {'L', 'K'}},  //         
        { REGDOMAIN_WORLD,   {'L', 'R'}},  //       
        { REGDOMAIN_ETSI,    {'L', 'S'}},  //       
        { REGDOMAIN_ETSI,    {'L', 'T'}},  //         
        { REGDOMAIN_ETSI,    {'L', 'U'}},  //          
        { REGDOMAIN_ETSI,    {'L', 'V'}},  //      
        { REGDOMAIN_NO_5GHZ, {'L', 'Y'}},  //                      
        { REGDOMAIN_APAC,    {'M', 'A'}},  //       
        { REGDOMAIN_ETSI,    {'M', 'C'}},  //      
        { REGDOMAIN_ETSI,    {'M', 'D'}},  //                    
        { REGDOMAIN_ETSI,    {'M', 'E'}},  //          
        { REGDOMAIN_NO_5GHZ, {'M', 'G'}},  //          
        { REGDOMAIN_WORLD,   {'M', 'H'}},  //                
        { REGDOMAIN_ETSI,    {'M', 'K'}},  //                                          
        { REGDOMAIN_NO_5GHZ, {'M', 'L'}},  //    
        { REGDOMAIN_WORLD,   {'M', 'M'}},  //       
        { REGDOMAIN_WORLD,   {'M', 'N'}},  //        
        { REGDOMAIN_APAC,    {'M', 'O'}},  //     
        { REGDOMAIN_FCC,     {'M', 'P'}},  //                        
        { REGDOMAIN_ETSI,    {'M', 'Q'}},  //          
        { REGDOMAIN_ETSI,    {'M', 'R'}},  //          
        { REGDOMAIN_ETSI,    {'M', 'S'}},  //          
        { REGDOMAIN_ETSI,    {'M', 'T'}},  //     
        { REGDOMAIN_ETSI,    {'M', 'U'}},  //         
        { REGDOMAIN_APAC,    {'M', 'V'}},  //        
        { REGDOMAIN_HI_5GHZ, {'M', 'W'}},  //      
        { REGDOMAIN_APAC,    {'M', 'X'}},  //      
        { REGDOMAIN_APAC,    {'M', 'Y'}},  //        
        { REGDOMAIN_WORLD,   {'M', 'Z'}},  //          
        { REGDOMAIN_WORLD,   {'N', 'A'}},  //       
        { REGDOMAIN_NO_5GHZ, {'N', 'C'}},  //             
        { REGDOMAIN_WORLD,   {'N', 'E'}},  //     
        { REGDOMAIN_WORLD,   {'N', 'F'}},  //              
        { REGDOMAIN_WORLD,   {'N', 'G'}},  //       
        { REGDOMAIN_WORLD,   {'N', 'I'}},  //         
        { REGDOMAIN_ETSI,    {'N', 'L'}},  //           
        { REGDOMAIN_ETSI,    {'N', 'O'}},  //      
        { REGDOMAIN_APAC,    {'N', 'P'}},  //     
        { REGDOMAIN_NO_5GHZ, {'N', 'R'}},  //     
        { REGDOMAIN_WORLD,   {'N', 'U'}},  //    
        { REGDOMAIN_APAC,    {'N', 'Z'}},  //           
        { REGDOMAIN_ETSI,    {'O', 'M'}},  //    
        { REGDOMAIN_APAC,    {'P', 'A'}},  //      
        { REGDOMAIN_WORLD,   {'P', 'E'}},  //    
        { REGDOMAIN_ETSI,    {'P', 'F'}},  //                
        { REGDOMAIN_WORLD,   {'P', 'G'}},  //                
        { REGDOMAIN_WORLD,   {'P', 'H'}},  //           
        { REGDOMAIN_HI_5GHZ, {'P', 'K'}},  //        
        { REGDOMAIN_ETSI,    {'P', 'L'}},  //      
        { REGDOMAIN_WORLD,   {'P', 'M'}},  //                         
        { REGDOMAIN_WORLD,   {'P', 'N'}},  //             
        { REGDOMAIN_FCC,     {'P', 'R'}},  //           
        { REGDOMAIN_WORLD,   {'P', 'S'}},  //                               
        { REGDOMAIN_ETSI,    {'P', 'T'}},  //        
        { REGDOMAIN_WORLD,   {'P', 'W'}},  //     
        { REGDOMAIN_WORLD,   {'P', 'Y'}},  //        
        { REGDOMAIN_HI_5GHZ, {'Q', 'A'}},  //     
        { REGDOMAIN_ETSI,    {'R', 'E'}},  //       
        { REGDOMAIN_ETSI,    {'R', 'O'}},  //       
        { REGDOMAIN_ETSI,    {'R', 'S'}},  //      
        { REGDOMAIN_APAC,    {'R', 'U'}},  //      
        { REGDOMAIN_WORLD,   {'R', 'W'}},  //      
        { REGDOMAIN_WORLD,   {'S', 'A'}},  //            
        { REGDOMAIN_NO_5GHZ, {'S', 'B'}},  //               
        { REGDOMAIN_NO_5GHZ, {'S', 'C'}},  //          
        { REGDOMAIN_WORLD,   {'S', 'D'}},  //     
        { REGDOMAIN_ETSI,    {'S', 'E'}},  //      
        { REGDOMAIN_APAC,    {'S', 'G'}},  //         
        { REGDOMAIN_WORLD,   {'S', 'H'}},  //            
        { REGDOMAIN_ETSI,    {'S', 'I'}},  //         
        { REGDOMAIN_WORLD,   {'S', 'J'}},  //                      
        { REGDOMAIN_ETSI,    {'S', 'K'}},  //        
        { REGDOMAIN_WORLD,   {'S', 'L'}},  //            
        { REGDOMAIN_ETSI,    {'S', 'M'}},  //          
        { REGDOMAIN_ETSI,    {'S', 'N'}},  //       
        { REGDOMAIN_NO_5GHZ, {'S', 'O'}},  //       
        { REGDOMAIN_NO_5GHZ, {'S', 'R'}},  //        
        { REGDOMAIN_WORLD,   {'S', 'T'}},  //                     
        { REGDOMAIN_APAC,    {'S', 'V'}},  //           
        { REGDOMAIN_NO_5GHZ, {'S', 'Y'}},  //                    
        { REGDOMAIN_NO_5GHZ, {'S', 'Z'}},  //         
        { REGDOMAIN_ETSI,    {'T', 'C'}},  //                        
        { REGDOMAIN_NO_5GHZ, {'T', 'D'}},  //    
        { REGDOMAIN_ETSI,    {'T', 'F'}},  //                           
        { REGDOMAIN_NO_5GHZ, {'T', 'G'}},  //    
        { REGDOMAIN_WORLD,   {'T', 'H'}},  //        
        { REGDOMAIN_NO_5GHZ, {'T', 'J'}},  //          
        { REGDOMAIN_WORLD,   {'T', 'K'}},  //       
        { REGDOMAIN_WORLD,   {'T', 'L'}},  //           
        { REGDOMAIN_NO_5GHZ, {'T', 'M'}},  //            
        { REGDOMAIN_N_AMER_EXC_FCC, {'T', 'N'}},  //       
        { REGDOMAIN_NO_5GHZ, {'T', 'O'}},  //     
        { REGDOMAIN_ETSI,    {'T', 'R'}},  //      
        { REGDOMAIN_WORLD,   {'T', 'T'}},  //                   
        { REGDOMAIN_NO_5GHZ, {'T', 'V'}},  //      
        { REGDOMAIN_FCC,     {'T', 'W'}},  //                         
        { REGDOMAIN_HI_5GHZ, {'T', 'Z'}},  //                            
        { REGDOMAIN_WORLD,   {'U', 'A'}},  //       
        { REGDOMAIN_KOREA,   {'U', 'G'}},  //      
        { REGDOMAIN_FCC,     {'U', 'M'}},  //                                    
        { REGDOMAIN_WORLD,   {'U', 'Y'}},  //       
        { REGDOMAIN_FCC,     {'U', 'Z'}},  //          
        { REGDOMAIN_ETSI,    {'V', 'A'}},  //                             
        { REGDOMAIN_WORLD,   {'V', 'C'}},  //                                
        { REGDOMAIN_HI_5GHZ, {'V', 'E'}},  //         
        { REGDOMAIN_ETSI,    {'V', 'G'}},  //                       
        { REGDOMAIN_FCC,     {'V', 'I'}},  //                  
        { REGDOMAIN_FCC,     {'V', 'N'}},  //        
        { REGDOMAIN_NO_5GHZ, {'V', 'U'}},  //       
        { REGDOMAIN_WORLD,   {'W', 'F'}},  //                 
        { REGDOMAIN_N_AMER_EXC_FCC, {'W', 'S'}},  //     
        { REGDOMAIN_NO_5GHZ, {'Y', 'E'}},  //     
        { REGDOMAIN_ETSI,    {'Y', 'T'}},  //       
        { REGDOMAIN_WORLD,   {'Z', 'A'}},  //            
        { REGDOMAIN_APAC,    {'Z', 'M'}},  //      
        { REGDOMAIN_ETSI,    {'Z', 'W'}},  //        
    }
};
typedef struct nvEFSTable_s
{
   v_U32_t    nvValidityBitmap;
   sHalNv     halnv;
} nvEFSTable_t;
nvEFSTable_t *gnvEFSTable;
/*                                           */ 
static nvEFSTable_t *pnvEFSTable;

const tRfChannelProps rfChannels[NUM_RF_CHANNELS] =
{
    //                  
    //                 
    { 2412, 1  , RF_SUBBAND_2_4_GHZ},        //          
    { 2417, 2  , RF_SUBBAND_2_4_GHZ},        //          
    { 2422, 3  , RF_SUBBAND_2_4_GHZ},        //          
    { 2427, 4  , RF_SUBBAND_2_4_GHZ},        //          
    { 2432, 5  , RF_SUBBAND_2_4_GHZ},        //          
    { 2437, 6  , RF_SUBBAND_2_4_GHZ},        //          
    { 2442, 7  , RF_SUBBAND_2_4_GHZ},        //          
    { 2447, 8  , RF_SUBBAND_2_4_GHZ},        //          
    { 2452, 9  , RF_SUBBAND_2_4_GHZ},        //          
    { 2457, 10 , RF_SUBBAND_2_4_GHZ},        //           
    { 2462, 11 , RF_SUBBAND_2_4_GHZ},        //           
    { 2467, 12 , RF_SUBBAND_2_4_GHZ},        //           
    { 2472, 13 , RF_SUBBAND_2_4_GHZ},        //           
    { 2484, 14 , RF_SUBBAND_2_4_GHZ},        //           
    { 4920, 240, RF_SUBBAND_4_9_GHZ},        //            
    { 4940, 244, RF_SUBBAND_4_9_GHZ},        //            
    { 4960, 248, RF_SUBBAND_4_9_GHZ},        //            
    { 4980, 252, RF_SUBBAND_4_9_GHZ},        //            
    { 5040, 208, RF_SUBBAND_4_9_GHZ},        //            
    { 5060, 212, RF_SUBBAND_4_9_GHZ},        //            
    { 5080, 216, RF_SUBBAND_4_9_GHZ},        //            
    { 5180, 36 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5200, 40 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5220, 44 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5240, 48 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5260, 52 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5280, 56 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5300, 60 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5320, 64 , RF_SUBBAND_5_LOW_GHZ},      //           
    { 5500, 100, RF_SUBBAND_5_MID_GHZ},      //            
    { 5520, 104, RF_SUBBAND_5_MID_GHZ},      //            
    { 5540, 108, RF_SUBBAND_5_MID_GHZ},      //            
    { 5560, 112, RF_SUBBAND_5_MID_GHZ},      //            
    { 5580, 116, RF_SUBBAND_5_MID_GHZ},      //            
    { 5600, 120, RF_SUBBAND_5_MID_GHZ},      //            
    { 5620, 124, RF_SUBBAND_5_MID_GHZ},      //            
    { 5640, 128, RF_SUBBAND_5_MID_GHZ},      //            
    { 5660, 132, RF_SUBBAND_5_MID_GHZ},      //            
    { 5680, 136, RF_SUBBAND_5_MID_GHZ},      //            
    { 5700, 140, RF_SUBBAND_5_MID_GHZ},      //            
    { 5745, 149, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 5765, 153, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 5785, 157, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 5805, 161, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 5825, 165, RF_SUBBAND_5_HIGH_GHZ},     //            
    { 2422, 3  , NUM_RF_SUBBANDS},           //               
    { 2427, 4  , NUM_RF_SUBBANDS},           //               
    { 2432, 5  , NUM_RF_SUBBANDS},           //               
    { 2437, 6  , NUM_RF_SUBBANDS},           //               
    { 2442, 7  , NUM_RF_SUBBANDS},           //               
    { 2447, 8  , NUM_RF_SUBBANDS},           //               
    { 2452, 9  , NUM_RF_SUBBANDS},           //               
    { 2457, 10 , NUM_RF_SUBBANDS},           //                
    { 2462, 11 , NUM_RF_SUBBANDS},           //                
    { 4930, 242, NUM_RF_SUBBANDS},           //                 
    { 4950, 246, NUM_RF_SUBBANDS},           //                 
    { 4970, 250, NUM_RF_SUBBANDS},           //                 
    { 5050, 210, NUM_RF_SUBBANDS},           //                 
    { 5070, 214, NUM_RF_SUBBANDS},           //                 
    { 5190, 38 , NUM_RF_SUBBANDS},           //                
    { 5210, 42 , NUM_RF_SUBBANDS},           //                
    { 5230, 46 , NUM_RF_SUBBANDS},           //                
    { 5250, 50 , NUM_RF_SUBBANDS},           //                
    { 5270, 54 , NUM_RF_SUBBANDS},           //                
    { 5290, 58 , NUM_RF_SUBBANDS},           //                
    { 5310, 62 , NUM_RF_SUBBANDS},           //                
    { 5510, 102, NUM_RF_SUBBANDS},           //                 
    { 5530, 106, NUM_RF_SUBBANDS},           //                 
    { 5550, 110, NUM_RF_SUBBANDS},           //                 
    { 5570, 114, NUM_RF_SUBBANDS},           //                 
    { 5590, 118, NUM_RF_SUBBANDS},           //                 
    { 5610, 122, NUM_RF_SUBBANDS},           //                 
    { 5630, 126, NUM_RF_SUBBANDS},           //                 
    { 5650, 130, NUM_RF_SUBBANDS},           //                 
    { 5670, 134, NUM_RF_SUBBANDS},           //                 
    { 5690, 138, NUM_RF_SUBBANDS},           //                 
    { 5755, 151, NUM_RF_SUBBANDS},           //                 
    { 5775, 155, NUM_RF_SUBBANDS},           //                 
    { 5795, 159, NUM_RF_SUBBANDS},           //                 
    { 5815, 163, NUM_RF_SUBBANDS},           //                 
};

extern const sHalNv nvDefaults;

const sRegulatoryChannel * regChannels = nvDefaults.tables.regDomains[0].channels;

/*                                                                            
                                         
                                                                            */
VOS_STATUS wlan_write_to_efs (v_U8_t *pData, v_U16_t data_len);
/*                                                                         
                                                 
                                                                       
                                                          
                                                                 
                                                
     
                                                                           */
VOS_STATUS vos_nv_init(void)
{
   return VOS_STATUS_SUCCESS;
}

VOS_STATUS vos_nv_open(void)
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    v_CONTEXT_t pVosContext= NULL;
    v_SIZE_t bufSize;
    v_SIZE_t nvReadBufSize;
    v_BOOL_t itemIsValid = VOS_FALSE;
    
    /*                       */
    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
    
    if(pVosContext == NULL)
    {
        return (eHAL_STATUS_FAILURE);
    }

    bufSize = sizeof(nvEFSTable_t);
    status = hdd_request_firmware(WLAN_NV_FILE,
                                  ((VosContextType*)(pVosContext))->pHDDContext,
                                  (v_VOID_t**)&gnvEFSTable, &nvReadBufSize);

    if ( (!VOS_IS_STATUS_SUCCESS( status )) || !gnvEFSTable)
    {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                   "%s: unable to download NV file %s",
                   __func__, WLAN_NV_FILE);
         return VOS_STATUS_E_RESOURCES;
    }

     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "INFO: NV binary file version=%d Driver default NV version=%d, continue...\n",
           gnvEFSTable->halnv.fields.nvVersion, WLAN_NV_VERSION);

     /*                                                    */
    {
        /*                                    */
        pnvEFSTable = (nvEFSTable_t *)vos_mem_malloc(sizeof(nvEFSTable_t));
        if (NULL == pnvEFSTable)
        {
            VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                      "%s : failed to allocate memory for NV", __func__);
            return VOS_STATUS_E_NOMEM;
        }

        /*                        */
        vos_mem_copy(&(pnvEFSTable->halnv),&nvDefaults,sizeof(sHalNv));

        /*               */
        if ( nvReadBufSize != bufSize)
        {
            pnvEFSTable->nvValidityBitmap = DEFAULT_NV_VALIDITY_BITMAP;
            VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                      "!!!WARNING: INVALID NV FILE, DRIVER IS USING DEFAULT CAL VALUES %d %d!!!",
                      nvReadBufSize, bufSize);
            return VOS_STATUS_SUCCESS;
        }

       /*                  */
       if (gnvEFSTable->halnv.fields.nvVersion != WLAN_NV_VERSION)
       {
           if ((WLAN_NV_VERSION == NV_VERSION_11N_11AC_FW_CONFIG) &&
               (gnvEFSTable->halnv.fields.nvVersion == NV_VERSION_11N_11AC_COUPER_TYPE))
           {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "!!!WARNING: Using Coupler Type field instead of Fw Config table,\n"
                     "Make sure that this is intented or may impact performance!!!\n");
           }
           else
           {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "!!!WARNING: NV binary file version doesn't match with Driver default NV version\n"
                     "Driver NV defaults will be used, may impact performance!!!\n");

               return VOS_STATUS_SUCCESS;
           }
       }

       pnvEFSTable->nvValidityBitmap = gnvEFSTable->nvValidityBitmap;
        /*                                                  */ 
        if (vos_nv_getValidity(VNV_FIELD_IMAGE, &itemIsValid) == 
           VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE) {

                if(vos_nv_read( VNV_FIELD_IMAGE, (v_VOID_t *)&pnvEFSTable->halnv.fields,
                   NULL, sizeof(sNvFields) ) != VOS_STATUS_SUCCESS)
                   goto error;
            }
        }

        if (vos_nv_getValidity(VNV_RATE_TO_POWER_TABLE, &itemIsValid) == 
             VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_RATE_TO_POWER_TABLE, 
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.pwrOptimum[0],
                  NULL, sizeof(tRateGroupPwr) * NUM_RF_SUBBANDS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_REGULARTORY_DOMAIN_TABLE, &itemIsValid) == 
               VOS_STATUS_SUCCESS)
        {
    
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_REGULARTORY_DOMAIN_TABLE,
                (v_VOID_t *)&pnvEFSTable->halnv.tables.regDomains[0],
                 NULL, sizeof(sRegulatoryDomains) * NUM_REG_DOMAINS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_DEFAULT_LOCATION, &itemIsValid) == 
            VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_DEFAULT_LOCATION,
                (v_VOID_t *)&pnvEFSTable->halnv.tables.defaultCountryTable,
                NULL, sizeof(sDefaultCountry) ) !=  VOS_STATUS_SUCCESS)
                    goto error;
            }
        }
    
        if (vos_nv_getValidity(VNV_TPC_POWER_TABLE, &itemIsValid) == 
            VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_TPC_POWER_TABLE, 
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.plutCharacterized[0],
                  NULL, sizeof(tTpcPowerTable) * NUM_RF_CHANNELS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }
    
        if (vos_nv_getValidity(VNV_TPC_PDADC_OFFSETS, &itemIsValid) == 
            VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_TPC_PDADC_OFFSETS,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.plutPdadcOffset[0],
                  NULL, sizeof(tANI_U16) * NUM_RF_CHANNELS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }
        if (vos_nv_getValidity(VNV_RSSI_CHANNEL_OFFSETS, &itemIsValid) == 
           VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_RSSI_CHANNEL_OFFSETS,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.rssiChanOffsets[0],
                  NULL, sizeof(sRssiChannelOffsets) * 2 ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }
    
        if (vos_nv_getValidity(VNV_HW_CAL_VALUES, &itemIsValid) == 
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_HW_CAL_VALUES, (v_VOID_t *)&pnvEFSTable->halnv
    .tables.hwCalValues, NULL, sizeof(sHwCalValues) ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_FW_CONFIG, &itemIsValid) == 
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_FW_CONFIG, (v_VOID_t *)&pnvEFSTable->halnv
    .tables.fwConfig, NULL, sizeof(sFwConfig) ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_ANTENNA_PATH_LOSS, &itemIsValid) == 
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_ANTENNA_PATH_LOSS,
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.antennaPathLoss[0], NULL, 
                sizeof(tANI_S16)*NUM_RF_CHANNELS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }
        if (vos_nv_getValidity(VNV_PACKET_TYPE_POWER_LIMITS, &itemIsValid) == 
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_PACKET_TYPE_POWER_LIMITS, 
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.pktTypePwrLimits[0], NULL, 
                sizeof(tANI_S16)*NUM_802_11_MODES*NUM_RF_CHANNELS ) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_OFDM_CMD_PWR_OFFSET, &itemIsValid) == 
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
                if(vos_nv_read( VNV_OFDM_CMD_PWR_OFFSET, 
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.ofdmCmdPwrOffset, NULL, 
                                sizeof(sOfdmCmdPwrOffset)) != VOS_STATUS_SUCCESS)
                    goto error;
            }
        }

        if (vos_nv_getValidity(VNV_TX_BB_FILTER_MODE, &itemIsValid) == 
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
               if(vos_nv_read(VNV_TX_BB_FILTER_MODE, 
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.txbbFilterMode, NULL, 
                sizeof(sTxBbFilterMode)) != VOS_STATUS_SUCCESS)
                   goto error;
            }
        }
        if (vos_nv_getValidity(VNV_TABLE_VIRTUAL_RATE, &itemIsValid) == 
         VOS_STATUS_SUCCESS)
        {
            if (itemIsValid == VOS_TRUE)
            {
               if(vos_nv_read(VNV_TABLE_VIRTUAL_RATE, 
                  (v_VOID_t *)&pnvEFSTable->halnv.tables.pwrOptimum_virtualRate, NULL, 
                sizeof(gnvEFSTable->halnv.tables.pwrOptimum_virtualRate)) != VOS_STATUS_SUCCESS)
                   goto error;
            }
        }
    }

    return VOS_STATUS_SUCCESS;
error:
    vos_mem_free(pnvEFSTable);
    return eHAL_STATUS_FAILURE ;
}

VOS_STATUS vos_nv_close(void)
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    v_CONTEXT_t pVosContext= NULL;
         /*                       */
    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
    status = hdd_release_firmware(WLAN_NV_FILE, ((VosContextType*)(pVosContext))->pHDDContext);
    if ( !VOS_IS_STATUS_SUCCESS( status ))
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                         "%s : vos_open failed\n",__func__);
        return VOS_STATUS_E_FAILURE;
    }
    vos_mem_free(pnvEFSTable);
    gnvEFSTable=NULL;
    return VOS_STATUS_SUCCESS;
}
/*                                                                         
                                                                            
                                  
                                                                              
                                                                       
                          
                                                   
                                   
                                                                               
                                                    
                                                          
                                                                          
     
                                                                           */
VOS_STATUS vos_nv_getRegDomainFromCountryCode( v_REGDOMAIN_t *pRegDomain,
      const v_COUNTRYCODE_t countryCode )
{
   int i;
   v_CONTEXT_t pVosContext = NULL;
   hdd_context_t *pHddCtx = NULL;
   struct wiphy *wiphy = NULL;
   //              
   if (NULL == pRegDomain)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            ("Invalid reg domain pointer\r\n") );
      return VOS_STATUS_E_FAULT;
   }
   *pRegDomain = REGDOMAIN_COUNT;

   if (NULL == countryCode)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            ("Country code array is NULL\r\n") );
      return VOS_STATUS_E_FAULT;
   }
   if (0 == countryInfoTable.countryCount)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            ("Reg domain table is empty\r\n") );
      return VOS_STATUS_E_EMPTY;
   }
   /*                                                           
                                      
                                                                
                                              
                                                      */
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
          "vos_nv_getRegDomainFromCountryCode %c%c\n",
          countryCode[0], countryCode[1]);

   if (crda_regulatory_entry_valid == VOS_TRUE)
   {
       if (crda_alpha2[0]==countryCode[0] && crda_alpha2[1]==countryCode[1])
       {
          *pRegDomain = NUM_REG_DOMAINS-1;
              VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
              "vos_nv_getRegDomainFromCountryCode return crda init entry\n");
          return VOS_STATUS_SUCCESS;
       }
       if (run_time_alpha2[0]==countryCode[0] &&
           run_time_alpha2[1]==countryCode[1] &&
           crda_regulatory_run_time_entry_valid == VOS_TRUE)
       {
          *pRegDomain = NUM_REG_DOMAINS-2;
              VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
              "vos_nv_getRegDomainFromCountryCode return crda none-default country entry\n");
           return VOS_STATUS_SUCCESS;
       }
       else
       {
           crda_regulatory_run_time_entry_valid = VOS_FALSE;
           pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
           if (NULL != pVosContext)
               pHddCtx = vos_get_context(VOS_MODULE_ID_HDD, pVosContext);
           else
               return VOS_STATUS_E_EXISTS;
           if (NULL == pHddCtx)
           {
              VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    ("Invalid pHddCtx pointer\r\n") );
              return VOS_STATUS_E_FAULT;
           }

           wiphy = pHddCtx->wiphy;
           init_completion(&pHddCtx->driver_crda_req);
           regulatory_hint(wiphy, countryCode);
           wait_for_completion_interruptible_timeout(&pHddCtx->driver_crda_req,
               CRDA_WAIT_TIME);
           if (crda_regulatory_run_time_entry_valid == VOS_TRUE)
           {
              VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
                 "vos_nv_getRegDomainFromCountryCode return crda new none-default country entry\n");
               return VOS_STATUS_SUCCESS;
           }
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "vos_nv_getRegDomainFromCountryCode failed to get crda new none-default country entry\n");
           return VOS_STATUS_E_EXISTS;
       }
   }

   //                                                                      
   //         
   for (i = 0; i < countryInfoTable.countryCount &&
         REGDOMAIN_COUNT == *pRegDomain; i++)
   {
      if (memcmp(countryCode, countryInfoTable.countryInfo[i].countryCode,
               VOS_COUNTRY_CODE_LEN) == 0)
      {
         //                      
         *pRegDomain = countryInfoTable.countryInfo[i].regDomain;
      }
   }
   if (REGDOMAIN_COUNT != *pRegDomain)
   {
      return VOS_STATUS_SUCCESS;
   }
   else
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_WARN,
            ("country code is not found\r\n"));
      return VOS_STATUS_E_EXISTS;
   }
}
/*                                                                         
                                                                     
               
                                                                       
                                                    
                                                                      
                                                                        
                                                                          
                                                
                                                                 
                                                                             
                                                                     
                                                                           
                                                                   
                                         
     
                                                                           */
VOS_STATUS vos_nv_getSupportedCountryCode( v_BYTE_t *pBuffer, v_SIZE_t *pBufferSize,
      v_SIZE_t paddingSize )
{
   v_SIZE_t providedBufferSize = *pBufferSize;
   int i;
   //                                                   
   *pBufferSize = countryInfoTable.countryCount * (VOS_COUNTRY_CODE_LEN + paddingSize );
   if ( NULL == pBuffer || providedBufferSize < *pBufferSize )
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
            ("Insufficient memory for country code list\r\n"));
      return VOS_STATUS_E_NOMEM;
   }
   for (i = 0; i < countryInfoTable.countryCount; i++)
   {
      memcpy( pBuffer, countryInfoTable.countryInfo[i].countryCode, VOS_COUNTRY_CODE_LEN );
      pBuffer += (VOS_COUNTRY_CODE_LEN + paddingSize );
   }
   return VOS_STATUS_SUCCESS;
}
/*                                                                         
                                                                  
                                          
                                         
     
                                                                           */
VOS_STATUS vos_nv_readTxAntennaCount( v_U8_t *pTxAntennaCount )
{
   sNvFields fieldImage;
   VOS_STATUS status;
   status = vos_nv_read( VNV_FIELD_IMAGE, &fieldImage, NULL,
         sizeof(fieldImage) );
   if (VOS_STATUS_SUCCESS == status)
   {
      *pTxAntennaCount = fieldImage.numOfTxChains;
   }
   return status;
}
/*                                                                         
                                                                  
                                          
                                         
     
                                                                           */
VOS_STATUS vos_nv_readRxAntennaCount( v_U8_t *pRxAntennaCount )
{
   sNvFields fieldImage;
   VOS_STATUS status;
   status = vos_nv_read( VNV_FIELD_IMAGE, &fieldImage, NULL,
         sizeof(fieldImage) );
   if (VOS_STATUS_SUCCESS == status)
   {
      *pRxAntennaCount = fieldImage.numOfRxChains;
   }
   return status;
}

/*                                                                         
                                                         
                                  
                                         
     
                                                                           */
VOS_STATUS vos_nv_readMacAddress( v_MAC_ADDRESS_t pMacAddress )
{
   sNvFields fieldImage;
   VOS_STATUS status;
   status = vos_nv_read( VNV_FIELD_IMAGE, &fieldImage, NULL,
         sizeof(fieldImage) );
   if (VOS_STATUS_SUCCESS == status)
   {
      memcpy( pMacAddress, fieldImage.macAddr, VOS_MAC_ADDRESS_LEN );
   }
   else
   {
      //                                                          
      const v_U8_t macAddr[VOS_MAC_ADDRESS_LEN] = VOS_HARD_CODED_MAC;
      memcpy( pMacAddress, macAddr, VOS_MAC_ADDRESS_LEN );
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_WARN,
          " fail to get MAC address from NV, hardcoded to %02X-%02X-%02X-%02X-%02X%02X",
          macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
      status = VOS_STATUS_SUCCESS;
   }
   return status;
}

/*                                                                         

                                                                         

                                  
                                                                     

                                         

     

                                                                           */
VOS_STATUS vos_nv_readMultiMacAddress( v_U8_t *pMacAddress,
                                              v_U8_t  macCount )
{
   sNvFields   fieldImage;
   VOS_STATUS  status;
   v_U8_t      countLoop;
   v_U8_t     *pNVMacAddress;

   if((0 == macCount) || (VOS_MAX_CONCURRENCY_PERSONA < macCount) ||
      (NULL == pMacAddress))
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
          " Invalid Parameter from NV Client macCount %d, pMacAddress %p",
          macCount, pMacAddress);
   }

   status = vos_nv_read( VNV_FIELD_IMAGE, &fieldImage, NULL,
                         sizeof(fieldImage) );
   if (VOS_STATUS_SUCCESS == status)
   {
      pNVMacAddress = fieldImage.macAddr;
      for(countLoop = 0; countLoop < macCount; countLoop++)
      {
         vos_mem_copy(pMacAddress + (countLoop * VOS_MAC_ADDRESS_LEN),
                      pNVMacAddress + (countLoop * VOS_MAC_ADDRESS_LEN),
                      VOS_MAC_ADDRESS_LEN);
      }
   }
   else
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "vos_nv_readMultiMacAddress Get NV Field Fail");
   }

   return status;
}

/*                                                                         
                                                               
                                                                        
                                              
                                                                           
                                                                
                                   
                                                                   
                                                           
                                                               
                                              
     
                                                                           */

VOS_STATUS vos_nv_setValidity( VNV_TYPE type, v_BOOL_t itemIsValid )
{
   v_U32_t lastNvValidityBitmap;
   v_U32_t newNvValidityBitmap;
   VOS_STATUS status = VOS_STATUS_SUCCESS;

   //                                      
   if (VNV_TYPE_COUNT <= type)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("%s: invalid type=%d"), __func__, type );
      return VOS_STATUS_E_INVAL;
   }
   //                         
   lastNvValidityBitmap = gnvEFSTable->nvValidityBitmap;
   //                           
   if (itemIsValid)
   {
       newNvValidityBitmap = lastNvValidityBitmap | (1 << type);
              //                                               
       if (newNvValidityBitmap != lastNvValidityBitmap)
       {
           gnvEFSTable->nvValidityBitmap = newNvValidityBitmap;
       }
   }
   else
   {
       newNvValidityBitmap = lastNvValidityBitmap & (~(1 << type));
       if (newNvValidityBitmap != lastNvValidityBitmap)
       {
           gnvEFSTable->nvValidityBitmap = newNvValidityBitmap;
           status = wlan_write_to_efs((v_U8_t*)gnvEFSTable,sizeof(nvEFSTable_t));
           if (! VOS_IS_STATUS_SUCCESS(status)) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, ("vos_nv_write_to_efs failed!!!\r\n"));
               status = VOS_STATUS_E_FAULT;
           }
       }
   }

   return status;
}
/*                                                                         
                                                               
                                                                    
                                              
                                                                           
                                                                
                                   
                                                                         
                               
                                                                  
                                                               
                                              
     
                                                                           */
VOS_STATUS vos_nv_getValidity( VNV_TYPE type, v_BOOL_t *pItemIsValid )
{
   v_U32_t nvValidityBitmap = gnvEFSTable->nvValidityBitmap;
   //                                      
   if (VNV_TYPE_COUNT <= type)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("%s: invalid type=%d"), __func__, type );
      return VOS_STATUS_E_INVAL;
   }
   *pItemIsValid = (v_BOOL_t)((nvValidityBitmap >> type) & 1);
   return VOS_STATUS_SUCCESS;
}
/*                                                                         
                                                           
                                                                           
                                                                      
                                                         
                                                                       
                                                          
                                                            
                             
                                       
                                        
                                         
                                                           
                                                               
                                                          
                                                      
                                              
     
                                                                           */
VOS_STATUS vos_nv_read( VNV_TYPE type, v_VOID_t *outputVoidBuffer,
      v_VOID_t *defaultBuffer, v_SIZE_t bufferSize )
{
    VOS_STATUS status             = VOS_STATUS_SUCCESS;
    v_SIZE_t itemSize;
    v_BOOL_t itemIsValid = VOS_TRUE;

    //             
    if (VNV_TYPE_COUNT <= type)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("%s: invalid type=%d"), __func__, type );
       return VOS_STATUS_E_INVAL;
    }
    if (NULL == outputVoidBuffer)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
             ("Buffer provided is NULL\r\n") );
       return VOS_STATUS_E_FAULT;
    }
    if (0 == bufferSize)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
             ("NV type=%d is invalid\r\n"), type );
       return VOS_STATUS_E_INVAL;
    }
    //                                    
    status = vos_nv_getValidity( type, &itemIsValid );
   if (!itemIsValid)
   {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_WARN,
            "NV type=%d does not have valid data\r\n", type );
       return VOS_STATUS_E_EMPTY;
   }
   switch(type)
   {
       case VNV_FIELD_IMAGE:
           itemSize = sizeof(gnvEFSTable->halnv.fields);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.fields,bufferSize);
           }
           break;
       case VNV_RATE_TO_POWER_TABLE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.pwrOptimum);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.pwrOptimum[0],bufferSize);
           }
           break;
       case VNV_REGULARTORY_DOMAIN_TABLE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.regDomains);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.regDomains[0],bufferSize);
           }
           break;
       case VNV_DEFAULT_LOCATION:
           itemSize = sizeof(gnvEFSTable->halnv.tables.defaultCountryTable);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.defaultCountryTable,bufferSize);
           }
           break;
       case VNV_TPC_POWER_TABLE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.plutCharacterized);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.plutCharacterized[0],bufferSize);
           }
           break;
       case VNV_TPC_PDADC_OFFSETS:
           itemSize = sizeof(gnvEFSTable->halnv.tables.plutPdadcOffset);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.plutPdadcOffset[0],bufferSize);
           }
           break;
       case VNV_RSSI_CHANNEL_OFFSETS:

           itemSize = sizeof(gnvEFSTable->halnv.tables.rssiChanOffsets);

           if(bufferSize != itemSize) {

               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.rssiChanOffsets[0],bufferSize);
           }
           break;
       case VNV_HW_CAL_VALUES:

           itemSize = sizeof(gnvEFSTable->halnv.tables.hwCalValues);

           if(bufferSize != itemSize) {

               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.hwCalValues,bufferSize);
           }
           break;
       case VNV_FW_CONFIG:
       
           itemSize = sizeof(gnvEFSTable->halnv.tables.fwConfig);
       
           if(bufferSize != itemSize) {
       
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.fwConfig,bufferSize);
           }
           break;
       case VNV_ANTENNA_PATH_LOSS:
           itemSize = sizeof(gnvEFSTable->halnv.tables.antennaPathLoss);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.antennaPathLoss[0],bufferSize);
           }
           break;
       case VNV_PACKET_TYPE_POWER_LIMITS:
           itemSize = sizeof(gnvEFSTable->halnv.tables.pktTypePwrLimits);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,gnvEFSTable->halnv.tables.pktTypePwrLimits,bufferSize);
           }
           break;
       case VNV_OFDM_CMD_PWR_OFFSET:
           itemSize = sizeof(gnvEFSTable->halnv.tables.ofdmCmdPwrOffset);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.ofdmCmdPwrOffset,bufferSize);
           }
           break;
       case VNV_TX_BB_FILTER_MODE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.txbbFilterMode);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.txbbFilterMode,bufferSize);
           }
           break;


       case VNV_TABLE_VIRTUAL_RATE:
           itemSize = sizeof(gnvEFSTable->halnv.tables.pwrOptimum_virtualRate);
           if(bufferSize != itemSize) {
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(outputVoidBuffer,&gnvEFSTable->halnv.tables.pwrOptimum_virtualRate,bufferSize);
           }
           break;

       default:
         break;
   }
   return status;
}

/*                                                                         
                                                                 
                                                                            
                                                            
                            
                                   
                                            
                                                           
                                                               
                                                           
                                                      
                                                
     
                                                                           */
VOS_STATUS vos_nv_write( VNV_TYPE type, v_VOID_t *inputVoidBuffer,
      v_SIZE_t bufferSize )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    v_SIZE_t itemSize;

    //             
    if (VNV_TYPE_COUNT <= type)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                  ("%s: invalid type=%d"), __func__, type );
       return VOS_STATUS_E_INVAL;
    }
    if (NULL == inputVoidBuffer)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
             ("Buffer provided is NULL\r\n") );
       return VOS_STATUS_E_FAULT;
    }
    if (0 == bufferSize)
    {
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
             ("NV type=%d is invalid\r\n"), type );
       return VOS_STATUS_E_INVAL;
    }
    switch(type)
    {
        case VNV_FIELD_IMAGE:
            itemSize = sizeof(gnvEFSTable->halnv.fields);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.fields,inputVoidBuffer,bufferSize);
            }
            break;
        case VNV_RATE_TO_POWER_TABLE:
            itemSize = sizeof(gnvEFSTable->halnv.tables.pwrOptimum);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.pwrOptimum[0],inputVoidBuffer,bufferSize);
            }
            break;
        case VNV_REGULARTORY_DOMAIN_TABLE:
            itemSize = sizeof(gnvEFSTable->halnv.tables.regDomains);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.regDomains[0],inputVoidBuffer,bufferSize);
            }
            break;
        case VNV_DEFAULT_LOCATION:
            itemSize = sizeof(gnvEFSTable->halnv.tables.defaultCountryTable);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.defaultCountryTable,inputVoidBuffer,bufferSize);
            }
            break;
        case VNV_TPC_POWER_TABLE:
            itemSize = sizeof(gnvEFSTable->halnv.tables.plutCharacterized);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.plutCharacterized[0],inputVoidBuffer,bufferSize);
            }
            break;
        case VNV_TPC_PDADC_OFFSETS:
            itemSize = sizeof(gnvEFSTable->halnv.tables.plutPdadcOffset);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.plutPdadcOffset[0],inputVoidBuffer,bufferSize);
            }
            break;
         case VNV_RSSI_CHANNEL_OFFSETS:

            itemSize = sizeof(gnvEFSTable->halnv.tables.rssiChanOffsets);

            if(bufferSize != itemSize) {

                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.rssiChanOffsets[0],inputVoidBuffer,bufferSize);
            }
            break;
         case VNV_HW_CAL_VALUES:

            itemSize = sizeof(gnvEFSTable->halnv.tables.hwCalValues);

            if(bufferSize != itemSize) {

                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.hwCalValues,inputVoidBuffer,bufferSize);
            }
            break;
        case VNV_FW_CONFIG:
        
           itemSize = sizeof(gnvEFSTable->halnv.tables.fwConfig);
        
           if(bufferSize != itemSize) {
        
               VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                 itemSize);
               status = VOS_STATUS_E_INVAL;
           }
           else {
               memcpy(&gnvEFSTable->halnv.tables.fwConfig,inputVoidBuffer,bufferSize);
           }
           break;
        case VNV_ANTENNA_PATH_LOSS:
            itemSize = sizeof(gnvEFSTable->halnv.tables.antennaPathLoss);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.antennaPathLoss[0],inputVoidBuffer,bufferSize);
            }
            break;

        case VNV_PACKET_TYPE_POWER_LIMITS:
            itemSize = sizeof(gnvEFSTable->halnv.tables.pktTypePwrLimits);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(gnvEFSTable->halnv.tables.pktTypePwrLimits,inputVoidBuffer,bufferSize);
            }
            break;

        case VNV_OFDM_CMD_PWR_OFFSET:
            itemSize = sizeof(gnvEFSTable->halnv.tables.ofdmCmdPwrOffset);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.ofdmCmdPwrOffset,inputVoidBuffer,bufferSize);
            }
            break;

        case VNV_TX_BB_FILTER_MODE:
            itemSize = sizeof(gnvEFSTable->halnv.tables.txbbFilterMode);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.txbbFilterMode,inputVoidBuffer,bufferSize);
            }
            break;
            

        case VNV_TABLE_VIRTUAL_RATE:
            itemSize = sizeof(gnvEFSTable->halnv.tables.pwrOptimum_virtualRate);
            if(bufferSize != itemSize) {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 ("type = %d buffer size=%d is less than data size=%d\r\n"),type, bufferSize,
                  itemSize);
                status = VOS_STATUS_E_INVAL;
            }
            else {
                memcpy(&gnvEFSTable->halnv.tables.pwrOptimum_virtualRate,inputVoidBuffer,bufferSize);
            }
            break;

        default:
          break;
    }
   if (VOS_STATUS_SUCCESS == status)
   {
      //                               
      status = vos_nv_setValidity( type, VOS_TRUE );
      if (! VOS_IS_STATUS_SUCCESS(status)) {
          VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, ("vos_nv_setValidity failed!!!\r\n"));
          status = VOS_STATUS_E_FAULT;
      }
      status = wlan_write_to_efs((v_U8_t*)gnvEFSTable,sizeof(nvEFSTable_t));

      if (! VOS_IS_STATUS_SUCCESS(status)) {
          VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, ("vos_nv_write_to_efs failed!!!\r\n"));
          status = VOS_STATUS_E_FAULT;
      }
   }
   return status;
}

/*                                                                         
                                                                          
                                                           
                                                 
                                                           
                                                 
                                                           
                                         
                                      
     
                                                                           */
VOS_STATUS vos_nv_getChannelListWithPower(tChannelListWithPower *channels20MHz /*                        */,
                                          tANI_U8 *num20MHzChannelsFound,
                                          tChannelListWithPower *channels40MHz /*                         */,
                                          tANI_U8 *num40MHzChannelsFound
                                          )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    int i, count;
    
    //                                                                              
    //                                          

    if( channels20MHz && num20MHzChannelsFound )
    {
        count = 0;
        for( i = 0; i <= RF_CHAN_14; i++ )
        {
            if( regChannels[i].enabled )
            {
                channels20MHz[count].chanId = rfChannels[i].channelNum;
                channels20MHz[count++].pwr  = regChannels[i].pwrLimit;
            }
        }
        for( i = RF_CHAN_36; i <= RF_CHAN_165; i++ )
        {
            if( regChannels[i].enabled )
            {
                channels20MHz[count].chanId = rfChannels[i].channelNum;
                channels20MHz[count++].pwr  = regChannels[i].pwrLimit;
            }
        }
        *num20MHzChannelsFound = (tANI_U8)count;
    }

    if( channels40MHz && num40MHzChannelsFound )
    {
        count = 0;
        //                                           
        for( i = RF_CHAN_BOND_3; i <= RF_CHAN_BOND_11; i++ )
        {
            
            if( regChannels[i].enabled )
            {
                channels40MHz[count].chanId = rfChannels[i].channelNum;
                channels40MHz[count++].pwr  = regChannels[i].pwrLimit;
            }
        }
        //                                         
        for( i = RF_CHAN_BOND_38; i <= RF_CHAN_BOND_163; i++ )
        {
            
            if( regChannels[i].enabled )
            {
                channels40MHz[count].chanId = rfChannels[i].channelNum;
                channels40MHz[count++].pwr  = regChannels[i].pwrLimit;
            }
        }
        *num40MHzChannelsFound = (tANI_U8)count;
    }
    return (status);
}

/*                                                                         
                                                                            
                                   
     
                                                                           */

v_REGDOMAIN_t vos_nv_getDefaultRegDomain( void )
{
    return countryInfoTable.countryInfo[0].regDomain;
}

/*                                                                         
                                                                       
                            
                                                 
                                                      
                                                 
                                                      
                                         
                                      
     
                                                                           */
VOS_STATUS vos_nv_getSupportedChannels( v_U8_t *p20MhzChannels, int *pNum20MhzChannels,
                                        v_U8_t *p40MhzChannels, int *pNum40MhzChannels)
{
    VOS_STATUS status = VOS_STATUS_E_INVAL;
    int i, count = 0;

    if( p20MhzChannels && pNum20MhzChannels )
    {
        if( *pNum20MhzChannels >= NUM_RF_CHANNELS )
        {
            for( i = 0; i <= RF_CHAN_14; i++ )
            {
                p20MhzChannels[count++] = rfChannels[i].channelNum;
            }
            for( i = RF_CHAN_36; i <= RF_CHAN_165; i++ )
            {
                p20MhzChannels[count++] = rfChannels[i].channelNum;
            }
            status = VOS_STATUS_SUCCESS;
        }
        *pNum20MhzChannels = count;
    }

    return (status);
}

/*                                                                         
                                                                            
                                                                          
                                         
     
                                                                           */
VOS_STATUS vos_nv_readDefaultCountryTable( uNvTables *tableData )
{
   
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   memcpy(&tableData->defaultCountryTable, &pnvEFSTable->halnv.tables.defaultCountryTable, sizeof(sDefaultCountry));
   pr_info("DefaultCountry is %c%c\n",
            tableData->defaultCountryTable.countryCode[0],
            tableData->defaultCountryTable.countryCode[1]);
   return status;
}

/*                                                                         
                            
                                                
                                      
                                         
     
                                                                           */
VOS_STATUS vos_nv_getNVBuffer(v_VOID_t **pNvBuffer,v_SIZE_t *pSize)
{

   /*                                */
   *pNvBuffer = (v_VOID_t *)(&pnvEFSTable->halnv);
   *pSize = sizeof(sHalNv);

   return VOS_STATUS_SUCCESS;
}

/*                                                                         
                               
                                                         
                                           
                                         
     
                                                                           */
VOS_STATUS vos_nv_setRegDomain(void * clientCtxt, v_REGDOMAIN_t regId)
{
    v_CONTEXT_t pVosContext = NULL;
    hdd_context_t *pHddCtx = NULL;
    struct wiphy *wiphy = NULL;
   /*                                            */
   if (regId >= REGDOMAIN_COUNT)
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "VOS set reg domain, invalid REG domain ID %d", regId);
      return VOS_STATUS_E_INVAL;
   }

   pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
   if (NULL != pVosContext)
      pHddCtx = vos_get_context(VOS_MODULE_ID_HDD, pVosContext);
   else
      return VOS_STATUS_E_EXISTS;
   /*                                                     */
   regChannels = pnvEFSTable->halnv.tables.regDomains[regId].channels;

   /*                                                    
                                                              
                                          */
   if ((NULL != pHddCtx) && (memcmp(pHddCtx->cfg_ini->crdaDefaultCountryCode,
                    CFG_CRDA_DEFAULT_COUNTRY_CODE_DEFAULT , 2) == 0))
   {
      wiphy = pHddCtx->wiphy;
      regulatory_hint(wiphy, "00");
   }
   return VOS_STATUS_SUCCESS;
}

/*                                                                         
                                         
                                                              
                                                         
                      
                       
                  
     
                                                                           */
eNVChannelEnabledType vos_nv_getChannelEnabledState
(
   v_U32_t     rfChannel
)
{
   v_U32_t       channelLoop;
   eRfChannels   channelEnum = INVALID_RF_CHANNEL;

   for(channelLoop = 0; channelLoop <= RF_CHAN_165; channelLoop++)
   {
      if(rfChannels[channelLoop].channelNum == rfChannel)
      {
         channelEnum = (eRfChannels)channelLoop;
         break;
      }
   }

   if(INVALID_RF_CHANNEL == channelEnum)
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "vos_nv_getChannelEnabledState, invalid channel %d", rfChannel);
      return NV_CHANNEL_INVALID;
   }

   return regChannels[channelEnum].enabled;
}

/*                                                                 
                            
                                                                  */

static int bw20_ch_index_to_bw40_ch_index(int k)
{
   int m = -1;
   if (k >= RF_CHAN_1 && k <= RF_CHAN_14)
   {
      m = k - RF_CHAN_1 + RF_CHAN_BOND_3 ;
      if (m > RF_CHAN_BOND_11)
         m = RF_CHAN_BOND_11;
   }
   else if (k >= RF_CHAN_240 && k <= RF_CHAN_216)
   {
      m = k - RF_CHAN_240 + RF_CHAN_BOND_242 ;
      if (m > RF_CHAN_BOND_214)
         m = RF_CHAN_BOND_214;
   }
   else if (k >= RF_CHAN_36 && k <= RF_CHAN_64)
   {
      m = k - RF_CHAN_36 + RF_CHAN_BOND_38;
      if (m > RF_CHAN_BOND_62)
         m = RF_CHAN_BOND_62;
   }
   else if (k >= RF_CHAN_100 && k <= RF_CHAN_140)
   {
      m = k - RF_CHAN_100 + RF_CHAN_BOND_102;
      if (m > RF_CHAN_BOND_138)
         m = RF_CHAN_BOND_138;
   }
   else if (k >= RF_CHAN_149 && k <= RF_CHAN_165)
   {
      m = k - RF_CHAN_149 + RF_CHAN_BOND_151;
      if (m > RF_CHAN_BOND_163)
         m = RF_CHAN_BOND_163;
   }
return m;
}

void crda_regulatory_entry_default(v_U8_t *countryCode, int domain_id)
{
   int k;
   pr_info("Country %c%c domain_id %d\n enable ch 1 - 11.\n",
       countryCode[0], countryCode[1], domain_id);
   for (k = RF_CHAN_1; k <= RF_CHAN_11; k++) {
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].enabled =
           NV_CHANNEL_ENABLE;
       /*                    */
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].pwrLimit = 20;
   }
   /*                                    */
   pr_info(" enable ch 12 - 14 to scan passively by setting DFS flag.\n");
   for (k = RF_CHAN_12; k <= MAX_2_4GHZ_CHANNEL; k++) {
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].enabled =
           NV_CHANNEL_DFS;
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].pwrLimit = 0;
   }
   pr_info(" enable 5GHz to scan passively by setting DFS flag.\n");
   for (k = MIN_5GHZ_CHANNEL; k <= MAX_5GHZ_CHANNEL; k++) {
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].enabled =
           NV_CHANNEL_DFS;
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].pwrLimit = 0;
   }
#ifdef PASSIVE_SCAN_4_9GHZ
   pr_info(" enable 4.9 GHz to scan passively by setting DFS flag.\n");
   for (k = RF_CHAN_240; k <= RF_CHAN_216; k++) {
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].enabled =
           NV_CHANNEL_DFS;
       pnvEFSTable->halnv.tables.regDomains[domain_id].channels[k].pwrLimit = 0;
   }
#endif
  if (domain_id == NUM_REG_DOMAINS-1)
  { /*           */
     crda_alpha2[0] = countryCode[0];
     crda_alpha2[1] = countryCode[1];
     crda_regulatory_entry_valid = VOS_TRUE;
     pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[0] = countryCode[0];
     pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[1] = countryCode[1];
     pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[2] = 'I';
     pnvEFSTable->halnv.tables.defaultCountryTable.regDomain = NUM_REG_DOMAINS-1;
  }
  if (domain_id == NUM_REG_DOMAINS-2)
  { /*                      */
     run_time_alpha2[0] = countryCode[0];
     run_time_alpha2[1] = countryCode[1];
     crda_regulatory_run_time_entry_valid = VOS_TRUE;
  }
}

static int crda_regulatory_entry_post_processing(struct wiphy *wiphy,
                struct regulatory_request *request,
                v_U8_t nBandCapability,
                int domain_id)
{
   if (request->alpha2[0] == '0' && request->alpha2[1] == '0') {
        pr_info("Country 00 special handling to enable passive scan.\n");
        crda_regulatory_entry_default(request->alpha2, domain_id);
   }
   return 0;
}

/*                                                                                   */
static int create_crda_regulatory_entry(struct wiphy *wiphy,
                struct regulatory_request *request,
                v_U8_t nBandCapability)
{
   int i, j, m;
   int k = 0, n = 0;

   if (run_time_alpha2[0]==request->alpha2[0] &&
        run_time_alpha2[1]==request->alpha2[1] &&
        crda_regulatory_run_time_entry_valid == VOS_TRUE)
        return 0; /*                 */

   /*                */
   if (nBandCapability == eCSR_BAND_24)
       pr_info("BandCapability is set to 2G only.\n");
   for (i=0,m=0;i<IEEE80211_NUM_BANDS;i++)
   {
       if (i == IEEE80211_BAND_2GHZ && nBandCapability == eCSR_BAND_5G) //        
          continue;
       else if (i == IEEE80211_BAND_5GHZ && nBandCapability == eCSR_BAND_24) //        
          continue;
       if (wiphy->bands[i] == NULL)
       {
          pr_info("error: wiphy->bands[i] is NULL, i = %d\n", i);
          return -1;
       }
       //                                                                    
       //                                                   
       if (i == 0)
           m = 0;
       else
           m = wiphy->bands[i-1]->n_channels + m;
       for (j=0;j<wiphy->bands[i]->n_channels;j++)
       {
           //                                                                
           //                                                            
           k = m + j;
           n = bw20_ch_index_to_bw40_ch_index(k);
           if (n == -1)
              return -1;
           if (wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_DISABLED)
           {
              if (pnvEFSTable == NULL)
              {
                 pr_info("error: pnvEFSTable is NULL, probably not parsed nv.bin yet\n");
                 return -1;
              }
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].enabled =
                 NV_CHANNEL_DISABLE;
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].enabled =
                 NV_CHANNEL_DISABLE;
              //                                                                                    
              //                              
           }
           else if (wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_RADAR)
           {
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].enabled =
                 NV_CHANNEL_DFS;
              //                                
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].pwrLimit =
                 (tANI_S8) ((wiphy->bands[i]->channels[j].max_power)/100);
              if ((wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_NO_HT40) == 0)
              {
                 pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].enabled =
                    NV_CHANNEL_DFS;
                 //                                               
                 pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].pwrLimit =
                    (tANI_S8) (((wiphy->bands[i]->channels[j].max_power)/100)-3);
              }
           }
           else //                                    
           {
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].enabled =
                 NV_CHANNEL_ENABLE;
              //                    
              pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[k].pwrLimit =
                 (tANI_S8) ((wiphy->bands[i]->channels[j].max_power)/100);
              if ((wiphy->bands[i]->channels[j].flags & IEEE80211_CHAN_NO_HT40) == 0)
              {
                 pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].enabled =
                    NV_CHANNEL_ENABLE;
                 //                                               
                 pnvEFSTable->halnv.tables.regDomains[NUM_REG_DOMAINS-2].channels[n].pwrLimit =
                    (tANI_S8) (((wiphy->bands[i]->channels[j].max_power)/100)-3);
              }
           }
           /*                                                                    
                                                                 */
       }
   }
   if (k == 0)
       return -1;
   run_time_alpha2[0] = request->alpha2[0];
   run_time_alpha2[1] = request->alpha2[1];
   crda_regulatory_run_time_entry_valid = VOS_TRUE;
   crda_regulatory_entry_post_processing(wiphy, request, nBandCapability, NUM_REG_DOMAINS-2);
return 0;
}
v_BOOL_t is_crda_regulatory_entry_valid(void)
{
return crda_regulatory_entry_valid;
}

/*                                                                                       
                                                           
                                                       
                                                           
                                                       
*/
static int bw20_start_freq_to_channel_index(u32 freq_khz)
{
int i;
u32 center_freq = freq_khz + 10000;
  //                                         
  //                  
  for (i=RF_CHAN_1;i<=RF_CHAN_14;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                                                        
  for (i=RF_CHAN_240;i<=RF_CHAN_216;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_36;i<=RF_CHAN_64;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_100;i<=RF_CHAN_140;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                     
  for (i=RF_CHAN_149;i<=RF_CHAN_165;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
return -1;
}

static int bw20_end_freq_to_channel_index(u32 freq_khz)
{
int i;
u32 center_freq = freq_khz - 10000;
  //                                         
  //                     
  for (i=RF_CHAN_165;i>=RF_CHAN_149;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_140;i>=RF_CHAN_100;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_64;i>=RF_CHAN_36;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                                                        
  for (i=RF_CHAN_216;i>=RF_CHAN_240;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                  
  for (i=RF_CHAN_14;i>=RF_CHAN_1;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
return -1;
}

static int bw40_start_freq_to_channel_index(u32 freq_khz)
{
int i;
u32 center_freq = freq_khz + 20000;
  //                                         
  //                  
  for (i=RF_CHAN_BOND_3;i<=RF_CHAN_BOND_11;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                                              
  for (i=RF_CHAN_BOND_242;i<=RF_CHAN_BOND_214;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_BOND_38;i<=RF_CHAN_BOND_62;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_BOND_102;i<=RF_CHAN_BOND_138;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                     
  for (i=RF_CHAN_BOND_151;i<=RF_CHAN_BOND_163;i++)
    if (center_freq <= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
return -1;
}

static int bw40_end_freq_to_channel_index(u32 freq_khz)
{
int i;
u32 center_freq = freq_khz - 20000;
  //                                         
  //                     
  for (i=RF_CHAN_BOND_163;i>=RF_CHAN_BOND_151;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_BOND_138;i>=RF_CHAN_BOND_102;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                    
  for (i=RF_CHAN_BOND_62;i>=RF_CHAN_BOND_38;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                                              
  for (i=RF_CHAN_BOND_214;i>=RF_CHAN_BOND_242;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
  //                  
  for (i=RF_CHAN_BOND_11;i>=RF_CHAN_BOND_3;i--)
    if (center_freq >= (u32) (rfChannels[i].targetFreq) * 1000)
      return i;
return -1;
}

static v_BOOL_t channel_in_capable_band(int j, v_U8_t nBandCapability)
{
   switch (nBandCapability)
   {
      case eCSR_BAND_ALL:
           return VOS_TRUE;
      case eCSR_BAND_24:
           if (j >= RF_CHAN_1 && j <= RF_CHAN_14)
              return VOS_TRUE;
           if (j >= RF_CHAN_BOND_3 && j <= RF_CHAN_BOND_11)
              return VOS_TRUE; //                   
           break;
      case eCSR_BAND_5G:
           if (j >= RF_CHAN_240 && j <= RF_CHAN_165)
              return VOS_TRUE;
           if (j >= RF_CHAN_BOND_242 && j <= RF_CHAN_BOND_163)
              return VOS_TRUE; //                   
           break;
      default:
           break;
   }
   return VOS_FALSE;
}

/*                                                                          */
static int create_crda_regulatory_entry_from_regd(struct wiphy *wiphy,
                struct regulatory_request *request,
                v_U8_t nBandCapability)
{
   int i, j, n, domain_id;
   int bw20_start_channel_index, bw20_end_channel_index;
   int bw40_start_channel_index, bw40_end_channel_index;

   if (wiphy == NULL || wiphy->regd == NULL)
   {
      wiphy_dbg(wiphy, "error: wiphy->regd is NULL\n");
      return -1;
   }
   if (crda_regulatory_entry_valid == VOS_FALSE)
      domain_id = NUM_REG_DOMAINS-1; /*           */
   else
      domain_id = NUM_REG_DOMAINS-2; /*                      */
   for (n = 0; n < NUM_RF_CHANNELS; n++)
      pnvEFSTable->halnv.tables.regDomains[domain_id].channels[n].enabled = NV_CHANNEL_DISABLE;

   for (i=0;i<wiphy->regd->n_reg_rules;i++)
   {
      wiphy_dbg(wiphy, "info: crda rule %d --------------------------------------------\n", i);
      bw20_start_channel_index =
         bw20_start_freq_to_channel_index(wiphy->regd->reg_rules[i].freq_range.start_freq_khz);
      bw20_end_channel_index =
      bw20_end_freq_to_channel_index(wiphy->regd->reg_rules[i].freq_range.end_freq_khz);
      if (bw20_start_channel_index == -1 || bw20_end_channel_index == -1)
      {
         wiphy_dbg(wiphy, "error: crda freq not supported, start freq (KHz) %d end freq %d\n",
          wiphy->regd->reg_rules[i].freq_range.start_freq_khz,
             wiphy->regd->reg_rules[i].freq_range.end_freq_khz);
         continue; //                                          
      }
      wiphy_dbg(wiphy, "20MHz start freq (KHz) %d end freq %d start ch index %d end ch index %d\n",
         wiphy->regd->reg_rules[i].freq_range.start_freq_khz,
          wiphy->regd->reg_rules[i].freq_range.end_freq_khz,
              bw20_start_channel_index, bw20_end_channel_index);
      for (j=bw20_start_channel_index;j<=bw20_end_channel_index;j++)
      {
         if (channel_in_capable_band(j, nBandCapability) == VOS_FALSE)
         {
             wiphy_dbg(wiphy, "info: CH %d is not in capable band\n",
                 rfChannels[j].channelNum);
             continue; //                                     
         }
         if (wiphy->regd->reg_rules[i].flags & NL80211_RRF_DFS)
         {
             pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].enabled = NV_CHANNEL_DFS;
             wiphy_dbg(wiphy, "info: CH %d is DFS, max EIRP (mBm) is %d\n", rfChannels[j].channelNum,
                wiphy->regd->reg_rules[i].power_rule.max_eirp);
         }
         else
         {
             pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].enabled = NV_CHANNEL_ENABLE;
             wiphy_dbg(wiphy, "info: CH %d is enabled, no DFS, max EIRP (mBm) is %d\n", rfChannels[j].channelNum,
                 wiphy->regd->reg_rules[i].power_rule.max_eirp);
         }
         /*                                       */
         pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].pwrLimit =
            (tANI_S8) ((wiphy->regd->reg_rules[i].power_rule.max_eirp)/100);
      }
      /*                                                                    
                                                               */
      if (wiphy->regd->reg_rules[i].freq_range.max_bandwidth_khz == 40000)
      {
         wiphy_dbg(wiphy, "info: 40MHz (channel bonding) is allowed\n");
         bw40_start_channel_index =
            bw40_start_freq_to_channel_index(wiphy->regd->reg_rules[i].freq_range.start_freq_khz);
         bw40_end_channel_index =
            bw40_end_freq_to_channel_index(wiphy->regd->reg_rules[i].freq_range.end_freq_khz);
         if (bw40_start_channel_index == -1 || bw40_end_channel_index == -1)
         {
            wiphy_dbg(wiphy, "error: crda freq not supported, start_freq_khz %d end_freq_khz %d\n",
                wiphy->regd->reg_rules[i].freq_range.start_freq_khz,
                   wiphy->regd->reg_rules[i].freq_range.end_freq_khz);
            continue; //                                          
         }
         wiphy_dbg(wiphy, "40MHz start freq (KHz) %d end freq %d start ch index %d end ch index %d\n",
            wiphy->regd->reg_rules[i].freq_range.start_freq_khz,
                wiphy->regd->reg_rules[i].freq_range.end_freq_khz,
                   bw40_start_channel_index, bw40_end_channel_index);
         for (j=bw40_start_channel_index;j<=bw40_end_channel_index;j++)
         {
            if (channel_in_capable_band(j, nBandCapability) == VOS_FALSE)
                continue; //                                     
            if (wiphy->regd->reg_rules[i].flags & NL80211_RRF_DFS)
            {
                pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].enabled = NV_CHANNEL_DFS;
                wiphy_dbg(wiphy, "info: 40MHz centered on CH %d is DFS\n", rfChannels[j].channelNum);
            }
            else
            {
                pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].enabled = NV_CHANNEL_ENABLE;
                wiphy_dbg(wiphy, "info: 40MHz centered on CH %d is enabled, no DFS\n", rfChannels[j].channelNum);
            }
            /*                                                   */
            pnvEFSTable->halnv.tables.regDomains[domain_id].channels[j].pwrLimit =
                (tANI_S8) (((wiphy->regd->reg_rules[i].power_rule.max_eirp)/100)-3);
         }
      }
  }
  /*                                                                
                                                                     
                                                */
  if (crda_regulatory_entry_valid == VOS_FALSE)
  { /*           */
     crda_alpha2[0] = request->alpha2[0];
     crda_alpha2[1] = request->alpha2[1];
     crda_regulatory_entry_valid = VOS_TRUE;
  }
  else
  { /*                      */
     run_time_alpha2[0] = request->alpha2[0];
     run_time_alpha2[1] = request->alpha2[1];
     crda_regulatory_run_time_entry_valid = VOS_TRUE;
  }
  crda_regulatory_entry_post_processing(wiphy, request, nBandCapability, domain_id);
  return 0;
}

/*
                                       
                                                                            
                                                                          
                                                                                 
                            
 */
int wlan_hdd_crda_reg_notifier(struct wiphy *wiphy,
                struct regulatory_request *request)
{
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    v_REGDOMAIN_t domainIdCurrent;
    tANI_U8 ccode[WNI_CFG_COUNTRY_CODE_LEN];
    tANI_U8 uBufLen = WNI_CFG_COUNTRY_CODE_LEN;
    tANI_U8 nBandCapability;
    int i,j,k,m;
    wiphy_dbg(wiphy, "info: cfg80211 reg_notifier callback for country"
                     " %c%c\n", request->alpha2[0], request->alpha2[1]);
    if (request->initiator == NL80211_REGDOM_SET_BY_USER)
    {
       wiphy_dbg(wiphy, "info: set by user\n");
       if (create_crda_regulatory_entry(wiphy, request, pHddCtx->cfg_ini->nBandCapability) != 0)
          return 0;
       //     
       /*                                                                    */
       /*                                                                   
                                        */
       //                                          
       //                                                      
    }
    else if (request->initiator == NL80211_REGDOM_SET_BY_COUNTRY_IE)
    {
       wiphy_dbg(wiphy, "info: set by country IE\n");
       if (create_crda_regulatory_entry(wiphy, request, pHddCtx->cfg_ini->nBandCapability) != 0)
          return 0;
       //     
       /*                                    */

       /*                                                                   */
       /*                                                                  
                                               */
       //                                           
       //                                                      
    }
    else if (request->initiator == NL80211_REGDOM_SET_BY_DRIVER ||
             (request->initiator == NL80211_REGDOM_SET_BY_CORE))
    {
       if ( eHAL_STATUS_SUCCESS !=  sme_GetCountryCode(pHddCtx->hHal, ccode, &uBufLen))
       {
          wiphy_dbg(wiphy, "info: set by driver CCODE ERROR\n");
          return 0;
       }
       if (eHAL_STATUS_SUCCESS != sme_GetRegulatoryDomainForCountry (pHddCtx->hHal,
                                           ccode, (v_REGDOMAIN_t *) &domainIdCurrent))
       {
          wiphy_dbg(wiphy, "info: set by driver ERROR\n");
          return 0;
       }
       wiphy_dbg(wiphy, "country: %c%c set by driver\n",ccode[0],ccode[1]);
       /*                                                             
                                                                           
                                                                          
                                                       */
       if (memcmp(pHddCtx->cfg_ini->crdaDefaultCountryCode,
                         CFG_CRDA_DEFAULT_COUNTRY_CODE_DEFAULT , 2) != 0)
       {
          if (create_crda_regulatory_entry_from_regd(wiphy, request, pHddCtx->cfg_ini->nBandCapability) == 0)
          {
             pr_info("crda entry created.\n");
             if (crda_alpha2[0] == request->alpha2[0] && crda_alpha2[1] == request->alpha2[1])
             {
                /*                                             */
                /*                                                                 */
                pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[0] = request->alpha2[0];
                pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[1] = request->alpha2[1];
                pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[2] = 'I';
                pnvEFSTable->halnv.tables.defaultCountryTable.regDomain = NUM_REG_DOMAINS-1;
                wiphy_dbg(wiphy, "info: init time default country code is %c%c%c\n",
                pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[0],
                    pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[1],
                       pnvEFSTable->halnv.tables.defaultCountryTable.countryCode[2]);
             }
             else /*                                              */
             {
                wiphy_dbg(wiphy, "info: crda none-default country code is %c%c\n",
                    request->alpha2[0], request->alpha2[1]);
             }
             //                                                   
          }
          complete(&pHddCtx->driver_crda_req);
       }
       else
       {
          nBandCapability = pHddCtx->cfg_ini->nBandCapability;
          for (i=0, m=0; i<IEEE80211_NUM_BANDS; i++)
          {
             if (NULL == wiphy->bands[i])
             {
                VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "error: wiphy->bands[i] is NULL, i = %d", i);
                continue;
             }

             //                                                                    
             //                                                   
             if (0 == i)
             {
                m = 0;
             }
             else
             {
                m = wiphy->bands[i-1]?wiphy->bands[i-1]->n_channels + m:m;
             }

             for (j=0; j<wiphy->bands[i]->n_channels; j++)
             {
                //                                                                
                //                                                            
                k = m + j;
                if (IEEE80211_BAND_2GHZ == i && eCSR_BAND_5G == nBandCapability) //        
                {
                   //                               
                   if ((2412 == wiphy->bands[i]->channels[j].center_freq ||
                       2437 == wiphy->bands[i]->channels[j].center_freq ||
                       2462 == wiphy->bands[i]->channels[j].center_freq ) &&
                       NV_CHANNEL_ENABLE == regChannels[k].enabled)
                   {
                       wiphy->bands[i]->channels[j].flags &= ~IEEE80211_CHAN_DISABLED;
                   }
                   else
                   {
                      wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_DISABLED;
                   }
                   continue;
                }
                else if (IEEE80211_BAND_5GHZ == i && eCSR_BAND_24 == nBandCapability) //        
                {
                   wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_DISABLED;
                   continue;
                }

                if (NV_CHANNEL_DISABLE == regChannels[k].enabled ||
                    NV_CHANNEL_INVALID == regChannels[k].enabled)
                {
                   wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_DISABLED;
                }
                else if (NV_CHANNEL_DFS == regChannels[k].enabled)
                {
                   wiphy->bands[i]->channels[j].flags &= ~(IEEE80211_CHAN_DISABLED
                                                          |IEEE80211_CHAN_RADAR);
                   wiphy->bands[i]->channels[j].flags |= IEEE80211_CHAN_PASSIVE_SCAN;
                }
                else
                {
                   wiphy->bands[i]->channels[j].flags &= ~(IEEE80211_CHAN_DISABLED
                                                          |IEEE80211_CHAN_PASSIVE_SCAN
                                                          |IEEE80211_CHAN_NO_IBSS
                                                          |IEEE80211_CHAN_RADAR);
                }
             }
          }
          /*                                                               
                                                                    */
          if (wiphy->bands[IEEE80211_BAND_5GHZ])
          {
             for (j=0; j<wiphy->bands[IEEE80211_BAND_5GHZ]->n_channels; j++)
             {
/*                                                                                     */
#if 0
                 //                                                         
                if ((wiphy->bands[IEEE80211_BAND_5GHZ ]->channels[j].center_freq == 5180 ||
                               wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5200 ||
                               wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5220 ||
                               wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5240) &&
                               (ccode[0]== 'U'&& ccode[1]=='S'))
                {
                   wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].flags |= IEEE80211_CHAN_PASSIVE_SCAN;
                }
                else if ((wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5180 ||
                                    wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5200 ||
                                    wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5220 ||
                                    wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5240) &&
                                    (ccode[0]!= 'U'&& ccode[1]!='S'))
                {
                   wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].flags &= ~IEEE80211_CHAN_PASSIVE_SCAN;
                }
#else
                if ((wiphy->bands[IEEE80211_BAND_5GHZ ]->channels[j].center_freq == 5180 ||
                               wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5200 ||
                               wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5220 ||
                               wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].center_freq == 5240)
                               )
                {
                   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "freq:%u unset PASSIVE_SCAN by lge",
						   wiphy->bands[IEEE80211_BAND_5GHZ ]->channels[j].center_freq );
                   wiphy->bands[IEEE80211_BAND_5GHZ]->channels[j].flags &= ~IEEE80211_CHAN_PASSIVE_SCAN;
                }
#endif
             }
          }
          if (request->initiator == NL80211_REGDOM_SET_BY_CORE)
          {
             request->processed = 1;
          }
       }
    }
return 0;
}
