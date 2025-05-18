#include "m_timer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

m_timer_manager_t m_timer_manager = {0};

void m_timer_init(void) { list_init(&(m_timer_manager.timer_list)); }

long long get_tick(void) {
  long long ms = 0;
#ifdef LINUX_PLATFORM
#include <time.h>
  ms = time(NULL) * 1000LL;
#endif
  return ms;
}

static uint32_t m_remaining_time(m_timer_t *timer) {

  uint32_t delta = 0;
  uint32_t cur_time = get_tick();

  delta = cur_time - timer->last_run;

  if (delta >= timer->interval)
    return 0;

  return timer->interval - delta;
}

bool m_timer_exec(m_timer_t *timer) {
  if (timer->is_paused)
    return false;

  bool ret = false;

  if (!m_remaining_time(timer)) {
    /* call timer cb if -1 or > 0 */
    if (timer->cb && timer->repeat_count)
      timer->cb(timer);

    timer->last_run = get_tick();

    if (timer->repeat_count > 0) {
      timer->repeat_count--;
    } else if (timer->auto_delete) {
      fprintf(stdout, "removing timer\n");
      list_del(&timer->node);
      memset(timer, 0, sizeof(m_timer_t));
      timer = NULL;
    }
  
    ret = true;
  }

  return ret;
}

m_timer_t *m_timer_create(m_timer_cb_t cb, uint32_t interval, int32_t repeat,
                          void *user_data) {
  m_timer_t *new_timer = NULL;
  new_timer = (m_timer_t *)calloc(1, sizeof(m_timer_t));
  if (!new_timer)
    exit(-1);

  new_timer->interval = interval;
  new_timer->cb = cb;
  new_timer->repeat_count = repeat;
  new_timer->is_paused = 0;
  new_timer->user_data = user_data;
  new_timer->auto_delete = 1;
  new_timer->last_run = get_tick();

  list_add(&m_timer_manager.timer_list, &new_timer->node);

  return new_timer;
}

void m_timer_run(void) {
  m_timer_t *timer, *next;

  list_for_each_entry_safe(timer, next, &(m_timer_manager.timer_list), node) {
    m_timer_exec(timer);
  }
}
