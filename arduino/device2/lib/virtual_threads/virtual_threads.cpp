#ifndef PLATFORMIO
#include <stdio.h>
#include <sys/time.h>
unsigned long millis() {
  struct timeval tp;
  gettimeofday(&tp, nullptr);
  unsigned long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  // printf("%ld\n", ms);
  return ms;
}
unsigned long micros() {
  struct timeval tp;
  gettimeofday(&tp, nullptr);
  unsigned long us = tp.tv_sec * 1000000 + tp.tv_usec;
  printf("%lu\n", us);
  return us;
}
#include <unistd.h>
#define delay(x) usleep((useconds_t)x * 1000)
#define delayMicroseconds(x) usleep(x)

#endif
#include <virtual_threads.h>

namespace VT {
//------------------------------------- (array class for storing routines)
template <typename T>
Array<T>::Array() {
  base = new Data;
  base->next = nullptr;
  current = base;
  _length = 0;
}

//-------------------------------------
template <typename T>
Array<T>::~Array() {
  current = base;
  do {
    auto next = current->next;
    delete current;
    current = next;
  } while (current != nullptr);
};

//-------------------------------------
template <typename T>
void Array<T>::push(T &val) {
  current->value = &val;
  current->next = new Data;
  current->next->next = nullptr;
  current = current->next;
  _length++;
};

//-------------------------------------
template <typename T>
unsigned int Array<T>::length() {
  return _length;
}

//-------------------------------------
template <typename T>
void Array<T>::clear() {
  _length = 0;
  current = base->next;
  while (current != nullptr) {
    auto next = current->next;
    delete current;
    current = next;
  }
}

//-------------------------------------
template <typename T>
T *Array<T>::get(unsigned int index) {
  if (index >= _length) {
    return nullptr;
  }

  Data *element = base;
  for (int i = 0; i <= index; i++) {
    if (i == index) {
      return element->value;
    }
    element = element->next;
  }
  // throw "impossible variant";
  return nullptr;
};

//-------------------------------------
template <typename T>
Iterator<T> Array<T>::getIterator() {
  return Iterator<T>(this);
};

// class used to iterate trought routines array
//-------------------------------------
template <typename T>
Iterator<T>::Iterator(Array<T> *arr) {
  index = 0;
  array = arr;
};

//-------------------------------------
template <typename T>
T *Iterator<T>::iterate() {
  if (index >= array->length()) {
    return nullptr;
  }
  return array->get(index++);
}

//-------------------------------------
template <typename T>
void Iterator<T>::reset() {
  index = 0;
}

//-------------------------------------
template <typename T>
unsigned int Iterator<T>::get_index() {
  return index - 1;
}

//-------------------------------------
template <typename T>
unsigned int Iterator<T>::length() {
  return array->length();
}

// Routine class
//-------------------------------------
Routine::Routine() {
  auto engine = VT::Engine::getInstance();
  engine->add_routine(*this);
}

//-------------------------------------
void Routine::make_one_step() {
  processing = false;
  started = true;
  steps_counter = 0;
  next_step_time = 0;

  body();  // call user defined program
};

//-------------------------------------
void Routine::next_step(unsigned long wait_time) {
  current_step++;
  processing = true;
  next_step_time = Time::now() + wait_time;
};

// start from the begining
//-------------------------------------
void Routine::first_step(unsigned long wait_time) {
  current_step = 0;
  processing = true;
  next_step_time = Time::now() + wait_time;
}

// repat current step one more time
//-------------------------------------
void Routine::repeat_step(unsigned long wait_time) {
  processing = true;
  next_step_time = Time::now() + wait_time;
}

// used for if branching in child class body()
//-------------------------------------
bool Routine::is_current_step() {
  return steps_counter++ == current_step;
};

//-------------------------------------
bool Routine::is_complete() {
  return started & !processing;
}

// debug methods:
//-------------------------------------
unsigned long Routine::get_current_step() {
  return current_step;
}

//-------------------------------------
unsigned long Routine::get_steps_counter() {
  return steps_counter;
}

// Engine
// singleton class to keep all operations
//-------------------------------------
Engine *Engine::engine = nullptr;

Engine *Engine::getInstance() {
  if (engine == nullptr) {
    engine = new Engine();
  }
  return engine;
}

//-------------------------------------
unsigned long Time::now() {
#ifdef USE_MICROSECONDS_TIMING
  return micros();
#else
  return millis();
#endif
};

//-------------------------------------
void Time::sleep(unsigned int t) {
#ifdef USE_MICROSECONDS_TIMING
  if (t > 16383) {
    delay(t / 16383);
    delayMicroseconds(t % 16383);
    return;
  }
  delayMicroseconds(t);
#else
  delay(t);
#endif
}

//---------------------------------------------
void Engine::add_routine(Routine &rt) {
  engine->routines.push(rt);
};

//---------------------------------------------
void Engine::clear() {
  engine->routines.clear();
};

//---------------------------------------------
void Engine::start() {
  Routine *rt;
  auto routines_iterator = engine->routines.getIterator();

  bool nearest_time_setted_up = false;
  bool all_tasks_complete = false;
  unsigned long nearest_step_time = 0;
  unsigned long current_time;
  unsigned long wait_time;

  while (true) {
    current_time = Time::now();
    rt = routines_iterator.iterate();

    if (rt == nullptr) {  // if all routines did one step
      if (all_tasks_complete) {
        break;
      }
      // begin new turn and continue to process all routenes one more time
      all_tasks_complete = true;
      routines_iterator.reset();
      nearest_time_setted_up = false;

      current_time = Time::now();
      // calculate if there are any time to have a sleep?
      if (nearest_step_time > current_time) {
        wait_time = nearest_step_time - current_time;
        // printf("wait %lu\n", wait_time);
        Time::sleep(wait_time);
      }
      continue;
    }

    // skip routines that finish thier job
    if (rt->is_complete()) {
      continue;
    }

    all_tasks_complete = false;
    // execute routine body() if time frame is appropriate
    if (current_time >= rt->next_step_time) {
      rt->make_one_step();
    }

    if (!nearest_time_setted_up) {
      nearest_time_setted_up = true;
      nearest_step_time = rt->next_step_time;
    }

    // find minimal sleep time for all routines
    if (rt->next_step_time < nearest_step_time) {
      nearest_step_time = rt->next_step_time;
    }
  }
};
}  // VIRTUALTHREADS namespase
