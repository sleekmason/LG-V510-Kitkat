/** @addtogroup MCD_MCDIMPL_KMOD_API Mobicore Driver Module API
 * @ingroup  MCD_MCDIMPL_KMOD
 * @{
 * Interface to Mobicore Driver Kernel Module.
 * @file
 *
 * <h2>Introduction</h2>
 * The MobiCore Driver Kernel Module is a Linux device driver, which represents
 * the command proxy on the lowest layer to the secure world (Swd). Additional
 * services like memory allocation via mmap and generation of a L2 tables for
 * given virtual memory are also supported. IRQ functionallity receives
 * information from the SWd in the non secure world (NWd).
 * As customary the driver is handled as linux device driver with "open",
 * "close" and "ioctl" commands. Access to the driver is possible after the
 * device "/dev/mobicore" has been opened.
 * The MobiCore Driver Kernel Module must be installed via
 * "insmod mcDrvModule.ko".
 *
 *
 * <h2>Version history</h2>
 * <table class="customtab">
 * <tr><td width="100px"><b>Date</b></td><td width="80px"><b>Version</b></td>
 * <td><b>Changes</b></td></tr>
 * <tr><td>2010-05-25</td><td>0.1</td><td>Initial Release</td></tr>
 * </table>
 *
 * <!-- Copyright Giesecke & Devrient GmbH 2010-2012 -->
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *	products derived from this software without specific prior
 *	written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MC_DRV_MODULEAPI_H_
#define _MC_DRV_MODULEAPI_H_

#include "version.h"

#define MC_DRV_MOD_DEVNODE		   "mobicore"
#define MC_DRV_MOD_DEVNODE_FULLPATH  "/dev/" MC_DRV_MOD_DEVNODE

/* 
                                                                   
                           
 */
union mc_ioctl_init_params {
	struct {
		/*                                       */
		uint32_t  base;
		/*                                                           */
		uint32_t  nq_offset;
		/*                               */
		uint32_t  nq_length;
		/*                                                  */
		uint32_t  mcp_offset;
		/*                       */
		uint32_t  mcp_length;
	} in;
	struct {
		/*         */
	} out;
};


/* 
                                                                   
                               
 */
union mc_ioctl_info_params {
	struct {
		uint32_t  ext_info_id; /*                    */
	} in;
	struct {
		uint32_t  state; /*         */
		uint32_t  ext_info; /*                 */
	} out;
};

/* 
                                                            
                                                                              
                                                                         
                                      
                                                                        
                      
                                                                     
                           
  
                                                                       
                                                                              
                                                                               
         
 */
enum mc_mmap_memtype {
	MC_DRV_KMOD_MMAP_WSM		= 0,
	MC_DRV_KMOD_MMAP_MCI		= 4096,
	MC_DRV_KMOD_MMAP_PERSISTENTWSM	= 8192
};

struct mc_mmap_resp {
	uint32_t  handle; /*              */
	uint32_t  phys_addr; /*                                     */
	bool	  is_reused; /*                                              */
};

/* 
                                                                       
 */
union mc_ioctl_free_params {
	struct {
		uint32_t  handle; /*                 */
		uint32_t  pid; /*              */
	} in;
	struct {
		/*         */
	} out;
};


/* 
                                                                                
  
                                                                    
                                                
                                                                              
                                       
 */
union mc_ioctl_app_reg_wsm_l2_params {
	struct {
		uint32_t  buffer; /*                                        */
		uint32_t  len; /*                                     */
		uint32_t  pid; /*              */
	} in;
	struct {
		uint32_t  handle; /*                                   */
		uint32_t  phys_wsm_l2_table; /*                                  */
	} out;
};


/* 
                                                                         
           
 */
struct mc_ioctl_app_unreg_wsm_l2_params {
	struct {
		uint32_t  handle; /*                                   */
		uint32_t  pid; /*              */
	} in;
	struct {
		/*         */
	} out;
};


/* 
                                                                               
 */
struct mc_ioctl_daemon_lock_wsm_l2_params {
	struct {
		uint32_t  handle; /*                                   */
	} in;
	struct {
		uint32_t phys_wsm_l2_table;
	} out;
};


/* 
                                                                        
           
 */
struct mc_ioctl_daemon_unlock_wsm_l2_params {
	struct {
		uint32_t  handle; /*                                   */
	} in;
	struct {
		/*         */
	} out;
};

/* 
                                                                         
 */
union mc_ioctl_fc_execute_params {
	struct {
		/*                                   */
		uint32_t  phys_start_addr;
		/*                      */
		uint32_t  length;
	} in;
	struct {
		/*         */
	} out;
};

/* 
                                                                          
 */
struct mc_ioctl_get_version_params {
	struct {
		uint32_t	kernel_module_version;
	} out;
};

/*                                                         */




/*                                                                             
                                        */
/* 
                                                                              
 */
enum mc_kmod_ioctl {

	/*
                                
  */
	MC_DRV_KMOD_IOCTL_DUMP_STATUS  = 200,

	/*
                       
  */
	MC_DRV_KMOD_IOCTL_FC_INIT  = 201,

	/*
                       
  */
	MC_DRV_KMOD_IOCTL_FC_INFO  = 202,

	/* 
                                                         
                                     
                                  
  */
	MC_DRV_KMOD_IOCTL_FC_YIELD =  203,
	/* 
                                                       
                                    
                                 
  */
	MC_DRV_KMOD_IOCTL_FC_NSIQ   =  204,
	/* 
                                                               
                                    
                                           
  */
	MC_DRV_KMOD_IOCTL_FC_EXECUTE =  205,

	/* 
                                                                  
                                                       
                                                                       
                                                                        
                                   
                                                                         
                     
  */
	MC_DRV_KMOD_IOCTL_FREE = 218,

	/* 
                                                                    
         
                                             
  */
	MC_DRV_KMOD_IOCTL_APP_REGISTER_WSM_L2 = 220,

	/* 
                                                                         
          
                                               
  */
	MC_DRV_KMOD_IOCTL_APP_UNREGISTER_WSM_L2 = 221,


	/*                     */
	MC_DRV_KMOD_IOCTL_DAEMON_LOCK_WSM_L2 = 222,
	MC_DRV_KMOD_IOCTL_DAEMON_UNLOCK_WSM_L2 = 223,

	/* 
                                 
                                          
  */
	MC_DRV_KMOD_IOCTL_GET_VERSION = 224,
};


#endif /*                      */
/*     */
