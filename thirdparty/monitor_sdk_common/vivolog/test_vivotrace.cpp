#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include "vivolog.h"
#include "vivotrace.h"

#define LOG_FILENAME "./logs/biz/vivo_biz_rerank.log"
#define TRACE_FILENAME "./logs/biz/vivo_biz_trace.log"

static int g_running = 1;
static int g_num = 100;
static int g_thread = 2;
static const int MAX_THREAD = 128;
static long g_ticks[MAX_THREAD] = {0};

bool SignalAction(int signo, sighandler_t func)
{
    struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(signo, &act, &oact) < 0)
    {
		LOG_CONSOLE("sigaction error: %s",strerror(errno));
		return false;
	}
	return true;
}

void SignalIgnore(int signo)
{
    LOG_INFO("ignore signal %d", signo);
}

void SignalQuit(int signo)
{
    LOG_INFO("quit signal %d", signo);
	g_running = 0;
}


static void * test_libvivolog_multi_thread(void *param)
{
	static int g_count;
	int index = g_count++;

	LOG_CONSOLE("start index %d",index);

	CTrace *obj = (CTrace *) param;

	long lValue = 0;
	std::string s = "string";
	std::vector<long> vInt;
	std::vector<std::string> vStr;

	char buf[64] = { 0 };

	for (int i = 0; i < 6; i++) {
		vInt.push_back(i);

		sprintf(buf, "%d", i);
		vStr.push_back(buf);
	}

	while (g_running && obj) {
		if (g_ticks[index] >= g_num) {
			usleep(1000 * 100);
			continue;
		}

		g_ticks[index]++;

		CSession *session = obj->get();

		if (NULL == session) {
			continue;
		}

		lValue++;

		if ((lValue % 2) == 0) {
			session->add(index);

			session->add(SPLIT_BIG_DATA);
			session->add(lValue);

			session->add(SPLIT_BIG_DATA);
			session->add(vInt);

			session->add(SPLIT_BIG_DATA);
			session->add("abc");

			session->add(SPLIT_BIG_DATA);
			session->add("len", 3);

			session->add(SPLIT_BIG_DATA);
			session->add(s);

			session->add(SPLIT_BIG_DATA);
			session->add(vStr);

			session->add("\r\n");
		} else {
			session->print("%d" SPLIT_BIG_DATA "%ld" SPLIT_BIG_DATA "%u" SPLIT_BIG_DATA "%lu" SPLIT_BIG_DATA "%llu" SPLIT_BIG_DATA "%f" SPLIT_BIG_DATA "%lf" SPLIT_BIG_DATA "%c" SPLIT_BIG_DATA "%s\r\n",
				index,lValue,1,2,3,4.0,5.000000,'a',"abc");
		}

		obj->add(session);
	}

	LOG_CONSOLE("exit index %d tick %ld",index, g_ticks[index]);

	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		g_num = atoi(argv[1]);
		if (argc > 2) {
			g_thread = atoi(argv[2]);
			g_thread = g_thread > MAX_THREAD ? MAX_THREAD : g_thread;
		}
	}

	vivolog_init(LOG_FILENAME);

	CTrace *obj = new CTrace(TRACE_FILENAME, 1024 * 8);

	g_running = 1;

	pthread_t tids[MAX_THREAD] = { 0 };

	for (int i = 0; i < g_thread; i++) {
		int ret = pthread_create(&tids[i], NULL, test_libvivolog_multi_thread, obj);
		if (ret) {
			LOG_CONSOLE("%s", "pthread_create fail");
			exit(0);
			return 0;
		}
	}

	SignalAction(SIGUSR2, SignalIgnore);
	SignalAction(SIGUSR1, SignalIgnore);
	SignalAction(SIGPIPE, SignalIgnore);
	SignalAction(SIGTERM, SignalQuit);
	SignalAction(SIGQUIT, SignalQuit);
	SignalAction(SIGINT, SignalQuit);

	while (g_running) {
		bool done = true;
		for (int index = 0; index < g_thread; index++) {
			//LOG("%d %ld",index,g_ticks[index]);
			if (g_ticks[index] < g_num) {
				done = false;
				break;
			}
		}

		if (done) {
			break;
		}

		usleep(1000 * 5);
	}

	g_running = 0;
	LOG_CONSOLE("g_running %d", g_running);

	for (int i = 0; i < g_thread; i++) {
		//pthread_cancel(tids[i]);
		pthread_join(tids[i], NULL);
	}

	sleep(1);

	delete obj;
	obj = NULL;

	vivolog_stop();

	LOG_CONSOLE("main exit");

	return 0;
}

