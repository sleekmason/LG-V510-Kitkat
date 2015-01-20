/*
                                
 */

/*
 * This file is distributed under GPL.
 *
 * This style and layout of this file is also copied
 * from many of the other linux network device drivers.
 */

/*                                                    */
#define SEEQ8005_IO_EXTENT	16

#define SEEQ_B		(ioaddr)

#define	SEEQ_CMD	(SEEQ_B)		/*            */
#define	SEEQ_STATUS	(SEEQ_B)		/*           */
#define SEEQ_CFG1	(SEEQ_B + 2)
#define SEEQ_CFG2	(SEEQ_B + 4)
#define	SEEQ_REA	(SEEQ_B + 6)		/*                           */
#define SEEQ_RPR	(SEEQ_B + 10)		/*                          */
#define	SEEQ_TPR	(SEEQ_B + 12)		/*                           */
#define	SEEQ_DMAAR	(SEEQ_B + 14)		/*                      */
#define SEEQ_BUFFER	(SEEQ_B + 8)		/*                        */

#define	DEFAULT_TEA	(0x3f)

#define SEEQCMD_DMA_INT_EN	(0x0001)	/*                      */
#define SEEQCMD_RX_INT_EN	(0x0002)	/*                          */
#define SEEQCMD_TX_INT_EN	(0x0004)	/*                           */
#define SEEQCMD_WINDOW_INT_EN	(0x0008)	/*                             */
#define SEEQCMD_INT_MASK	(0x000f)

#define SEEQCMD_DMA_INT_ACK	(0x0010)	/*         */
#define SEEQCMD_RX_INT_ACK	(0x0020)
#define SEEQCMD_TX_INT_ACK	(0x0040)
#define	SEEQCMD_WINDOW_INT_ACK	(0x0080)
#define SEEQCMD_ACK_ALL		(0x00f0)

#define SEEQCMD_SET_DMA_ON	(0x0100)	/*                           */
#define SEEQCMD_SET_RX_ON	(0x0200)	/*                   */
#define SEEQCMD_SET_TX_ON	(0x0400)	/*               */
#define SEEQCMD_SET_DMA_OFF	(0x0800)
#define SEEQCMD_SET_RX_OFF	(0x1000)
#define SEEQCMD_SET_TX_OFF	(0x2000)
#define SEEQCMD_SET_ALL_OFF	(0x3800)	/*                   */

#define SEEQCMD_FIFO_READ	(0x4000)	/*                                          */
#define SEEQCMD_FIFO_WRITE	(0x8000)	/*                        */

#define SEEQSTAT_DMA_INT_EN	(0x0001)	/*                            */
#define SEEQSTAT_RX_INT_EN	(0x0002)
#define SEEQSTAT_TX_INT_EN	(0x0004)
#define SEEQSTAT_WINDOW_INT_EN	(0x0008)

#define	SEEQSTAT_DMA_INT	(0x0010)	/*                   */
#define SEEQSTAT_RX_INT		(0x0020)
#define SEEQSTAT_TX_INT		(0x0040)
#define	SEEQSTAT_WINDOW_INT	(0x0080)
#define SEEQSTAT_ANY_INT	(0x00f0)

#define SEEQSTAT_DMA_ON		(0x0100)	/*              */
#define SEEQSTAT_RX_ON		(0x0200)	/*              */
#define SEEQSTAT_TX_ON		(0x0400)	/*            */

#define SEEQSTAT_FIFO_FULL	(0x2000)
#define SEEQSTAT_FIFO_EMPTY	(0x4000)
#define SEEQSTAT_FIFO_DIR	(0x8000)	/*                 */

#define SEEQCFG1_BUFFER_MASK	(0x000f)	/*                                           */
#define SEEQCFG1_BUFFER_MAC0	(0x0000)	/*                           */
#define SEEQCFG1_BUFFER_MAC1	(0x0001)
#define SEEQCFG1_BUFFER_MAC2	(0x0002)
#define SEEQCFG1_BUFFER_MAC3	(0x0003)
#define SEEQCFG1_BUFFER_MAC4	(0x0004)
#define SEEQCFG1_BUFFER_MAC5	(0x0005)
#define SEEQCFG1_BUFFER_PROM	(0x0006)	/*                      */
#define SEEQCFG1_BUFFER_TEA	(0x0007)	/*                   */
#define SEEQCFG1_BUFFER_BUFFER	(0x0008)	/*                      */
#define SEEQCFG1_BUFFER_INT_VEC	(0x0009)	/*                  */

#define SEEQCFG1_DMA_INTVL_MASK	(0x0030)
#define SEEQCFG1_DMA_CONT	(0x0000)
#define SEEQCFG1_DMA_800ns	(0x0010)
#define SEEQCFG1_DMA_1600ns	(0x0020)
#define SEEQCFG1_DMA_3200ns	(0x0030)

#define SEEQCFG1_DMA_LEN_MASK	(0x00c0)
#define SEEQCFG1_DMA_LEN1	(0x0000)
#define SEEQCFG1_DMA_LEN2	(0x0040)
#define SEEQCFG1_DMA_LEN4	(0x0080)
#define SEEQCFG1_DMA_LEN8	(0x00c0)

#define SEEQCFG1_MAC_MASK	(0x3f00)	/*                                   */
#define SEEQCFG1_MAC0_EN	(0x0100)
#define SEEQCFG1_MAC1_EN	(0x0200)
#define SEEQCFG1_MAC2_EN	(0x0400)
#define SEEQCFG1_MAC3_EN	(0x0800)
#define	SEEQCFG1_MAC4_EN	(0x1000)
#define SEEQCFG1_MAC5_EN	(0x2000)

#define	SEEQCFG1_MATCH_MASK	(0xc000)	/*                                */
#define SEEQCFG1_MATCH_SPECIFIC	(0x0000)	/*                             */
#define SEEQCFG1_MATCH_BROAD	(0x4000)	/*                                  */
#define SEEQCFG1_MATCH_MULTI	(0x8000)	/*                                   */
#define SEEQCFG1_MATCH_ALL	(0xc000)	/*                  */

#define SEEQCFG1_DEFAULT	(SEEQCFG1_BUFFER_BUFFER | SEEQCFG1_MAC0_EN | SEEQCFG1_MATCH_BROAD)

#define SEEQCFG2_BYTE_SWAP	(0x0001)	/*                    */
#define SEEQCFG2_AUTO_REA	(0x0002)	/*                                                                   */

#define SEEQCFG2_CRC_ERR_EN	(0x0008)	/*                                              */
#define SEEQCFG2_DRIBBLE_EN	(0x0010)	/*                                          */
#define SEEQCFG2_SHORT_EN	(0x0020)	/*                                    */

#define	SEEQCFG2_SLOTSEL	(0x0040)	/*                                                        */
#define SEEQCFG2_NO_PREAM	(0x0080)	/*                                      */
#define SEEQCFG2_ADDR_LEN	(0x0100)	/*                    */
#define SEEQCFG2_REC_CRC	(0x0200)	/*                                                      */
#define SEEQCFG2_XMIT_NO_CRC	(0x0400)	/*                                                    */
#define SEEQCFG2_LOOPBACK	(0x0800)
#define SEEQCFG2_CTRLO		(0x1000)
#define SEEQCFG2_RESET		(0x8000)	/*                         */

struct seeq_pkt_hdr {
	unsigned short	next;			/*                               */
	unsigned char	babble_int:1,		/*                                 */
			coll_int:1,		/*                         */
			coll_16_int:1,		/*                             */
			xmit_int:1,		/*                                                    */
			unused:1,
			data_follows:1,		/*                                                       */
			chain_cont:1,		/*                                                                   */
			xmit_recv:1;		/*                                              */
	unsigned char	status;
};

#define SEEQPKTH_BAB_INT_EN	(0x01)		/*           */
#define SEEQPKTH_COL_INT_EN	(0x02)		/*           */
#define SEEQPKTH_COL16_INT_EN	(0x04)		/*           */
#define SEEQPKTH_XMIT_INT_EN	(0x08)		/*           */
#define SEEQPKTH_DATA_FOLLOWS	(0x20)		/*                         */
#define SEEQPKTH_CHAIN		(0x40)		/*                     */
#define SEEQPKTH_XMIT		(0x80)

#define SEEQPKTS_BABBLE		(0x0100)	/*           */
#define SEEQPKTS_OVERSIZE	(0x0100)	/*           */
#define SEEQPKTS_COLLISION	(0x0200)	/*           */
#define SEEQPKTS_CRC_ERR	(0x0200)	/*           */
#define SEEQPKTS_COLL16		(0x0400)	/*           */
#define SEEQPKTS_DRIB		(0x0400)	/*           */
#define SEEQPKTS_SHORT		(0x0800)	/*           */
#define SEEQPKTS_DONE		(0x8000)
#define SEEQPKTS_ANY_ERROR	(0x0f00)
