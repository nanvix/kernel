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
#include "test.h"

/*============================================================================*
 * strlen()                                                                   *
 *============================================================================*/

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

/*============================================================================*
 * puts()                                                                     *
 *============================================================================*/

/**
 * The puts() function writes to the standard output device the string
 * pointed to by @p str.
 */
void puts(const char *str)
{
	size_t len;

	len = strlen(str);

	nanvix_write(0, str, len);
}

/*============================================================================*
 * main()                                                                     *
 *============================================================================*/

/**
 * @brief User-land testing units.
 */
static struct
{
	void (*test_fn)(void); /**< Test function. */
	const char *name;      /**< Test Name.     */
} user_tests[] = {
	{ test_api_kthread_self,      "[test][user][api]    thread identification       [passed]\n" },
	{ test_api_kthread_create,    "[test][user][api]    thread creation/termination [passed]\n" },
	{ test_fault_kthread_create,  "[test][user][fault]  thread creation/termination [passed]\n" },
	{ test_stress_kthread_create, "[test][user][stress] thread creation/termination [passed]\n" },
	{ NULL,                        NULL                                                         },
};

/**
 * @brief Lunches user-land testing units.
 *
 * @param argc Argument counter.
 * @param argv Argument variables.
 */
int main(int argc, const char *argv[])
{
	((void) argc);
	((void) argv);

	/* Launch tests. */
	for (int i = 0; user_tests[i].test_fn != NULL; i++)
	{
		user_tests[i].test_fn();
		puts(user_tests[i].name);
	}

	return (0);
}
