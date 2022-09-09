#ifndef MATRIX_MULT
#define MATRIX_MULT

namespace matrix_mult {

const int BENCH_COUNT = 6;

const char* type_names[BENCH_COUNT] = {
	"ijk", // 0
	"ikj", // 1
	"jik", // 2
	"jki", // 3
	"kij", // 4
	"kji", // 5
};

template <typename T, typename AT>
void Init(AT a, AT b, AT c, int size)
{
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
		{
			a[i][j] = locality::utils::RRand(i, j);
			b[i][j] = locality::utils::RRand(j, i);
			c[i][j] = 0.0;
		}
}

template <typename T, typename AT>
T Check(AT c, int size)
{
	T s = 0.0;

	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			s += c[i][j] / size / size;

	return s;
}

#define OUTER_LOOP(VAR) for(int VAR = 0; VAR < size; VAR ++)

template <typename T, typename AT>
void KernelIJK(AT a, AT b, AT c, int size)
{
#pragma omp parallel for num_threads(LOC_THREADS)
	OUTER_LOOP(i)
		OUTER_LOOP(j)
		{
			T tmp = c[i][j];

			OUTER_LOOP(k)
				tmp += a[i][k]*b[k][j];

			c[i][j] = tmp;
		}

}

template <typename T, typename AT>
void KernelIKJ(AT a, AT b, AT c, int size)
{

#pragma omp parallel for num_threads(LOC_THREADS)
	OUTER_LOOP(i)
		OUTER_LOOP(k)
		{
			T tmp = a[i][k];
			OUTER_LOOP(j)
				c[i][j] = c[i][j] + tmp*b[k][j];
		}

}

template <typename T, typename AT>
void KernelJIK(AT a, AT b, AT c, int size)
{

#pragma omp parallel for num_threads(LOC_THREADS)
	OUTER_LOOP(j)
		OUTER_LOOP(i)
		{
			T tmp = c[i][j];

			OUTER_LOOP(k)
				tmp += a[i][k]*b[k][j];

			c[i][j] = tmp;
		}
}

template <typename T, typename AT>
void KernelJKI(AT a, AT b, AT c, int size)
{

#pragma omp parallel for num_threads(LOC_THREADS)
	OUTER_LOOP(j)
		OUTER_LOOP(k)
		{
			T tmp = b[k][j];

			OUTER_LOOP(i)
				c[i][j] = c[i][j] + a[i][k]*tmp;
		}

}

template <typename T, typename AT>
void KernelKIJ(AT a, AT b, AT c, int size)
{

#pragma omp parallel for num_threads(LOC_THREADS)
	OUTER_LOOP(k)
		OUTER_LOOP(i)
		{
			T tmp = a[i][k];

			OUTER_LOOP(j)
				c[i][j] = c[i][j] + tmp*b[k][j];
		}

}

template <typename T, typename AT>
void KernelKJI(AT a, AT b, AT c, int size)
{

#pragma omp parallel for num_threads(LOC_THREADS)
	OUTER_LOOP(k)
		OUTER_LOOP(j)
		{
			T tmp = b[k][j];

			OUTER_LOOP(i)
				c[i][j] = c[i][j] + a[i][k]*tmp;
		}

}

template <typename T, typename AT>
void CallKernel(int core_type, AT a, AT b, AT c, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	switch(core_type)
	{
		//ijk
		case 0: KernelIJK<T, AT>(a, b, c, size); break;

		//ikj
		case 1: KernelIKJ<T, AT>(a, b, c, size); break;

		//jik
		case 2: KernelJIK<T, AT>(a, b, c, size); break;

		//jki
		case 3: KernelJKI<T, AT>(a, b, c, size); break;

		//kij
		case 4: KernelKIJ<T, AT>(a, b, c, size); break;

		//kji
		case 5: KernelKJI<T, AT>(a, b, c, size); break;

		default: fprintf(stderr, "unexpected core type");
	}

	LOC_PAPI_END_BLOCK
}

} /*namespace*/

#endif /*MATRIX_MULT*/
