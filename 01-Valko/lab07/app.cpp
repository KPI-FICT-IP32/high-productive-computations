#include <iostream>
#include <cmath>

#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>

class Integrand {
    public:
        double operator()(double x) const {
            return pow(x, x + 3);
        }
};

bool check_range(double i2, double i, double epsilon){
    return fabs(i2 - i) < epsilon;
}

template<typename Integrand>
class IntegrateLeftRectangle {
    public:
        IntegrateLeftRectangle(Integrand f_, double epsilon_):
            f(f_),
            epsilon(epsilon_)
        {}
        double operator()(const tbb::blocked_range<double> &r, double start) const {
            int num_iterations = 1;
            double last_res = 0.0;
            double res = -1.0;
            double h = 0.0;

            double x1;

            while(!check_range(res, last_res, epsilon)) {
                last_res = res;
                res = 0.0;
                h = (r.end() - r.begin()) / num_iterations;

                for (int i = 0; i < num_iterations; ++i) {
                    x1 = r.begin() + i * h;
                    res += h * f(x1);
                }

                num_iterations *= 2;
            }
            return start + res;
        }
        Integrand f;
        double epsilon;
};

class IntegrateLeftRectangleReduction {
    public:
        double operator()(double acc, double val) const {
            return acc + val;
        }
};

template<typename Integrand>
double parallel_integrate_left_rectangle(
    Integrand f, double start, double end, double epsilon
) {
    return tbb::parallel_reduce(
        tbb::blocked_range<double>(start, end, 0),
        0.0,
        IntegrateLeftRectangle<Integrand>(f, epsilon),
        IntegrateLeftRectangleReduction()
    );
}

int main() {
    const int P_max = tbb::task_scheduler_init::default_num_threads();
    for (int P = 1; P <= P_max; P++) {
        tbb::task_scheduler_init init(P_max);
        tbb::tick_count t0 = tbb::tick_count::now();
        std::cout << parallel_integrate_left_rectangle(Integrand(), 0, 3.5, 1e-4) << std::endl;
        tbb::tick_count t1 = tbb::tick_count::now();
        double t = (t1 - t0).seconds();
        std::cout << "time = " << t <<" with " << P << " threads " << std::endl;
    }
}
