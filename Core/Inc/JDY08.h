
#ifndef INC_JDY08_H_
#define INC_JDY08_H_
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
/*-------USER CONFIG AREA----------*/

#include "stm32f7xx_hal.h"   /*Change this header according to the MCU used : "stm32[SERIES]xx.h" */
#define RSSI_D0        (-56) /*The RSSI value at a distance of 1 meter, can be obtained by using the LightBlue app*/
#define RING_BUFFER_SIZE 64  /*Size of the buffer used to store the data received from the JDY08 module via bluetooth*/
#define TEMP_BUFFER_SIZE 32  /*Size of the buffer used to store temporaily the incoming data*/
#define JDY08_UART       USART2
/*Change this handle according to your peripheral*/
extern UART_HandleTypeDef huart2;
#define UART_HANDLE &huart2

/*---------------------------------*/
typedef struct
{
	uint8_t ring_buf[RING_BUFFER_SIZE];
	uint8_t temp_buf[TEMP_BUFFER_SIZE];
	uint16_t head;
	uint16_t tail;
	bool rx_unlocked;

}RingBuf_TypeDef;
bool buffer_full(void);
void JDY08_init(void);

bool buffer_empty(void);
bool is_response(char*);
void copy_after(char* str,char* dest_buf,uint16_t dest_len);
bool copy_between(char* str1, char* str2, char* dest_buf,uint16_t len);
bool get_after(char* string,char* dest_buf,uint16_t len);

bool copy_until(char* str_delim,char* dest_buf,uint16_t len);
uint16_t get_buff_size(void);
void clear_ring_buf(void);
void store_buffer(uint8_t* src_buf,uint16_t len);
int32_t check_for_string(char* str);
/*Use these functins for receiving and sending data to the module*/
bool JDY08_Write(char* buf,uint16_t len);
bool JDY08_Read(char* buf, uint16_t len);
/*****************************************************************/
float rssi_get_n_factor(int16_t rssi_value,uint8_t dist);
float rssi_get_distance(float n_factor,int16_t rssi_value);
#endif /* INC_JDY08_H_ */
