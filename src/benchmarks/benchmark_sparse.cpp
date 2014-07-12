/* =========================================================================
   Copyright (c) 2010-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.
   Portions of this software are copyright by UChicago Argonne, LLC.

                            -----------------
                  ViennaCL - The Vienna Computing Library
                            -----------------

   Project Head:    Karl Rupp                   rupp@iue.tuwien.ac.at

   (A list of authors and contributors can be found in the PDF manual)

   License:         MIT (X11), see file LICENSE in the base directory
============================================================================= */

/*
*   Benchmark:  Sparse matrix operations, i.e. matrix-vector products (sparse.cpp and sparse.cu are identical, the latter being required for compilation using CUDA nvcc)
*
*/

#include "benchmark_sparse.h"
#include <QDebug>

#define BENCHMARK_RUNS          10

Benchmark_Sparse::Benchmark_Sparse(QObject *parent) :
  AbstractBenchmark(parent)
{
  finalResultCounter = 0;
  finalResultValue = 0;
}

template<typename ScalarType>
void Benchmark_Sparse::run_benchmark()
{
  Timer timer;
  double exec_time;

  //ScalarType std_result = 0;

  ScalarType std_factor1 = ScalarType(3.1415);
  ScalarType std_factor2 = ScalarType(42.0);
  viennacl::scalar<ScalarType> vcl_factor1(std_factor1);
  viennacl::scalar<ScalarType> vcl_factor2(std_factor2);

//  viennacl::vector<ScalarType> ublas_vec1;
//  viennacl::vector<ScalarType> ublas_vec2;
  boost::numeric::ublas::vector<ScalarType> ublas_vec1;
  boost::numeric::ublas::vector<ScalarType> ublas_vec2;

  //create paths to data files, qt automatically takes care of / and \ characters
  QString  absoluteAppRootPath = QDir::currentPath();
  QString resultPathString = absoluteAppRootPath + "/testdata/result65025.txt" ;
  QString matrixPathString = absoluteAppRootPath + "/testdata/mat65k.mtx";

  if (!readVectorFromFile<ScalarType>( resultPathString , ublas_vec1))
  {
    std::cout << "Error reading RHS file" << std::endl;
    return;
  }
  std::cout << "done reading rhs results" << std::endl;
  ublas_vec2 = ublas_vec1;

  viennacl::compressed_matrix<ScalarType, 1> vcl_compressed_matrix_1;
  viennacl::compressed_matrix<ScalarType, 4> vcl_compressed_matrix_4;
  viennacl::compressed_matrix<ScalarType, 8> vcl_compressed_matrix_8;

  viennacl::coordinate_matrix<ScalarType> vcl_coordinate_matrix_128;

  viennacl::ell_matrix<ScalarType, 1> vcl_ell_matrix_1;
  viennacl::hyb_matrix<ScalarType, 1> vcl_hyb_matrix_1;

//  viennacl::compressed_matrix<ScalarType> ublas_matrix;
  boost::numeric::ublas::compressed_matrix<ScalarType> ublas_matrix;

  if (!viennacl::io::read_matrix_market_file(ublas_matrix, matrixPathString.toStdString() ) )
  {
    std::cout << "Error reading Matrix file" << std::endl;
    return;
  }

  std::cout << "done reading matrix" << std::endl;

  viennacl::vector<ScalarType> vcl_vec1(ublas_vec1.size());
  viennacl::vector<ScalarType> vcl_vec2(ublas_vec1.size());

  //cpu to gpu:
  viennacl::copy(ublas_matrix, vcl_compressed_matrix_1);
#ifndef VIENNACL_EXPERIMENTAL_DOUBLE_PRECISION_WITH_STREAM_SDK_ON_GPU
  viennacl::copy(ublas_matrix, vcl_compressed_matrix_4);
  viennacl::copy(ublas_matrix, vcl_compressed_matrix_8);
#endif
  viennacl::copy(ublas_matrix, vcl_coordinate_matrix_128);
  viennacl::copy(ublas_matrix, vcl_ell_matrix_1);
  viennacl::copy(ublas_matrix, vcl_hyb_matrix_1);
  viennacl::copy(ublas_vec1, vcl_vec1);
  viennacl::copy(ublas_vec2, vcl_vec2);


  ///////////// Matrix operations /////////////////

  double tempResultValue;

  std::cout << "------- Matrix-Vector product on CPU ----------" << std::endl;
  timer.start();
  for (int runs=0; runs<BENCHMARK_RUNS; ++runs)
  {
//    ublas_vec1 = viennacl::linalg::prod(ublas_matrix, ublas_vec2);
    ublas_vec1 = boost::numeric::ublas::prod(ublas_matrix, ublas_vec2);
    //    boost::numeric::ublas::axpy_prod(ublas_matrix, ublas_vec2, ublas_vec1, true);
  }
  exec_time = timer.get();
  std::cout << "CPU time: " << exec_time << std::endl;
  std::cout << "CPU "; tempResultValue = printOps(2.0 * static_cast<double>(ublas_matrix.nnz()), static_cast<double>(exec_time) / static_cast<double>(BENCHMARK_RUNS));
  std::cout << ublas_vec1[0] << std::endl;
  emit resultSignal("Matrix-Vector product on CPU", tempResultValue );
  finalResultValue += tempResultValue;
  finalResultCounter++;


  std::cout << "------- Matrix-Vector product with compressed_matrix ----------" << std::endl;


  vcl_vec1 = viennacl::linalg::prod(vcl_compressed_matrix_1, vcl_vec2); //startup calculation
  vcl_vec1 = viennacl::linalg::prod(vcl_compressed_matrix_4, vcl_vec2); //startup calculation
  vcl_vec1 = viennacl::linalg::prod(vcl_compressed_matrix_8, vcl_vec2); //startup calculation
  //std_result = 0.0;

  viennacl::backend::finish();
  timer.start();
  for (int runs=0; runs<BENCHMARK_RUNS; ++runs)
  {
    vcl_vec1 = viennacl::linalg::prod(vcl_compressed_matrix_1, vcl_vec2);
  }
  viennacl::backend::finish();
  exec_time = timer.get();
  std::cout << "GPU time align1: " << exec_time << std::endl;
  std::cout << "GPU align1 "; tempResultValue = printOps(2.0 * static_cast<double>(ublas_matrix.nnz()), static_cast<double>(exec_time) / static_cast<double>(BENCHMARK_RUNS));
  std::cout << vcl_vec1[0] << std::endl;
  emit resultSignal("Matrix-Vector product(compressed_matrix) align1", tempResultValue );
  finalResultValue += tempResultValue;
  finalResultCounter++;

  std::cout << "Testing triangular solves: compressed_matrix" << std::endl;

  viennacl::copy(ublas_vec1, vcl_vec1);
  viennacl::linalg::inplace_solve(trans(vcl_compressed_matrix_1), vcl_vec1, viennacl::linalg::unit_lower_tag());
  viennacl::copy(ublas_vec1, vcl_vec1);
  std::cout << "ublas..." << std::endl;
  timer.start();
//  viennacl::linalg::inplace_solve(trans(ublas_matrix), ublas_vec1, viennacl::linalg::unit_lower_tag());
  boost::numeric::ublas::inplace_solve(trans(ublas_matrix), ublas_vec1, boost::numeric::ublas::unit_lower_tag());
  std::cout << "Time elapsed: " << timer.get() << std::endl;
  std::cout << "ViennaCL..." << std::endl;
  viennacl::backend::finish();
  timer.start();
  viennacl::linalg::inplace_solve(trans(vcl_compressed_matrix_1), vcl_vec1, viennacl::linalg::unit_lower_tag());
  viennacl::backend::finish();
  std::cout << "Time elapsed: " << timer.get() << std::endl;

//  ublas_vec1 = viennacl::linalg::prod(ublas_matrix, ublas_vec2);
  ublas_vec1 = boost::numeric::ublas::prod(ublas_matrix, ublas_vec2);

  viennacl::backend::finish();
  timer.start();
  for (int runs=0; runs<BENCHMARK_RUNS; ++runs)
  {
    vcl_vec1 = viennacl::linalg::prod(vcl_compressed_matrix_4, vcl_vec2);
  }
  viennacl::backend::finish();
  exec_time = timer.get();
  std::cout << "GPU time align4: " << exec_time << std::endl;
  std::cout << "GPU align4 "; tempResultValue = printOps(2.0 * static_cast<double>(ublas_matrix.nnz()), static_cast<double>(exec_time) / static_cast<double>(BENCHMARK_RUNS));
  std::cout << vcl_vec1[0] << std::endl;
  emit resultSignal("Matrix-Vector product(compressed_matrix) align4", tempResultValue );
  finalResultValue += tempResultValue;
  finalResultCounter++;


  viennacl::backend::finish();
  timer.start();
  for (int runs=0; runs<BENCHMARK_RUNS; ++runs)
  {
    vcl_vec1 = viennacl::linalg::prod(vcl_compressed_matrix_8, vcl_vec2);
  }
  viennacl::backend::finish();
  exec_time = timer.get();
  std::cout << "GPU time align8: " << exec_time << std::endl;
  std::cout << "GPU align8 "; tempResultValue = printOps(2.0 * static_cast<double>(ublas_matrix.nnz()), static_cast<double>(exec_time) / static_cast<double>(BENCHMARK_RUNS));
  std::cout << vcl_vec1[0] << std::endl;
  emit resultSignal("Matrix-Vector product(compressed_matrix) align8", tempResultValue );
  finalResultValue += tempResultValue;
  finalResultCounter++;


  std::cout << "------- Matrix-Vector product with coordinate_matrix ----------" << std::endl;
  vcl_vec1 = viennacl::linalg::prod(vcl_coordinate_matrix_128, vcl_vec2); //startup calculation
  viennacl::backend::finish();

  viennacl::copy(vcl_vec1, ublas_vec2);
  long err_cnt = 0;
  for (std::size_t i=0; i<ublas_vec1.size(); ++i)
  {
    if ( fabs(ublas_vec1[i] - ublas_vec2[i]) / std::max(fabs(ublas_vec1[i]), fabs(ublas_vec2[i])) > 1e-2)
    {
      std::cout << "Error at index " << i << ": Should: " << ublas_vec1[i] << ", Is: " << ublas_vec2[i] << std::endl;
      ++err_cnt;
      if (err_cnt > 5)
        break;
    }
  }

  viennacl::backend::finish();
  timer.start();
  for (int runs=0; runs<BENCHMARK_RUNS; ++runs)
  {
    vcl_vec1 = viennacl::linalg::prod(vcl_coordinate_matrix_128, vcl_vec2);
  }
  viennacl::backend::finish();
  exec_time = timer.get();
  std::cout << "GPU time: " << exec_time << std::endl;
  std::cout << "GPU "; tempResultValue = printOps(2.0 * static_cast<double>(ublas_matrix.nnz()), static_cast<double>(exec_time) / static_cast<double>(BENCHMARK_RUNS));
  std::cout << vcl_vec1[0] << std::endl;
  emit resultSignal("Matrix-Vector product(coordinate_matrix)", tempResultValue );
  finalResultValue += tempResultValue;
  finalResultCounter++;

  std::cout << "------- Matrix-Vector product with ell_matrix ----------" << std::endl;
  vcl_vec1 = viennacl::linalg::prod(vcl_ell_matrix_1, vcl_vec2); //startup calculation
  viennacl::backend::finish();

  viennacl::copy(vcl_vec1, ublas_vec2);
  err_cnt = 0;
  for (std::size_t i=0; i<ublas_vec1.size(); ++i)
  {
    if ( fabs(ublas_vec1[i] - ublas_vec2[i]) / std::max(fabs(ublas_vec1[i]), fabs(ublas_vec2[i])) > 1e-2)
    {
      std::cout << "Error at index " << i << ": Should: " << ublas_vec1[i] << ", Is: " << ublas_vec2[i] << std::endl;
      ++err_cnt;
      if (err_cnt > 5)
        break;
    }
  }

  viennacl::backend::finish();
  timer.start();
  for (int runs=0; runs<BENCHMARK_RUNS; ++runs)
  {
    vcl_vec1 = viennacl::linalg::prod(vcl_ell_matrix_1, vcl_vec2);
  }
  viennacl::backend::finish();
  exec_time = timer.get();
  std::cout << "GPU time: " << exec_time << std::endl;
  std::cout << "GPU "; tempResultValue = printOps(2.0 * static_cast<double>(ublas_matrix.nnz()), static_cast<double>(exec_time) / static_cast<double>(BENCHMARK_RUNS));
  std::cout << vcl_vec1[0] << std::endl;
  emit resultSignal("Matrix-Vector product(ell_matrix)", tempResultValue );
  finalResultValue += tempResultValue;
  finalResultCounter++;

  std::cout << "------- Matrix-Vector product with hyb_matrix ----------" << std::endl;
  vcl_vec1 = viennacl::linalg::prod(vcl_hyb_matrix_1, vcl_vec2); //startup calculation
  viennacl::backend::finish();

  viennacl::copy(vcl_vec1, ublas_vec2);
  err_cnt = 0;
  for (std::size_t i=0; i<ublas_vec1.size(); ++i)
  {
    if ( fabs(ublas_vec1[i] - ublas_vec2[i]) / std::max(fabs(ublas_vec1[i]), fabs(ublas_vec2[i])) > 1e-2)
    {
      std::cout << "Error at index " << i << ": Should: " << ublas_vec1[i] << ", Is: " << ublas_vec2[i] << std::endl;
      ++err_cnt;
      if (err_cnt > 5)
        break;
    }
  }

  viennacl::backend::finish();
  timer.start();
  for (int runs=0; runs<BENCHMARK_RUNS; ++runs)
  {
    vcl_vec1 = viennacl::linalg::prod(vcl_hyb_matrix_1, vcl_vec2);
  }
  viennacl::backend::finish();
  exec_time = timer.get();
  std::cout << "GPU time: " << exec_time << std::endl;
  std::cout << "GPU "; tempResultValue = printOps(2.0 * static_cast<double>(ublas_matrix.nnz()), static_cast<double>(exec_time) / static_cast<double>(BENCHMARK_RUNS));
  std::cout << vcl_vec1[0] << std::endl;
  emit resultSignal("Matrix-Vector product(hyb_matrix)", tempResultValue );
  finalResultValue += tempResultValue;
  finalResultCounter++;

}

void Benchmark_Sparse::execute()
{
  emit benchmarkStarted(SPARSE);
  emit unitMeasureSignal("GFLOPs");
  std::cout << std::endl;
  std::cout << "----------------------------------------------" << std::endl;
  std::cout << "               Device Info" << std::endl;
  std::cout << "----------------------------------------------" << std::endl;

#ifdef VIENNACL_WITH_OPENCL
  std::cout << viennacl::ocl::current_device().info() << std::endl;
#endif
  std::cout << std::endl;
  std::cout << "----------------------------------------------" << std::endl;
  std::cout << "----------------------------------------------" << std::endl;
  std::cout << "## Benchmark :: Sparse" << std::endl;
  std::cout << "----------------------------------------------" << std::endl;
  std::cout << std::endl;
  std::cout << "   -------------------------------" << std::endl;
  std::cout << "   # benchmarking single-precision" << std::endl;
  std::cout << "   -------------------------------" << std::endl;
  run_benchmark<float>();
#ifdef VIENNACL_WITH_OPENCL
  if( viennacl::ocl::current_device().double_support() )
#endif
  {
    std::cout << std::endl;
    std::cout << "   -------------------------------" << std::endl;
    std::cout << "   # benchmarking double-precision" << std::endl;
    std::cout << "   -------------------------------" << std::endl;
    run_benchmark<double>();
  }
  emit finalResultSignal("Sparse", finalResultValue/finalResultCounter);
  emit benchmarkComplete();
}
