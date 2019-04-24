/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis     <davidsondfgl@gmail.com>
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

#include <nanvix/const.h>
#include <nanvix/mm.h>
#include <nanvix/klib.h>
#include <errno.h>

/**
 * @cond release_test
 */

/*============================================================================*
 * Frame Allocator Unit Tests                                                 *
 *============================================================================*/

/**
 * @brief API Test: Page Frame Address Translation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_api_frame_translation(void)
{
	KASSERT(frame_num_to_id(UBASE_PHYS >> PAGE_SHIFT) == 0);
	KASSERT(frame_id_to_num(0) == (UBASE_PHYS >> PAGE_SHIFT));
}

/**
 * @brief API Test: Page Frame Allocation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_api_frame_allocation(void)
{
	frame_t frame;

	KASSERT((frame = frame_alloc()) != FRAME_NULL);
	KASSERT(frame_free(frame) == 0);
}

/**
 * @brief Fault Injection Test: Invalid Page Frame Address Translation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_frame_invalid_translation(void)
{
	KASSERT(frame_num_to_id(KBASE_VIRT >> PAGE_SHIFT) == -1);
	KASSERT(frame_num_to_id((UBASE_VIRT - PAGE_SIZE) >> PAGE_SHIFT) == -1);
	KASSERT(frame_id_to_num(-1) == FRAME_NULL);
	KASSERT(frame_id_to_num((UMEM_SIZE/PAGE_SIZE) + 1) == FRAME_NULL);
}

/**
 * @brief Fault Injection Test: Invalid Page Frame Release
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_frame_invalid_free(void)
{
	KASSERT(frame_free(0) == -EINVAL);
	KASSERT(frame_free((UBASE_VIRT + UMEM_SIZE) >> PAGE_SHIFT) == -EINVAL);
}

/**
 * @brief Fault Injection Test: Bad Page Frame Release
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_frame_bad_free(void)
{
	KASSERT(frame_free(frame_id_to_num(0)) == -EFAULT);
	KASSERT(frame_free(frame_id_to_num(NUM_UFRAMES - 1)) == -EFAULT);
}

/**
 * @brief Fault Injection Test: Double Page Frame Release
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_frame_double_free(void)
{
	frame_t frame;

	KASSERT((frame = frame_alloc()) != FRAME_NULL);
	KASSERT(frame_free(frame) == 0);
	KASSERT(frame_free(frame) == -EFAULT);
}

/**
 * @brief Fault Injection Test: Page Frame Allocation Overflow
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_frame_allocation_overflow(void)
{
	/* Allocate all page frames. */
	for (frame_t i = 0; i < NUM_UFRAMES; i++)
		KASSERT(frame_alloc() != FRAME_NULL);

	/* Fail to allocate an extra page frame. */
	KASSERT(frame_alloc() == FRAME_NULL);

	/* Release all page frames. */
	for (frame_t i = 0; i < NUM_UFRAMES; i++)
		KASSERT(frame_free(frame_id_to_num(i)) == 0);
}

/**
 * @brief Stress Test: Page Frame Allocation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_stress_frame_allocation(void)
{
	/* Allocate all page frames. */
	for (frame_t  i = 0; i < NUM_UFRAMES; i++)
		KASSERT(frame_alloc() != FRAME_NULL);

	/* Release all page frames. */
	for (frame_t i = 0; i < NUM_UFRAMES; i++)
		KASSERT(frame_free(frame_id_to_num(i)) == 0);
}

/**
 * @brief Page Frame unit tests.
 *
 * Unit tests for the Page Frame Allocator subsystem.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE struct
{
	void (*test_fn)(void); /**< Test function.     */
	const char *type;      /**< Name of test type. */
	const char *name;      /**< Test Name.         */
} frame_tests[] = {
	{ test_api_frame_translation,           "api",    "frame address translation"         },
	{ test_api_frame_allocation,            "api",    "frame allocation"                  },
	{ test_fault_frame_invalid_translation, "fault",  "invalid frame address translation" },
	{ test_fault_frame_invalid_free,        "fault",  "invalid frame release"             },
	{ test_fault_frame_bad_free,            "fault",  "bad frame release"                 },
	{ test_fault_frame_double_free,         "fault",  "double frame release"              },
	{ test_stress_frame_allocation,         "stress", "frame allocation"                  },
	{ test_fault_frame_allocation_overflow, "fault",  "frame allocation overflow"         },
	{ NULL,                                 NULL,     NULL                                },
};

/**
 * @brief Runs unit tests on the Page Frame Allocator.
 *
 * The frame_test_driver() function runs API, fault injection and
 * stress tests on the Page Frame Allocator.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void frame_test_driver(void)
{
	for (int i = 0; frame_tests[i].test_fn != NULL; i++)
	{
		frame_tests[i].test_fn();
		kprintf("[test][%s] %s [passed]", frame_tests[i].type, frame_tests[i].name);
	}
}

/*============================================================================*
 * Kernel Page Pool Unit Tests                                                *
 *============================================================================*/

/**
 * @brief API Test: Kernel Page Address Translation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_api_kpage_translation(void)
{
	KASSERT(kpool_id_to_addr(0) == KPOOL_VIRT);
	KASSERT(kpool_addr_to_id(KPOOL_VIRT) == 0);
	KASSERT(kpool_addr_to_frame(KPOOL_VIRT) == (KPOOL_PHYS >> PAGE_SHIFT));
	KASSERT(kpool_frame_to_addr(KPOOL_PHYS >> PAGE_SHIFT) == KPOOL_VIRT);
}

/**
 * @brief API Test: Kernel Page Allocation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_api_kpage_allocation(void)
{
	void *kpg;

	KASSERT((kpg = kpage_get(0)) != NULL);
	KASSERT(kpage_put(kpg) == 0);
}

/**
 * @brief API Test: Kernel Page Write
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_api_kpage_write(void)
{
	unsigned *kpg;
	const unsigned magic = 0xdeadbeef;

	/* Allocate. */
	KASSERT((kpg = kpage_get(0)) != NULL);

	/* Write. */
	for (size_t i = 0; i < PAGE_SIZE/sizeof(unsigned); i++)
		kpg[i] = magic;
	for (size_t i = 0; i < PAGE_SIZE/sizeof(unsigned); i++)
		KASSERT(kpg[i] == magic);

	/* Release. */
	KASSERT(kpage_put(kpg) == 0);
}

/**
 * @brief API Test: Kernel Page Clean Allocation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_api_kpage_clean_allocation(void)
{
	unsigned *kpg;
	const unsigned magic = 0xdeadbeef;

	/* Make a dirty kernel. */
	KASSERT((kpg = kpage_get(0)) != NULL);
	for (unsigned i = 0; i < PAGE_SIZE/sizeof(unsigned); i++)
		kpg[i] = magic;
	KASSERT(kpage_put(kpg) == 0);

	/* Allocate. */
	KASSERT((kpg = kpage_get(1)) != NULL);

	/* Sanity check. */
	for (unsigned i = 0; i < PAGE_SIZE/sizeof(unsigned); i++)
		KASSERT(kpg[i] == 0);

	/* Release. */
	KASSERT(kpage_put(kpg) == 0);
}

/**
 * @brief Fault Injection Test: Invalid Kernel Page Release
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_kpage_invalid_free(void)
{
	KASSERT(kpage_put((void *)(KPOOL_VIRT - PAGE_SIZE)) == -EINVAL);
	KASSERT(kpage_put((void *)(KPOOL_VIRT + KPOOL_SIZE)) == -EINVAL);
}

/**
 * @brief Fault Injection Test: Bad Kernel Page Release
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_kpage_bad_free(void)
{
	KASSERT(kpage_put((void *) KPOOL_VIRT) == -EFAULT);
}

/**
 * @brief Fault Injection Test: Kernel Page Double Free
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_kpage_double_free(void)
{
	void *kpg;

	KASSERT((kpg = kpage_get(0)) != NULL);
	KASSERT(kpage_put(kpg) == 0);
	KASSERT(kpage_put(kpg) == -EFAULT);
}

/**
 * @brief Fault Injection Test: Kernel Page Allocation Overflow
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_kpage_allocation_overflow(void)
{
	unsigned *kpg;

	/* Allocate all kernel pages. */
	for (unsigned i = 0; i < NUM_KPAGES; i++)
		KASSERT((kpg = kpage_get(0)) != NULL);

	/* Fail to allocate one more page. */
	KASSERT((kpg = kpage_get(0)) == NULL);

	/* Free all kernel pages. */
	for (unsigned i = 0; i < NUM_KPAGES; i++)
		KASSERT(kpage_put((void *)(KPOOL_VIRT + i*PAGE_SIZE)) == 0);
}

/**
 * @brief Stress test: Kernel Page Allocation
 */
PRIVATE void test_stress_kpage_allocation(void)
{
	unsigned *kpg;

	/* Allocate all kernel pages. */
	for (unsigned i = 0; i < NUM_KPAGES; i++)
		KASSERT((kpg = kpage_get(0)) != NULL);

	/* Free all kernel pages. */
	for (unsigned i = 0; i < NUM_KPAGES; i++)
		KASSERT(kpage_put((void *)(KPOOL_VIRT + i*PAGE_SIZE)) == 0);
}

/**
 * @brief Stress Test: Kernel Page Write
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_stress_kpage_write(void)
{
	unsigned *kpg;
	const unsigned magic = 0xdeadbeef;

	/* Allocate all kernel pages. */
	for (unsigned i = 0; i < NUM_KPAGES; i++)
	{
		KASSERT((kpg = kpage_get(1)) != NULL);

		/* Write to kernel page. */
		for (size_t j = 0; j < PAGE_SIZE/sizeof(unsigned); j++)
			kpg[j] = magic;
	}

	/* Free all kernel pages. */
	for (unsigned i = 0; i < NUM_KPAGES; i++)
	{
		kpg = (void *)(KPOOL_VIRT + i*PAGE_SIZE);

		/* Sanity check. */
		for (size_t j = 0; j < PAGE_SIZE/sizeof(unsigned); j++)
			KASSERT(kpg[j] == magic);

		KASSERT(kpage_put(kpg) == 0);
	}
}

/**
 * @brief Kernel Pool unit tests.
 *
 * Unit tests for the Kernel Page Pool subsystem.
 *
 * @author Pedro Henrique Penna.
 */
PRIVATE struct
{
	void (*test_fn)(void); /**< Test function.     */
	const char *type;      /**< Name of test type. */
	const char *name;      /**< Test Name.         */
} kpool_tests[] = {
	{ test_api_kpage_translation,           "api",    "kernel page address translation" },
	{ test_api_kpage_allocation,            "api",    "kernel page allocation"          },
	{ test_api_kpage_write,                 "api",    "kernel page write"               },
	{ test_api_kpage_clean_allocation,      "api",    "kernel page clean allocation"    },
	{ test_fault_kpage_invalid_free,        "fault",  "kernel page invalid release"     },
	{ test_fault_kpage_bad_free,            "fault",  "kernel page bad release"         },
	{ test_fault_kpage_double_free,         "fault",  "kernel page double release"      },
	{ test_stress_kpage_allocation,         "stress", "kernel page allocation"          },
	{ test_fault_kpage_allocation_overflow, "fault",  "kernel page allocation overflow" },
	{ test_stress_kpage_write,              "stress", "kernel page write"               },
	{ NULL,                                 NULL,     NULL                              },
};

/**
 * @brief Runs unit tests on the Kernel Page Pool
 *
 * The kpool_test_driver() function runs API, fault injection and
 * stress tests on the Kernel Page Pool.
 */
PUBLIC void kpool_test_driver(void)
{
	for (int i = 0; kpool_tests[i].test_fn != NULL; i++)
	{
		kpool_tests[i].test_fn();
		kprintf("[test][%s] %s [passed]", kpool_tests[i].type, kpool_tests[i].name);
	}
}

/*============================================================================*
 * Unit Tests for User Page Pool                                              *
 *============================================================================*/

/**
 * @brief API Test: User Page Allocation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_api_upage_allocation(void)
{
	KASSERT(upage_alloc(root_pgdir, UBASE_VIRT) == 0);
	KASSERT(upage_free(root_pgdir, UBASE_VIRT) == 0);
}

/**
 * @brief API Test: User Page Write
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_api_upage_write(void)
{
	unsigned *upg;
	const unsigned magic = 0xdeadbeef;

	upg = (unsigned  *)(UBASE_VIRT);

	/* Allocate.*/
	KASSERT(upage_alloc(root_pgdir, VADDR(upg)) == 0);

	/* Write. */
	for (size_t i = 0; i < PAGE_SIZE/sizeof(unsigned); i++)
		upg[i] = magic;
	for (size_t i = 0; i < PAGE_SIZE/sizeof(unsigned); i++)
		KASSERT(upg[i] == magic);

	/* Unmap. */
	KASSERT(upage_free(root_pgdir, VADDR(upg)) == 0);
}

/**
 * @brief Fault Injection Test: Invalid User Page Allocation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_upage_invalid_allocation(void)
{
	KASSERT(upage_alloc(NULL, UBASE_VIRT) == -EINVAL);
	KASSERT(upage_alloc(root_pgdir, KBASE_VIRT) == -EFAULT);
	KASSERT(upage_alloc(root_pgdir, UBASE_VIRT - PAGE_SIZE) == -EFAULT);
	KASSERT(upage_alloc(root_pgdir, UBASE_VIRT + PAGE_SIZE - 1) == -EINVAL);
}

/**
 * @brief Fault Injection Test: Double User Allocation Test
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_upage_double_allocation(void)
{
	KASSERT(upage_alloc(root_pgdir, UBASE_VIRT) == 0);
	KASSERT(upage_alloc(root_pgdir, UBASE_VIRT) == -EADDRINUSE);
	KASSERT(upage_free(root_pgdir, UBASE_VIRT) == 0);
}

/**
 * @brief Fault Injection Test: Invalid User Page Release
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_upage_invalid_free(void)
{
	KASSERT(upage_free(NULL, UBASE_VIRT) == -EFAULT);
	KASSERT(upage_free(root_pgdir, KBASE_VIRT) == -EFAULT);
	KASSERT(upage_free(root_pgdir, UBASE_VIRT - PAGE_SIZE) == -EFAULT);
	KASSERT(upage_free(root_pgdir, UBASE_VIRT + PAGE_SIZE - 1) == -EFAULT);
}

/**
 * @brief Fault Injection Test: Bad User Page Release
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_upage_bad_free(void)
{
	KASSERT(upage_free(root_pgdir, UBASE_VIRT) == -EFAULT);
}

/**
 * @brief Fault Injection Test: Double User Page Release
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_upage_double_free(void)
{
	KASSERT(upage_alloc(root_pgdir, UBASE_VIRT) == 0);
	KASSERT(upage_free(root_pgdir, UBASE_VIRT) == 0);
	KASSERT(upage_free(root_pgdir, UBASE_VIRT) == -EFAULT);
}

/**
 * @brief Fault Injection Test: User Page Allocation Overflow
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_fault_upage_allocation_overflow(void)
{
	/* Allocate pages. */
	for (unsigned i = 0; i < NUM_UPAGES; i++)
		KASSERT(upage_alloc(root_pgdir, UBASE_VIRT + i*PAGE_SIZE) == 0);

	/* Fail to allocate an extra page. */
	KASSERT(upage_alloc(root_pgdir, UBASE_VIRT + NUM_UPAGES*PAGE_SIZE) == -EAGAIN);

	/* Release pages. */
	for (unsigned i = 0; i < NUM_UPAGES; i++)
		KASSERT(upage_free(root_pgdir, UBASE_VIRT + i*PAGE_SIZE) == 0);
}

/**
 * @brief Stress Test: User Page Allocation
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_stress_upage_allocation(void)
{
	/* Allocate pages. */
	for (unsigned i = 0; i < NUM_UPAGES; i++)
		KASSERT(upage_alloc(root_pgdir, UBASE_VIRT + i*PAGE_SIZE) == 0);

	/* Release pages. */
	for (unsigned i = 0; i < NUM_UPAGES; i++)
		KASSERT(upage_free(root_pgdir, UBASE_VIRT + i*PAGE_SIZE) == 0);
}

/**
 * @brief Stress Test: User Page Write
 *
 * @author Pedro Henrique Penna
 */
PRIVATE void test_stress_upage_write(void)
{
	unsigned num_upages;
	unsigned *upg;
	const unsigned magic = 0xdeadbeef;

	#if (!CORE_HAS_TLB_HW)
	num_upages = NUM_UPAGES/TLB_LENGTH;
	#else
	num_upages = NUM_UPAGES;
	#endif

	/* Allocate pages. */
	for (unsigned i = 0; i < num_upages; i++)
	{
		upg = (void *)(UBASE_VIRT + i*PAGE_SIZE);

		KASSERT(upage_alloc(root_pgdir, VADDR(upg)) == 0);

		/* Write to kernel page. */
		for (unsigned j = 0; j < PAGE_SIZE/sizeof(unsigned); j++)
			upg[j] = magic;
	}

	/* Release pages. */
	for (unsigned i = 0; i < num_upages; i++)
	{
		upg = (void *)(UBASE_VIRT + i*PAGE_SIZE);

		/* Sanity check. */
		for (unsigned j = 0; j < PAGE_SIZE/sizeof(unsigned); j++)
			KASSERT(upg[j] == magic);

		KASSERT(upage_free(root_pgdir, VADDR(upg)) == 0);
	}
}

/**
 * @brief User Page Pool unit tests.
 *
 * Unit tests for the User Page Pool subsystem.
 *
 * @author Pedro Henrique Penna
 */
PRIVATE struct
{
	void (*test_fn)(void); /**< Test function.     */
	const char *type;      /**< Name of test type. */
	const char *name;      /**< Test Name.         */
} upool_tests[] = {
	{ test_api_upage_allocation,            "api",    "user page allocation"          },
	{ test_api_upage_write,                 "api",    "user page write"               },
	{ test_fault_upage_invalid_allocation,  "fault",  "user page invalid allocation"  },
	{ test_fault_upage_double_allocation,   "fault",  "user page double allocation"   },
	{ test_fault_upage_invalid_free,        "fault",  "user page invalid free"        },
	{ test_fault_upage_bad_free,            "fault",  "user page bad free"            },
	{ test_fault_upage_double_free,         "fault",  "user page double free"         },
	{ test_stress_upage_allocation,         "stress", "user page allocation"          },
	{ test_fault_upage_allocation_overflow, "fault",  "user page allocation overflow" },
	{ test_stress_upage_write,              "stress", "user page write"               },
	{ NULL,                                 NULL,     NULL                            },
};

/**
 * @brief Runs unit tests on the User Page Pool.
 *
 * The upool_test_driver() function runs API, fault injection and
 * stress tests on the User Page Pool.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void upool_test_driver(void)
{
	for (int i = 0; upool_tests[i].test_fn != NULL; i++)
	{
		upool_tests[i].test_fn();
		kprintf("[test][%s] %s [passed]", upool_tests[i].type, upool_tests[i].name);
	}
}

/*============================================================================*/

/**@endcond*/

