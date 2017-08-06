#include "UART.h"

/**
 * Initialize the UART module with given baud rate and speed mode
 * @param baud_rate
 * @param BRGH speed mode (high speed or low speed)
 */
void UARTInit(const uint32_t baud_rate, const uint8_t BRGH) {
    // Calculate BRG
    if (BRGH == 0) {
        SPBRG = _XTAL_FREQ/(64*baud_rate) - 1;
        TXSTAbits.BRGH = 0;
    } else {
        SPBRG = _XTAL_FREQ/(16*baud_rate) - 1;
        TXSTAbits.BRGH = 1;
    }
   
    // TXSTA register
    TXSTAbits.TX9 = 0;      // 8-bit transmission
    TXSTAbits.TXEN = 1;     // Enable transmission
    TXSTAbits.SYNC = 0;     // Asynchronous mode
    
    // RCSTA register
    RCSTAbits.SPEN = 1;     // Enable serial port
    RCSTAbits.RX9 = 0;      // 8-bit reception
    RCSTAbits.CREN = 1;     // Enable continuous reception
    RCSTAbits.FERR = 0;     // Disable framing error
    RCSTAbits.OERR = 0;     // Disable overrun error
    
    // Set up direction of RX/TX pins
    UART_TRIS_RX = 1;
    UART_TRIS_TX = 0;
}

/**
 * Send a character
 * @param c character to send
 */
void UARTSendChar(const char c) {
    while (TXSTAbits.TRMT == 0);    // Wait for buffer to be empty
    TXREG = c;
}

/**
 * Send a string, until '\0' is read, up to max_length characters
 * @param str
 * @param max_length
 */
void UARTSendString(const char* str, const uint8_t max_length) {
    int i = 0;
    for (i=0 ; i<max_length && str[i]!='\0' ; i++) {
        UARTSendChar(str[i]);
    }
}

/**
 * Determines whether a new data is available to read
 * @return 
 */
uint8_t UARTDataReady() {
    return PIR1bits.RCIF;
}

/**
 * Read one character
 * @return  byte read
 */
char UARTReadChar() {
    while (!UARTDataReady());   // Wait for data to be available
    return RCREG;
}

/**
 * Read a string, until we read '\0', up to max_length characters. 
 * Null termination will use 1 byte, max_length must be set accordingly.
 * @param buf   pre-allocated buffer
 * @param max_length
 * @return number of bytes read
 */
uint8_t UARTReadString(char *buf, uint8_t max_length) {
    uint8_t i = 0;
    char tmp = 1;
    for (i=0 ; i<max_length-1 ; i++) {
        tmp = UARTReadChar();
        // Stop reading if end of string is read
        if (tmp == '\0' || tmp == '\n' || tmp == '\r') {
            break;
        }
        buf[i] = tmp;
    }
    
    buf[i+1] = '\0';
    
    return i;
}
