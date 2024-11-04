#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QFile>
#include <QtCharts/QtCharts>
#include <QToolTip>
#include <QMap>
#include <QPointF>
#include <QVector>
#include <QSpinBox>
#include <QtDataVisualization/Q3DScatter>


#include <vector>
#include <iostream>
#include "Eigen/Dense"
#include "rowfeature.h"
#include "leastsquare.h"
#include "covariance.h"
#include "pca.h"
#include "kmeans.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialization

    setWindowTitle("Medical Data Analytics");

    initialize();

    shortcuts();

    snpGlobal();    // Signal n Plot: Global

}

MainWindow::~MainWindow()
{
    delete ui;
}

//functions

void MainWindow::initialize(){
    if(chartView) {
        delete chartView;
        chartView = nullptr;
    }

    if(normalDistributionSeries) {
        delete normalDistributionSeries;
        normalDistributionSeries = nullptr;
    }

    if(showNormalDistributionButton){
        delete showNormalDistributionButton;
        showNormalDistributionButton = nullptr;
    }

    if(scatterSeries) {
        delete scatterSeries;
        scatterSeries = nullptr;
    }

    if(scatterChart) {
        delete scatterChart;
        scatterChart = nullptr;
    }

    if(spinBoxDegree) {
        delete spinBoxDegree;
        spinBoxDegree = nullptr;
    }

    if(fitButton) {
        delete fitButton;
        fitButton = nullptr;
    }

    if(overlappingCountLabel){
        delete overlappingCountLabel;
        overlappingCountLabel = nullptr;
    }

    if(currentFitDegree) {
        delete currentFitDegree;
        currentFitDegree = nullptr;
    }

    if(pValueLabel) {
        delete pValueLabel;
        pValueLabel = nullptr;
    }

    if(r2ValueLabel) {
        delete r2ValueLabel;
        r2ValueLabel = nullptr;
    }

    if(stackedHeatmap) {
        delete stackedHeatmap;
        stackedHeatmap = nullptr;
    }

     if(whichHeatmapLabel) {
        delete whichHeatmapLabel;
        whichHeatmapLabel = nullptr;
    }

    if(switchHeatmapButton) {
        delete switchHeatmapButton;
        switchHeatmapButton = nullptr;
    }

    if(stackedPca) {
        delete stackedPca;
        stackedPca = nullptr;
       }

    if(whichPcaLabel) {
        delete whichPcaLabel;
        whichPcaLabel = nullptr;
    }

    if(switchPcaButton) {
        delete switchPcaButton;
        switchPcaButton = nullptr;
    }

    if(stackedKmeans) {
        delete stackedKmeans;
        stackedKmeans = nullptr;
    }

    if(whichKmeansLabel) {
        delete whichKmeansLabel;
        whichKmeansLabel = nullptr;
    }

    if(switchKmeansButton) {
        delete switchKmeansButton;
        switchKmeansButton = nullptr;
    }

    clearTabLayout(ui->histogramTab);
    clearTabLayout(ui->scatterPlotTab);
    clearTabLayout(ui->heatmapTab);
    clearTabLayout(ui->pcaTab);
    clearTabLayout(ui->kmeansTab);

    isBarchartWidgetsAdded = false;
    isScatterPlotWidgetsAdded = false;
    isHeatmapWidgetsAdded = false;
    isPcaWidgetsAdded = false;
    isKmeansWidgetsAdded = false;

    // Data

    ui->tabWidget->setCurrentIndex(0);

    // Histogram

    // Create the chartView to display the histogram and the button
    chartView = new QChartView(ui->histogramTab);
    chartView->setVisible(false);
    showNormalDistributionButton = new QPushButton("Show Normal Distribution", ui->histogramTab);
    showNormalDistributionButton->setVisible(false);
    normalDistributionSeries = new QLineSeries;

    // Scatter Plot

    scatterChart = nullptr;

    // 创建QSpinBox来输入拟合多项式的次数
    spinBoxDegree = new QSpinBox(ui->scatterPlotTab);
    spinBoxDegree->setObjectName("spinBoxDegree");
    spinBoxDegree->setRange(1, 10);
    spinBoxDegree->setValue(1);
    spinBoxDegree->setToolTip(tr("拟合多项式次数"));
    spinBoxDegree->setVisible(false);

    // 创建按钮用于执行拟合操作
    fitButton = new QPushButton(tr("拟合曲线"), ui->scatterPlotTab);
    fitButton->setObjectName("fitButton");
    fitButton->setVisible(false);

    // 初始化函数参数显示
    overlappingCountLabel = new QLabel("重叠点的数量: ");
    currentFitDegree = new QLabel(QString("fitting curve degree:"));
    pValueLabel = new QLabel(QString("p值: "));
    r2ValueLabel = new QLabel(QString("r^2值: "));

    // Heatmap

    stackedHeatmap = new QStackedWidget(ui->heatmapTab);
    whichHeatmapLabel =new QLabel(QString("Heatmap: null"));
    switchHeatmapButton = new QPushButton(tr("切换热图显示"),ui->heatmapTab);
    switchHeatmapButton->setVisible(false);
    switchHeatmapButton->setObjectName("switchHeatmapButton");

    // PCA

    stackedPca = new QStackedWidget(ui->pcaTab);
    whichPcaLabel =new QLabel(QString("PCA dimension: null"));
    switchPcaButton = new QPushButton(tr("切换PCA显示维度"),ui->pcaTab);
    switchPcaButton->setVisible(false);
    switchPcaButton->setObjectName("switchPcaButton");

    // Kmeans

    ui->kmeansSpinBox->setRange(1, 10);
    ui->kmeansSpinBox->setValue(2);
    stackedKmeans = new QStackedWidget(ui->kmeansTab);
    whichKmeansLabel =new QLabel(QString("Kmeans PCA dimension: null"));
    switchKmeansButton = new QPushButton(tr("切换Kmeans下PCA显示维度"),ui->kmeansTab);
    switchKmeansButton->setVisible(false);
    switchKmeansButton->setObjectName("switchKmeansButton");

    //Signal n Plots: Data-concerned
    connect(showNormalDistributionButton, &QPushButton::clicked, this, &MainWindow::showNormalDistribution);
    connect(fitButton, &QPushButton::clicked, this, &MainWindow::fitCurve);
    connect(switchHeatmapButton, &QPushButton::clicked, this, &MainWindow::switchHeatmap);
    connect(switchPcaButton, &QPushButton::clicked, this, &MainWindow::switchPca);
    connect(switchKmeansButton, &QPushButton::clicked, this, &MainWindow::switchKmeans);

}

void MainWindow::shortcuts(){
    ui->actionClose->setShortcut(QKeySequence::Close);  // Cmd+W to close file
    ui->actionOpen->setShortcut(QKeySequence::Open);    // Cmd+O to open file
    ui->avgvarButton->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_A));
    ui->barChartButton->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_B));
    ui->scatterPlotButton->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_S));
    ui->heatmapButton->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_H));
    ui->pcaButton->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_P));
    ui->kmeansButton->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_K));
}

void MainWindow::snpGlobal(){
    // Signal n Plot: Global

    connect(ui->actionOpen,&QAction::triggered, this, &MainWindow::openDataFile);
    connect(ui->actionClose,&QAction::triggered, this, &MainWindow::closeDataFile);

    connect(ui->avgvarButton, &QPushButton::clicked, this, &MainWindow::calculateAvgVar);
    connect(ui->barChartButton, &QPushButton::clicked, this, &MainWindow::drawHistogram);


    connect(ui->scatterPlotButton, &QPushButton::clicked, this, &MainWindow::drawScatterPlot);

    connect(ui->heatmapButton, &QPushButton::clicked, this, &MainWindow::drawHeatmap);

    connect(ui->pcaButton, &QPushButton::clicked, this, &MainWindow::drawPcaScatterPlot);

    connect(ui->kmeansButton, &QPushButton::clicked, this, &MainWindow::doKmeans);
    connect(ui->kmeansColorButton, &QRadioButton::toggled, this, &MainWindow::judgeKmeansColor);
}

void MainWindow::clearTabLayout(QWidget* tabWidget) {
    if (QLayout* layout = tabWidget->layout()) {
        // Delete all items in layout
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (QWidget* widget = item->widget()) {
                delete widget;
            }
            delete item;
        }

        delete layout;
        tabWidget->setLayout(nullptr);  // Ensure the widget doesn't have a dangling pointer to the layout
    }
}

bool operator<(const QPointF &a, const QPointF &b) {
    if (a.x() < b.x()) {
        return true;
    } else if (a.x() == b.x() && a.y() < b.y()) {
        return true;
    }
    return false;
}

float mapValueToHue(float value, float minValue, float maxValue) {
    return 240.0f - (240.0f * (value - minValue) / (maxValue - minValue));
}

std::vector<float> MainWindow::getDataPoints(int selectedColumn){
    std::vector<float> dataPoints;

    // 收集选定列的数据
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem *item = ui->tableWidget->item(row, selectedColumn);
        if (item) {
            bool ok;
            float value = item->text().toFloat(&ok);
            if (ok) {
                dataPoints.push_back(value);
            }
        }
    }
    return dataPoints;
}

float MainWindow::mean(const QVector<float>& data) {
    float sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

float MainWindow::standardDeviation(const QVector<float>& data, float meanValue) {
    float sum = 0.0;
    for (float value : data) {
        sum += (value - meanValue) * (value - meanValue);
    }
    return std::sqrt(sum / data.size());
}

QLineSeries* MainWindow::generateCurve(const QVector<float>& data) {
    float dataMean = mean(data);
    float dataStdDev = standardDeviation(data, dataMean);
    QChart *currentChart = chartView->chart();
    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis*>(currentChart->axisX());
    if (!axisX) {
        // Handle the error, maybe return null or throw an exception
        return nullptr;
    }
    int binCount = axisX->categories().size();
    float minValue = *std::min_element(data.begin(), data.end());
    float maxValue = *std::max_element(data.begin(), data.end());

    QLineSeries *normalSeries = new QLineSeries();
    for (float x = minValue; x <= maxValue; x += (maxValue - minValue) / 100.0) {
        float y = (1.0 / (dataStdDev * std::sqrt(2.0 * M_PI))) * std::exp(-0.5 * std::pow((x - dataMean) / dataStdDev, 2));
        normalSeries->append(x, y * data.size() * (maxValue - minValue) / binCount);
    }
    normalSeries->setName("Normal Distribution"); // 设置曲线的名称
    return normalSeries;
}

QPixmap* MainWindow::generateHeatmap(const Eigen::MatrixXf &mat, const std::vector<QString>& rowNames,
                                     const std::vector<QString>& colNames) {
    int rows = mat.rows();
    int cols = mat.cols();

    int cellWidth = 80;  // 假设每个单元格的宽度为40像素
    int cellHeight = 80; // 假设每个单元格的高度为40像素
    const int nameWidth = 80; // 预留给名称的宽度
    const int colorbarWidth = 80;

    int heatmapWidth = cols * cellWidth + nameWidth + colorbarWidth;  // 为colorbar预留80像素
    int heatmapHeight = rows * cellHeight+ nameWidth;

    float minValue = mat.minCoeff();
    float maxValue = mat.maxCoeff();

    QPixmap* heatmapPixmap = new QPixmap(heatmapWidth, heatmapHeight);
    heatmapPixmap->fill(Qt::white);

    QPainter* painter = new QPainter(heatmapPixmap);
    painter->setPen(Qt::black);

    // 为数据定义一个颜色映射并显示数据
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float value = mat(i, j);
            float hue = mapValueToHue(value, minValue, maxValue);
            QColor color = QColor::fromHsv(static_cast<int>(hue), 255, 255);
            painter->fillRect(nameWidth + j * cellWidth, i * cellHeight, cellWidth, cellHeight, color);

            // 显示对应数值
            painter->drawText(QRect(nameWidth + j * cellWidth, i * cellHeight, cellWidth, cellHeight),
                              Qt::AlignCenter, QString::number(value));
        }

    }

    // 为名称调小字体
    QFont font = painter->font();
    font.setPointSize(font.pointSize() - 6);  // 调小5号
    painter->setFont(font);

    // 显示每行名称
    for (int i = 0; i < rows; i++) {
        if(i < rowNames.size()) {
            painter->drawText(0, i * cellHeight, nameWidth, cellHeight, Qt::AlignCenter, rowNames[i]);
        }
    }

    // 显示每列名称
    for (int j = 0; j < cols; j++) {
        if(j < colNames.size()) {
            painter->drawText(nameWidth + j * cellWidth, rows * cellHeight, cellWidth, nameWidth,
                              Qt::AlignCenter, colNames[j]);
        }
    }

    // 绘制colorbar
    for (int k = 0; k < 100; k++) {
        float normValue = minValue + (maxValue - minValue) * (k / 100.0f);
        float hue = mapValueToHue(normValue, minValue, maxValue);
        QColor color = QColor::fromHsv(static_cast<int>(hue), 255, 255);
        int y = ((heatmapHeight - nameWidth) * k / 100);

        // 绘制colorbar部分
        painter->fillRect(heatmapWidth - colorbarWidth, y, colorbarWidth / 2, (heatmapHeight - nameWidth) / 100, color);

        // 每10个单位显示一个刻度
        if (k % 10 == 0) {
            float labelValue = minValue + (maxValue - minValue) * (k / 100.0f);

            // 调整y值以使刻度标签与colorbar垂直对齐
            int adjustedY = y - ((heatmapHeight - nameWidth) / 7);  // 将刻度标签上移半个单元高度

            // 绘制刻度标签部分
            painter->drawText(heatmapWidth - colorbarWidth/2, adjustedY, colorbarWidth/2, cellHeight,
                              Qt::AlignLeft | Qt::AlignVCenter, QString::number(labelValue, 'f', 2));  // 使用两位小数
        }
    }


    delete painter;  // 确保释放QPainter对象
    return heatmapPixmap;  // 返回QPixmap指针
}

QChart* MainWindow::generate2DScatterPlot(const Eigen::MatrixXf &data, const std::vector<int> &tag, const int &type) {
    QChart *chart = new QChart();

    // 创建一组散点系列，每个系列对应一个不同的tag值
    QMap<float, QScatterSeries*> tagSeriesMap;

    for (int i = 0; i < data.rows(); ++i) {
        int currentTag = tag[i];
        if (!tagSeriesMap.contains(currentTag)) {
            // 如果还没有为该tag创建系列，就创建一个新的系列
            QScatterSeries *series = new QScatterSeries();
            series->setName(QString("Tag %1").arg(currentTag));

            // 设置系列的颜色
            series->setColor(getColorForTag(currentTag,type));

            tagSeriesMap[currentTag] = series;
            //chart->addSeries(series);
        }

        // 将数据点添加到对应的系列
        tagSeriesMap[currentTag]->append(data(i, 0), data(i, 1));
    }

    // 将所有系列添加到图表中
    for (QScatterSeries *series : tagSeriesMap.values()) {
        series->setMarkerSize(8);
        chart->addSeries(series);
    }

    chart->createDefaultAxes();
    chart->setTitle("2D PCA Scatter Plot");

    return chart;
}

Q3DScatter* MainWindow::generate3DScatterPlot(const Eigen::MatrixXf &data, const std::vector<int> &tag, const int &type) {
    Q3DScatter *scatter = new Q3DScatter();
    scatter->setTitle("3D PCA Scatter Plot");
    scatter->scene()->activeCamera()->setCameraPosition(80, 20);

    // 创建一个映射，将不同颜色映射到数据系列
    std::map<QString, QScatter3DSeries*> colorSeriesMap;

    for (int i = 0; i < data.rows(); ++i) {
        int currentTag = tag[i];

        // 获取当前tag值对应的颜色
        QColor pointColor = getColorForTag(currentTag,type);
        QString colorKey = pointColor.name();  // 将颜色转化为字符串作为键

        // 创建或获取对应颜色的数据系列
        QScatter3DSeries *series3D = nullptr;
        if (colorSeriesMap.find(colorKey) != colorSeriesMap.end()) {
            series3D = colorSeriesMap[colorKey];
        } else {
            series3D = new QScatter3DSeries();
            series3D->setBaseColor(pointColor);
            colorSeriesMap[colorKey] = series3D;
            series3D->setItemSize(0.1);
        }

        // 添加数据点
        series3D->dataProxy()->addItem(QVector3D(data(i, 0), data(i, 1), data(i, 2)));
    }

    // 将所有数据系列添加到scatter中
    for (const auto &pair : colorSeriesMap) {
        scatter->addSeries(pair.second);
    }

    return scatter;
}

QColor MainWindow::getColorForTag(float tag, const int &type) {
    // 创建一个映射，将不同的tag值映射到不同的颜色
    QMap<float, QColor> tagColorMap;

    if(type == 0){
    tagColorMap[0] = Qt::cyan;
    tagColorMap[1] = Qt::magenta;
    tagColorMap[2] = Qt::yellow;
    tagColorMap[3] = Qt::gray;
    tagColorMap[4] = Qt::darkRed;
    tagColorMap[5] = Qt::darkGreen;
    tagColorMap[6] = Qt::darkBlue;
    tagColorMap[7] = Qt::darkCyan;
    tagColorMap[8] = Qt::darkMagenta;
    tagColorMap[9] = Qt::darkYellow;
    }
    else if(type == 1)
    {
    tagColorMap[0] = Qt::red;
    tagColorMap[1] = Qt::green;
    tagColorMap[2] = Qt::blue;
    }

    // 如果找到对应的颜色，返回颜色，否则返回默认颜色
    if (tagColorMap.contains(tag)) {
        return tagColorMap[tag];
    } else {
        return Qt::black; // 返回黑色作为默认颜色，或者您可以选择其他颜色
    }
}

void MainWindow::drawKmeansColor() {
    int totalColumns = ui->tableWidget->columnCount();
    int totalRows = ui->tableWidget->rowCount();

    int diagnosisColumn = 0;
    // 从0开始遍历所有列，直到找到名为"diagnosis"的列为止，赋给diagnosisColumn
    for (int col = 0; col < totalColumns; ++col) {
        QTableWidgetItem *headerItem = ui->tableWidget->horizontalHeaderItem(col);
        if (headerItem && headerItem->text() == "diagnosis") {
            diagnosisColumn = col;
            break;
        }
    }

    // 给diagnosis列单独上色
    for (int row = 0; row < totalRows; ++row) {
        QTableWidgetItem *item = ui->tableWidget->item(row, diagnosisColumn);
        if (item) {
            int value = item->text().toInt();
            switch (value) {
            case 0:
                item->setBackground(Qt::red);
                break;
            case 1:
                item->setBackground(Qt::green);
                break;
            case 2:
                item->setBackground(Qt::blue);
                break;
            default:
                // 默认情况，可以选择不做任何事或设置为一个默认颜色
                break;
            }
        }
    }

    // 检查最后一列是否为"KMeans"
    QTableWidgetItem *headerItem = ui->tableWidget->horizontalHeaderItem(totalColumns - 1);
    if (headerItem && headerItem->text() == "KMeans") {
        // 遍历每一行
        for (int row = 0; row < totalRows; ++row) {
            QTableWidgetItem *item = ui->tableWidget->item(row, totalColumns - 1);
            if (item) {
                bool ok;
                float clusterTag = item->text().toFloat(&ok);
                if (ok) {
                    QColor rowColor = getColorForTag(clusterTag, 0);// 对应KMeans用0，不用具有明显良莠暗示的红绿
                    for (int col = diagnosisColumn + 1; col < totalColumns; ++col) {// 从非diagnosis列开始绘制
                        QTableWidgetItem *cellItem = ui->tableWidget->item(row, col);
                        if (!cellItem) {
                            cellItem = new QTableWidgetItem();
                            ui->tableWidget->setItem(row, col, cellItem);
                        }
                        cellItem->setBackground(rowColor);
                    }
                }
            }
        }
    }
}

void MainWindow::clearKmeansColor() {
    int totalColumns = ui->tableWidget->columnCount();
    int totalRows = ui->tableWidget->rowCount();

    // 遍历每一行
    for (int row = 0; row < totalRows; ++row) {
        // 在当前行中遍历每一列
        for (int col = 0; col < totalColumns; ++col) {
            QTableWidgetItem *cellItem = ui->tableWidget->item(row, col);
            if (cellItem) {
                cellItem->setBackground(Qt::transparent);  // 还可能是Qt::white
            }
        }
    }
}

void MainWindow::clearPreviousKmeans(){
    int lastColumnIndex = ui->tableWidget->columnCount() - 1; // 获取最后一列的索引

    if (lastColumnIndex >= 0) { // 确保表中至少有一列
        QTableWidgetItem *headerItem = ui->tableWidget->horizontalHeaderItem(lastColumnIndex); // 获取最后一列的表头项

        if (headerItem && headerItem->text() == "KMeans") { // 如果表头项存在并且其文本为"Kmeans"
            ui->tableWidget->removeColumn(lastColumnIndex); // 删除最后一列
        }
    }
}



//slots

void MainWindow::openDataFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开数据文件", "", "CSV 文件 (*.csv);;所有文件 (*.*)");
    if (!fileName.isEmpty()) {
        // Read the CSV file and populate the table
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);

            // Clear the existing table
            ui->tableWidget->clear();
            ui->tableWidget->setRowCount(0);
            ui->tableWidget->setColumnCount(0);

            int row = 0;
            QMap<QString, int> categoryMapping; // Map non-numeric categories to integers

            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(",");

                if (row == 0) {
                    // Set column count and header labels
                    ui->tableWidget->setColumnCount(fields.size());
                    ui->tableWidget->setHorizontalHeaderLabels(fields);
                } else {
                    // Add a new row for data
                    ui->tableWidget->insertRow(row - 1);

                    for (int col = 0; col < fields.size(); ++col) {
                        QTableWidgetItem *item = new QTableWidgetItem(fields[col]);

                        // Check if the current column contains non-numeric data
                        if (!isNumeric(fields[col])) {
                            // Check if the category is already mapped
                            if (!categoryMapping.contains(fields[col])) {
                                // Assign a new integer value to the category
                                int categoryValue = categoryMapping.size();
                                categoryMapping[fields[col]] = categoryValue;
                            }

                            // Map the non-numeric category to its integer value
                            item->setData(Qt::EditRole, categoryMapping[fields[col]]);
                        }

                        ui->tableWidget->setItem(row - 1, col, item);
                    }
                }
                ++row;
            }
            file.close();
        }
    }
    for (int i = 0; i < ui->tableWidget->columnCount(); i++) {
        int contentWidth = ui->tableWidget->columnWidth(i);
        ui->tableWidget->resizeColumnToContents(i);
        if (ui->tableWidget->columnWidth(i) < contentWidth) {
            ui->tableWidget->setColumnWidth(i, contentWidth);
        }
    }
}

void MainWindow::closeDataFile(){
    // 弹出一个确认对话框询问用户是否真的想清除数据
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认清除", "是否确认清除数据？",
                                  QMessageBox::Yes|QMessageBox::No);

    // 如果用户选择了"Yes"
    if (reply == QMessageBox::Yes) {
        ui->tableWidget->setRowCount(0);    // 清除所有行
        ui->tableWidget->setColumnCount(0);
        ui->tableWidget->clear();
        initialize();
        //showNormalDistributionButton->setVisible(false);    // for debug use:这个按钮不应该不消失啊？

        // 弹出一个信息对话框告诉用户数据已成功清除
        QMessageBox::information(this, "清除成功", "数据已成功清除!");
    }
    // 如果用户选择了"No"或关闭对话框，不做任何事情
}

bool MainWindow::isNumeric(const QString &str)
{
    static QRegularExpression re("^-?\\d*\\.?\\d*$"); //check if a given string is numeric
    return re.match(str).hasMatch();
}

void MainWindow::calculateAvgVar()
    {
        // Get the current selected column
        int currentColumn = ui->tableWidget->currentColumn();

        if (currentColumn >= 0) {
            // Extract the data from the selected column
            std::vector<float> columnData;
            for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                QTableWidgetItem *item = ui->tableWidget->item(row, currentColumn);
                if (item) {
                    bool ok;
                    float value = item->text().toFloat(&ok);
                    if (ok) {
                        columnData.push_back(value);
                    }
                }
            }

            if (!columnData.empty()) {
                // Calculate the average and variance
                std::tuple<float, float> avgVar = getAvgVar(columnData);

                // Display the result in a message box
                QString resultText = QString("均值: %1\n方差: %2").arg(std::get<0>(avgVar)).arg(std::get<1>(avgVar));
                QMessageBox::information(this, "均值和方差", resultText);
            } else {
                QMessageBox::warning(this, "警告", "选择的列没有有效数据.");
            }
        } else {
            QMessageBox::warning(this, "警告", "请选择一个列.");
        }
    }

void MainWindow::drawHistogram()
    {
        // 获取选择的范围
        QList<QTableWidgetSelectionRange> selectedRanges = ui->tableWidget->selectedRanges();

        // 计算选择的列的数量
        QSet<int> selectedColumns;
        for (const QTableWidgetSelectionRange &range : selectedRanges) {
            for (int col = range.leftColumn(); col <= range.rightColumn(); ++col) {
                selectedColumns.insert(col);
            }
        }

        if (selectedColumns.size() == 0) {
            QMessageBox::warning(this, "警告", "请选择要绘制直方图的列。");
            return;
        }

        if (selectedColumns.size() > 1) {
            QMessageBox::warning(this, "警告", "请只选择一个列来绘制直方图。");
            return;
        }

        int selectedColumn = *selectedColumns.begin();

        QVector<double> dataPoints;
        QStringList categories;

        // 收集选定列的数据
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QTableWidgetItem *item = ui->tableWidget->item(row, selectedColumn);
            if (item) {
                bool ok;
                double value = item->text().toDouble(&ok);
                if (ok) {
                    dataPoints.append(value);
                    categories.append(QString::number(value));
                }
            }
        }

        if (dataPoints.isEmpty()) {
            QMessageBox::warning(this, "警告", "选定列没有有效的数值数据。");
            return;
        }

        // 获取已有的图表或创建新图表
        QChart *checkChart = chartView->chart();
        if (!checkChart) {
            checkChart = new QChart;
            chartView->setChart(checkChart);
        } else {
            // 移除旧的系列
            QList<QAbstractSeries *> oldSeries = checkChart->series();
            for (QAbstractSeries *old : oldSeries) {
                checkChart->removeSeries(old);
                delete old;
            }
        }

        // Prompt the user to specify the number of bins or calculate it programmatically
        int numBins = QInputDialog::getInt(this, "输入区间数量", "请输入要分为多少个区间(1至20)：", 5, 1, 20);

        // Calculate bin boundaries
        double minVal = *std::min_element(dataPoints.constBegin(), dataPoints.constEnd());
        double maxVal = *std::max_element(dataPoints.constBegin(), dataPoints.constEnd());
        double binWidth = (maxVal - minVal) / numBins;

        QVector<int> binCounts(numBins, 0);

        // Count data points in each bin
        for (double value : dataPoints) {
            int binIndex = static_cast<int>((value - minVal) / binWidth);
            if (binIndex < 0) {
                binIndex = 0;
            } else if (binIndex >= numBins) {
                binIndex = numBins - 1;
            }
            binCounts[binIndex]++;
        }

        // 创建直方图数据集
        QBarSet *set = new QBarSet("Data");
        for (int value : binCounts) {
            *set << value;
        }

        // 创建直方图系列
        QBarSeries *series = new QBarSeries;
        series->append(set);

        // 创建图表视图
        QChart *chart = new QChart;
        chart->addSeries(series);
        chart->setTitle(ui->tableWidget->horizontalHeaderItem(selectedColumn)->text());
        chart->setAnimationOptions(QChart::SeriesAnimations);

        QStringList binLabels;
        double binStart = minVal;
        for (int i = 0; i < numBins; ++i) {
            double binEnd = binStart + binWidth;
            if(binEnd > 100){
                binLabels << QString("%1-%2").arg(QString::number(binStart, 'f', 0), QString::number(binEnd, 'f', 0));
            }else if((binStart < 0.1) && (binEnd <0.1)){
                binLabels << QString("%1-%2").arg(QString::number(binStart), QString::number(binEnd));
            }else{
                binLabels << QString("%1-%2").arg(QString::number(binStart, 'f', 2), QString::number(binEnd, 'f', 2));
            }
            binStart = binEnd;
        }

        // 创建X轴
        QBarCategoryAxis *axisX = new QBarCategoryAxis;
        axisX->append(binLabels);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        // 创建Y轴
        int maxCount = (*std::max_element(binCounts.constBegin(), binCounts.constEnd())) * 1.15;
        QValueAxis *axisY = new QValueAxis;
        axisY->setRange(0, maxCount);
        axisY->setLabelFormat("%d");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        // 设置图表视图
        chartView->setChart(chart);
        if(!isBarchartWidgetsAdded){
            showNormalDistributionButton->setVisible(true);
            QVBoxLayout *histogramTabLayout;
            if (!ui->histogramTab->layout()) {
                histogramTabLayout = new QVBoxLayout(ui->histogramTab);
                ui->histogramTab->setLayout(histogramTabLayout);
            } else {
                // 否则，将ui->histogram的layout赋值给histogramTabLayout
                histogramTabLayout = qobject_cast<QVBoxLayout*>(ui->histogramTab->layout());
            }
            histogramTabLayout->addWidget(chartView);
            histogramTabLayout->addWidget(showNormalDistributionButton);
            chartView->setVisible(true);
            chartView->setRenderHint(QPainter::Antialiasing);
            chart->legend()->setVisible(true);
            isBarchartWidgetsAdded = true;
        }

        // 刷新UI
        ui->tabWidget->setCurrentIndex(1);
    }

void MainWindow::showNormalDistribution()
    {
        // Gather data from your QTableWidget or however you're storing the data
        QVector<float> data;
        // populate data from your table
        int selectedColumn = ui->tableWidget->currentColumn();
        if (selectedColumn < 0) {
            QMessageBox::warning(this, "警告", "请选择要绘制直方图的列。");
            return;
        }
        else if(selectedColumn == 1) {  // diagnosis离散数据
            QMessageBox::warning(this, "警告", "对于离散数据，正态分布曲线不具统计意义。");
            //return;
        }
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QTableWidgetItem *item = ui->tableWidget->item(row, selectedColumn);
            if (item) {
                bool ok;
                double value = item->text().toDouble(&ok);
                if (ok) {
                    data.append(value);
                }
            }
        }

        if (data.isEmpty()) {
            QMessageBox::warning(this, "警告", "选定列没有有效的数值数据。");
            return;
        }

        QChart *chart = chartView->chart();

        // Check if a series with the name "Normal Distribution" already exists
        for (auto series : chart->series()) {
            if (series->name() == "Normal Distribution") {
                return; // Exit without adding another normal distribution series
            }
        }

        QLineSeries* normalSeries = generateCurve(data);
        chart->addSeries(normalSeries);

        // Attach the series to the axes, which you might already have set up for the histogram
        //chart->createDefaultAxes();
    }

void MainWindow::drawScatterPlot()
    {
        // 删除之前的scatterChart和scatterSeries
        if (scatterChart) {
            scatterSeries->clear();
            delete scatterChart;
            scatterChart = nullptr;
        }
        scatterSeries = new QScatterSeries();
        connect(scatterSeries, &QScatterSeries::hovered, this, &MainWindow::displayToolTip);

        // 在界面上显示currentFitDegree,pValue和r2Value
        currentFitDegree->setText(QString("fitting curve degree:"));
        pValueLabel->setText(QString("p值: "));
        r2ValueLabel->setText(QString("r2值: "));


        QMap<QPointF, int> pointCounts;

        int xAxisColumn = -1;
        int yAxisColumn = -1;

        // 获取用户选择的两列数据
        QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
        QSet<int> selectedColumns;
        foreach (QTableWidgetItem* item, selectedItems) {
            selectedColumns.insert(item->column());
        }

        if (selectedColumns.size() == 2) {
            xAxisColumn = *selectedColumns.begin();
            yAxisColumn = *(++selectedColumns.begin());
        } else {
            QMessageBox::warning(this, "警告", "请选择两列数据以绘制散点图。");
            return;
        }


        if (xAxisColumn < 0 || yAxisColumn < 0) {
            QMessageBox::warning(this, "警告", "请选择要绘制散点图的两列数据。");
            return;
        }

        QVector<double> xData;
        QVector<double> yData;

        // 收集选定列的数据
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QTableWidgetItem *xItem = ui->tableWidget->item(row, xAxisColumn);
            QTableWidgetItem *yItem = ui->tableWidget->item(row, yAxisColumn);

            if (xItem && yItem) {
                bool okX, okY;
                double xValue = xItem->text().toDouble(&okX);
                double yValue = yItem->text().toDouble(&okY);

                if (okX && okY) {
                    xData.append(xValue);
                    yData.append(yValue);
                    QPointF point(xValue, yValue);
                    if (pointCounts.contains(point)) {
                        pointCounts[point]++;
                    } else {
                        pointCounts[point] = 1;
                    }
                     // 更新点计数
                }
            }
        }

        if (xData.isEmpty() || yData.isEmpty()) {
            QMessageBox::warning(this, "警告", "选定的列中没有有效的数据。");
            return;
        }

        // 创建散点图Series
        for (int i = 0; i < xData.size(); ++i) {
            scatterSeries->append(xData[i], yData[i]);
        }
        scatterSeries->setName("Data");
        scatterSeries->setMarkerSize(5);

        // 创建图表
        scatterChart = new QChart;
        scatterChart->addSeries(scatterSeries);
        scatterChart->setTitle("散点图");

        // 创建X轴和Y轴
        QValueAxis *xAxis = new QValueAxis;
        QValueAxis *yAxis = new QValueAxis;

        //读取并添加X轴和Y轴的对应的数据名称
        QTableWidgetItem *xHeaderItem = ui->tableWidget->horizontalHeaderItem(xAxisColumn);
        QTableWidgetItem *yHeaderItem = ui->tableWidget->horizontalHeaderItem(yAxisColumn);

        if (xHeaderItem) {
            xAxis->setTitleText(xHeaderItem->text());
        } else {
            xAxis->setTitleText(QString("X-Axis (%1)").arg(xAxisColumn));
        }

        if (yHeaderItem) {
            yAxis->setTitleText(yHeaderItem->text());
        } else {
            yAxis->setTitleText(QString("Y-Axis (%1)").arg(yAxisColumn));
        }

        scatterChart->setAxisX(xAxis, scatterSeries);
        scatterChart->setAxisY(yAxis, scatterSeries);
        //scatterChart->legend()->setVisible(true);

        // 设置图表视图
        QChartView *scatterView = new QChartView(scatterChart);
        scatterView->setMouseTracking(true);

        // 遍历pointCounts并将重叠的点添加到QListWidget中
        int overlappingCount = 0;
        for (auto count : pointCounts.values()) {
            if (count > 1) {
                overlappingCount++;
            }
        }
        overlappingCountLabel->setText(QString("重叠点的数量: %1").arg(overlappingCount));


        QVBoxLayout *labelsLayout = new QVBoxLayout;
        labelsLayout->addWidget(overlappingCountLabel);
        labelsLayout->addWidget(currentFitDegree);
        labelsLayout->addWidget(pValueLabel);
        labelsLayout->addWidget(r2ValueLabel);

        // Create a QWidget container for the scatter plot
        QWidget *scatterContainer = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addLayout(labelsLayout);
        layout->addWidget(scatterView);
        scatterContainer->setLayout(layout);

        if (!ui->scatterPlotTab->layout()) {
            ui->scatterPlotTab->setLayout(new QVBoxLayout);
        }

        // Add the QWidget container to the scatterPlotTab
        QLayoutItem* item;
        while ((item = ui->scatterPlotTab->layout()->takeAt(0)) != nullptr) {
            QWidget *widget = item->widget();

            if (widget) {
                // 通过检查对象名称来跳过初始化控件
                if (widget->objectName() != "spinBoxDegree" && widget->objectName() != "fitButton") {
                    delete widget;
                }
            }
            delete item;
        }

        // 将QSpinBox和QPushButton添加到布局中
        QHBoxLayout *fittingLayout = new QHBoxLayout;
        spinBoxDegree->setVisible(true);
        fitButton->setVisible(true);
        fittingLayout->addWidget(spinBoxDegree);
        fittingLayout->addWidget(fitButton);

        ui->scatterPlotTab->layout()->addWidget(scatterContainer);
        ui->scatterPlotTab->layout()->addItem(fittingLayout);
        isScatterPlotWidgetsAdded = true;   //unused here but reserved for other possible use

        // 刷新UI
        ui->tabWidget->setCurrentIndex(2);
    }

void MainWindow::displayToolTip(const QPointF &point, bool hovered)
    {
        if (hovered) {
            QToolTip::showText(QCursor::pos(), QString("X: %1\nY: %2").arg(point.x()).arg(point.y()));
        } else {
            QToolTip::hideText();
        }
    }

void MainWindow::fitCurve() {
        // 获取拟合多项式的次数
        int degree = spinBoxDegree->value();

        // 在创建曲线数据之前，检查图表中是否已存在同名的系列
        for (auto series : scatterChart->series()) {
            if (series->name() == QString("Fit-%1").arg(degree)) {
                QMessageBox::information(this, "提示", "已经存在一个同名的拟合曲线。");
                return;
            }
        }

        int xAxisColumn = -1;
        int yAxisColumn = -1;

        // 获取用户选择的两列数据
        QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
        QSet<int> selectedColumns;
        foreach (QTableWidgetItem* item, selectedItems) {
            selectedColumns.insert(item->column());
        }

        if (selectedColumns.size() == 2) {
            xAxisColumn = *selectedColumns.begin();
            yAxisColumn = *(++selectedColumns.begin());
        } else {
            QMessageBox::warning(this, "警告", "请选择两列数据以绘制散点图。");
            return;
        }


        if (xAxisColumn < 0 || yAxisColumn < 0) {
            QMessageBox::warning(this, "警告", "请选择要绘制散点图的两列数据。");
            return;
        }

        QVector<double> xData;
        QVector<double> yData;

        // 收集选定列的数据
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QTableWidgetItem *xItem = ui->tableWidget->item(row, xAxisColumn);
            QTableWidgetItem *yItem = ui->tableWidget->item(row, yAxisColumn);

            if (xItem && yItem) {
                bool okX, okY;
                double xValue = xItem->text().toDouble(&okX);
                double yValue = yItem->text().toDouble(&okY);

                if (okX && okY) {
                    xData.append(xValue);
                    yData.append(yValue);
                }
            }
        }

        if (xData.isEmpty() || yData.isEmpty()) {
            QMessageBox::warning(this, "警告", "选定的列中没有有效的数据。");
            return;
        }

        // 将QVector转换为std::vector
        std::vector<float> xDataVec(xData.begin(), xData.end());
        std::vector<float> yDataVec(yData.begin(), yData.end());

        // 调用拟合函数
        auto [coefficients, pValue, r2Value] = fitLeastSquareAndPR(xDataVec, yDataVec, degree);

        // 使用coefficients创建曲线数据
        std::vector<std::pair<double, double>> dataPoints;
        QSplineSeries *curveSeries = new QSplineSeries();
        curveSeries->setName(QString("Fit-%1").arg(degree));
        for (double x : xDataVec) {
            double y = 0.0;
            for (int i = 0; i <= degree; ++i) {
                y += coefficients[i] * std::pow(x, i);
            }
            dataPoints.push_back({x, y});
        }

        std::sort(dataPoints.begin(), dataPoints.end(),
                  [](const std::pair<double, double> &a, const std::pair<double, double> &b) {
                      return a.first < b.first; // 基于x值排序
                  });

        for (const auto &point : dataPoints) {
            curveSeries->append(point.first, point.second);
        }

        // 将拟合曲线添加到图表中
        scatterChart->addSeries(curveSeries);
        //chart->removeSeries(scatterSeries);  // 移除旧的散点图
        //chart->addSeries(scatterSeries);  // 添加回散点图，使其位于顶层

        // 重新连接轴
        //chart->setAxisX(chart->axisX(scatterSeries), curveSeries);
        //chart->setAxisY(chart->axisY(scatterSeries), curveSeries);

        // 在界面上显示currentFitDegree,pValue和r2Value
        currentFitDegree->setText(QString("fitting curve degree:%1").arg(degree));
        pValueLabel->setText(QString("p值: %1").arg(pValue));
        r2ValueLabel->setText(QString("r2值: %1").arg(r2Value));
    }

void MainWindow::drawHeatmap(){

    std::vector<std::vector<float>> allData;

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择要绘制直方图的列。");
    }

    // 获取所有选定的列
    QSet<int> selectedColumns;
    std::vector<QString> colNames;
    for (QTableWidgetItem* item : selectedItems) {
        selectedColumns.insert(item->column());
    }

    for (int column : selectedColumns) {
        std::vector<float> columnData = getDataPoints(column);
        if (!columnData.empty()) {
                allData.push_back(columnData);
        }
        QTableWidgetItem *headerItem = ui->tableWidget->horizontalHeaderItem(column);
        if (headerItem) {
                colNames.push_back(headerItem->text());
        }
    }

    if (allData.empty()) {
        QMessageBox::warning(this, "警告", "选定的列没有有效的数值数据。");
    }

    // generate cov n pea matrices
    Eigen::MatrixXf cov = getCovariance(allData);   // covariance matrix
    std::vector<float> vars;
    for (auto vec : allData)
    {
        auto avgVar = getAvgVar(vec);
        vars.push_back(std::get<1>(avgVar));
    }
    Eigen::MatrixXf pea = getPearsonCorr(cov, vars);    //pearson correlation matrix

    // generate heatmap of both matrices
    QPixmap* covPixmap = generateHeatmap(cov, colNames, colNames);
    QPixmap* peaPixmap = generateHeatmap(pea, colNames, colNames);
    QLabel* covLabel = new QLabel();
    QLabel* peaLabel = new QLabel();
    covLabel->setPixmap(*covPixmap);
    peaLabel->setPixmap(*peaPixmap);

    // check if previous label exists then delete them
    if (stackedHeatmap->count() > 1) {                    // 检查是否至少有一个小部件
        QWidget *widgetToRemove = stackedHeatmap->widget(0);
        if (widgetToRemove) {                             // 确保指针不为空
            stackedHeatmap->removeWidget(widgetToRemove); // 从QStackedWidget中移除小部件
            delete widgetToRemove;                        // 释放小部件
        }

        widgetToRemove = stackedHeatmap->widget(0);
        if (widgetToRemove) {                             // 确保指针不为空
            stackedHeatmap->removeWidget(widgetToRemove); // 从QStackedWidget中移除小部件
            delete widgetToRemove;                        // 释放小部件
        }
    }

    stackedHeatmap->addWidget(covLabel);
    stackedHeatmap->addWidget(peaLabel);
    delete covPixmap;  // 释放内存
    delete peaPixmap;  // 释放内存

    // show heatmap of cov in heatmapTab
    if(!isHeatmapWidgetsAdded){
        switchHeatmapButton->setVisible(true);
        QHBoxLayout *labelsLayout = new QHBoxLayout;
        labelsLayout->addWidget(whichHeatmapLabel);
        labelsLayout->addWidget(switchHeatmapButton);

        QVBoxLayout* layout = new QVBoxLayout(ui->heatmapTab);
        layout->addWidget(stackedHeatmap);
        layout->addLayout(labelsLayout);

        ui->heatmapTab->setLayout(layout);
        isHeatmapWidgetsAdded = true;
    }

    stackedHeatmap->setCurrentIndex(0); // to Covariance
    whichHeatmapLabel->setText("Heatmap: Covariance");

    ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::switchHeatmap(){
    if (stackedHeatmap->currentIndex() == 0) {
        stackedHeatmap->setCurrentIndex(1); // to Pearson Correlation
        whichHeatmapLabel->setText("Heatmap: Pearson Correlation");
    } else {
        stackedHeatmap->setCurrentIndex(0); // to Covariance
        whichHeatmapLabel->setText("Heatmap: Covariance");
    }
}

void MainWindow::drawPcaScatterPlot(){
    // 读取数据
    std::vector<float> tagFloat = getDataPoints(1);
    std::vector<int> tag;
    tag.reserve(tagFloat.size());

    for (const float &f : tagFloat) {
        tag.push_back(static_cast<int>(round(f)));
    }


    std::vector<std::vector<float>> allData;

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择要绘制直方图的列。");
    }

    // 获取所有选定的列
    QSet<int> selectedColumns;
    std::vector<QString> colNames;
    for (QTableWidgetItem* item : selectedItems) {
        selectedColumns.insert(item->column());
    }

    // 初始化allData为一个大小为行数的vector，每个元素是一个大小为选定列数的vector
    int rowCount = ui->tableWidget->rowCount();
    int columnCount = selectedColumns.size();
    allData.resize(rowCount, std::vector<float>(columnCount));

    int row = 0;
    for (int column : selectedColumns) {
        std::vector<float> columnData = getDataPoints(column);
        if (!columnData.empty()) {
                for (size_t i = 0; i < columnData.size(); ++i) {
                    allData[i][row] = columnData[i];
                }
        }
        QTableWidgetItem *headerItem = ui->tableWidget->horizontalHeaderItem(column);
        if (headerItem) {
                colNames.push_back(headerItem->text());
        }
        ++row;
    }

    if (allData.empty()) {
        QMessageBox::warning(this, "警告", "选定的列没有有效的数值数据。");
    }

    // pca计算
    Eigen::MatrixXf dataLowered2D = pca(allData, 2);
    Eigen::MatrixXf dataLowered3D = pca(allData, 3);

    // 要求：利用 pca 降维方法后，将每一行数据作为一个点绘制到散点图上

    // 清理stackedPca
    if (stackedPca->count() > 0) {                    // 检查是否至少有一个小部件
        QWidget *widgetToRemove = stackedPca->widget(0);
        if (widgetToRemove) {                             // 确保指针不为空
                stackedPca->removeWidget(widgetToRemove); // 从QStackedWidget中移除小部件
                delete widgetToRemove;                        // 释放小部件
        }
    }
    if (stackedPca->count() > 0) {                    // 检查是否至少有一个小部件
        QWidget *widgetToRemove = stackedPca->widget(0);
        if (widgetToRemove) {                             // 确保指针不为空
                stackedPca->removeWidget(widgetToRemove); // 从QStackedWidget中移除小部件
                delete widgetToRemove;                        // 释放小部件
        }
    }

    // 绘制dataLowered2D对应的立体散点图
    QChart *chart2D = generate2DScatterPlot(dataLowered2D, tag, 1); // 对应diagnosis用1，生病与否的红绿蓝鲜明对比
    QWidget *chartView2D = new QChartView(chart2D, this);
    stackedPca->addWidget(chartView2D);

    // 绘制dataLowered3D对应的立体散点图
    Q3DScatter *scatter3D = generate3DScatterPlot(dataLowered3D, tag, 1);// 对应diagnosis用1，生病与否的红绿蓝鲜明对比
    QWidget *container = QWidget::createWindowContainer(scatter3D);
    stackedPca->addWidget(container);

    // 将平面散点图作为新widget显示在pcaTab中
    if(!isPcaWidgetsAdded){
        switchPcaButton->setVisible(true);
        QHBoxLayout *labelsLayout = new QHBoxLayout;
        QVBoxLayout* layout = new QVBoxLayout(ui->pcaTab);
        labelsLayout->addWidget(whichPcaLabel);
        labelsLayout->addWidget(switchPcaButton);
        layout->addWidget(stackedPca);
        layout->addLayout(labelsLayout);

        ui->pcaTab->setLayout(layout);
        isPcaWidgetsAdded = true;
    }

    stackedPca->setCurrentIndex(0); // 2D
    whichPcaLabel->setText("PCA: 2D");

    ui->tabWidget->setCurrentIndex(4);

}

void MainWindow::switchPca(){
    if (stackedPca->currentIndex() == 0) {
        if(stackedPca->count() > 1){
            stackedPca->setCurrentIndex(1); // 3D
            whichPcaLabel->setText("PCA: 3D");
        }
    } else {
        stackedPca->setCurrentIndex(0); // 2D
        whichPcaLabel->setText("PCA: 2D");
    }
}

void MainWindow::doKmeans(){
    // region: input

    clearPreviousKmeans();
    std::vector<std::vector<float>> allData;
    std::vector<QString> colNames;

    int totalColumns = ui->tableWidget->columnCount();
    int totalRows = ui->tableWidget->rowCount();

    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择要绘制直方图的列。");
    }

    // 获取所有选定的列
    QSet<int> selectedColumns;
    for (QTableWidgetItem* item : selectedItems) {
        selectedColumns.insert(item->column());
    }

    // 初始化allData为一个大小为行数的vector，每个元素是一个大小为选定列数的vector
    int rowCount = ui->tableWidget->rowCount();
    int columnCount = selectedColumns.size();
    allData.resize(rowCount, std::vector<float>(columnCount));

    int row = 0;
    for (int column : selectedColumns) {
        std::vector<float> columnData = getDataPoints(column);
        if (!columnData.empty()) {
                for (size_t i = 0; i < columnData.size(); ++i) {
                    allData[i][row] = columnData[i];
                }
        }
        QTableWidgetItem *headerItem = ui->tableWidget->horizontalHeaderItem(column);
        if (headerItem) {
                colNames.push_back(headerItem->text());
        }
        ++row;
    }

    if (allData.empty()) {
        QMessageBox::warning(this, "警告", "选定的列没有有效的数值数据。");
    }

    // endregion

    // region: calculate kmeans

    int kTheKmeans = ui->kmeansSpinBox->value();
    auto res = clusterKMeans(allData, kTheKmeans);
    std::vector<int> labels = std::get<1>(res);

    // endregion

    // region: draw kmeans

    // pca计算
    Eigen::MatrixXf dataLowered2D = pca(allData, 2);
    Eigen::MatrixXf dataLowered3D = pca(allData, 3);

    // 要求：利用 pca 降维方法后，将每一行数据作为一个点绘制到散点图上

    // 清理stackedKmeans
    if (stackedKmeans->count() > 0) {                    // 检查是否至少有一个小部件
        QWidget *widgetToRemove = stackedKmeans->widget(0);
        if (widgetToRemove) {                             // 确保指针不为空
                stackedKmeans->removeWidget(widgetToRemove); // 从QStackedWidget中移除小部件
                delete widgetToRemove;                        // 释放小部件
        }
    }
    if (stackedKmeans->count() > 0) {                    // 检查是否至少有一个小部件
        QWidget *widgetToRemove = stackedKmeans->widget(0);
        if (widgetToRemove) {                             // 确保指针不为空
                stackedKmeans->removeWidget(widgetToRemove); // 从QStackedWidget中移除小部件
                delete widgetToRemove;                        // 释放小部件
        }
    }

    // 绘制dataLowered2D对应的立体散点图
    QChart *chart2D = generate2DScatterPlot(dataLowered2D, labels, 0);// 对应KMeans用0，不用具有明显良莠暗示的红绿
    QWidget *chartView2D = new QChartView(chart2D, this);
    stackedKmeans->addWidget(chartView2D);

    // 绘制dataLowered3D对应的立体散点图
    Q3DScatter *scatter3D = generate3DScatterPlot(dataLowered3D, labels, 0);// 对应KMeans用0，不用具有明显良莠暗示的红绿
    QWidget *container = QWidget::createWindowContainer(scatter3D);
    stackedKmeans->addWidget(container);

    if(!isKmeansWidgetsAdded){
        switchKmeansButton->setVisible(true);

        // 将平面散点图作为新widget显示在kmeansTab中
        QVBoxLayout* layout = new QVBoxLayout(ui->kmeansTab);
        layout->addWidget(stackedKmeans);

        // 将状态标签和按钮合为新layout显示在kmeansTab中
        QHBoxLayout *labelsLayout = new QHBoxLayout;
        labelsLayout->addWidget(whichKmeansLabel);
        labelsLayout->addWidget(switchKmeansButton);
        layout->addLayout(labelsLayout);

        ui->kmeansTab->setLayout(layout);
        isKmeansWidgetsAdded = true;
    }

    stackedKmeans->setCurrentIndex(0); // 2D
    whichKmeansLabel->setText("Kmeans PCA: 2D");

    // endregion

    // region: output kmeans labels

    // Add a new column named "KMeans"
    ui->tableWidget->insertColumn(totalColumns);
    QTableWidgetItem *header = new QTableWidgetItem("KMeans");
    ui->tableWidget->setHorizontalHeaderItem(totalColumns, header);

    // Fill the new column with the cluster labels
    for (int row = 0; row < totalRows; ++row) {
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(labels[row]));
        ui->tableWidget->setItem(row, totalColumns, newItem);
    }

    // draw color & update
    ui->kmeansColorButton->setChecked(true);
    drawKmeansColor();
    ui->tabWidget->setCurrentIndex(5);

    // endregion
}

void MainWindow::judgeKmeansColor(){
    if(ui->kmeansColorButton->isChecked()){
        drawKmeansColor();
    }
    else{
        clearKmeansColor();
    }
}

void MainWindow::switchKmeans(){
    if (stackedKmeans->currentIndex() == 0) {
        if(stackedKmeans->count() > 1){
            stackedKmeans->setCurrentIndex(1); // 3D
            whichKmeansLabel->setText("Kmeans PCA: 3D");
        }
    } else {
        stackedKmeans->setCurrentIndex(0); // 2D
        whichKmeansLabel->setText("Kmeans PCA: 2D");
    }
}
