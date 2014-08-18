#include "expertbenchmark.h"
#include "ui_expertbenchmark.h"

ExpertBenchmark::ExpertBenchmark(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ExpertBenchmark)
{
  ui->setupUi(this);

  startBenchmarkButton = ui->expert_StartBenchmarkButton;
  stopBenchmarkButton = ui->expert_StopBenchmarkButton;
  progressBar = ui->expert_ProgressBar;
  benchmarkListWidget = ui->expert_BenchmarkListWidget;
  contextComboBox = ui->expert_contextComboBox;
  singlePrecisionButton = ui->expert_SingleButton;

  maximumBenchProgress = 0;
  currentBenchProgress = 0;

  connect(ui->expert_DoubleButton, SIGNAL(clicked()), this, SLOT(updateDoublePrecisionButtons()) );
  connect(ui->expert_SingleButton, SIGNAL(clicked()), this, SLOT(updateSinglePrecisionButtons()) );
  initExpert();
}

ExpertBenchmark::~ExpertBenchmark()
{
  delete ui;
}

void ExpertBenchmark::initExpert(){
  //  connect(ui->expert_BenchmarkListWidget, SIGNAL(itemPressed(QListWidgetItem*)), ui->expert_BenchmarkListWidget, SLOT() );
  //  connect(ui->expert_BenchmarkListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(updateBenchmarkListWidget(QListWidgetItem*)) );

  expert_DetailedPlotTab = new QTabWidget(this);
  expert_DetailedPlotTab->setStyleSheet("QTabBar::tab{width: 75px;height: 25px;}");

  blas3_DetailedPlot = new QCustomPlot();
  copy_DetailedPlot = new QCustomPlot();
  //  qr_expertDetailedPlot = new QCustomPlot();
  //  solver_expertDetailedPlot = new QCustomPlot();
  sparse_DetailedPlot = new QCustomPlot();
  vector_DetailedPlot = new QCustomPlot();

  expert_DetailedPlotsVector.insert(BLAS3, blas3_DetailedPlot);
  expert_DetailedPlotsVector.insert(COPY, copy_DetailedPlot);
  //  expert_DetailedPlotsVector.insert(QR, qr_DetailedPlot);
  //  expert_DetailedPlotsVector.insert(SOLVER, solver_DetailedPlot);
  expert_DetailedPlotsVector.insert(SPARSE, sparse_DetailedPlot);
  expert_DetailedPlotsVector.insert(VECTOR, vector_DetailedPlot);

  expert_DetailedPlotTab->insertTab(BLAS3, blas3_DetailedPlot,"Blas3");
  expert_DetailedPlotTab->insertTab(COPY, copy_DetailedPlot,"Copy");
  //  expert_DetailedPlotTab->insertTab(QR, qr_DetailedPlot,"Qr");
  //  expert_DetailedPlotTab->insertTab(SOLVER, solver_DetailedPlot,"Solver");
  expert_DetailedPlotTab->insertTab(SPARSE, sparse_DetailedPlot,"Sparse");
  expert_DetailedPlotTab->insertTab(VECTOR, vector_DetailedPlot,"Vector");

  ui->expert_CollapseWidget->setChildWidget(expert_DetailedPlotTab);
  ui->expert_CollapseWidget->setText("Detailed Test Results");

  //xAxis bottom
  //yAxis left
  //xAxis2 top
  //yAxis2 right
  QColor backgroundColor(240,240,240);
  QBrush backgroundBrush(backgroundColor);

  foreach(QCustomPlot* plot, expert_DetailedPlotsVector){

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(plot, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));

    //filter each item selection
    connect(plot, SIGNAL( selectionChangedByUser()), this, SLOT(selectionChanged()) );
    //    plot->axisRect()->setAutoMargins(QCP::msNone);
    //    plot->axisRect()->setMargins(QMargins( 0, 0, 50, 0 ));

    plot->axisRect()->setupFullAxesBox();
    //Disable secondary axes
    plot->yAxis2->setVisible(false);
    plot->xAxis2->setVisible(false);

    plot->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend);
    plot->legend->setVisible(false);


    plot->yAxis->setTickLength( 0, 2);
    plot->yAxis->grid()->setVisible(true);
    plot->yAxis->setTickLabelRotation( 0 );

    plot->yAxis->setAutoSubTicks(false);
    plot->yAxis->setAutoTickLabels(false);
    plot->yAxis->setAutoTicks(false);
    plot->yAxis->setAutoTickStep(false);
    QVector<double> emptyTickVector;
    plot->yAxis->setTickVector(emptyTickVector);
    QVector<QString> emptyTickVectorLabels;
    plot->yAxis->setTickVectorLabels(emptyTickVectorLabels);

    plot->xAxis->setAutoSubTicks(true);
    plot->xAxis->setAutoTickLabels(true);
    plot->xAxis->setAutoTicks(true);
    plot->xAxis->setAutoTickStep(true);

    plot->setBackground(backgroundBrush);
  }


  ui->expert_FinalResultPlot->axisRect()->setAutoMargins(QCP::msNone);
  ui->expert_FinalResultPlot->axisRect()->setMargins(QMargins( 100, 15, 60, 40 ));
  ui->expert_FinalResultPlot->axisRect()->setupFullAxesBox();
  //Disable secondary axes & legend
  ui->expert_FinalResultPlot->yAxis2->setVisible(false);
  ui->expert_FinalResultPlot->xAxis2->setVisible(false);
  ui->expert_FinalResultPlot->legend->setVisible(false);
  //Enable selecting plots
  ui->expert_FinalResultPlot->setInteractions(QCP::iSelectPlottables|QCP::iRangeDrag|QCP::iRangeZoom);

  // connect slot that shows a message in the status bar when a graph is clicked:
  connect(ui->expert_FinalResultPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));

  QVector<QString> finalResultPlotLabels;
  finalResultPlotLabels.append("Vector - GFLOPs");
  finalResultPlotLabels.append("Sparse - GFLOPs");
  //  finalResultPlotLabels.append("Solver - GFLOPs");
  //  finalResultPlotLabels.append("Qr - GFLOPs");
  finalResultPlotLabels.append("Copy - GB/s");
  finalResultPlotLabels.append("Blas3 - GFLOPs");

  //  Plot mapping
  //  Vector - 1
  //  Sparse - 2
  //  Copy - 3
  //  Blas3 - 4

  QVector<double> finalResultPlotTicks;
  finalResultPlotTicks.append(1);
  finalResultPlotTicks.append(2);
  finalResultPlotTicks.append(3);
  finalResultPlotTicks.append(4);

  ui->expert_FinalResultPlot->yAxis->setAutoTickLabels(false);
  ui->expert_FinalResultPlot->yAxis->setAutoTicks(false);
  ui->expert_FinalResultPlot->yAxis->setTickVectorLabels(finalResultPlotLabels);
  ui->expert_FinalResultPlot->yAxis->setTickVector(finalResultPlotTicks);
  ui->expert_FinalResultPlot->yAxis->setSubTickCount( 0 );
  ui->expert_FinalResultPlot->yAxis->setTickLength( 0, 2);
  ui->expert_FinalResultPlot->yAxis->setRange( 0.5, 5.0);
  ui->expert_FinalResultPlot->yAxis->grid()->setVisible(true);
  ui->expert_FinalResultPlot->yAxis->setTickLabelRotation( 0 );


  ui->expert_FinalResultPlot->xAxis->grid()->setSubGridVisible(true);
  ui->expert_FinalResultPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
  ui->expert_FinalResultPlot->xAxis->setScaleLogBase(10);
  ui->expert_FinalResultPlot->xAxis->setNumberFormat("f"); // e = exponential, b = beautiful decimal powers
  ui->expert_FinalResultPlot->xAxis->setNumberPrecision(0);
  ui->expert_FinalResultPlot->xAxis->setAutoTicks(false);
  ui->expert_FinalResultPlot->xAxis->setAutoTickLabels(false);
  ui->expert_FinalResultPlot->xAxis->setAutoTickStep(false);
  QVector<double> ticks;
  ticks.append(0.5);
  ticks.append(1);
  ticks.append(5);
  ticks.append(10);
  ticks.append(20);
  ticks.append(50);
  ticks.append(100);
  ticks.append(200);
  ticks.append(500);
  ticks.append(1000);
  ticks.append(2000);
  ui->expert_FinalResultPlot->xAxis->setTickVector(ticks);

  QVector<QString> tickLabels;
  tickLabels.append("0.5");
  tickLabels.append("1");
  tickLabels.append("5");
  tickLabels.append("10");
  tickLabels.append("20");
  tickLabels.append("50");
  tickLabels.append("100");
  tickLabels.append("200");
  tickLabels.append("500");
  tickLabels.append("1000");
  tickLabels.append("2000");
  ui->expert_FinalResultPlot->xAxis->setTickVectorLabels(tickLabels);

  //  ui->expert_FinalResultPlot->xAxis->setTickLengthOut(200);
  ui->expert_FinalResultPlot->xAxis->setRangeLower(0);
  //  ui->expert_FinalResultPlot->xAxis->setRange(0,1);

  ui->expert_FinalResultPlot->setBackground(backgroundBrush);

  ui->expert_StopBenchmarkButton->hide();

  connect(ui->expert_BenchmarkListWidget, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(updateBenchmarkListWidget(QListWidgetItem*)) );
  connect(ui->expert_BenchmarkListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(updateBenchmarkListWidget(QListWidgetItem*)) );
  for ( int i = 0; i < ui->expert_BenchmarkListWidget->count(); i++ ) {
    ui->expert_BenchmarkListWidget->item(i)->setSelected(true);
  }
  connect(ui->expert_CustomMatrixBrowsebutton, SIGNAL(clicked()), this, SLOT(setCustomSparseMatrixPath()) );
  connect(ui->expert_CustomMatrixDefaultButton, SIGNAL(clicked()), ui->expert_SparseCustomMatrix, SLOT(clear()) );
}

void ExpertBenchmark::showBenchmarkStartButton(){
  ui->expert_StopBenchmarkButton->hide();
  ui->expert_StartBenchmarkButton->show();
  ui->expert_ProgressBar->setFormat("Done");
}

void ExpertBenchmark::updateSinglePrecisionButtons(){
  ui->expert_SingleButton->setChecked(true);
  ui->expert_SingleButton->setIcon(QIcon(":/icons/icons/checkTrue.png"));

  ui->expert_DoubleButton->setChecked(false);
  ui->expert_DoubleButton->setIcon(QIcon(":/icons/icons/empty.png"));

}

void ExpertBenchmark::updateDoublePrecisionButtons(){
  ui->expert_DoubleButton->setChecked(true);
  ui->expert_DoubleButton->setIcon(QIcon(":/icons/icons/checkTrue.png"));

  ui->expert_SingleButton->setChecked(false);
  ui->expert_SingleButton->setIcon(QIcon(":/icons/icons/empty.png"));

}

void ExpertBenchmark::setActiveBenchmarkPlot(int benchmarkIdNumber){
  expert_DetailedPlotTab->setCurrentIndex(benchmarkIdNumber);
  activeBenchmark = benchmarkIdNumber;
}

void ExpertBenchmark::updateFinalResultPlot(QString benchmarkName, double finalResult){
  plotFinalResult(benchmarkName, finalResult, ui->expert_FinalResultPlot);
}

//shows the detailed graph of a clicked final result bar
void ExpertBenchmark::graphClicked(QCPAbstractPlottable *plottable)
{
  QString clickedBenchmarkBar = plottable->name();
  if(clickedBenchmarkBar == "Blas3"){
    expert_DetailedPlotTab->setCurrentIndex(BLAS3);
  }
  else if(clickedBenchmarkBar == "Copy"){
    expert_DetailedPlotTab->setCurrentIndex(COPY);
  }
  else if(clickedBenchmarkBar == "Qr"){
    expert_DetailedPlotTab->setCurrentIndex(QR);
  }
  else if(clickedBenchmarkBar == "Scheduler"){
    expert_DetailedPlotTab->setCurrentIndex(SCHEDULER);
  }
  else if(clickedBenchmarkBar == "Solver"){
    expert_DetailedPlotTab->setCurrentIndex(SOLVER);
  }
  else if(clickedBenchmarkBar == "Sparse"){
    expert_DetailedPlotTab->setCurrentIndex(SPARSE);
  }
  else if(clickedBenchmarkBar == "Vector"){
    expert_DetailedPlotTab->setCurrentIndex(VECTOR);
  }
}

//detailed plot selection filter
void ExpertBenchmark::selectionChanged()
{
  int currentPlotIndex = expert_DetailedPlotTab->currentIndex();
  QCustomPlot *currentPlot = expert_DetailedPlotsVector[currentPlotIndex];
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i< currentPlot->graphCount(); ++i)
  {
    QCPGraph *graph = currentPlot->graph(i);
    QCPPlottableLegendItem *item = currentPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}
void ExpertBenchmark::updateBenchProgress(){
  currentBenchProgress++;
  ui->expert_ProgressBar->setValue(currentBenchProgress);
  ui->expert_ProgressBar->setFormat("Running Test %v of %m");
}

void ExpertBenchmark::updateBenchmarkListWidget(QListWidgetItem *item)
{
  //item(0) is the 'All' benchmarks selection option
  if(ui->expert_BenchmarkListWidget->row(item) == 0){
    if(item->isSelected()){
      ui->expert_BenchmarkListWidget->selectAllItems();
    }
    else{
      ui->expert_BenchmarkListWidget->deselectAllItems();
    }
  }
  else{
    if(item->isSelected()){
      item->setIcon(QIcon(":/icons/icons/checkTrue.png"));
      ui->expert_BenchmarkListWidget->checkSelectedItems();
    }
    else{
      ui->expert_BenchmarkListWidget->item(0)->setSelected(false);
      ui->expert_BenchmarkListWidget->item(0)->setIcon(QIcon(":/icons/icons/checkFalse.png"));
      item->setIcon(QIcon(":/icons/icons/checkFalse.png"));
    }
  }
}

void ExpertBenchmark::resetAllPlots(){
  resetPlotData(ui->expert_FinalResultPlot);
  //reset all plots
  foreach(QCustomPlot* plot, expert_DetailedPlotsVector){
    resetPlotData(plot);
    plot->yAxis->setTickVector(QVector<double>() );
    plot->yAxis->setTickVectorLabels(QVector<QString>() );
    plot->yAxis->setRange(0,5);
    plot->xAxis->setTickVector(QVector<double>() );
    plot->xAxis->setTickVectorLabels(QVector<QString>() );
    plot->xAxis->setRange(0,5);
    plot->replot();
  }
}

void ExpertBenchmark::setCustomSparseMatrixPath(){
  ui->expert_SparseCustomMatrix->setText( QFileDialog::getOpenFileName(this,
                                                                       QString("Select a Custom Sparse Matrix"),
                                                                       ArchiveExtractor::getMatrixMarketUserFolder(),
                                                                       "Matrix Market Files (*.mtx)" )
                                          );
}

BenchmarkSettings ExpertBenchmark::getExpertSettings()
{
  BenchmarkSettings settings;

  settings.blas3MatSizeA = ui->expert_Blas3AMatSize->text().toInt();
  settings.blas3MatSizeB = ui->expert_Blas3BMatSize->text().toInt();
  settings.blas3MatSizeC = ui->expert_Blas3CMatSize->text().toInt();

  settings.copyIncFactor = ui->expert_CopyIncFactor->text().toInt();
  settings.copyMaxVectorSize = ui->expert_CopyVecMax->text().toInt();
  settings.copyMinVectorSize = ui->expert_CopyVecMin->text().toInt();

  settings.vectorMinVectorSize = ui->expert_VectorVecMin->text().toInt();
  settings.vectorMaxVectorSize = ui->expert_VectorVecMax->text().toInt();
  settings.vectorIncFactor = ui->expert_VectorIncFactor->text().toInt();

  settings.sparseMatSizeA = ui->expert_SparseAMatSize->text().toInt();
  settings.sparseMatSizeB = ui->expert_SparseBMatSize->text().toInt();
  settings.sparseCustomMatrix = ui->expert_SparseCustomMatrix->text();

  return settings;
}

void ExpertBenchmark::hideStopButton()
{
  ui->expert_StopBenchmarkButton->hide();
  ui->expert_StartBenchmarkButton->show();
}
//reset the graph
void ExpertBenchmark::resetPlotData(QCustomPlot *benchmarkGraph)
{
  benchmarkGraph->clearGraphs();
  benchmarkGraph->clearPlottables();
  benchmarkGraph->clearItems();
  benchmarkGraph->xAxis->setRange(0,1);
  benchmarkGraph->replot();
}
//set the benchmark's unit measure
void ExpertBenchmark::updateBenchmarkUnitMeasure(QString unitMeasureName, int axis)
{
  switch(axis){
  case Qt::XAxis:
    qDebug()<<""<<unitMeasureName;
    expert_DetailedPlotsVector[activeBenchmark]->xAxis->setLabel(unitMeasureName);
    break;
  case Qt::YAxis:
    qDebug()<<""<<unitMeasureName;
    expert_DetailedPlotsVector[activeBenchmark]->yAxis->setLabel(unitMeasureName);
    break;
  default:
    break;
  }

}//parse the received benchmark result name and value

void ExpertBenchmark::parseBenchmarkResult(QString benchmarkName, double key, double resultValue, int graphType, int testId){
  if(graphType == BAR_GRAPH){
    plotBarResult(benchmarkName, key, resultValue, expert_DetailedPlotsVector[activeBenchmark]);
  }
  else if(graphType == LINE_GRAPH){
    plotLineResult(benchmarkName, key, resultValue, expert_DetailedPlotsVector[activeBenchmark], testId);
  }
}

void ExpertBenchmark::plotLineResult(QString benchmarkName, double key, double value, QCustomPlot *customPlot, int testId){
  if(customPlot->legend->visible() == false){
    customPlot->plotLayout()->addElement(0,1, customPlot->legend);
    customPlot->legend->setVisible(true);
    customPlot->legend->setSelectableParts( QCPLegend::spItems );
    customPlot->legend->setMaximumSize( 150, QWIDGETSIZE_MAX );
  }
  customPlot->legend->setFont(QFont("Helvetica", 9));
  customPlot->legend->setRowSpacing(-3);

  customPlot->xAxis->setAutoTicks(false);
  customPlot->xAxis->setAutoTickLabels(false);
  customPlot->xAxis->setAutoTickStep(false);
  customPlot->xAxis->setAutoSubTicks(false);

  QVector<double> tickPositions = customPlot->xAxis->tickVector();
  QVector<QString> tickLabels =  customPlot->xAxis->tickVectorLabels();

  double newTickPosition = key;
  QString newTickLabel = QString::number(key, 'f', 0);

  tickPositions.append( newTickPosition );
  tickLabels.append( newTickLabel );

  customPlot->xAxis->setTickVector(tickPositions);
  customPlot->xAxis->setTickVectorLabels(tickLabels);

  //  customPlot->xAxis->grid()->setSubGridVisible(true);
  customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
  customPlot->xAxis->setScaleLogBase(10);
  customPlot->xAxis->setNumberFormat("f"); // e = exponential, b = beautiful decimal powers
  customPlot->xAxis->setNumberPrecision(0);
  QFont axisTickFont;
  axisTickFont.setBold(false);
  customPlot->xAxis->setTickLabelFont(QFont(axisTickFont));

  customPlot->xAxis->setLabelFont(axisTickFont);
  customPlot->xAxis->setTickLabelFont(QFont(axisTickFont));

  //  customPlot->xAxis->setAutoTicks(true);
  customPlot->yAxis->setAutoTicks(true);

  //  customPlot->xAxis->setAutoTickStep(true);
  customPlot->yAxis->setAutoTickStep(true);

  //  customPlot->xAxis->setAutoTickLabels(true);
  customPlot->yAxis->setAutoTickLabels(true);

  QCPGraph *currentResultGraph;

  if(customPlot->graph(testId) == 0){
    //no graph for this test result
    //create one
    qDebug()<<"adding new graph";
    currentResultGraph = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
  }
  else{
    //a graph for this test result already exists
    //add the new data to it
    currentResultGraph = customPlot->graph(testId);
  }

  QPen pen;//(QColor("red"));
  switch(testId){
  case 0: pen.setColor("green");
    break;
  case 1: pen.setColor("blue");
    break;
  case 2: pen.setColor("black");
    break;
  case 3: pen.setColor("red");
    break;
  case 4: pen.setColor("cyan");
    break;
  case 5: pen.setColor("magenta");
    break;
  case 6: pen.setColor("gray");
    break;
  case 7: pen.setColor("yellow");
    break;
  case 8: pen.setColor("pink");
    break;
  default: pen.setColor("orange");
  }
  currentResultGraph->setName(benchmarkName);
  currentResultGraph->addData( key, value );
  currentResultGraph->rescaleAxes(true);
  currentResultGraph->setPen(pen);
  currentResultGraph->setLineStyle(QCPGraph::lsLine);
  currentResultGraph->setScatterStyle(QCPScatterStyle::ssCrossSquare);

  customPlot->rescaleAxes();

  //  customPlot->axisRect()->setupFullAxesBox();
  customPlot->replot();
}

//main result diplay function
//x and y axis are swapped to achieve horizontal bar display
void ExpertBenchmark::plotBarResult(QString benchmarkName, double key, double value, QCustomPlot *customPlot){
  customPlot->axisRect()->setAutoMargins(QCP::msLeft);
  QMargins margins = customPlot->axisRect()->margins();
  margins.setRight(60);//reserve space for the largest result
  margins.setBottom(35);//reserve space for measure label

  customPlot->axisRect()->setMargins( margins );
  QVector<double> currentTickVector = customPlot->yAxis->tickVector();
  QVector<QString> currentTickVectorLabels =  customPlot->yAxis->tickVectorLabels();

  double currentValue = value;
  double currentKey = currentTickVector.size();

  QCPBars *resultBar = new QCPBars(customPlot->yAxis, customPlot->xAxis);
  resultBar->setPen(QPen(Qt::NoPen));
  resultBar->setName(benchmarkName);
  resultBar->addData(currentKey, currentValue);

  currentTickVector.append(currentKey);
  currentTickVectorLabels.append(benchmarkName);

  customPlot->yAxis->setTickVector(currentTickVector);
  customPlot->yAxis->setTickVectorLabels(currentTickVectorLabels);

  customPlot->addPlottable(resultBar);

  customPlot->rescaleAxes();

  QCPItemText *text = new QCPItemText(customPlot);

  text->setClipToAxisRect(false);//allow this item to flow over the main plot rectangle, hence showing it when it can't fit the plot rectangle

  text->position->setType(QCPItemPosition::ptPlotCoords);
  text->position->setCoords(  currentValue , currentKey );
  text->setFont(QFont(font().family(), 10, QFont::Bold)); // make font a bit larger

  //Add a whitespace in front of the result value to separate it from the result bar
  //Prolly could've also used margins, but meh
  //And format the result number to two decimals
  text->setText( QString(" ") + QString::number( currentValue, 'f', 2  ));

  text->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  customPlot->addItem(text);

  customPlot->yAxis->setRangeLower(-0.5);
  customPlot->replot();
}

void ExpertBenchmark::plotFinalResult(QString benchmarkName, double value, QCustomPlot *customPlot){
  //  Plot mapping
  //  Vector - 1
  //  Sparse - 2
  //  Copy - 3
  //  Blas3 - 4
  QCPBarData currentData;
  currentData.value = value;
  if(benchmarkName == "Blas3"){
    currentData.key = 4;
  }
  else if(benchmarkName == "Copy"){
    currentData.key = 3;
  }
  else if(benchmarkName == "Qr"){
    currentData.key = 4;
  }
  else if(benchmarkName == "Scheduler"){
    currentData.key = 0;
    currentData.value = 0;
  }
  else if(benchmarkName == "Solver"){
    currentData.key = 3;
  }
  else if(benchmarkName == "Sparse"){
    currentData.key = 2;
  }
  else if(benchmarkName == "Vector"){
    currentData.key = 1;
  }
  else{
    qDebug()<<"Error parsing benchmark name";
  }

  QCPBars *resultBar = new QCPBars(customPlot->yAxis, customPlot->xAxis);
  resultBar->setName(benchmarkName);
  resultBar->addData(currentData);
  resultBar->setPen(QPen(Qt::NoPen));

  customPlot->addPlottable(resultBar);

  QCPItemText *text = new QCPItemText(customPlot);

  //  text->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
  text->setClipToAxisRect(false);//allow this item to flow over the main plot rectangle, hence showing it when it can't fit the plot rectangle
  text->position->setType(QCPItemPosition::ptPlotCoords);
  text->position->setCoords(  currentData.value , currentData.key );
  text->setFont(QFont(font().family(), 10, QFont::Bold)); // make font a bit larger

  //Add a whitespace in front of the result value to separate it from the result bar
  //Prolly could've also used margins, but meh
  //And format the result number to two decimals
  text->setText(QString(" ") + QString::number( currentData.value, 'f', 2  ));

  text->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  customPlot->addItem(text);

  customPlot->xAxis->rescale();
  customPlot->replot();
}