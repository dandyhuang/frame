#include "thirdparty/monitor_sdk_common/base/tocken_bucket.h"
#include <stdlib.h>
#include "thirdparty/monitor_sdk_common/system/event_dispatcher/event_dispatcher.h"

using namespace common;

TockenBucket g_tb(16);

static void OnTimer(bool* received, int num) {
  *received = true;
  printf("recv %d\n", num);
  if (g_tb.TryConsume(num)) {
    printf("\t\t consumed\n");
  } else {
    printf("\t\t not consumed\n");
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s timer_repeat_ms\n", argv[0]);
    return -1;
  }
  using namespace std::placeholders;
  EventDispatcher dispatcher;
  bool received = false;
  int timer_repeat_ms = atoi(argv[1]);
  TimerEventWatcher watcher(&dispatcher, std::bind(OnTimer, &received, _1), 200,
                            timer_repeat_ms);
  watcher.Start();
  dispatcher.Run();
}

