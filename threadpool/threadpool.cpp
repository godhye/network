#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <math.h>
#include <unordered_set>
#include <string>
#include <vector>
#include <map>
#include <queue>
  
//thread pool
/*
thread 미리 만들어서 필요할때 사용가능 하도록 
원하는 함수 등록해서 처리 할수있도록 설계

thread 벡터 , 함수 받을 수 있게 void형으로

작업 보관하는 컨터이너 ,가장 먼저 들어온 잡 처리 하도록  queue
멀티 스레드환경에서 queue는 불안전 ->mutex 필요

*/

#include <thread>
#include <condition_variable>
#include <future>
class Threadpool {

public:
	Threadpool(size_t num);
	~Threadpool();
	//add job
	template <class F , class... Args>
	std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(F f, Args&&... args);
	//void EnqueueJob(std::function<void()> job);

private:

	size_t nthreads;

	//thread가 담긴 벡터
	std::vector<std::thread> workerthreads;
	
	//실행될 함수가 담긴 큐
	std::queue<std::function<void()>> jobs;
	
	//이벤트 개체, 신호받을때 까지 쓰레드 실행 멈춤 notify_xxx로 다시 실행 
	std::condition_variable cv_jobq;
	std::mutex mu_jobq;

	bool stop_all;

	//worker thread
	void WorkThread();
};


 //	std::packaged_task<return_type()> job(std::bind(f, args...));
	//std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(F f, Args... args);
 //

Threadpool::Threadpool(size_t num)
	:nthreads(num), stop_all(false)
{
	workerthreads.reserve(nthreads);
	for (size_t i = 0; i < nthreads; ++i) 
	{
		workerthreads.emplace_back([this]() {this->WorkThread(); });
	}

}

Threadpool::~Threadpool()
{
	stop_all = true;
	cv_jobq.notify_all();

	for (auto& t : workerthreads) {
		t.join();
	}
}

//void Threadpool::EnqueueJob(std::function<void()> job)
//{
//	//실행중이면
//	if (stop_all)
//	{
//		throw std::runtime_error("ThreadPool 사용 중지됨");
//	}
//	{	
//		//뮤텍스 on
//		std::future<return_type> job_result_future = job.get_future();
//
//		std::lock_guard<std::mutex> lock(mu_jobq);
//		//queue에 추가
//		jobs.push([&job]() {job(); });
//
//	}
//	//job 추가 알림 
//	cv_jobq.notify_one();
//	
// 
//}

//unique_lock
//lock_guard
/*
둘다 소멸하면서 lock 해제 되는데
lock_guard는 생성과 동시에 lock ,소멸자에서 unlock
unique_lock은 lock거는 시점 조절 가능 , 소멸할때 unlock
(* 생성과 동시에 락을 걸지 않으려면 두번째 인자로 std::defer_lock을 넘겨주면 된다.)
*/
void Threadpool::WorkThread()
{
	while (true) 
	{
		//Job의 데이터 접근 위해 뮤텍스 걸고 , job()처리후에 뮤텍스 획득
		std::unique_lock<std::mutex> lock(mu_jobq);
		//조건만족할때 까지 wait = 뮤텍스 반납하고 스레드 수행 중지
		cv_jobq.wait(lock, [this]() {return !this->jobs.empty() || stop_all; });
		if (stop_all && this->jobs.empty())
		{
			return;
		}

		//맨앞의 잡을 빼서 잡 수행
		std::function<void()> job = std::move(jobs.front());
		jobs.pop();

		//뮤텍스 풀고 
		lock.unlock();
			 
		//잡 실행 
		job();
	}
}

void work(int t, int id) {

	printf("%d start \n", id);
	std::this_thread::sleep_for(std::chrono::seconds(t));
	printf("%d end after %ds\n", id, t);
}

int main()
{
	Threadpool  pool(4);

	for (int i = 0; i < 10; i++) {
		pool.EnqueueJob([i]() {work(i % 4 + 1, i); });
	}

}

template<class F, class ...Args>
std::future<typename std::result_of<F(Args...)>::type> Threadpool::EnqueueJob(F f, Args&& ...args)
{
	if (stop_all) {
		throw std::runtime_error("ThreadPool 사용 중지");
	}

	using return_type = typename std::result_of<F(Args...)>::type;

	//job은 지역변수로 이 함수 리턴되면 파괴 -> shared_ptr에 담아줌(사용하는것 없을때 소멸됨 )
	//std::packaged_task<return_type()> job(std::bind(f, args...));

	//레퍼런스 전달위해 forward사용
	auto job = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward(args)...));
	std::future<return_type> job_result_future = job->get_future();
	{
		std::lock_guard<std::mutex> lock(mu_jobq);
		jobs.push([job]() {(*job)(); });

	}

	cv_jobq.notify_one();

	return job_result_future;
}
