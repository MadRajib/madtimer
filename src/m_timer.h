#ifndef _M_TIMER_H
#define _M_TIMER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef container_of
#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offsetof(type, member));                         \
  })
#endif

typedef struct _m_timer_t m_timer_t;

typedef void (*m_timer_cb_t)(m_timer_t *);

struct list_head {
  struct list_head *prev;
  struct list_head *next;
};

static inline void list_init(struct list_head *list) {
  list->prev = list->next = list;
}

static inline void list_add(struct list_head *list, struct list_head *item) {
  struct list_head *prev = list->prev;

  item->next = list;
  item->prev = prev;

  prev->next = list->prev = item;
}

static inline void list_del(struct list_head *item) {
  item->prev->next = item->next;
  item->next->prev = item->prev;
}

#define list_entry_first(list, type, member)                                   \
  container_of((list)->next, type, member)

#define list_entry_next(item, member)                                          \
  container_of((item)->member.next, typeof(*(item)), member)

#define list_for_each_entry_safe(item, next, list, member)                     \
  for (item = list_entry_first(list, typeof(*(item)), member),                 \
      next = list_entry_next(item, member);                                    \
       &item->member != list;                                                  \
       item = next, next = list_entry_next(item, member))

/*
 *  interval :  how often to run
 *  last_run: last ran at
 *  repeat_count: number of times to repeat, -1 infinite
 *  is_paused : is paused
 *  auto_delete: delete when done
 */
typedef struct _m_timer_t {
  struct list_head node;
  uint32_t interval;
  uint32_t last_run;
  int32_t repeat_count;
  uint32_t is_paused : 1;
  uint32_t auto_delete : 1;
  m_timer_cb_t cb;
  void *user_data;
} m_timer_t;

typedef struct {
  struct list_head timer_list;
  bool timer_added;
  bool timer_removed;
} m_timer_manager_t;

void m_timer_init(void);

m_timer_t *m_timer_create(m_timer_cb_t cb, uint32_t interval, int32_t repeat,
                          void *user_data);

void m_timer_run(void);

long long get_tick(void);

#endif // !_M_TIMER_H
