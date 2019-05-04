/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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
#include <stdint.h>
#include "kbench.h"

#ifdef __BENCHMARK_MATRIX__

/**
 * @brief Maximum number of threads to spawn.
 */
#define NTHREADS_MAX 8

/**
 * @brief Maximum matrix dimension log 2.
 */
#define N_LOG2_MAX 7

/**
 * @brief Maximum matrix dimension.
 */
#define N_MAX (1 << N_LOG2_MAX)

/**
 * @brief Number of working threads.
 */
static int NTHREADS;

/**
 * @brief Matrix dimension log 2.
 */
static int N_LOG2 = N_LOG2_MAX;

/**
 * @brief Matrix dimension.
 */
static int N = N_MAX;

/**
 * @brief Task info.
 */
struct tdata
{
	int tnum; /**< Thread Number */
	int i0;   /**< Start Line    */
	int in;   /**< End Line      */
} tdata[NTHREADS_MAX] ALIGN(CACHE_LINE_SIZE);

/**
 * @brief Matrices for test.
 */
/**@{*/
static float a[N_MAX*N_MAX];
static float b[N_MAX*N_MAX];
static float ret[N_MAX*N_MAX];
/**@}*/

/**
 * @brief Initializes a chunk of the matrix.
 *
 * @param i0 Start line.
 * @param in End line.
 */
static inline void matrix_init(int i0, int in)
{
	for (int i = i0; i < in; i++)
	{
		for (int j = 0; j < N; j++)
		{
			ret[(i << N_LOG2) + j] = 0;
			a[(i << N_LOG2) + j] = 1.0;
			b[(i << N_LOG2) + j] = 1.0;
		}
	}
}

/**
 * @brief Multiples a chunk of the matrices.
 *
 * @param i0 Start line.
 * @param in End line.
 */
static inline void matrix_mult(int i0, int in)
{
	for (int i = i0; i < in; i++)
	{
		for (int j = 0; j < N; j++)
		{
			for (int k = 0; k < N; k++)
			{
				ret[(i << N_LOG2) + j] +=
					a[(i << N_LOG2) + k]*b[(k << N_LOG2) + j];
			}
		}
	}
}

/**
 * @brief Multiplies matrices.
 *
 * @param arg Unused argument.
 */
static void *task(void *arg)
{
	struct tdata *t = arg;
	int i0 = t->i0;
	int in = t->in;

	for (int i = SKIP; i < (NITERATIONS + SKIP); i++)
	{
		for (size_t j = 0; j < ARRAY_LENGTH(perf_events); j++)
		{
			uint64_t reg;

			matrix_init(i0, in);

			nanvix_perf_start(0, perf_events[j].num);

				matrix_mult(i0, in);

			nanvix_perf_stop(0);
			reg = nanvix_perf_read(0);

			if (i >= SKIP)
			{
				kprintf("%s %d %d %d %d %s %d",
					"[benchmarks][matrix]",
					i - SKIP,
					(1 << N_LOG2),
					NTHREADS,
					t->tnum,
					perf_events[j].name,
					UINT32(reg)
				);
			}
		}
	}

	return (NULL);
}

/**
 * @brief Matrix Multiplication Benchmark
 *
 * @param nthreads Number of working threads.
 * @param nlog2    Matrix dimension log2.
 */
void benchmark_matrix(int nthreads, int nlog2)
{
	kthread_t tid[NTHREADS_MAX];

	/* Invalid number of working threads. */
	assert(nlog2 <= N_LOG2_MAX);
	if (nthreads > 1)
		assert((nthreads & 1) == 0);

	/*
	 * TODO: Assert required performance
	 * monitoring capabilities.
	 */

	/* Initialize benchmark parameters. */
	NTHREADS = nthreads;
	N_LOG2 = nlog2;
	N = 1 << nlog2;

	/* Spawn threads. */
	for (int i = 0; i < nthreads; i++)
	{
		tdata[i].i0 = (N/nthreads)*i;
		tdata[i].in = (N/nthreads)*(i + 1);
		tdata[i].tnum = i;
		kthread_create(&tid[i], task, &tdata[i]);
	}

	/* Wait for threads. */
	for (int i = 0; i < nthreads; i++)
		kthread_join(tid[i], NULL);
}


#endif /* __BENCHMARK_MATRIX__ */
