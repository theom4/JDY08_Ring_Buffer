#include "JDY08.h"

RingBuf_TypeDef BT_RingBuf;
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

	if(huart ->Instance == JDY08_UART)/*Callback triggered for bluetooth port*/
	{
		store_buffer(BT_RingBuf.temp_buf,Size);
		BT_RingBuf.rx_unlocked = true;
		 HAL_UARTEx_ReceiveToIdle_DMA(UART_HANDLE,BT_RingBuf.temp_buf, RING_BUFFER_SIZE);
                __HAL_DMA_DISABLE_IT(DMA_HANDLE, DMA_IT_HT);
	}

}
bool JDY08_transmit(char* str, uint16_t len)
{
	int status = HAL_UART_Transmit(UART_HANDLE,(uint8_t*)str,len,100);
	if(status == HAL_OK) return true;
			else return false;
}
bool JDY08_getData(char* str, uint16_t len)
{
	if(buffer_empty() == 1)
	{
		return false;
	}
	for(uint16_t idx = 0; idx < len; idx ++)
	{
		str[idx] = BT_RingBuf.ring_buf[BT_RingBuf.tail];
		BT_RingBuf.tail = (BT_RingBuf.tail + 1) % RING_BUFFER_SIZE;

	}
	return true;
}

void JDY08_init(void)
{
	clear_ring_buf();
	BT_RingBuf.rx_unlocked = true;
	HAL_UARTEx_ReceiveToIdle_DMA(UART_HANDLE,BT_RingBuf.temp_buf, RING_BUFFER_SIZE);
       __HAL_DMA_DISABLE_IT(DMA_HANDLE, DMA_IT_HT);
}
void clear_ring_buf(void)
{
        memset(BT_RingBuf.ring_buf,'0',RING_BUFFER_SIZE * sizeof(BT_RingBuf.ring_buf[0]));
	BT_RingBuf.head = 0;
	BT_RingBuf.tail = 0;
	
}
uint16_t get_buff_size(void)
{
	uint16_t elem_num = (BT_RingBuf.head - BT_RingBuf.tail) % RING_BUFFER_SIZE;
	if(elem_num < 0)
	{
		elem_num += RING_BUFFER_SIZE;
	}
	return elem_num;
}
bool buffer_empty(void)
{
	if(BT_RingBuf.head == BT_RingBuf.tail)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool buffer_full(void)
{
	uint16_t next = (BT_RingBuf.head + 1) % RING_BUFFER_SIZE;
	if(BT_RingBuf.tail == next)
	{
		return true;
	}
	else
	{
		return false;
	}
}
/* moves the tail to the first occurence of the specified string.
 * If this is found,returns 1, otherwise 0
 * USE IT ONLY WHEN SEARCHING FOR A SINGLE STRING WITHIN THE BUFFER, THE REMAINING STRINGS WILL BE "DELETED" */
bool is_response(char* str)
{
	if(buffer_empty() == true)
	{
		return false;
	}
	uint16_t len = strlen(str);
	uint16_t count = 0;

	while(buffer_empty() == false)
	{
		while(BT_RingBuf.ring_buf[BT_RingBuf.tail] == str[count])
		{
			BT_RingBuf.tail = (BT_RingBuf.tail + 1) % RING_BUFFER_SIZE;
			count++;
		}
	    if(count == len)
	    {
	    	return true;
	    }
	    else
	    {
	    	count = 0;
	    	BT_RingBuf.tail = (BT_RingBuf.tail + 1) % RING_BUFFER_SIZE;
	    }
	}
    return false;

}
/*brief Stores the received data via UART from the JDY module into the Ring Buffer*/
void store_buffer(uint8_t* src_buf,uint16_t len)
{
	for(uint16_t i = 0;i< len ;i++)
	{
		BT_RingBuf.ring_buf[BT_RingBuf.head] = src_buf[i];
		BT_RingBuf.head = (BT_RingBuf.head + 1) % RING_BUFFER_SIZE;
	}
}

/*@brief copies the data from the Ring Buffer into the input buffer
 * @param "len" is the number of elements to be copied
 * @retval true if the number of elements copied == len otherwise false*/
bool get_after(char* string,char* dest_buf,uint16_t len)
{
	while(!is_response(string))
	{
		//...
	}
	for(uint16_t idx = 0;idx < len ;idx++)
	{
		if(buffer_empty() == 1)
		{
			return false; // couldn't copy all the data
		}
		dest_buf[idx] = BT_RingBuf.ring_buf[BT_RingBuf.tail];
		BT_RingBuf.tail = (BT_RingBuf.tail + 1) % RING_BUFFER_SIZE;
	}
	return true;
}
/* @brief checks for the occurence of a particular response in the buffer,
 * @retval  num of chars until the given string, if the string is not found -1 is returned*/

int32_t check_for_string(char* str)
{
	if(buffer_empty() == 1)
	{
		return -1;
	}
	uint16_t len = strlen(str);
	uint16_t so_far = 0;
	uint16_t ring_index = BT_RingBuf.tail;
	//bool string_found = false;
	uint16_t len_until_str = 0;/*no of bytes until the given string*/
	while(so_far != len && ring_index != BT_RingBuf.head)
	{
		while((BT_RingBuf.ring_buf[ring_index] != str[so_far]) && (ring_index != BT_RingBuf.head))
		{
			len_until_str ++;
			ring_index = (ring_index + 1) % RING_BUFFER_SIZE;
		    if(ring_index == BT_RingBuf.head)
		    {
		        //string_found = false;
		    	//return string_found; // we've searched through all the elements
		    	return -1;
		    }
		}
		if(ring_index == BT_RingBuf.head)return -1;

		if(BT_RingBuf.ring_buf[ring_index] == str[so_far])
		{
			while(BT_RingBuf.ring_buf[ring_index] == str[so_far])
			{

				so_far ++;
				ring_index = (ring_index + 1) % RING_BUFFER_SIZE;
			}
		}
		if(so_far != len)
		{

			len_until_str = len_until_str + so_far;/*str not found here, hence update the len*/
			so_far = 0;
		}
		else
		{

			//BT_RingBuf.tail = BT_RingBuf.head;/*Update the tail,the string was found*/
			return len_until_str;
			//string_found = true;
			//return string_found;
		}

	}
	//string_found = false;
	//return string_found;
	return -1;/*str not found*/
}

/*@brief Copies the data from the Ring Buffer into the input buffer between 2 specified strings
 *@param "len" is the length of the input buffer
 *@retval true if the length between the strings < length of the input buffer otherwise false*/
bool copy_between(char* start_string, char* end_string, char* dest_buf,uint16_t len)
{
	while(!is_response(start_string))
	{
		//Wait here till the first string appears
	}
	uint16_t len_between = check_for_string(end_string);
	if(len_between == -1) return false; //There is no end_string
	if(len_between > len)
	{
		strncpy(dest_buf,(char*)(BT_RingBuf.ring_buf + BT_RingBuf.tail),len);
		BT_RingBuf.tail = (BT_RingBuf.tail + len) % RING_BUFFER_SIZE;
		return false; // Could not copy all the data
	}
	else
	{
		strncpy(dest_buf,(char*)(BT_RingBuf.ring_buf + BT_RingBuf.tail),len_between);
		BT_RingBuf.tail = (BT_RingBuf.tail + len_between) % RING_BUFFER_SIZE;
		return true;//all the data has been copied
	}
}

/*Functions used for getting the distance between the phone and the module (a rough estimation)*/
/*This function should be used at first in order to get the n factor*/
float rssi_get_n_factor(int16_t rssi_value,uint8_t dist)
{
    float n_factor = (RSSI_D0 - rssi_value) / (10.0f * log10(dist));
	return n_factor;
}
float rssi_get_distance(float n_factor,int16_t rssi_value)
{
	float distance = pow(10,(RSSI_D0 - rssi_value) / (n_factor * 10.0f));
	return distance;
}
/*******************************************/
/* @brief Copies the data from the Ring Buffer into the input buffer
 * @param The specified string
 * @param The input buffer
 * @param The length of the input buffer
 * %retval true if more than one element has been copied otherwise false*/
bool copy_until(char* str_delim,char* dest_buf,uint16_t len)
{
	if(buffer_empty() == 1)
	{
		return false;
	}
	int32_t idx2 = 0;
	//int32_t len2 = len;
	int32_t len_until_str = check_for_string(str_delim);
	//uint16_t start_tail = BT_RingBuf.tail;
	if(len_until_str == -1) return false; /*string not found*/

	for(int32_t idx = 0; idx < len_until_str; idx++)
	{
		dest_buf[idx2 ++] = BT_RingBuf.ring_buf[BT_RingBuf.tail];
		BT_RingBuf.tail = (BT_RingBuf.tail + 1) % RING_BUFFER_SIZE;
        if(idx2 == len && idx < len_until_str)
        {
        	BT_RingBuf.tail = (BT_RingBuf.tail + len_until_str - len) % RING_BUFFER_SIZE;
        	return true;
        }
	}
	/*no of elements copied is %len_until_str,update the tail*/
	BT_RingBuf.tail = (BT_RingBuf.tail + strlen(str_delim)) % RING_BUFFER_SIZE;
	return true;
}

bool string_present(char* str)
{
	int16_t len_until_str = check_for_string(str);
	uint16_t len = strlen(str);
	if(len_until_str != -1)
	{
		BT_RingBuf.tail = (BT_RingBuf.tail + len_until_str + len) % RING_BUFFER_SIZE;
		return 1;
	}
	else return 0;
}
















