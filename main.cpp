#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;
	
int n, p;
vector<int> v[2];
pthread_barrier_t barrier;

int main(int argc, char const *argv[]) {
	n = atoi(argv[1]);

	// No point using more threads than elements
	p = min(n, atoi(argv[2]));

	// Using 2 arrays to be able to read the previous state in each iteration
	v[0].resize(n);
	v[1].resize(n);

	transform(argv + 3, argv + argc, v[0].begin(), [](const char *s) { return atoi(s); });

	// Using a barrier to separate each iteration, so that each thread can read the previous complete state
	pthread_barrier_init(&barrier, NULL, p);

	vector<pthread_t> threads(p);

	// Using an array of ids to avoid passing a local address to a thread
	vector<int> ids(p);
	for (int i = 0; i < p; ++i) {
		ids[i] = i;

		// Lambda functions are cool :D
		pthread_create(&threads[i], NULL, [] (void *arg) -> void * {
			// Set each thread's bounds
			int id = *(int *) arg, start = id * n / p, end = (id + 1) * n / p, current = 0;

			// https://mobylab.docs.crescdi.pub.ro/assets/files/scan-5c3f4088b1506f41ee22829cdf5bb9e8.pdf
			// adapted to pthreads, with a barrier to make each thread wait for the previous iteration to finish
			for (int offset = 1; offset < n; offset <<= 1, current = 1 - current) {
				for (int i = start; i < end; ++i) {
					v[1 - current][i] = v[current][i];
					if (i >= offset)
						v[1 - current][i] += v[current][i - offset];
				}

				pthread_barrier_wait(&barrier);
			}

			// Print the result from the first thread
			if (id == 0) {
				copy(v[current].begin(), v[current].end(), ostream_iterator<int>(cout, " "));
				cout << '\n';
			}
	

			pthread_exit(NULL);
		}, ids.data() + i);
	}

	// Could be avoided with jthreads :(
	for (int i = 0; i < p; ++i)
		pthread_join(threads[i], NULL);
}
