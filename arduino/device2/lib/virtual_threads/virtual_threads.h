#ifdef PLATFORMIO
#include <Arduino.h>
#endif

#ifndef src_virtual_threads_h
#define src_virtual_threads_h

namespace VT {
template <typename T>
class Array;

// class used to iterate trought routines array
template <typename T>
class Iterator {
 public:
  Iterator(Array<T> *arr);
  T *iterate();
  void reset();
  unsigned int get_index();
  unsigned int length();

 private:
  unsigned int index;
  Array<T> *array;
};

// array class for storing routines
template <typename T>
class Array {
 public:
  Array();
  ~Array();

  unsigned int length();
  void push(T &val);
  T *get(unsigned int index);
  Iterator<T> getIterator();
  void clear();

 private:
  struct Data {
    T *value;
    Data *next;
  };

  Data *base;
  Data *current;
  unsigned int _length;
};

// not very accurate:
// #define USE_MICROSECONDS_TIMING
struct Time {
  static void sleep(unsigned int t);
  static unsigned long now();
};

// Singleton class to keep all operations
//---------------------------------------------
class Routine;

class Engine {
  static Engine *engine;
  Array<Routine> routines;

 public:
  static Engine *getInstance();
  void add_routine(Routine &rt);
  void start();
  void clear();
};

// parent class for user routine implementation
//---------------------------------------------
class Routine {
 public:
  Routine();

 protected:
  virtual void body() = 0;

  void next_step(unsigned long wait_time = 0);
  void first_step(unsigned long wait_time = 0);
  void repeat_step(unsigned long wait_time = 0);

  bool is_current_step();
  bool is_complete();

  // debug methods:
  unsigned long get_current_step();
  unsigned long get_steps_counter();

 private:
  friend class Engine;
  unsigned int current_step = 0;
  unsigned int steps_counter = 0;
  unsigned long next_step_time = 0;
  bool processing = false;
  bool started = false;

  void make_one_step();
};
}  // namespace VT
#endif