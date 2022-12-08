#include "distribute.h"

namespace lhts {
namespace distribute {
MatrixXf top_down(const MatrixXi S_compact, const MatrixXf P,
                  const MatrixXf yhat, int num_base, int num_total,
                  int num_levels) {
  MatrixXf y = MatrixXf::Zero(num_base, yhat.cols());

  assert(S_compact.rows() == num_total);
  assert(S_compact.cols() == num_levels);
  assert(num_levels > 1);

#pragma omp parallel for
  for (int i = 0; i < num_total; i++) {
    int co = S_compact(i, 0);
    int root = -1;
    bool is_base = true;
    for (int j = 1; j < num_levels; j++) {
      int ro = S_compact(i, j);
      if (ro == -1) {
        is_base = false;
        break;
      }
      root = ro;
    }
    if (is_base) {
      y.middleRows(co, 1) = P(co, 0) * yhat.middleRows(root, 1);
    }
  }

  return y;
}

MatrixXf middle_out(const MatrixXi S_compact, const MatrixXf P,
                    const MatrixXf yhat, int level, int num_base, int num_total,
                    int num_levels) {
  MatrixXf y = MatrixXf::Zero(num_base, yhat.cols());

  assert(S_compact.rows() == num_total);
  assert(S_compact.cols() == num_levels);
  assert(num_levels > 1);

#pragma omp parallel for
  for (int i = 0; i < num_total; i++) {
    int co = S_compact(i, 0);
    int root = co;
    int lvl = num_levels - level;
    bool is_base = true;
    for (int j = 1; j < num_levels; j++) {
      int ro = S_compact(i, j);
      if (ro == -1) {
        is_base = false;
        break;
      }
      if (lvl > 0) {
        root = ro;
        lvl--;
      }
    }
    if (is_base) {
      y.middleRows(co, 1) = P(co, 0) * yhat.middleRows(root, 1);
    }
  }

  return y;
}
}  // namespace distribute
}  // namespace lhts