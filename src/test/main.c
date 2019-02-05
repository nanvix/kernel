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

#include <nanvix.h>

/**
 * @brief Number of threads to spawn.
 */
#define NTHREADS 4

/**
 * @brief Returns the length of a string.
 *
 * @param str String to be evaluated.
 *
 * @returns The length of the string.
 */
static size_t strlen(const char *str)
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
static void puts(const char *str)
{
	size_t len;

	len = strlen(str);

	nanvix_write(0, str, len);
}

/**
 * @brief Says hello to the world.
 *
 * @param str String to print.
 */
static void *task(void *str)
{
	puts(str);

	return (NULL);
}

/**
 * @brief Says many hellos.
 *
 * @param argc Argument counter.
 * @param argv Argument variables.
 */
int main(int argc, const char *argv[])
{
	char *strings[NTHREADS] = {
		"hello from thread 0!\n",
		"hello from thread 1!\n",
		"hello from thread 2!\n",
		"hello from thread 3!\n"
	};

	kthread_t tid[NTHREADS];

	((void) argc);
	((void) argv);

	/* Spawn threads. */
	for (int i = 1; i < NTHREADS; i++)
		kthread_create(&tid[i], task, strings[i]);

	task(strings[1]);

	/* Wait for threads. */
	for (int i = 1; i < NTHREADS; i++)
		kthread_join(tid[i], NULL);

	return (0);
}
