#include "serial_data_transfer.h"

void SerialDataTransfer::init(HardwareSerial &_uart_handler, uint32_t _baud_rate, int _format, uint8_t _start_byte, uint8_t _finish_byte)
{
    pUART = &_uart_handler;
    start_byte = _start_byte;
    finish_byte = _finish_byte;
    pUART->begin(_baud_rate, _format);
}

int SerialDataTransfer::set_rx_buffer(size_t _dest_package_size)
{
    num_of_bytes_to_receive = _dest_package_size;
    buf = new uint8_t[num_of_bytes_to_receive];
    if (buf != NULL)
    {
        return 0;
    }
    return -1;
}

int SerialDataTransfer::receive(void *_dest)
{
    uart_data_status = DATA_NOT_YET_READY;
    while (pUART->available() >= (num_of_bytes_to_receive + 2))
    {
        if ((uint8_t)(pUART->read()) == start_byte) // Check for whether the start byte is correct
        {
            pUART->readBytes(buf, num_of_bytes_to_receive); // Copy data bytes from serial buffer

            if ((uint8_t)(pUART->read()) == finish_byte) // Check for whether the finish byte is correct
            {
                uart_data_status = DATA_OK;

                /* Copy Data */
                memcpy(_dest, buf, num_of_bytes_to_receive);
            }
            else
            {
                uart_data_status = DATA_LOST_FINISH_BYTE;
            }
        }
        else
        {
            uart_data_status = DATA_LOST_BEGIN_BYTE;
        }
    }
    return uart_data_status;
}

int SerialDataTransfer::transmit(void *_src, int num_of_bytes_to_transmit)
{
    if (pUART->availableForWrite() >= num_of_bytes_to_transmit )
    {
        pUART->write(&start_byte, 1);
        pUART->write((uint8_t *)_src, num_of_bytes_to_transmit);
        pUART->write(&finish_byte, 1);
        // pUART->flush();

        return 0;
    }
    return -1;
}