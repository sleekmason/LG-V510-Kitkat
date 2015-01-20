/***********************license start***************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2008 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
 ***********************license end**************************************/

/*
                                                    
                  
 */
#include <asm/octeon/octeon.h>

#include <asm/octeon/cvmx-config.h>

#include <asm/octeon/cvmx-helper.h>
#include <asm/octeon/cvmx-pip-defs.h>

/* 
                                                           
                                                           
                   
  
                                 
  
                                                             
 */
int __cvmx_helper_loop_probe(int interface)
{
	union cvmx_ipd_sub_port_fcs ipd_sub_port_fcs;
	int num_ports = 4;
	int port;

	/*                                                                  
                            */
	for (port = 0; port < num_ports; port++) {
		union cvmx_pip_prt_cfgx port_cfg;
		int ipd_port = cvmx_helper_get_ipd_port(interface, port);
		port_cfg.u64 = cvmx_read_csr(CVMX_PIP_PRT_CFGX(ipd_port));
		port_cfg.s.maxerr_en = 0;
		port_cfg.s.minerr_en = 0;
		cvmx_write_csr(CVMX_PIP_PRT_CFGX(ipd_port), port_cfg.u64);
	}

	/*                                          */
	ipd_sub_port_fcs.u64 = cvmx_read_csr(CVMX_IPD_SUB_PORT_FCS);
	ipd_sub_port_fcs.s.port_bit2 = 0;
	cvmx_write_csr(CVMX_IPD_SUB_PORT_FCS, ipd_sub_port_fcs.u64);
	return num_ports;
}

/* 
                                                              
                                                          
                            
  
                                    
  
                                               
 */
int __cvmx_helper_loop_enable(int interface)
{
	/*             */
	return 0;
}
