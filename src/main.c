#include "m_timer.h"
#include <stdio.h>
#include <time.h>

void print_func(m_timer_t *timer) {
  printf("%s called at time %lld\n", __func__, time(NULL) * 1000LL);
}

void print_func_1(m_timer_t *timer) {
  printf("%s called at time %lld\n", __func__, time(NULL) * 1000LL);
}

int main(int argc, char **argv) {
  m_timer_init();

  m_timer_t *my_timer = m_timer_create(print_func, 1000, 5, NULL);
  (void)my_timer;

  m_timer_t *my_timer_1 = m_timer_create(print_func_1, 2000, 5, NULL);

  while (true) {
    m_timer_run();
  }

  return 0;
}
