#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
using std::string;

bool done = false;
std::condition_variable cv;
std::mutex mu;
int ncount = 0;
std::queue<int> jobq;
std::vector<std::thread> vectorthread;
std::vector<std::thread> consumethread;

void work(int a)
{
	{
		std::lock_guard<std::mutex> lock(mu);
		jobq.push(a);
		ncount++;
		printf("work =  %d ncount = %d \n", a, ncount);

	}


	cv.notify_one();

}

void consume() {

	while (1)
	{
		std::unique_lock<std::mutex> lock(mu);
		// jobq.empty()가 아니고 done이 false이면  wait
		cv.wait(lock, [&] {return !jobq.empty(); });

		if (done && jobq.empty())
		{
			printf("done \nconsume =  %d , nCount =  %d\n", jobq.front(), ncount);
			lock.unlock();
			return;
		}
		printf("consume =  %d , nCount =  %d\n", jobq.front(), ncount);
		jobq.pop();
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::microseconds(50));
	}




}




int main() {

	consumethread.reserve(3);
	vectorthread.reserve(3);
	for (int i = 0; i < 3; i++)
	{


		consumethread.emplace_back([]() {consume(); });

	}

	for (int i = 0; i < 3; i++)
		vectorthread.emplace_back([&]() {work(i); });
	for (int i = 0; i < 3; i++)
	{
		vectorthread[i].join();
	}
	cv.notify_all();
	printf("end\n");

	for (int i = 0; i < 3; i++)
	{

		consumethread[i].join();
	}

	return 0;
}