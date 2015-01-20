/*
                                         
  
                                                            
  
                                
                                                  
                                                                      
                                                                  
                    
 */
#include <linux/init.h>

#include <mach/hardware.h>
#include <asm/leds.h>

#include "leds.h"


#define LED_STATE_ENABLED	1
#define LED_STATE_CLAIMED	2

static unsigned int led_state;
static unsigned int hw_led_state;

#define LED_23    GPIO_GPIO23
#define LED_MASK  (LED_23)

void lart_leds_event(led_event_t evt)
{
	unsigned long flags;

	local_irq_save(flags);

	switch(evt) {
	case led_start:
		/*                      */
		GPDR |= LED_23;
		hw_led_state = LED_MASK;
		led_state = LED_STATE_ENABLED;
		break;

	case led_stop:
		led_state &= ~LED_STATE_ENABLED;
		break;

	case led_claim:
		led_state |= LED_STATE_CLAIMED;
		hw_led_state = LED_MASK;
		break;

	case led_release:
		led_state &= ~LED_STATE_CLAIMED;
		hw_led_state = LED_MASK;
		break;

#ifdef CONFIG_LEDS_TIMER
	case led_timer:
		if (!(led_state & LED_STATE_CLAIMED))
			hw_led_state ^= LED_23;
		break;
#endif

#ifdef CONFIG_LEDS_CPU
	case led_idle_start:
		/*                                                
                                     */
		if (!(led_state & LED_STATE_CLAIMED))
			hw_led_state &= ~LED_23;
		break;

	case led_idle_end:
		/*                                      */
		if (!(led_state & LED_STATE_CLAIMED))
			hw_led_state |= LED_23;
		break;
#endif

	case led_red_on:
		if (led_state & LED_STATE_CLAIMED)
			hw_led_state &= ~LED_23;
		break;

	case led_red_off:
		if (led_state & LED_STATE_CLAIMED)
			hw_led_state |= LED_23;
		break;

	default:
		break;
	}

	/*                                                       */
	if (led_state & LED_STATE_ENABLED) {
		GPSR = hw_led_state;
		GPCR = hw_led_state ^ LED_MASK;
	}

	local_irq_restore(flags);
}
