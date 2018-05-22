#include <iostream>
#include <cmath>

#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>


class Integrand {
  public:
    double operator()(double x) const {
      // log(10, x) | sin x|
      return log10(x) * fabs(sin(x));
    }
};


bool check_range(double i2, double i, double epsilon) {
  return fabs(i2 - i) < epsilon;
};


template<typename Integrand>
class IntegrateParabols {
  public:
    IntegrateParabols(Integrand f_, double epsilon_):
      f(f_),
      epsilon(epsilon_)
    {}
    double operator()(const tbb::blocked_range<double> &r, double start) const {
      int num_iterations = 1;
      double last_res = 0.0;
      double res = -1.0;
      double h = 0;

      double x1, x2, x3;


      while (!check_range(res, last_res, epsilon)) {
        last_res = res;
        res = 0.0;
        h = (r.end() - r.begin()) / num_iterations;

        for (int i = 0; i < num_iterations; ++i) {
          x1 = r.begin() + i * h;
          x3 = x1 + h;
          x2 = (x1 + x3) / 2.0;

          res += (h / 6.0) * (f(x1) + 4 * f(x2) + f(x3));
        }

        num_iterations *= 2;
      };

      return start + res;
    }

    Integrand f;
    double epsilon;
};


class IntegrateParabolsReduction {
  public:
    double operator()(double acc, double val) const {
      return acc + val;
    }
};


template<typename Integrand>
double parallel_integrate_parabols(
  Integrand f, double start, double end, double epsilon
) {
  return tbb::parallel_reduce(
    tbb::blocked_range<double>(start, end, 0),
    0.0,
    IntegrateParabols<Integrand>(f, epsilon),
    IntegrateParabolsReduction()
  );
}


int main() {
  const int P_max = tbb::task_scheduler_init::default_num_threads();
  for (int P = 1; P <= P_max; P++) {
    tbb::task_scheduler_init init(P);
    tbb::tick_count t0 = tbb::tick_count::now();
    std::cout << parallel_integrate_parabols(Integrand(), 1, 250, 1e-5) << std::endl;
    tbb::tick_count t1 = tbb::tick_count::now();
    double t = (t1 - t0).seconds();
    std::cout << "time: " << t  << " with " << P << " threads" << std::endl;
  }
}
