
#include <stdio.h>
#include "44b.h"
#include "button.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"

struct RLstat {
	int moving;
	int speed;
	int direction;
	int position;
};

static struct RLstat RL = {
	.moving = 0,
	.speed = 5,
	.direction = 0,
	.position = 0,
};

void timer_ISR(void) __attribute__ ((interrupt ("IRQ")));
void button_ISR(void) __attribute__ ((interrupt ("IRQ")));
void keyboard_ISR(void) __attribute__ ((interrupt ("IRQ")));

void timer_ISR(void)
{
	if (RL.direction)
		RL.position = (RL.position + 1) % 6;
	else
		RL.position = (RL.position + 5) % 6;

	D8Led_segment(RL.position);

	ic_cleanflag(INT_TIMER0);
}


static int cont = 0;

void button_ISR(void)
{
	unsigned int whicheint = rEXTINTPND;
	unsigned int buttons = (whicheint >> 2) & 0x3;

	if (buttons & BUT1) {
		led1_off();
		led2_off();
		RL.direction = ~RL.direction;
	}

	if (buttons & BUT2) {
		cont++;
		if (cont % 2 == 0)
			led1_switch();
		else
			led2_switch();
		RL.moving = ~RL.moving;
		if(RL.moving)
			tmr_start(TIMER0);
		else
			tmr_stop(TIMER0);
	}

	// eliminamos rebotes
	Delay(2000);
	// borramos el flag en extintpnd
	ic_cleanflag(INT_EINT4567);
}

void keyboard_ISR(void)
{
	int key;

	/* Eliminar rebotes de presión */
	Delay(200);
	
	/* Escaneo de tecla */
	key = kb_scan();

	if (key != -1) {
		/* Visualizacion en el display */
		D8Led_digit(key);

		switch (key) {
			case 0:
				tmr_set_prescaler(TIMER0, 255);
				tmr_set_divider(TIMER0, D1_8);
				tmr_set_count(TIMER0, 62500, 1);
				tmr_update(TIMER0);
				break;
			case 1:
				tmr_set_prescaler(TIMER0, 255);
				tmr_set_divider(TIMER0, D1_8);
				tmr_set_count(TIMER0, 31250, 1);
				tmr_update(TIMER0);
				break;
			case 2:
				tmr_set_prescaler(TIMER0, 255);
				tmr_set_divider(TIMER0, D1_8);
				tmr_set_count(TIMER0, 15625, 1);
				tmr_update(TIMER0);
				break;
			case 3:
				tmr_set_prescaler(TIMER0, 255);
				tmr_set_divider(TIMER0, D1_4);
				tmr_set_count(TIMER0, 15625, 1);
				tmr_update(TIMER0);
				break;
			default:
				break;
		}
		
		/* Esperar a que la tecla se suelte, consultando el registro de datos */		
		while (( rPDATG & (1 << 1)) != (1 << 1)) {};
	}

    /* Eliminar rebotes de depresión */
    Delay(200);
     
    /* Borrar interrupciones pendientes */
    ic_cleanflag(INT_EINT1);
}

int setup(void)
{
	leds_init();
	D8Led_init();
	D8Led_segment(RL.position);

	/* Port G: configuración para generación de interrupciones externas,
	 *         botones y teclado
	 **/

	portG_conf(1, EINT);
	portG_conf(6, EINT);
	portG_conf(7, EINT);
	portG_eint_trig(1, FALLING);
	portG_eint_trig(6, FALLING);
	portG_eint_trig(7, FALLING);
	portG_conf_pup(1, ENABLE);
	portG_conf_pup(6, ENABLE);
	portG_conf_pup(7, ENABLE);

	/********************************************************************/

	/* Configuración del timer */

	tmr_set_mode(TIMER0, RELOAD);
	tmr_set_prescaler(TIMER0, 255);
	tmr_set_divider(TIMER0, D1_8);
	tmr_set_count(TIMER0, 62500, 1);
	tmr_update(TIMER0);

	if (RL.moving)
		tmr_start(TIMER0);

	/***************************/

	// Registramos las ISRs
	pISR_TIMER0   = (unsigned) timer_ISR;
	pISR_EINT4567 = (unsigned) button_ISR;
	pISR_EINT1    = (unsigned) keyboard_ISR;

	/* Configuración del controlador de interrupciones
	 * Habilitamos la línea IRQ, en modo vectorizado 
	 * Configuramos el timer 0 en modo IRQ y habilitamos esta línea
	 * Configuramos la línea EINT4567 en modo IRQ y la habilitamos
	 * Configuramos la línea EINT1 en modo IRQ y la habilitamos
	 */
	ic_init();

	ic_conf_irq(ENABLE, VEC); 		//		habilitar la línea IRQ en modo vectorizado
	ic_conf_fiq(DISABLE); 			//		deshabilitar la línea FIQ
	ic_conf_line(INT_TIMER0, IRQ); 	//		configurar la línea INT_TIMER0 en modo IRQ
	ic_conf_line(INT_EINT4567, IRQ);//		configurar la línea INT_EINT4567 en modo IRQ
	ic_conf_line(INT_EINT1, IRQ); 	//		configurar la línea INT_EINT1 en modo IRQ
	ic_enable(INT_TIMER0); 			//		habilitar la línea INT_TIMER0
	ic_enable(INT_EINT4567); 		//		habilitar la línea INT_EINT4567
	ic_enable(INT_EINT1); 			//		habilitar la línea INT_EINT1

	/***************************************************/

	Delay(0);
	return 0;
}

int loop(void)
{
	return 0;
}


int main(void)
{
	setup();

	while (1) {
		loop();
	}
}
