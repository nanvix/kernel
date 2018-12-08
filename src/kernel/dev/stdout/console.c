/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <nanvix/hal/hal.h>
#include <driver/console.h>
#include <nanvix/const.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Hardware cursor.
 */
PRIVATE struct 
{
	int x; /**< Horizontal axis position. */
	int y; /**< Vertical axis position.   */
} cursor;

/**
 * @brief Video memory.
 */
PRIVATE uint16_t *video = (uint16_t*)VIDEO_ADDR;

/**
 * @brief Moves the hardware cursor.
 *
 * Moves the hardware cursor of the console device..
 */
PRIVATE void cursor_move(void)
{
	uint16_t cursor_location = cursor.y*VIDEO_WIDTH + cursor.x;
	
	hal_outputb(VIDEO_CRTL_REG, VIDEO_CLH);
	hal_outputb(VIDEO_DATA_REG, (uint8_t) ((cursor_location >> 8) & 0xFF));
	hal_outputb(VIDEO_CRTL_REG, VIDEO_CLL);
	hal_outputb(VIDEO_DATA_REG, (uint8_t) (cursor_location & 0xFF));
}

/**
 * @brief Scrolls down the console.
 *
 * Scrolls down the console by one row.
 */
PRIVATE void console_scrolldown(void)
{
	uint16_t *p;
	
	/* Pull lines up. */
	for (p = video; p < (video + (VIDEO_HIGH-1)*(VIDEO_WIDTH)); p++)
		*p = *(p + VIDEO_WIDTH);
	
	/* Blank last line. */
	for (; p < video + VIDEO_HIGH*VIDEO_WIDTH; p++)
		*p = (BLACK << 8) | (' ');
		
	/* Set cursor position. */
	cursor.x = 0; cursor.y = VIDEO_HIGH - 1;
}

/**
 * @brief Writes a colered ASCII character on the console device.
 *
 * Writes the ASCII character @p on the console device using @p color.
 *
 * @param ch    Target ASCII character.
 * @param color Target color.
 */
PRIVATE void console_put(uint8_t ch, uint8_t color)
{
	/* Parse character. */
    switch (ch)
    {
        /* New line. */
        case '\n':
            cursor.y++;
            cursor.x = 0;
            break;
            
        /* Tabulation. */
        case '\t':
            /* FIXME. */
            cursor.x += 4 - (cursor.x & 3);
            break;
            
        /* Backspace. */
        case '\b':
            if (cursor.x > 0)
                cursor.x--;
            else if (cursor.y > 0)
            {
                cursor.x = VIDEO_WIDTH - 1;
                cursor.y--;
            }
            video[cursor.y*VIDEO_WIDTH +cursor.x] = (color << 8) | (' ');
            break;			
        
        /* Any other. */
        default:
            video[cursor.y*VIDEO_WIDTH +cursor.x] = (color << 8) | (ch);
            cursor.x++;
            break;
    }

    /* Set cursor position. */
    if (cursor.x >= VIDEO_WIDTH)
    {
        cursor.x = 0;
        cursor.y++;
    }
    if (cursor.y >= VIDEO_HIGH)
        console_scrolldown();
    cursor_move();
}

/**
 * @brief Clears the console.
 *
 * Cleans up the console, so that the screen is completely blank.
 */
PRIVATE void console_clear(void)
{
	/* Blank all lines. */
	for (uint16_t *p = video; p < (video + VIDEO_HIGH*VIDEO_WIDTH); p++)
		*p = (BLACK << 8) | (' ');
	
	/* Set console cursor position. */
	cursor.x = cursor.y = 0;
	cursor_move();
}

/**
 * The console_write() function writes @p n bytes of the data buffer
 * pointer to by @p buf to the console device.
 */
PUBLIC void console_write(const char *buf, size_t n)
{
	for (size_t i = 0; i < n; i++)
		console_put((uint8_t) buf[i], WHITE);
}

/**
 * The console_init() function initializes the console driver. It does
 * so by first setting up the hardware cursor and then cleaning up the
 * video memory of the device itself.
 *
 * @returns A function pointer to the console write function.
 *
 * @see console_write()
 */
PUBLIC void console_init(void)
{
	/* Set cursor shape. */
	hal_outputb(VIDEO_CRTL_REG, VIDEO_CS);
	hal_outputb(VIDEO_DATA_REG, 0x00);
	hal_outputb(VIDEO_CRTL_REG, VIDEO_CE);
	hal_outputb(VIDEO_DATA_REG, 0x1f);
	
	/* Clear the console. */
	console_clear();
}

