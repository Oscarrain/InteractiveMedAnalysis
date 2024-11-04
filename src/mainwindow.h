#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QtCharts/QtCharts>
#include <QtDataVisualization/Q3DScatter>

#include <vector>
#include <iostream>
#include "Eigen/Dense"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initialize();
    void shortcuts();
    void snpGlobal();
    void clearTabLayout(QWidget* tabWidget);
    std::vector<float> getDataPoints(int selectedColumn);
    float mean(const QVector<float>& data);
    float standardDeviation(const QVector<float>& data, float meanValue);
    QLineSeries* generateCurve(const QVector<float>& data);
    QPixmap* generateHeatmap(const Eigen::MatrixXf &mat, const std::vector<QString>& rowNames,
                             const std::vector<QString>& colNames);
    QChart* generate2DScatterPlot(const Eigen::MatrixXf &data, const std::vector<int> &tag, const int &type);
    Q3DScatter* generate3DScatterPlot(const Eigen::MatrixXf &data, const std::vector<int> &tag, const int &type);
    QColor getColorForTag(float tag, const int &type);
    void drawKmeansColor();
    void clearKmeansColor();
    void clearPreviousKmeans();

private slots:
    void openDataFile();
    void closeDataFile();
    bool isNumeric(const QString &str);

    void calculateAvgVar();
    void drawHistogram();
    void showNormalDistribution();

    void drawScatterPlot();
    void displayToolTip(const QPointF &point, bool hovered);
    void fitCurve();

    void drawHeatmap();
    void switchHeatmap();

    void drawPcaScatterPlot();
    void switchPca();

    void doKmeans();
    void judgeKmeansColor();
    void switchKmeans();

private:
    Ui::MainWindow *ui;

    bool isBarchartWidgetsAdded = false;
    QChartView *chartView = nullptr;
    QLineSeries *normalDistributionSeries = nullptr;
    QPushButton *showNormalDistributionButton = nullptr;

    bool isScatterPlotWidgetsAdded = false;
    QScatterSeries *scatterSeries = nullptr;
    QChart *scatterChart = nullptr;
    QSpinBox* spinBoxDegree = nullptr;
    QPushButton *fitButton = nullptr;
    QLabel* overlappingCountLabel = nullptr;
    QLabel *currentFitDegree = nullptr;
    QLabel *pValueLabel = nullptr;
    QLabel *r2ValueLabel = nullptr;

    bool isHeatmapWidgetsAdded = false;
    QStackedWidget *stackedHeatmap = nullptr;
    QLabel *whichHeatmapLabel = nullptr;
    QPushButton *switchHeatmapButton = nullptr;

    bool isPcaWidgetsAdded = false;
    QStackedWidget *stackedPca = nullptr;
    QLabel *whichPcaLabel = nullptr;
    QPushButton *switchPcaButton = nullptr;

    bool isKmeansWidgetsAdded = false;
    QStackedWidget *stackedKmeans = nullptr;
    QLabel *whichKmeansLabel = nullptr;
    QPushButton *switchKmeansButton = nullptr;


};
#endif // MAINWINDOW_H
