/* semaphore.h
 *
 * Copyright 2012 Christopher Anderson <chris@nullcode.org>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __KERNEL_SEMAPHORE_H
#define __KERNEL_SEMAPHORE_H

#include "compiler.h"
#include "kernel/thread.h"
#include "kernel/mutex.h"

__BEGIN_CDECLS;

struct semaphore;
typedef struct semaphore  semaphore_t;

semaphore_t * sem_init(unsigned int);
void sem_destroy(semaphore_t *);
int sem_post(semaphore_t *, bool resched);
status_t sem_wait(semaphore_t *);
status_t sem_trywait(semaphore_t *);
status_t sem_timedwait(semaphore_t *, lk_time_t);

__END_CDECLS;
#endif
