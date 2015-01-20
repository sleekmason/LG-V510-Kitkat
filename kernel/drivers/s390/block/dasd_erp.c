/*
                                                   
                                                                  
                                              
                                       
                                                   
                                         
                                                                   
  
 */

#define KMSG_COMPONENT "dasd"

#include <linux/ctype.h>
#include <linux/init.h>

#include <asm/debug.h>
#include <asm/ebcdic.h>
#include <asm/uaccess.h>

/*                 */
#define PRINTK_HEADER "dasd_erp:"

#include "dasd_int.h"

struct dasd_ccw_req *
dasd_alloc_erp_request(char *magic, int cplength, int datasize,
		       struct dasd_device * device)
{
	unsigned long flags;
	struct dasd_ccw_req *cqr;
	char *data;
	int size;

	/*               */
	BUG_ON( magic == NULL || datasize > PAGE_SIZE ||
	     (cplength*sizeof(struct ccw1)) > PAGE_SIZE);

	size = (sizeof(struct dasd_ccw_req) + 7L) & -8L;
	if (cplength > 0)
		size += cplength * sizeof(struct ccw1);
	if (datasize > 0)
		size += datasize;
	spin_lock_irqsave(&device->mem_lock, flags);
	cqr = (struct dasd_ccw_req *)
		dasd_alloc_chunk(&device->erp_chunks, size);
	spin_unlock_irqrestore(&device->mem_lock, flags);
	if (cqr == NULL)
		return ERR_PTR(-ENOMEM);
	memset(cqr, 0, sizeof(struct dasd_ccw_req));
	INIT_LIST_HEAD(&cqr->devlist);
	INIT_LIST_HEAD(&cqr->blocklist);
	data = (char *) cqr + ((sizeof(struct dasd_ccw_req) + 7L) & -8L);
	cqr->cpaddr = NULL;
	if (cplength > 0) {
		cqr->cpaddr = (struct ccw1 *) data;
		data += cplength*sizeof(struct ccw1);
		memset(cqr->cpaddr, 0, cplength*sizeof(struct ccw1));
	}
	cqr->data = NULL;
	if (datasize > 0) {
		cqr->data = data;
 		memset(cqr->data, 0, datasize);
	}
	strncpy((char *) &cqr->magic, magic, 4);
	ASCEBC((char *) &cqr->magic, 4);
	set_bit(DASD_CQR_FLAGS_USE_ERP, &cqr->flags);
	dasd_get_device(device);
	return cqr;
}

void
dasd_free_erp_request(struct dasd_ccw_req *cqr, struct dasd_device * device)
{
	unsigned long flags;

	spin_lock_irqsave(&device->mem_lock, flags);
	dasd_free_chunk(&device->erp_chunks, cqr);
	spin_unlock_irqrestore(&device->mem_lock, flags);
	atomic_dec(&device->ref_count);
}


/*
                                                       
 */
struct dasd_ccw_req *
dasd_default_erp_action(struct dasd_ccw_req *cqr)
{
	struct dasd_device *device;

	device = cqr->startdev;

        /*                                                                   */
        if (cqr->retries > 0) {
		DBF_DEV_EVENT(DBF_DEBUG, device,
                             "default ERP called (%i retries left)",
                             cqr->retries);
		if (!test_bit(DASD_CQR_VERIFY_PATH, &cqr->flags))
			cqr->lpm = device->path_data.opm;
		cqr->status = DASD_CQR_FILLED;
        } else {
		pr_err("%s: default ERP has run out of retries and failed\n",
		       dev_name(&device->cdev->dev));
		cqr->status = DASD_CQR_FAILED;
		cqr->stopclk = get_clock();
        }
        return cqr;
}				/*                             */

/*
              
                                                               
                                                                      
                                                     
                                                                   
                  
  
            
                          
  
                
                                     
 */
struct dasd_ccw_req *dasd_default_erp_postaction(struct dasd_ccw_req *cqr)
{
	int success;

	BUG_ON(cqr->refers == NULL || cqr->function == NULL);

	success = cqr->status == DASD_CQR_DONE;

	/*                                          */
	while (cqr->refers != NULL) {
		struct dasd_ccw_req *refers;

		refers = cqr->refers;
		/*                                         */
		list_del(&cqr->blocklist);
		/*                               */
		dasd_free_erp_request(cqr, cqr->memdev);
		cqr = refers;
	}

	/*                                          */
	if (success)
		cqr->status = DASD_CQR_DONE;
	else {
		cqr->status = DASD_CQR_FAILED;
		cqr->stopclk = get_clock();
	}

	return cqr;

}				/*                            */

void
dasd_log_sense(struct dasd_ccw_req *cqr, struct irb *irb)
{
	struct dasd_device *device;

	device = cqr->startdev;
	/*                 */
	if (device->discipline && device->discipline->dump_sense)
		device->discipline->dump_sense(device, cqr, irb);
}

void
dasd_log_sense_dbf(struct dasd_ccw_req *cqr, struct irb *irb)
{
	struct dasd_device *device;

	device = cqr->startdev;
	/*                                     */
	if (device->discipline && device->discipline->dump_sense_dbf)
		device->discipline->dump_sense_dbf(device, irb, "log");
}
EXPORT_SYMBOL(dasd_log_sense_dbf);

EXPORT_SYMBOL(dasd_default_erp_action);
EXPORT_SYMBOL(dasd_default_erp_postaction);
EXPORT_SYMBOL(dasd_alloc_erp_request);
EXPORT_SYMBOL(dasd_free_erp_request);
EXPORT_SYMBOL(dasd_log_sense);

