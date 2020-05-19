/*-------------------------------------------------------------------
**
**  Fichero:
**    intcontroller.c  3/3/2016
**
**    Estructura de Computadores
**    Dpto. de Arquitectura de Computadores y Automática
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las implementación del módulo intcontroller
**
**-----------------------------------------------------------------*/

// FALTA MUCHO

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "intcontroller.h"

void ic_init(void)
{
	/* Configuración por defector del controlador de interrupciones:
	 *    Lineas IRQ y FIQ no habilitadas
	 *    Linea IRQ en modo no vectorizado
	 *    Todo por la línea IRQ
	 *    Todas las interrupciones enmascaradas
	 **/
	rINTMOD = 0x0; // Configura las lineas como de tipo IRQ
	rINTCON = 0x7; // IRQ y FIQ enmascaradas, IRQ en modo no vectorizado
	rINTMSK = ~(0x0); // Enmascara todas las lineas
}

int ic_conf_irq(enum enable st, enum int_vec vec)
{
	int conf = rINTCON;

	if (st != ENABLE && st != DISABLE)
		return -1;

	if (vec == VEC) {
        conf = conf & ~(1 << 2); //suponiendo que bit[2] es el 2do contando por la izquierda
    }
    
		//COMPLETAR: poner la linea IRQ en modo vectorizado

	else {
        conf = conf | (1 << 2);
    }
		//COMPLETAR: poner la linea IRQ en modo no vectorizado

	if (st == ENABLE) {
        conf = conf & ~(1 << 1);
    }
		//COMPLETAR: habilitar la linea IRQ
	else {
        conf = conf | (1 << 1);
    }
		//COMPLETAR: deshabilitar la linea IRQ

	rINTCON = conf;
	return 0;
}

int ic_conf_fiq(enum enable st)
{
	int ret = 0;

	if (st == ENABLE) {
        rINTCON = rINTCON & ~(1);
    }
		//COMPLETAR: habilitar la linea FIQ
	else if (st == DISABLE) {
        rINTCON = rINTCON | 1;
    }
		//COMPLETAR: deshabilitar la linea FIQ
	else
		ret = -1;

	return ret;
}

int ic_conf_line(enum int_line line, enum int_mode mode)
{
	unsigned int bit = INT_BIT(line);

	if (line < 0 || line > 26)
		return -1;

	if (mode != IRQ && mode != FIQ)
		return -1;

	if (mode == IRQ) {
        rINTMOD = rINTMOD & ~(bit);
    }
		//COMPLETAR: poner la linea line en modo IRQ
	else {
        rINTMOD = rINTMOD | (bit);
    }
		//COMPLETAR: poner la linea line en modo FIQ

	return 0;
}

int ic_enable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;
    
    unsigned int bit = INT_BIT(line);
    rINTMSK = rINTMSK & ~(bit);
	//COMPLETAR: habilitar las interrupciones por la linea line

	return 0;
}

int ic_disable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;

    unsigned int bit = INT_BIT(line);
    rINTMSK = rINTMSK | (bit);
	//COMPLETAR: enmascarar las interrupciones por la linea line
	
	return 0;
}

int ic_cleanflag(enum int_line line)
{
	int bit;

	if (line < 0 || line > 26)
		return -1;

	bit = INT_BIT(line);

	if (rINTMOD & bit) {
        rF_ISPC = rF_ISPC | bit;
    }
		//COMPLETAR: borrar el flag de interrupcion correspondiente a la linea line
		//con la linea configurada por FIQ
	else {
        rI_ISPC = rI_ISPC | bit;
    }
		//COMPLETAR: borrar el flag de interrupcion correspondiente a la linea line
		//con la linea configurada por IRQ
	return 0;
}



