#include <chrono>
#include <iostream>

// int (*func)(void);
// using func = int (*)();
// template <typename F>

class Base {
 public:
  virtual void run() = 0;
  virtual uint32_t wait() = 0;
};

class Service1 : public Base {
 public:
  Service1(char n, int s1, int s2) : name(n), s1(s1), s2(s2) {
    start_time = std::chrono::high_resolution_clock::now();
  }
  void run() {
    // std::cout << "run" << step << std::endl;
    auto current_time = std::chrono::high_resolution_clock::now();
    if(0 == step){
        std::cout << name << ":0" << std::endl;
        // std::cout << "Program has been running for " << std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time).count() << " microseconds" << std::endl;
        wait_time = current_time + std::chrono::microseconds(s1);
        // std::cout << "wait1:" << std::chrono::duration_cast<std::chrono::microseconds>(wait_time-current_time).count() << std::endl;
        step++;
        return;
    }
    if(1 == step){
        std::cout << name << ":1" << std::endl;
        // std::cout << "Program has been running for " << std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time).count() << " microseconds" << std::endl;
        wait_time = current_time + std::chrono::microseconds(s2);
        // std::cout << "wait2:" << std::chrono::duration_cast<std::chrono::microseconds>(wait_time-current_time).count() << std::endl;
        step++;
        return;
    }
    step = 0;
  }

  uint32_t wait() {
    auto current_time = std::chrono::high_resolution_clock::now();
    auto val =  std::chrono::duration_cast<std::chrono::microseconds>(wait_time-current_time).count();
    // std::cout << "wait0:" << val << std::endl;
    if (val < 0){
      return 0;
    }
    return val;
  }

  char name;
  int s1, s2;
  int step = 0;
  std::chrono::high_resolution_clock::time_point start_time;
  std::chrono::high_resolution_clock::time_point wait_time;
};

void add(Base &b) {
  b.run();
}

int main() {
  Service1 s1('*', 100000, 200000);
  Service1 s2('>', 500000, 500000);
  Service1 s3('=', 200000, 200000);

  Base *todo[] = {&s1, &s2, &s3};

  for(auto rt = 0; rt < 1000; rt++){
    long time = 1000000;
    int len = sizeof(todo)/sizeof(&s1);
    for(auto i = 0; i < len ; i++){
      auto w = todo[i]->wait();
      if(w <= 0){
        todo[i]->run();
      }
      if(w < time) {
        time = w;
      }
    }
    usleep(time);    
  }

/*  for (auto i = 0; i < 1000; ++i) {
    auto s1w = s1.wait();
    if(s1w <= 0){
      s1.run();
    }
    if(s1w < time) {
      time = s1w;
    }

    auto s2w = s2.wait();
    if(s2w <= 0){
      s2.run();
    }
    if(s2w < time) {
      time = s2w;
    }

    auto s3w = s3.wait();
    if(s3w <= 0){
      s3.run();
    }
    if(s3w < time) {
      time = s3w;
    }
    // std::cout << "usleep" << time << std::endl; 
    usleep(time);
  }/*/
}