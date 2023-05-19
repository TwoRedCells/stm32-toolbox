/**
 * \file       devices/displays/Hd44780.h
 * \class      Hd44780
 * \brief      Interface to a Hitachi HD44780-based LCD display.
 */

#ifndef INC_OUTPUTS_Hd44780_H_
#define INC_OUTPUTS_Hd44780_H_

#include "utility/PrintLite.h"


class Hd44780 : public Print
{
private:
    GPIO_TypeDef  *rs_port, *rw_port, *en_port, *d4_port, *d5_port, *d6_port, *d7_port;
    uint16_t rs_pin, rw_pin, en_pin, d4_pin, d5_pin, d6_pin, d7_pin;
    static constexpr uint8_t CLEAR_DISPLAY = 0x01;
    static constexpr uint8_t CURSOR = 0x10;
    static constexpr uint8_t CURSOR_CURSOR_SHIFT = 0x00;
    static constexpr uint8_t CURSOR_DISPLAY_SHIFT = 0x08;
    static constexpr uint8_t CURSOR_DISPLAY_SHIFT_LEFT = 0x00;
    static constexpr uint8_t CURSOR_DISPLAY_SHIFT_RIGHT = 0x04;
    static constexpr uint8_t CURSOR_HOME = 0x02;
    static constexpr uint8_t DISPLAY = 0x08;
    static constexpr uint8_t DISPLAY_BLINK = 0x01;
    static constexpr uint8_t DISPLAY_CURSOR = 0x02;
    static constexpr uint8_t DISPLAY_ONOFF = 0x04;
    static constexpr uint8_t ENTRY = 0x04;
    static constexpr uint8_t ENTRY_MOVE_RIGHT = 0x02;
    static constexpr uint8_t ENTRY_MOVE_LEFT = 0x00;
    static constexpr uint8_t ENTRY_SHIFT = 0x01;
    static constexpr bool RW_WRITE = 0x00;
    static constexpr bool RW_READ = 0x01;
    static constexpr uint8_t FUNCTION = 0x20;
    static constexpr uint8_t FUNCTION_INTERFACE_8BIT = 0x10;
    static constexpr uint8_t FUNCTION_INTERFACE_4BIT = 0x00;
    static constexpr uint8_t FUNCTION_SINGLE_LINE = 0x00;
    static constexpr uint8_t FUNCTION_MULTIPLE_LINES = 0x08;
    static constexpr uint8_t FUNCTION_FONT_SMALL = 0x00;
    static constexpr uint8_t FUNCTION_FONT_LARGE = 0x04;
    static constexpr uint8_t SET_DDRAM_ADDRESS = 0x80;
    static constexpr uint8_t SET_CGRAM_ADDRESS = 0x40;

public:
    uint8_t column_count;
    uint8_t row_count;


    /**
     * Initializes the hardware interface.
     */
    void setup(uint8_t rows=4, uint8_t columns=20)
    {
        row_count = rows;
        column_count = columns;

        this->rs_port = LCD_RS_GPIO_Port;
        this->rs_pin = LCD_RS_Pin;
        this->rw_port = LCD_RW_GPIO_Port;
        this->rw_pin = LCD_RW_Pin;
        this->en_port = LCD_EN_GPIO_Port;
        this->en_pin = LCD_EN_Pin;
        this->d4_port = LCD_EN_GPIO_Port;
        this->d4_pin = LCD_D0_Pin;
        this->d5_port = LCD_EN_GPIO_Port;
        this->d5_pin = LCD_D1_Pin;
        this->d6_port = LCD_EN_GPIO_Port;
        this->d6_pin = LCD_D2_Pin;
        this->d7_port = LCD_EN_GPIO_Port;
        this->d7_pin = LCD_D3_Pin;

        delay_ms(100L);
        HAL_GPIO_WritePin(this->d4_port, this->d4_pin, LOW);
        HAL_GPIO_WritePin(this->d5_port, this->d5_pin, LOW);
        HAL_GPIO_WritePin(this->d6_port, this->d6_pin, LOW);
        HAL_GPIO_WritePin(this->d7_port, this->d7_pin, LOW);

        set_rs(LOW);
        set_rw(RW_WRITE);
        set_enable(LOW);
        delay_ms(1);
        reset();

        // 0x04 = F (font) 0 is 5x8, 1 is 5x10
        // 0x08 = N (duty) lines 0 is 1, 1 is 2?
        // 0x10 = DL (data length) 0 is 4, 1 is 8
        // 0x20 = Set the above
        send(FUNCTION | FUNCTION_FONT_SMALL | FUNCTION_MULTIPLE_LINES | FUNCTION_INTERFACE_4BIT);
        send(DISPLAY | DISPLAY_ONOFF | DISPLAY_CURSOR | DISPLAY_BLINK); // display
        send(ENTRY | ENTRY_MOVE_RIGHT); // entry

        // Ready for data.
        set_rs(HIGH);
       // custom_characters();
    }


    /**
     * Clears the screen.
     */
    void clear(void)
    {
        set_rs(LOW);
        send(CLEAR_DISPLAY);
        delay_ms(50);
        set_rs(HIGH);
    }

    /**
     * Clears the row with the specified index.
     * @param row The row to clear.
     */
    void clear_row(uint8_t row)
    {
        clear_to_eol(row, 0);
    }

    /**
     * Clears from the specified position to the end of the line.
     * @param row The row to clear.
     * @param column The column to start clearing.
     */
    void clear_to_eol(uint8_t row, uint8_t column)
    {
        move(row, column);
        for (uint8_t i=0; i<column_count-column; i++)
            send(0x20);
    }

    /**
     * Set the cursor position to the upper-left corner.
     */
    void home(void)
    {
        send_command(CURSOR_HOME);
        delay_ms(10);
    }

    /**
     * Writes a character at the current cursor position.
     * @param s character to write.
     */
    void write(const uint8_t s)
    {
        send(s);
    }

    void scroll_down(void)
    {
    	for (int i=0; i<column_count; i++)
    	{
    		set_rw(RW_READ);
    		move(0, i);
    		uint8_t c = receive();
    		set_rw(RW_WRITE);
    		move(1, i);
    		send(c);
    	}
    }

    /**
     * Prints formatted text at the specified position.
     * @param row The row position.
     * @param column The column position.
     * @param f The format string.
     * @param args The variadic arguments.
     */
    template<typename... Args>
    void printat(uint8_t row, uint8_t column, const char *format, Args... args)
    {
        move(row, column);
        printf(format, args...);
    }

    /**
     * Prints formatted text at the specified position.
     * @param f The format string.
     * @param args The variadic arguments.
     */
    template<typename... Args>
    void print(const char *format, Args... args)
    {
        printf(format, args...);
    }


    /**
     * Prints a numeric value at the specified position.
     * @param row The row position.
     * @param column The column position.
     * @param value The numeric value.
     * @param base The base of the value.
     */
    void printat(uint8_t row, uint8_t column, uint32_t value, uint8_t base=HEX)
    {
        move(row, column);
        return Print::print(value, base);
    }

    /**
     * Moves the cursor position to the specified location.
     * @param row The row position.
     * @param column The column position.
     */
    void move(uint8_t row, uint8_t column)
    {
        const uint8_t offsets[] = { 0, 64, 20, 84 };
        set_ddram_address(offsets[row] + column);
    }


    /**
     * Shifts the display by the specified distance.
     * @note A positive value will shift right, negative left.
     * @param distance The number of spaces to shift.
     */
    void shift_display(int8_t distance)
    {
    	uint8_t direction = distance > 0 ? CURSOR_DISPLAY_SHIFT_RIGHT : CURSOR_DISPLAY_SHIFT_LEFT;
    	if (distance < 0)
    		distance = 0-distance;
    	for(int i=0; i<distance; i++)
    		send_command(CURSOR | CURSOR_DISPLAY_SHIFT | direction);
    }



    /**
     * Shifts the cursor by the specified distance.
     * @note A positive value will shift right, negative left.
     * @param distance The number of spaces to shift.
     */
    void shift_cursor(int8_t distance)
    {
    	uint8_t direction = distance > 0 ? CURSOR_DISPLAY_SHIFT_RIGHT : CURSOR_DISPLAY_SHIFT_LEFT;
    	if (distance < 0)
    		distance = 0-distance;
    	for(int i=0; i<distance; i++)
    		send_command(CURSOR | CURSOR_CURSOR_SHIFT | direction);
    }

    /**
     * Sets the style and visibility of the cursor.
     * @param onoff true to display the cursor, false to hide it.
     * @param blink true to blink the cursor, false to make it solid.
     */
    void set_cursor(bool onoff, bool blink=false)
    {
        uint8_t cursor = DISPLAY | DISPLAY_ONOFF;
        if (onoff)
            cursor |= DISPLAY_CURSOR;
        if (blink)
            cursor |= DISPLAY_BLINK;
        send_command(cursor);
    }

    /**
     * Sends a command to the LCD controller.
     * @param command The command to send.
     */
    void send_command(uint8_t command)
    {
        set_rs(LOW);
        send(command);
        set_rs(HIGH);
    }

private:
    /**
     * Sends 8 bits to the LCD controller.
     * @param value The byte to send.
     */
    void send(uint8_t value)
    {
        delay_ms(1);
        send4(value >> 4);
        send4(value);
    }

    /** Sends 4 bits to the LCD controller.
     * @param value The nibble to send.
     */
    void send4(uint8_t value)
    {
    	HAL_GPIO_WritePin(this->d4_port, this->d4_pin, (GPIO_PinState)((value >> 0) & 0x01));
    	HAL_GPIO_WritePin(this->d5_port, this->d5_pin, (GPIO_PinState)((value >> 1) & 0x01));
    	HAL_GPIO_WritePin(this->d6_port, this->d6_pin, (GPIO_PinState)((value >> 2) & 0x01));
    	HAL_GPIO_WritePin(this->d7_port, this->d7_pin, (GPIO_PinState)((value >> 3) & 0x01));
        commit();
    }


    /**
     * Reads 8 bits from the LCD controller.
     * @bug Not tested.
     * @return The byte of data.
     */
    uint8_t receive(void)
    {
    	//set_rw(RW_READ);
    	uint8_t c = (receive4() << 4) | (receive4() << 0);
    	//set_rw(RW_WRITE);
    	return c;
    }


    /**
     * Reads 4 bits from the LCD controller.
     * @bug Not tested.
     * @return The byte of data.
     */
    uint8_t receive4(void)
    {
    	uint8_t value = 0;
    	//commit();
    	value |= HAL_GPIO_ReadPin(this->d4_port, this->d4_pin) << 0;
    	value |= HAL_GPIO_ReadPin(this->d5_port, this->d5_pin) << 1;
    	value |= HAL_GPIO_ReadPin(this->d6_port, this->d6_pin) << 2;
    	value |= HAL_GPIO_ReadPin(this->d7_port, this->d7_pin) << 3;
    	return value;
    }


    /**
     * Sets the DDRAM address, at which the next output will display.
     * @param address The device-specific address.
     */
    void set_ddram_address(uint8_t address)
    {
        send_command(SET_DDRAM_ADDRESS | address);
    }


    /**
     * Sends a reset sequence to the LCD controller.
     */
    void reset(void)
    {
        send4(0x3);
        delay_ms(10);
        send4(0x3);
        delay_ms(10);
        send4(0x3);
        delay_ms(10);
        send4(0x2);
        delay_ms(10);
    }


    /**
     * Toggles the enable pin to indicate to the LCD controller that data is ready for it to process.
     */
    void commit(void)
    {
        set_enable(HIGH);
        delay_ms(1);
        set_enable(LOW);
    }


    /**
     * Sets the enable pin.
     * @param state true for HIGH, false for LOW.
     */
    void set_enable(bool state)
    {
    	HAL_GPIO_WritePin(this->en_port, this->en_pin, (GPIO_PinState)state);
    }


    /**
     * Sets the register select pin.
     * @param state true for HIGH, false for LOW.
     */
    void set_rs(bool state)
    {
    	HAL_GPIO_WritePin(this->rs_port, this->rs_pin, (GPIO_PinState)state);
    }


    /**
     * Sets the read/write pin.
     * @param state true for read, false for write.
     */
    void set_rw(bool state)
    {
    	HAL_GPIO_WritePin(this->rw_port, this->rw_pin, (GPIO_PinState)state);
    }


    /**
     * Waits for the specified duration, blocking all other instructions.
     * @param ms The time to wait in microseconds.
     */
    void delay_ms(uint32_t ms)
    {
    	osDelay(ms);
    }


    /**
     * Defines some custom characters, currently unused.
     */
    void custom_characters(void)
    {
//        const byte customChar1[] = {
//          B00000,
//          B11111,
//          B10000,
//          B10000,
//          B10000,
//          B10000,
//          B11111,
//          B00000
//        };
//        const byte customChar2[] = {
//          B00001,
//          B11111,
//          B00000,
//          B00000,
//          B00000,
//          B00000,
//          B11111,
//          B00001
//        };
//        const byte customChar3[] = {
//          B00000,
//          B11111,
//          B00000,
//          B00000,
//          B00000,
//          B00000,
//          B11111,
//          B00000
//        };
//        set_rs(LOW);
//        send(0x40);
//        delay_ms(50);
//        set_rs(HIGH);
//        for (int i=0; i<8; i++)
//            send(customChar1[i]);

    }
};

#endif /* INC_OUTPUTS_LCD_H_ */

