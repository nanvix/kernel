/*
 * MIT License
 *
 * Copyright(c) 2011-2016 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#ifndef TYPES_H_
#define TYPES_H_

#ifndef _ASM_FILE_

	#include <stdint.h>
	
	/* Used for system times in clock ticks. */
	typedef int clock_t;

	/* Used for device IDs. */
	typedef unsigned dev_t;
	
	/* Used for disk addresses. */
	typedef unsigned daddr_t;
	
	/* Used as a general identifier. */
	typedef int id_t;
	
	/* Used for some file attributes. */
	typedef int gid_t;
	
	/* Used for some file attributes. */
	typedef int mode_t;
	
	/* Used for file sizes. */
	typedef signed off_t;

	/* Used for process IDs and process group IDs. */
	typedef signed pid_t;
	
	/* Used for time in seconds. */
	typedef signed time_t;

	/* Used for file serial numbers. */
	typedef uint16_t ino_t;

	/* Used for link counts. */
	typedef int nlink_t;

	/* Used for sizes of objects. */
	typedef unsigned size_t;
	
	/* Used for a count of bytes or an error indication. */
	typedef signed ssize_t;
	
	/* Used for user IDs. */
	typedef int uid_t;
	
#endif /* _ASM_FILE */

#endif /* TYPES_H_ */
