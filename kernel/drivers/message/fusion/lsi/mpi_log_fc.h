/*
 *  Copyright (c) 2000-2008 LSI Corporation. All rights reserved.
 *
 *  NAME:           fc_log.h
 *  SUMMARY:        MPI IocLogInfo definitions for the SYMFC9xx chips
 *  DESCRIPTION:    Contains the enumerated list of values that may be returned
 *                  in the IOCLogInfo field of a MPI Default Reply Message.
 *
 *  CREATION DATE:  6/02/2000
 *  ID:             $Id: fc_log.h,v 4.6 2001/07/26 14:41:33 sschremm Exp $
 */


/*
                       
  
                                                                        
            
                                
                                                                               
                                                                  
                                                          
                                     
                                  
                           
                                         
                               
                                       
                                            
                                         
                                                          
                                                     
  
                                                                               
                                                          
 */
typedef enum _MpiIocLogInfoFc
{
    MPI_IOCLOGINFO_FC_INIT_BASE                     = 0x20000000,
    MPI_IOCLOGINFO_FC_INIT_ERROR_OUT_OF_ORDER_FRAME = 0x20000001, /*                                              */
    MPI_IOCLOGINFO_FC_INIT_ERROR_BAD_START_OF_FRAME = 0x20000002, /*                                            */
    MPI_IOCLOGINFO_FC_INIT_ERROR_BAD_END_OF_FRAME   = 0x20000003, /*                                          */
    MPI_IOCLOGINFO_FC_INIT_ERROR_OVER_RUN           = 0x20000004, /*                       */
    MPI_IOCLOGINFO_FC_INIT_ERROR_RX_OTHER           = 0x20000005, /*                                                  */
    MPI_IOCLOGINFO_FC_INIT_ERROR_SUBPROC_DEAD       = 0x20000006, /*                                                   */
    MPI_IOCLOGINFO_FC_INIT_ERROR_RX_OVERRUN         = 0x20000007, /*                         */
    MPI_IOCLOGINFO_FC_INIT_ERROR_RX_BAD_STATUS      = 0x20000008, /*                                                       */
    MPI_IOCLOGINFO_FC_INIT_ERROR_RX_UNEXPECTED_FRAME= 0x20000009, /*                                         */
    MPI_IOCLOGINFO_FC_INIT_ERROR_LINK_FAILURE       = 0x2000000A, /*                        */
    MPI_IOCLOGINFO_FC_INIT_ERROR_TX_TIMEOUT         = 0x2000000B, /*                           */

    MPI_IOCLOGINFO_FC_TARGET_BASE                   = 0x21000000,
    MPI_IOCLOGINFO_FC_TARGET_NO_PDISC               = 0x21000001, /*                                                                */
    MPI_IOCLOGINFO_FC_TARGET_NO_LOGIN               = 0x21000002, /*                                                          */
    MPI_IOCLOGINFO_FC_TARGET_DOAR_KILLED_BY_LIP     = 0x21000003, /*                                                */
    MPI_IOCLOGINFO_FC_TARGET_DIAR_KILLED_BY_LIP     = 0x21000004, /*                                               */
    MPI_IOCLOGINFO_FC_TARGET_DIAR_MISSING_DATA      = 0x21000005, /*                                             */
    MPI_IOCLOGINFO_FC_TARGET_DONR_KILLED_BY_LIP     = 0x21000006, /*                                              */
    MPI_IOCLOGINFO_FC_TARGET_WRSP_KILLED_BY_LIP     = 0x21000007, /*                                                   */
    MPI_IOCLOGINFO_FC_TARGET_DINR_KILLED_BY_LIP     = 0x21000008, /*                                                  */
    MPI_IOCLOGINFO_FC_TARGET_DINR_MISSING_DATA      = 0x21000009, /*                                           */
    MPI_IOCLOGINFO_FC_TARGET_MRSP_KILLED_BY_LIP     = 0x2100000a, /*                                       */
    MPI_IOCLOGINFO_FC_TARGET_NO_CLASS_3             = 0x2100000b, /*                                                       */
    MPI_IOCLOGINFO_FC_TARGET_LOGIN_NOT_VALID        = 0x2100000c, /*                                                     */
    MPI_IOCLOGINFO_FC_TARGET_FROM_OUTBOUND          = 0x2100000e, /*                                                */
    MPI_IOCLOGINFO_FC_TARGET_WAITING_FOR_DATA_IN    = 0x2100000f, /*                                         */

    MPI_IOCLOGINFO_FC_LAN_BASE                      = 0x22000000,
    MPI_IOCLOGINFO_FC_LAN_TRANS_SGL_MISSING         = 0x22000001, /*                                 */
    MPI_IOCLOGINFO_FC_LAN_TRANS_WRONG_PLACE         = 0x22000002, /*                                         */
    MPI_IOCLOGINFO_FC_LAN_TRANS_RES_BITS_SET        = 0x22000003, /*                                                 */
    MPI_IOCLOGINFO_FC_LAN_WRONG_SGL_FLAG            = 0x22000004, /*                   */

    MPI_IOCLOGINFO_FC_MSG_BASE                      = 0x23000000,

    MPI_IOCLOGINFO_FC_LINK_BASE                     = 0x24000000,
    MPI_IOCLOGINFO_FC_LINK_LOOP_INIT_TIMEOUT        = 0x24000001, /*                               */
    MPI_IOCLOGINFO_FC_LINK_ALREADY_INITIALIZED      = 0x24000002, /*                                                        */
    MPI_IOCLOGINFO_FC_LINK_LINK_NOT_ESTABLISHED     = 0x24000003, /*                                                                          */
    MPI_IOCLOGINFO_FC_LINK_CRC_ERROR                = 0x24000004, /*                                            */

    MPI_IOCLOGINFO_FC_CTX_BASE                      = 0x25000000,

    MPI_IOCLOGINFO_FC_INVALID_FIELD_BYTE_OFFSET     = 0x26000000, /*                                                                                            */
    MPI_IOCLOGINFO_FC_INVALID_FIELD_MAX_OFFSET      = 0x26ffffff,

    MPI_IOCLOGINFO_FC_STATE_CHANGE                  = 0x27000000  /*                                                                       */

} MpiIocLogInfoFc_t;