#include "contiki.h"  // Contiki core
#include "contiki-net.h"
#include <stdio.h>
#include "button-sensor.h"
#include "dev/leds.h"
#include "board-peripherals.h"

#include "simple-udp.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-debug.h"
#include "net/ip/uiplib.h"

// Etimer settings
#define SEND_INTERVAL	(CLOCK_SECOND * 5)
static struct etimer periodic_timer;
//static radio_value_t radio_actual_tx_power = 0;

// UDP settings and payload structure
#define UDP_PORT	4000
#define	UIP_DS6_DEFAULT_PREFIX	0xaaaa
//static struct simple_udp_connection unicast_connection;
struct udp_payload{
	//uint32_t	sequence_number;
	uint8_t		uid;
	float		temperature;
	//char		name[15];
};
static struct udp_payload message;



// Global variables
static int value;


/*---------------------------------------------------------------------------*/

// Floor function
float my_floor(float num){
	if(num>=0.0f) return (float) ((int)num);
	else return(float)((int)num-1);
}


static uip_ipaddr_t *
set_global_address(void)
{
	static uip_ipaddr_t ipaddr;
	uip_ip6addr(&ipaddr, UIP_DS6_DEFAULT_PREFIX, 0, 0, 0, 0, 0, 0, 0);
	uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

	printf("IPv6 addresses: ");
	uint16_t i = 0;
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		uint8_t state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused &&
				(state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
			printf("\n");
		}
	}
	return &ipaddr;
}


/*---------------------------------------------------------------------------*/

PROCESS(wireless_communication_process, "Wireless Communication process");
AUTOSTART_PROCESSES(&wireless_communication_process);
PROCESS_THREAD(wireless_communication_process, ev, data)
{
	PROCESS_EXITHANDLER()
	PROCESS_BEGIN();

	//radio_actual_tx_power = 5;   // [dBm]
	//NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, radio_actual_tx_power);

	// Setting node address
	uip_ipaddr_t *my_ip_address;
	my_ip_address = set_global_address();

	// UDP register
	//simple_udp_register(&unicast_connection, UDP_PORT, NULL, UDP_PORT, udp_received);

	while(1) {

		// Wait for a fixed amount of time
		etimer_set(&periodic_timer, SEND_INTERVAL);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));



		// Create the message to be sent
		message.uid = 1;
		//strcpy(message.name, "Carlo");

		// Read Barometric Pressure/Temperature Sensor BMP280
		SENSORS_ACTIVATE(bmp_280_sensor);
		PROCESS_YIELD();
		value = bmp_280_sensor.value(BMP_280_SENSOR_TYPE_TEMP);
		if(value != CC26XX_SENSOR_READING_ERROR) {
			message.temperature = (float)value / 100;
			//int uid=1;
			printf("\n{'uid': %d, 'Temperature': %ld.%02d}\n", (int) message.uid, (long) message.temperature, (unsigned) ((message.temperature-my_floor(message.temperature))*100));
		} else {
			printf("Temperature Read Error\n");
			message.temperature = 0;
		}

	    // Deriving the address of the node we are talking to
	    uip_ipaddr_t recipient_address;
	    uiplib_ipaddrconv("fe80::212:4b00:69e:d589", &recipient_address);

	    // Sending the message
/*	    if(uip_ipaddr_cmp(my_ip_address, &recipient_address)) {
	    	printf("ERROR: Sending to myself: stopping...\n");
	    	uip_debug_ipaddr_print(&recipient_address);
	    	printf(" = ");
	    	uip_debug_ipaddr_print(my_ip_address);
	    	printf("\n");
	     }
	    else {
	    	simple_udp_sendto(&unicast_connection, &message, sizeof(struct udp_payload), &recipient_address);
	        printf("Sending message number %lu to ", message.sequence_number);
	    	uip_debug_ipaddr_print(&recipient_address);
	    	printf("\n\n");
	    }
*/
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
