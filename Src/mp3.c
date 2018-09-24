#include "mp3.h"

uint8_t send_buf[10];

void send_func () {
	for (int i=0; i<10; i++) {
		printf("%c",send_buf[i]);
	}
}

static void fill_uint16_bigend (uint8_t *thebuf, uint16_t data) {
	*thebuf =	(uint8_t)(data>>8);
	*(thebuf+1) =	(uint8_t)data;
}

uint16_t mp3_get_checksum (uint8_t *thebuf) {
	uint16_t sum = 0;
	for (int i=1; i<7; i++) {
		sum += thebuf[i];
	}
	return -sum;
}

void mp3_fill_checksum () {
	uint16_t checksum = mp3_get_checksum (send_buf);
	fill_uint16_bigend (send_buf+7, checksum);
}

void mp3_send_cmd (uint8_t cmd, uint16_t arg) {
	send_buf[3] = cmd;
	fill_uint16_bigend ((send_buf+5), arg);
	mp3_fill_checksum ();
	send_func ();
}

void mp3_send_cmd_s (uint8_t cmd) {
	send_buf[3] = cmd;
	fill_uint16_bigend ((send_buf+5), 0);
	mp3_fill_checksum ();
	send_func ();
}

void mp3_play (uint16_t num) {
	mp3_send_cmd (0x12, num);
}

void mp3_stop () {
	mp3_send_cmd_s (0x16);
}

void mp3_single_loop (bool state) {
	mp3_send_cmd (0x19, !state);
}

//add 
void mp3_single_play (uint16_t num) {
	mp3_play (num);
	//nrf_delay_ms (10);
	mp3_single_loop (true); 
	//mp3_send_cmd (0x19, !state);
}

void mp3_set_volume (uint16_t volume) {
	mp3_send_cmd (0x06, volume);
}
