/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
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

/**
 * @defgroup kernel-mailbox Mailbox Facility
 * @ingroup kernel
 *
 * @brief Mailbox Facility
 */

#ifndef NANVIX_KPORTAL_H_
#define NANVIX_KPORTAL_H_

	#include <nanvix/hal.h>
	#include <nanvix/const.h>
    #include <nanvix/hlib.h>
	#include <posix/stdarg.h>
    #include <posix/errno.h>

    /**
    * @brief Composes the logic address based on @p fd @p port.
    */
    #define DO_LADDRESS_COMPOSE(fd, port) (fd * KPORTAL_PORT_NR + port)

    EXTERN int do_portal_alloc(int, int, int, int);
    EXTERN int do_portal_release(int);
    EXTERN ssize_t do_portal_aread(int, const struct comm_config *, struct pstats *);
    EXTERN ssize_t do_portal_awrite(int, const struct comm_config *, struct pstats *);
    EXTERN int do_portal_wait(int, const struct comm_config *, struct pstats *);

    EXTERN void do_portal_init(void);

#endif /* NANVIX_KMAILBOX_H_ */

/**@}*/