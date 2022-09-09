
#define _CRT_RAND_S

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>

#include "mem_profile.h"

#include "valgrind/callgrind.h"

//#define unsigned int uint

#define type float // type of main array elements
#define LENGTH 1024*16 // default length of array
#define MY_RAND_MAX 10 // for rand value of scalars
#define NUM_THREADS 8 // default number of threads
#define BILLION 1000000.0
#define SPARSE_COEF 10 // max number of not null values in string will be length/SPARSE_COEF

#define CACHE_ANNIL (2*1024*1024) // size of array for cache spoiling
#define ITERS 1 // number of iterations for more precise time counting


struct ret {
	type *a1;
	type *x1;
        int *ip1;
        int *ia1;
};


static int count_time = 1;
// this function is responsible for creating new array with random elements
void createArray(type *arr, int length) {

        srand(time(NULL)*count_time);
        count_time++;
        type tmp_elem = 0;
        int i;
        for (i = 0; i < length; i++) {
                tmp_elem = (type)1/(i+2); // if type=int, here will be 0
                if (tmp_elem == 0) tmp_elem = (type)(i+1); // if int, tmp will be some int between 1 and length
                arr[i] = tmp_elem;
        }
}


// this function creates random sparse matrix and returns length of a array
struct ret initA(int length) {

	struct ret ret1;

        type tmp_elem = 0;
        int i,j;
	uint num = 0;
	int sum = 0;
	int user_max = length;
	uint *lengths = new uint[length];

        srand(time(NULL)*count_time);

	//cout << "lengths: " << endl;
	// generate number of elements in each string of matrix A, 1<= number of elements <= length/SPARSE_COEF
	for (i = 0; i < length; i++) {
		lengths[i] = (uint)((rand()%(length/SPARSE_COEF)) + 1); // value between 1 and length/SPARSE_COEF
		sum += lengths[i];
		//cout << lengths[i] << ' ';
	}
	//cout << endl << sum << endl;

	ret1.a1 = new type[sum];
	ret1.x1 = new type[length];
	ret1.ia1 = new int[sum];
	ret1.ip1 = new int[length+1];
	
	// generate x vector
        srand(time(NULL)*count_time);
	for (i = 0; i < length; i++) {
		if ((type)1/2 == 0) ret1.x1[i] = ((rand()%user_max)+1); // if type=int, then random value between 1 and length^2
                else ret1.x1[i] = ((float)(rand()-0.01)/RAND_MAX*user_max+0.01);
		count_time++;
	}
	//cout << "2" << endl;

	// main cycle for generating arrays: a,ip,ia
	int count = 0;
	int col = 0;
	int not_first = 0;
	int length_2 = length*length;

	ret1.ip1[0] = 0;
	for (i = 0; i < length; i++) {
		srand(time(NULL)*count_time);
		count_time++;
		col = 0;
		// for each string generate number of elements in it
		for (j = 0; j < lengths[i]; j++) {
			// for a
			if ((type)1/2 == 0) ret1.a1[count] = ((rand()%user_max)+1); // if type=int, then random value between 1 and length^2
			else ret1.a1[count] = ((float)(rand()-0.01)/RAND_MAX*user_max+0.01); // if type=float or double, then random type value between 0.01 and length^2
			
			// for ia
//			col = (rand()%(length-col-1-(lengths[i]-j-1)))+(col+not_first);
//                      not_first = 1;
			col = (rand()%(length/lengths[i]))+j*length/lengths[i];
			ret1.ia1[count] = col;

			count++;	
		}
		// for ip
		ret1.ip1[i+1] = count;
	}

	///////////// print /////////////////
	
	/*count = 0;
	cout << "A: " << endl;
	for (i = 0; i < length; i++) {
		int ii = 0;
		for (j = ret1.ip1[i]; j<=ret1.ip1[i+1]-1; j++) {
			while (ii < ret1.ia1[count]) {cout << "0\t"; ii++;}
			cout << fixed << setprecision(2) << ret1.a1[count] << "\t";
			ii++;
			count++;
		}
                while (ii < length) {cout << "0\t"; ii++;}
		cout << endl;
	}
	cout << "IP: " << endl;
        for (i = 0; i < length+1; i++) cout << fixed << setprecision(2) << ret1.ip1[i] << "\t";
        cout << endl;

	cout << "IA: " << endl;
        for (i = 0; i < sum; i++) cout << fixed << setprecision(2) << ret1.ia1[i] << "\t";
        cout << endl;

	cout << "X: " << endl;
	for (i = 0; i < length; i++) cout << fixed << setprecision(2) << ret1.x1[i] << "\t";
	cout << endl;*/

	return ret1;
}


// this function creates tridiagonal matrix
/*
int initA(type *a, int *ip, int *ia, int length) {
	type tmp_elem = 0;
	int i;
	int a_length = 3*length-2;
        // matrix a
//        cout << "a:  ";
        for (i = 0; i < a_length; i++) {
                tmp_elem = (type)1/(i+2); // if type=int, here will be 0
                if (tmp_elem == 0) tmp_elem = (type)(i+1); // if int, tmp will be some int between 1 and length
                a[i] = tmp_elem;
//                cout << a[i] << ' ';
        }
//        cout << endl << "ip:  ";
        // array ip
        int tmp_i = 0;
        for (i = 0; i < length+1; i++) {
                ip[i] = tmp_i;
                if ((i == 0) || (i == length-1)) tmp_i += 2;
                else tmp_i += 3;
//                cout << ip[i] << ' ';
        }
//        cout << endl << "ia:  ";
        // array ia
        tmp_i = 0;
        for (i = 0; i < a_length; i++) {
                if (i == 0) ia[i] = 0;
                else if (i == 1) ia[i] = 1;
                else if (i == a_length-2) ia[i] = length-2;
                else if (i == a_length-1) ia[i] = length-1;
                else {
                        ia[i] = tmp_i;
                        if (i % 3 == 1) tmp_i--;
                        else tmp_i++;
                }
//                ia[i] = tmp_i;
  //              if (i % 3 == 1) tmp_i++;

//                cout << ia[i] << ' ';
        }
//        cout << endl;
	return 0;
}
*/

//////////////////////
/**** Start here ****/
//////////////////////
int main(int argc, char** argv) {


	if ((argc > 5)) {
		cout << "Wrong number of parameters!" << endl;
		return -1;
	}
	int length = LENGTH;
	type elem_min = 0;
	type elem_max = 1;
	int i;
	int len_set = 0, min_set = 0, max_set = 0;
	int num_threads = NUM_THREADS;
	int create_arr = 1;
	string proc_type = "clovertown1";
	for (i = 1; i < argc; i++) {
		switch(argv[i][0]) {
			case 'l': {
				length = atoi(&(argv[i][1]));
                                length = round(pow((double)2,length));
				len_set = 1;
				break;
			}
			case 'm': {
				elem_min = atoi(&(argv[i][1]));
				min_set = 1;
				break;
			}
			case 'M': {
				elem_max = atoi(&(argv[i][1]));
				max_set = 1;
				break;
			}
			case 't': {
				num_threads = atoi(&(argv[i][1]));
				break;
			}
			case '-': {
				switch (argv[i][1]) {
					case 'r': {
						create_arr = 0;
						break;
					}
					case '-': {
						proc_type = argv[i]+2*sizeof(char); // +2 - skips first 2 elements
						break;
					}
				}
				break;
			}
		}
	}
	// end of reading input parameters

	if (length <= 0) {
		cout << length << endl;
		cout << "Wrong value for length!" << endl;
		return -1;
	}

	if (elem_max <= elem_min) {
		cout << "Minimum should be less than maximum!" << endl;
		return -1;
	}
	if (num_threads < 0) {
		cout << "Wrong value for number of threads!" << endl;
		return -1;
	}

	// getting performance of processor type
	double peak = 0;
	if (!proc_type.compare("clovertown1")) peak = 6.4;
	if (!proc_type.compare("clovertown2")) peak = 9.32;
	if (!proc_type.compare("clovertown3")) peak = 10.64;
        if (!proc_type.compare("woodcrest2")) peak = 10.64;
        if (!proc_type.compare("woodcrest3")) peak = 10.64;
	if (!proc_type.compare("woodcrest5")) peak = 12;
	if (!proc_type.compare("woodcrest6")) peak = 8;
	//if (!proc_type.compare("woodcrest7")) peak = 8;
	//if (!proc_type.compare("pentium41")) peak = 6;
	//if (!proc_type.compare("pentiumd1")) peak = 6.8;
	if (!proc_type.compare("xeon1")) peak = 6.4;
	if (!proc_type.compare("dempsey1")) peak = 6;
	if (!proc_type.compare("opteron3")) peak = 4.8;
	if (!proc_type.compare("opteron4")) peak = 3.6;
	if (!proc_type.compare("opteron5")) peak = 3.6;
	if (!proc_type.compare("barcelona1")) peak = 10.8;
        if (!proc_type.compare("harpertown")) peak = 12;
        //if (!proc_type.compare("nehalem")) peak = ;

	// end of checks

	// writing into csv file
	// at very first iteration I want to write info about testing
	bool new_csv_file = false;
	ifstream fin("csv_res.csv",ios:: in);
	if (!fin.is_open()) { // if there is no such file
		new_csv_file = true;
	}
	fin.close();

        ofstream fres("csv_res.csv", ios::app | ios::out); // file with kpd results in csv format
        if (length == 2) { // if it's all for this processor, writing endl
                fres << " Processor type: " << proc_type.c_str() << endl;
                return 0;
        }


	string filename = "arr.txt";


	// main variables that are used in algorithm itself
	/////// !!!if changes with arrays and scalars, uncomment necessary declarations;
	type* a; // input matrix
	type* b = new type[length]; // output vector
        type* x; // input vector
        int* ip; // array with indexes of non-nil elements of matrix a
        int* ia; // ia[i]- string number of a[i] element
	// end of main variables
	
	struct timeval st,en,bef,aft,fi;
	gettimeofday(&bef,NULL);
	///////// initialization
	type tmp_elem = 0;
	// matrix b
        for (i = 0; i < length; i++) {
		b[i] = 0;
	}
	// create all input vectors
	struct ret ret1;
	ret1 = initA(length);
	a = ret1.a1;
	x = ret1.x1;
        ip = ret1.ip1;
        ia = ret1.ia1;
        gettimeofday(&aft,NULL);
        cout << "Init time: " << ((aft.tv_sec-bef.tv_sec)+(aft.tv_usec-bef.tv_usec)/BILLION) << " sec" << endl << endl;
	///////// end of initialization

	
	// service variables for main part
	int helper = 0;
	int it, j, ii;
	double taken_time = 0;
	for (it = 0; it < ITERS; it++) {

		for (i = 0; i < length; i++) b[i] = 0;

		helper = 0;
		int *cache_annil;
		cache_annil = new int[CACHE_ANNIL];		
		cache_annil[0] = 0;
		for (i = 1; i < CACHE_ANNIL; i++) {
                	cache_annil[i] = it-i;
			helper += cache_annil[i-1];
		}
		delete cache_annil;
		if ((it%10) == 0) cout << "it: " << it << endl;
		/***************************/
		/**** main part is here ****/
		/***************************/
		gettimeofday(&st,NULL); // gets start time
		CALLGRIND_TOGGLE_COLLECT
		for (i = 0; i < length; i++) {
			for (j = ip[i]; j <= ip[i+1]-1; j++) {
				b[i] = b[i] + a[j]*x[ia[j]];
			}
		}
		CALLGRIND_TOGGLE_COLLECT
		gettimeofday(&en,NULL);
		/***************************/
		/****  main part ends   ****/
		/***************************/
		if ((taken_time > ((en.tv_sec-st.tv_sec)+(en.tv_usec-st.tv_usec)/BILLION)) || (it == 0)) {
			taken_time = ((double)(en.tv_sec-st.tv_sec)+(double)(en.tv_usec-st.tv_usec)/BILLION);
		}
		//gain.tv_sec += (en.tv_sec-st.tv_sec);
		//gain.tv_usec += (en.tv_usec-st.tv_usec);
	}
	cout << "helper: " << helper << endl << endl;
	ofstream fout("results.txt", ios::app | ios::out);

//	double taken_time = ((gain.tv_sec) + (gain.tv_usec)/BILLION)/ITERS;
	taken_time = taken_time*10000; // because in seconds it will be too small
	printf("Taken time: %.2f*10^(04) sec\n",taken_time);
	// write into file
	fout << "Processor type: " << proc_type.c_str() << endl;
        //fout << "Time taken on " << s << "array: " << ((st.tv_sec-bef.tv_sec) + (st.tv_usec-bef.tv_usec)/BILLION) << endl;
	// write type of elements of array
	fout << "Element type: " << typeid(type).name() << endl;
	// write peak and real performance
	double kpd = ((2*ip[length])/taken_time)/(peak*1000);
	gettimeofday(&fi,NULL);
	fout << "length: " << length << ", length of A: " << ip[length] << endl;
	fout << "taken_time: " << taken_time << ", overall time: " << (((fi.tv_sec-bef.tv_sec)+(fi.tv_usec-bef.tv_usec)/BILLION)*10000) << endl; 
	fout <<  "peak: " << peak << ", KPD: " << kpd << "%" << endl;
	fout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl << endl;
	cout << "Overall time: " << (((fi.tv_sec-bef.tv_sec)+(fi.tv_usec-bef.tv_usec)/BILLION)) << " sec" << endl;

	
	////// print B array ///////
	/*cout << endl << "b:  ";
	for (i = 0; i < length; i++) {
        	cout << b[i] << ' ';
        }
	cout << endl;*/

	return 0;
}
