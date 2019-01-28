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

#include <nanvix/thread.h>
#include <nanvix/syscall.h>
#include <stddef.h>

/**
 * @brief Number of threads in the program.
 */
#define NTHREADS 16

/* Import definitions. */
extern ssize_t write(int, const char *, size_t);

/**
 * @brief Returns the length of a string.
 * 
 * @param str String to be evaluated.
 * 
 * @returns The length of the string. 
 */
size_t strlen(const char *str)
{
	const char *p;

	/* Count the number of characters. */
	for (p = str; *p != '\0'; p++)
		/* noop */;

	return (p - str);
}

/**
 * @brief Writes a string to the standard output device.
 */
void puts(const char *str)
{
	size_t len;

	len = strlen(str);

	write(0, str, len);
}

/**
 * @brief Thread strings.
 */
static const char *strings[NTHREADS] = {
	"hello from core 0\n",
	"hello from core 1\n",
	"hello from core 2\n",
	"hello from core 3\n",
	"hello from core 4\n",
	"hello from core 5\n",
	"hello from core 6\n",
	"hello from core 7\n",
	"hello from core 8\n",
	"hello from core 9\n",
	"hello from core 10\n",
	"hello from core 11\n",
	"hello from core 12\n",
	"hello from core 13\n",
	"hello from core 14\n",
	"hello from core 15\n"
};

/**
 * @brief Prints a string and syncs.
 */
void *task(void *arg)
{
	puts(arg);

	return (NULL);
}

/**
 * @brief A simple hello world application.
 */
int main(int argc, const char *argv[])
{
	((void) argc);
	((void) argv);

	puts(strings[core_get_id()]);

	for (int i = 2; i < NTHREADS; i++)
	{
		tid_t tid;
		thread_create(&tid, task, (void *) strings[i]);
	}	   

	return (0);
}
