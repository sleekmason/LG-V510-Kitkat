#ifndef __ASM_AVR32_PARAM_H
#define __ASM_AVR32_PARAM_H

#ifdef __KERNEL__
# define HZ		CONFIG_HZ
# define USER_HZ	100		/*                                */
# define CLOCKS_PER_SEC	(USER_HZ)	/*                                   */
#endif

#ifndef HZ
# define HZ		100
#endif

/*                              */
#define EXEC_PAGESIZE	4096

#ifndef NOGROUP
# define NOGROUP	(-1)
#endif

#define MAXHOSTNAMELEN	64

#endif /*                     */
