/*
 * serial2sock.h
 *
 *  Created on: 22.05.2014
 *      Author: maxbit
 */

#ifndef SERIAL2SOCK_H_
#define SERIAL2SOCK_H_
#include "gbem.h"

/* Serial Controll Register(HWREG_SC):
 * Bit 7 - Transfer Start Flag (0=No transfer is in progress or requested, 1=Transfer in progress, or requested)
 * Bit 1 - Clock Speed (0=Normal, 1=Fast) ** CGB Mode Only **
 * Bit 0 - Shift Clock (0=External Clock, 1=Internal Clock)
 */

#define SER_RXTX_ENABLE   0x80
#define SER_CLOCK_SPEED 0x02
#define SER_CLOCK_SELECT 0x01

void serial_tx(Byte sb, Byte sc);

#endif /* SERIAL2SOCK_H_ */
