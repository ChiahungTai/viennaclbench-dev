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
*
*   Benchmark:   Vector operations (vector.cpp and vector.cu are identical, the latter being required for compilation using CUDA nvcc)
*
*/

#include "benchmark_vector.h"

/*!
 * \brief Default constructor.
 * Sets the precision to double and load default vector sizes & increment factor from \ref BenchmarkSettings
 * \param parent Optional parent object.
 */
Benchmark_Vector::Benchmark_Vector(QObject *parent) :
  AbstractBenchmark(parent)
{
  testResultHolder.clear();
  setPrecision(DOUBLE_PRECISION);
  BenchmarkSettings settings;
  MAX_BENCHMARK_VECTOR_SIZE = settings.vectorMaxVectorSize;
  MIN_BENCHMARK_VECTOR_SIZE = settings.vectorMinVectorSize;
  INCREMENT_FACTOR = settings.vectorIncFactor;
}

/*!
 * \brief Constructor with precision and settings
 * \param precision Benchmark precision
 * \param settings Settings from which to load vector sizes & increment factor
 */
Benchmark_Vector::Benchmark_Vector(bool precision, BenchmarkSettings settings)
{
  Benchmark_Vector();
  setPrecision(precision);
  MAX_BENCHMARK_VECTOR_SIZE = settings.vectorMaxVectorSize;
  MIN_BENCHMARK_VECTOR_SIZE = settings.vectorMinVectorSize;
  INCREMENT_FACTOR = settings.vectorIncFactor;
}

template<typename ScalarType>
/*!
 * \brief Resizes the benchmark vectors to \a size.
 * Not used. The benchmark runs a for-loop and initiates new vectors on each run.
 * Should be used if any changes are to occur in the future.
 * \param size New vector size.
 * \param std_vec1 std vector 1
 * \param std_vec2 std vector 2
 * \param vcl_vec1 vcl vector 1
 * \param vcl_vec2 vcl vector 2
 */
void Benchmark_Vector::resizeVectors(int size, std::vector<ScalarType> &std_vec1, std::vector<ScalarType> &std_vec2,
                                     viennacl::vector<ScalarType> &vcl_vec1, viennacl::vector<ScalarType> &vcl_vec2){

  std_vec1.resize(size);
  std_vec2.resize(size);
  vcl_vec1.resize(size);
  vcl_vec2.resize(size);

  std_vec1[0] = 1.0;
  std_vec2[0] = 1.0;
  for (std::size_t i=1; i<size; ++i)
  {
    std_vec1[i] = std_vec1[i-1] * ScalarType(1.000001);
    std_vec2[i] = std_vec1[i-1] * ScalarType(0.999999);
  }

  // warmup:
  viennacl::copy(std_vec1, vcl_vec1);
  viennacl::fast_copy(std_vec2, vcl_vec2);
  viennacl::async_copy(std_vec2, vcl_vec1);
  viennacl::backend::finish();
}

/*!
 * \brief Main benchmarking function
 * Should only be called by the \ref Benchmark_Vector::execute() function,
 * since there are certain requirements that need to be fulfilled before starting the benchmarking procedure.
 */
template<typename ScalarType>
void Benchmark_Vector::run_benchmark()
{
  Timer timer;
  double exec_time;

//  std::cout << "Benchmarking..." << std::endl;
//  std::cout << "Platform id: "<< viennacl::ocl::current_context().platform_index() //platform id != context id
//            <<" Context value: " << viennacl::ocl::current_context().handle().get() << std::endl;

//  std::cout << "Running on device name: "<< viennacl::ocl::current_device().name() << std::endl;


  /* HOLD MY BEER, IMMA GONNA FORLOOP EVERYTHING */
  /* Seriously, I run the entire test suite for each vector size */

  for(int vectorSize = MIN_BENCHMARK_VECTOR_SIZE; vectorSize <= MAX_BENCHMARK_VECTOR_SIZE; vectorSize *= INCREMENT_FACTOR){

    int testId = 0;
    ScalarType std_result = 0;

    ScalarType std_factor1 = static_cast<ScalarType>(3.1415);
    ScalarType std_factor2 = static_cast<ScalarType>(42.0);
    viennacl::scalar<ScalarType> vcl_factor1(std_factor1);
    viennacl::scalar<ScalarType> vcl_factor2(std_factor2);

    std::vector<ScalarType> std_vec1(vectorSize);
    std::vector<ScalarType> std_vec2(vectorSize);
    std::vector<ScalarType> std_vec3(vectorSize);
    viennacl::vector<ScalarType> vcl_vec1(vectorSize);
    viennacl::vector<ScalarType> vcl_vec2(vectorSize);
    viennacl::vector<ScalarType> vcl_vec3(vectorSize);


    ///////////// Vector operations /////////////////

    double effective_bandwidth;

    std_vec1[0] = 1.0;
    std_vec2[0] = 1.0;
    for (std::size_t i=1; i<vectorSize; ++i)
    {
      std_vec1[i] = std_vec1[i-1] * ScalarType(1.000001);
      std_vec2[i] = std_vec1[i-1] * ScalarType(0.999999);
    }

    viennacl::copy(std_vec1, vcl_vec1);
    viennacl::copy(std_vec2, vcl_vec2);

    //
    // inner product
    //
    viennacl::backend::finish();

    timer.start();
    std_result = 0;
    for (std::size_t runs=0; runs<BENCHMARK_RUNS; ++runs)
    {
      for (std::size_t i=0; i<vectorSize; ++i)
        std_result += std_vec1[i] * std_vec2[i];
    }
    exec_time = timer.get();

    if (std_result > 0) // trivially true, but ensures nothing is optimized away
      effective_bandwidth = 2 * vectorSize * sizeof(ScalarType) / exec_time * BENCHMARK_RUNS / 1e9;

    emit resultSignal("Vector inner products - CPU", vectorSize, effective_bandwidth, LINE_GRAPH, testId );
    testId++;
    testResultHolder.append(effective_bandwidth);
    emit testProgress();


    std_result = viennacl::linalg::inner_prod(vcl_vec1, vcl_vec2); //startup calculation
    std_result = 0.0;
    viennacl::backend::finish();
    timer.start();
    for (std::size_t runs=0; runs<BENCHMARK_RUNS; ++runs)
    {
      vcl_factor2 = viennacl::linalg::inner_prod(vcl_vec1, vcl_vec2);
    }
    viennacl::backend::finish();
    exec_time = timer.get();

    effective_bandwidth = 2 * vectorSize * sizeof(ScalarType) / exec_time * BENCHMARK_RUNS / 1e9;
    emit resultSignal("Vector inner products - GPU", vectorSize, effective_bandwidth, LINE_GRAPH, testId );
    testId++;
    testResultHolder.append(effective_bandwidth);
    emit testProgress();



    //
    // vector addition
    //

    timer.start();
    for (std::size_t runs=0; runs<BENCHMARK_RUNS; ++runs)
    {
      for (std::size_t i=0; i<vectorSize; ++i)
        std_vec3[i] = std_vec1[i] + std_vec2[i];
    }
    exec_time = timer.get();

    effective_bandwidth = 3 * vectorSize * sizeof(ScalarType) / exec_time * BENCHMARK_RUNS / 1e9;
    emit resultSignal("Vector addition - CPU", vectorSize, effective_bandwidth, LINE_GRAPH, testId );
    testId++;
    testResultHolder.append(effective_bandwidth);
    emit testProgress();

    vcl_vec3 = vcl_vec1 + vcl_vec2; //startup calculation
    viennacl::backend::finish();
    std_result = 0.0;
    timer.start();
    for (std::size_t runs=0; runs<BENCHMARK_RUNS; ++runs)
    {
      vcl_vec3 = vcl_vec1 + vcl_vec2;
    }
    viennacl::backend::finish();
    exec_time = timer.get();

    effective_bandwidth = 3 * vectorSize * sizeof(ScalarType) / exec_time * BENCHMARK_RUNS / 1e9;
    emit resultSignal("Vector addition - GPU", vectorSize, effective_bandwidth, LINE_GRAPH, testId );
    testId++;
    testResultHolder.append(effective_bandwidth);
    emit testProgress();

    //
    // multiply add:
    //
    timer.start();
    for (std::size_t runs=0; runs<BENCHMARK_RUNS; ++runs)
    {
      for (std::size_t i=0; i<vectorSize; ++i)
        std_vec1[i] += std_factor1 * std_vec2[i];
    }
    exec_time = timer.get();

    effective_bandwidth = 3 * vectorSize * sizeof(ScalarType) / exec_time * BENCHMARK_RUNS / 1e9;
    emit resultSignal("Vector multiply add - CPU", vectorSize, effective_bandwidth, LINE_GRAPH, testId );
    testId++;
    testResultHolder.append(effective_bandwidth);
    emit testProgress();

    vcl_vec1 += vcl_factor1 * vcl_vec2; //startup calculation
    viennacl::backend::finish();
    timer.start();
    for (std::size_t runs=0; runs<BENCHMARK_RUNS; ++runs)
    {
      vcl_vec1 += vcl_factor1 * vcl_vec2;
    }
    viennacl::backend::finish();
    exec_time = timer.get();

    effective_bandwidth = 3 * vectorSize * sizeof(ScalarType) / exec_time * BENCHMARK_RUNS / 1e9;
    emit resultSignal("Vector multiply add - GPU", vectorSize, effective_bandwidth, LINE_GRAPH, testId );
    testId++;
    testResultHolder.append(effective_bandwidth);
    emit testProgress();


  }
}

/*!
 * \brief Begins the benchmark execution.
 */
void Benchmark_Vector::execute()
{
  emit benchmarkStarted(VECTOR);
  emit unitMeasureSignal("GB/sec", Qt::YAxis);
  emit unitMeasureSignal("Vector Size", Qt::XAxis);

  if(getPrecision() == SINGLE_PRECISION)
  {//Single
    run_benchmark<float>();
  }

  else if( getPrecision() == DOUBLE_PRECISION)
  {//Double
#ifdef VIENNACL_WITH_OPENCL
    if( viennacl::ocl::current_device().double_support() )
#endif
      //what if current device does not support double precision?
    {
      run_benchmark<double>();
    }
  }

  qSort(testResultHolder);//sort test results in ascending order
  emit finalResultSignal("Vector", testResultHolder[testResultHolder.size()-1]);
  emit benchmarkComplete();
}
