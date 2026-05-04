#include "chartdialog.h"
#include "ui_chartdialog.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QtCharts>
QT_CHARTS_USE_NAMESPACE

static double toPercentByThreshold(double rawValue, int threshold)
{
    if (threshold <= 0) {
        return 0.0;
    }

    return qBound(0.0, (rawValue / static_cast<double>(threshold)) * 100.0, 100.0);
}

ChartDialog::ChartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChartDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 暂时不调用setupUi，因为它会自动连接所有信号槽
    // ui->setupUi(this);
    
    // 手动设置UI并进行连接，避免重复连接
    // 首先初始化UI对象
    ui->setupUi(this);
    
    // 断开所有自动连接，特别是对exportChartButton的连接
    disconnect(ui->exportChartButton, nullptr, this, nullptr);
    
    // 初始化成员变量
    m_chart = nullptr;
    m_tempSeries = nullptr;
    m_humiditySeries = nullptr;
    m_concentrationSeries = nullptr;
    m_axisX = nullptr;
    m_axisY = nullptr;
    m_yTickStepPercent = 20;
    m_drawingStopped = false;
    
    // 初始化图表
    initChart();
    
    // 重新建立信号槽连接，但这次我们只建立一次
    connect(ui->adjustPrecisionButton, &QPushButton::clicked, this, &ChartDialog::adjustPrecision);
    connect(ui->continueDrawingButton, &QPushButton::clicked, this, &ChartDialog::continueCurrentDrawing);
    connect(ui->stopDrawingButton, &QPushButton::clicked, this, &ChartDialog::stopCurrentDrawing);
    connect(ui->showTempCheckBox, &QCheckBox::toggled, this, &ChartDialog::toggleTempSeries);
    connect(ui->showHumidityCheckBox, &QCheckBox::toggled, this, &ChartDialog::toggleHumiditySeries);
    connect(ui->showConcentrationCheckBox, &QCheckBox::toggled, this, &ChartDialog::toggleConcentrationSeries);
    connect(ui->exportChartButton, &QPushButton::clicked, this, &ChartDialog::exportChart); // 只建立一次连接
    
    // 初始化统计信息
    updateStats(0, 0, 0);
}

ChartDialog::~ChartDialog()
{
    // 先移除事件过滤器，防止在析构时仍收到事件
    if (m_chartView) {
        m_chartView->removeEventFilter(this);
    }
    
    // 清理UI资源
    delete ui;
    
    // 清理图表相关资源
    // 注意：不需要手动删除系列，因为它们是图表的子对象，会随图表一起被删除
    if (m_chart) {
        delete m_chart;
        m_chart = nullptr;
    }
    
    // 将指针设为nullptr避免重复删除
    m_tempSeries = nullptr;
    m_humiditySeries = nullptr;
    m_concentrationSeries = nullptr;
    m_chartView = nullptr;
    
    qDebug() << "ChartDialog资源已完全清理并析构完成"; // 修改调试信息，更加清晰地表示这是正常的析构流程
}

void ChartDialog::setHistoryData(const QList<QPair<QDateTime, QByteArray>>& historyData)
{
    m_historyData = historyData;
}

void ChartDialog::updateChart()
{
    qDebug() << "updateChart()方法被调用，开始更新图表数据";
    // 准备数据
    prepareChartData();
    
    // 通知已更新
    qDebug() << "updateChart()方法完成，数据已更新";
    emit chartUpdated();
}

void ChartDialog::startNewDrawingSession()
{
    m_drawingStopped = false;
    if (ui && ui->stopDrawingButton) {
        ui->stopDrawingButton->setText("结束绘制");
    }
}

bool ChartDialog::isDrawingStopped() const
{
    return m_drawingStopped;
}

void ChartDialog::prepareChartData()
{
    // 创建空的JSON对象作为默认参数
    prepareChartData(QJsonObject());
}

void ChartDialog::handleNewData(const QList<QPair<QDateTime, QByteArray>>& newData)
{
    // 合并新数据
    m_historyData.append(newData);
    
    // 更新图表
    updateChart();
}

void ChartDialog::onChartClicked(const QPointF &point)
{
    // 可以在这里实现图表点击交互
    qDebug() << "Chart clicked at:" << point;
}

void ChartDialog::initChart()
{
    qDebug() << "initChart()方法被调用，开始创建或更新图表";
    
    // 如果已经存在图表，先清理旧图表
    if (m_chart) {
        qDebug() << "检测到已有图表存在，正在清理旧图表资源";
        
        // 移除事件过滤器
        if (m_chartView) {
            m_chartView->removeEventFilter(this);
        }
        
        // 删除图表对象，它会自动删除其包含的系列和坐标轴
        delete m_chart;
        m_chart = nullptr;
        m_tempSeries = nullptr;
        m_humiditySeries = nullptr;
        m_concentrationSeries = nullptr;
        m_chartView = nullptr;
        
        // 清理布局
        QLayout *oldLayout = ui->chartContainer->layout();
        if (oldLayout) {
            delete oldLayout;
        }
    }
    
    // 设置对话框样式
    this->setWindowTitle("数据监控图表");
    this->resize(900, 700); // 增大对话框尺寸
    
    // 设置容器样式
    ui->chartContainer->setStyleSheet(
        "QGroupBox { " \
        "border: 1px solid #ccc; " \
        "border-radius: 6px; " \
        "margin-top: 10px; " \
        "padding: 10px; " \
        "background-color: #fff; " \
        "} " \
        "QGroupBox::title { " \
        "subcontrol-origin: margin; " \
        "left: 10px; " \
        "padding: 0 3px 0 3px; " \
        "background-color: #fff; " \
        "color: #333; " \
        "font-weight: bold; " \
        "}"
    );
    
    // 创建新图表
    m_chart = new QChart();
    m_chart->setTitle("温度、湿度和浓度百分比图表");
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    m_chart->setTheme(QChart::ChartThemeBlueCerulean); // 设置主题
    m_chart->setDropShadowEnabled(true); // 启用阴影效果
    m_chart->setMargins(QMargins(36, 12, 12, 12));
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom); // 图例放在底部
    
    // 创建温度数据系列
    m_tempSeries = new QLineSeries();
    m_tempSeries->setName("温度 (%)");
    QPen tempPen(QColor(59, 130, 246)); // 蓝色
    tempPen.setWidth(2);
    m_tempSeries->setPen(tempPen);
    m_tempSeries->setUseOpenGL(true); // 使用OpenGL加速渲染
    m_tempSeries->setPointLabelsVisible(false); // 禁用点标签以提高性能

    // 创建湿度数据系列
    m_humiditySeries = new QLineSeries();
    m_humiditySeries->setName("湿度 (%)");
    QPen humidityPen(QColor(76, 217, 100)); // 绿色
    humidityPen.setWidth(2);
    m_humiditySeries->setPen(humidityPen);
    m_humiditySeries->setUseOpenGL(true); // 使用OpenGL加速渲染
    m_humiditySeries->setPointLabelsVisible(false); // 禁用点标签以提高性能
    
    // 创建浓度数据系列
    m_concentrationSeries = new QLineSeries();
    m_concentrationSeries->setName("浓度 (%)");
    QPen concentrationPen(QColor(239, 68, 68)); // 红色
    concentrationPen.setWidth(2);
    m_concentrationSeries->setPen(concentrationPen);
    m_concentrationSeries->setUseOpenGL(true); // 使用OpenGL加速渲染
    m_concentrationSeries->setPointLabelsVisible(false); // 禁用点标签以提高性能
    
    // 创建坐标轴
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat("HH:mm:ss");
    axisX->setTitleText("时间");
    axisX->setTickCount(10); // 设置合理的刻度数量
    axisX->setGridLineVisible(true);
    axisX->setMinorGridLineVisible(false);
    
    QCategoryAxis *axisY = new QCategoryAxis;
    axisY->setTitleText("百分比(%)");
    axisY->setRange(0, 100);
    axisY->setStartValue(0.0);
    axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisY->setLabelsVisible(true);
    axisY->setGridLineVisible(true);
    axisY->setMinorGridLineVisible(false);
    
    // 设置坐标轴样式
    QFont axisFont = axisX->labelsFont();
    axisFont.setPointSize(9);
    axisX->setLabelsFont(axisFont);
    axisY->setLabelsFont(axisFont);
    
    QFont titleFont = axisX->titleFont();
    titleFont.setPointSize(10);
    titleFont.setBold(true);
    axisX->setTitleFont(titleFont);
    axisY->setTitleFont(titleFont);
    
    // 将坐标轴添加到图表
    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_axisY = axisY;
    applyYAxisTickStep(20);
    
    // 将系列添加到图表
    m_chart->addSeries(m_tempSeries);
    m_chart->addSeries(m_humiditySeries);
    m_chart->addSeries(m_concentrationSeries);
    
    // 关联系列和坐标轴
    m_tempSeries->attachAxis(axisX);
    m_tempSeries->attachAxis(axisY);
    m_humiditySeries->attachAxis(axisX);
    m_humiditySeries->attachAxis(axisY);
    m_concentrationSeries->attachAxis(axisX);
    m_concentrationSeries->attachAxis(axisY);
    
    // 创建图表视图
    QChartView *chartView = new QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::RectangleRubberBand);
    chartView->setBackgroundBrush(QBrush(QColor(245, 245, 245))); // 设置背景色
    chartView->setMinimumSize(600, 400); // 设置最小尺寸
    
    // 添加事件过滤器来捕获点击事件
    chartView->installEventFilter(this);
    
    // 先检查并移除chartContainer中已有的布局
    QLayout *oldLayout = ui->chartContainer->layout();
    if (oldLayout) {
        delete oldLayout;
    }
    
    // 添加到布局
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(chartView);
    ui->chartContainer->setLayout(layout);
    
    // 保存chartView指针以便后续使用
    m_chartView = chartView;
    
    qDebug() << "initChart()方法完成，图表已成功创建";
}

void ChartDialog::applyYAxisTickStep(int stepPercent)
{
    if (!m_chart || stepPercent <= 0) {
        return;
    }

    const int clampedStep = qBound(1, stepPercent, 100);
    m_yTickStepPercent = clampedStep;

    if (!m_axisY) {
        const QList<QAbstractAxis*> axes = m_chart->axes();
        for (auto *axis : axes) {
            if (qobject_cast<QValueAxis*>(axis) && axis->alignment() == Qt::AlignLeft) {
                m_axisY = static_cast<QValueAxis*>(axis);
                break;
            }
        }
    }

    if (!m_axisY) {
        return;
    }

    QCategoryAxis *categoryAxis = qobject_cast<QCategoryAxis*>(m_axisY);
    if (!categoryAxis) {
        return;
    }

    categoryAxis->setRange(0.0, 100.0);
    categoryAxis->setStartValue(0.0);
    categoryAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    const QStringList oldLabels = categoryAxis->categoriesLabels();
    for (const QString &label : oldLabels) {
        categoryAxis->remove(label);
    }

    for (int value = m_yTickStepPercent; value <= 100; value += m_yTickStepPercent) {
        categoryAxis->append(QString::number(value), value);
    }

    QFont yLabelFont = categoryAxis->labelsFont();
    yLabelFont.setPointSize(m_yTickStepPercent <= 10 ? 9 : 10);
    categoryAxis->setLabelsFont(yLabelFont);

    m_chart->setMargins(QMargins(40, 12, 12, 12));

    m_chart->update();
}

// 确保只保留一个函数定义，移除可能的重复声明
void ChartDialog::prepareChartData(const QJsonObject &jsonObject)
{
    // 检查图表组件是否初始化
    if (!m_chart || !m_tempSeries || !m_humiditySeries || !m_concentrationSeries) {
        qDebug() << "图表组件尚未初始化，无法准备数据";
        return;
    }
    
    qDebug() << "准备图表数据，使用传入的JSON对象和/或历史数据";

    QSettings settings("myendchart", "thresholds");
    const int thresholdT = qBound(0, settings.value("T", 100).toInt(), 100);
    const int thresholdH = qBound(20, settings.value("H", 100).toInt(), 100);
    const int thresholdG = qBound(0, settings.value("G", 5000).toInt(), 5000);
    
    // 清空现有数据
    m_tempSeries->clear();
    m_humiditySeries->clear();
    m_concentrationSeries->clear();
    
    // 使用集合去重，避免添加重复的时间点
    QSet<qint64> tempTimeSet;
    QSet<qint64> humidityTimeSet;
    QSet<qint64> concentrationTimeSet;
    
    int validDataCount = 0;
    int tempDataCount = 0;
    int humidityDataCount = 0;
    int concentrationDataCount = 0;
    qint64 earliestTime = -1, latestTime = -1;
    double minTemp = 100.0, maxTemp = 0.0;
    double minHumidity = 100.0, maxHumidity = 0.0;
    double minConcentration = 100.0, maxConcentration = 0.0;
    
    // 处理类中存储的历史数据
    if (jsonObject.isEmpty() && !m_historyData.isEmpty()) {
        qDebug() << "处理历史数据，数据条数:" << m_historyData.size();
        
        for (const auto &dataPair : m_historyData) {
            const QDateTime &timestamp = dataPair.first;
            const QByteArray &dataBytes = dataPair.second;
            
            qint64 timeValue = timestamp.toMSecsSinceEpoch();
            qDebug() << "历史数据时间:" << timestamp.toString();
            
            // 尝试解析JSON数据
            QJsonDocument jsonDoc = QJsonDocument::fromJson(dataBytes);
            if (!jsonDoc.isNull() && jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                
                // 处理data.list格式的历史数据 (来自历史查询)
                if (jsonObj.contains("data") && jsonObj["data"].isObject()) {
                    QJsonObject dataObj = jsonObj["data"].toObject();
                    if (dataObj.contains("list") && dataObj["list"].isArray()) {
                        QJsonArray listArray = dataObj["list"].toArray();
                        qDebug() << "  找到data.list数组，长度:" << listArray.size();
                        
                        for (const auto value : listArray) {
                            if (value.isObject()) {
                                QJsonObject itemObj = value.toObject();
                                
                                // 提取时间和数值
                                if (itemObj.contains("time") && itemObj["time"].isDouble() &&
                                    itemObj.contains("value") && itemObj["value"].isString()) {
                                    
                                    qint64 itemTimeValue = static_cast<qint64>(itemObj["time"].toDouble());
                                    QString valueStr = itemObj["value"].toString();
                                    
                                    QStringList dataList = valueStr.split(",");
                                    
                                    double temperature = 0.0;
                                    double humidity = 0.0;
                                    double concentration = 0.0;
                                    bool hasTemperature = false;
                                    bool hasHumidity = false;
                                    bool hasConcentration = false;
                                    
                                    for (const auto &dataItem : dataList) {
                                        QStringList keyValue = dataItem.split(":");
                                        if (keyValue.size() == 2) {
                                            QString key = keyValue[0].trimmed().toLower();
                                            QString val = keyValue[1].trimmed();
                                            
                                            if (key == "t") {
                                                temperature = toPercentByThreshold(val.toDouble(), thresholdT);
                                                hasTemperature = true;
                                                tempDataCount++;
                                                qDebug() << "    找到温度数据:" << temperature;
                                            } else if (key == "h") {
                                                humidity = toPercentByThreshold(val.toDouble(), thresholdH);
                                                hasHumidity = true;
                                                humidityDataCount++;
                                                qDebug() << "    找到湿度数据:" << humidity;
                                            } else if (key == "g") {
                                                concentration = toPercentByThreshold(val.toDouble(), thresholdG);
                                                hasConcentration = true;
                                                concentrationDataCount++;
                                                qDebug() << "    找到浓度数据:" << concentration;
                                            }
                                        }
                                    }
                                    
                                    if (hasTemperature || hasHumidity || hasConcentration) {
                                        // 更新数据范围
                                        if (earliestTime == -1 || itemTimeValue < earliestTime) earliestTime = itemTimeValue;
                                        if (latestTime == -1 || itemTimeValue > latestTime) latestTime = itemTimeValue;
                                        
                                        // 添加数据点
                                        if (hasTemperature && !tempTimeSet.contains(itemTimeValue)) {
                                            minTemp = qMin(minTemp, temperature);
                                            maxTemp = qMax(maxTemp, temperature);
                                            m_tempSeries->append(itemTimeValue, temperature);
                                            tempTimeSet.insert(itemTimeValue);
                                        }

                                        if (hasHumidity && !humidityTimeSet.contains(itemTimeValue)) {
                                            minHumidity = qMin(minHumidity, humidity);
                                            maxHumidity = qMax(maxHumidity, humidity);
                                            m_humiditySeries->append(itemTimeValue, humidity);
                                            humidityTimeSet.insert(itemTimeValue);
                                        }
                                        
                                        if (hasConcentration && !concentrationTimeSet.contains(itemTimeValue)) {
                                            minConcentration = qMin(minConcentration, concentration);
                                            maxConcentration = qMax(maxConcentration, concentration);
                                            m_concentrationSeries->append(itemTimeValue, concentration);
                                            concentrationTimeSet.insert(itemTimeValue);
                                        }
                                        
                                        validDataCount++;
                                    }
                                }
                            }
                        }
                    }
                }
                // 处理data数组格式的实时数据
                else if (jsonObj.contains("data") && jsonObj["data"].isArray()) {
                    QJsonArray dataArray = jsonObj["data"].toArray();
                    qDebug() << "  找到data数组，长度:" << dataArray.size();
                    
                    double temperature = 0.0;
                    double humidity = 0.0;
                    double concentration = 0.0;
                    bool hasTemperature = false;
                    bool hasHumidity = false;
                    bool hasConcentration = false;
                    
                    for (const auto value : dataArray) {
                        if (value.isObject()) {
                            QJsonObject itemObj = value.toObject();
                            
                            if (itemObj.contains("identifier") && itemObj["identifier"].toString() == "message" && 
                                itemObj.contains("value") && itemObj["value"].isString()) {
                                
                                QString messageValue = itemObj["value"].toString();
                                qDebug() << "  消息值:" << messageValue;
                                
                                QStringList dataList = messageValue.split(",");
                                
                                for (const auto &dataItem : dataList) {
                                    QStringList keyValue = dataItem.split(":");
                                    if (keyValue.size() == 2) {
                                        QString key = keyValue[0].trimmed().toLower();
                                        QString val = keyValue[1].trimmed();
                                        
                                        if (key == "t") {
                                            temperature = toPercentByThreshold(val.toDouble(), thresholdT);
                                            hasTemperature = true;
                                            tempDataCount++;
                                            qDebug() << "    找到温度数据:" << temperature;
                                        } else if (key == "h") {
                                            humidity = toPercentByThreshold(val.toDouble(), thresholdH);
                                            hasHumidity = true;
                                            humidityDataCount++;
                                            qDebug() << "    找到湿度数据:" << humidity;
                                        } else if (key == "g") {
                                            concentration = toPercentByThreshold(val.toDouble(), thresholdG);
                                            hasConcentration = true;
                                            concentrationDataCount++;
                                            qDebug() << "    找到浓度数据:" << concentration;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    if (hasTemperature || hasHumidity || hasConcentration) {
                        // 更新数据范围
                        if (earliestTime == -1 || timeValue < earliestTime) earliestTime = timeValue;
                        if (latestTime == -1 || timeValue > latestTime) latestTime = timeValue;
                        
                        // 添加数据点
                        if (hasTemperature && !tempTimeSet.contains(timeValue)) {
                            minTemp = qMin(minTemp, temperature);
                            maxTemp = qMax(maxTemp, temperature);
                            m_tempSeries->append(timeValue, temperature);
                            tempTimeSet.insert(timeValue);
                        }

                        if (hasHumidity && !humidityTimeSet.contains(timeValue)) {
                            minHumidity = qMin(minHumidity, humidity);
                            maxHumidity = qMax(maxHumidity, humidity);
                            m_humiditySeries->append(timeValue, humidity);
                            humidityTimeSet.insert(timeValue);
                        }
                        
                        if (hasConcentration && !concentrationTimeSet.contains(timeValue)) {
                            minConcentration = qMin(minConcentration, concentration);
                            maxConcentration = qMax(maxConcentration, concentration);
                            m_concentrationSeries->append(timeValue, concentration);
                            concentrationTimeSet.insert(timeValue);
                        }
                        
                        validDataCount++;
                    }
                }
            } else {
                qDebug() << "  无法解析JSON数据，尝试直接查找温度、湿度和浓度信息";
                // 作为后备方案，尝试直接在字符串中查找温度、湿度和浓度信息
                QString valueStr = QString::fromUtf8(dataBytes);
                
                // 尝试匹配常见的温度、湿度和浓度格式
                QRegularExpression tempRegex("\\bT[:=]\\s*([\\d.]+)", QRegularExpression::CaseInsensitiveOption);
                QRegularExpression humRegex("\\bH[:=]\\s*([\\d.]+)", QRegularExpression::CaseInsensitiveOption);
                QRegularExpression concRegex("\\bG[:=]\\s*([\\d.]+)", QRegularExpression::CaseInsensitiveOption);
                
                QRegularExpressionMatch tempMatch = tempRegex.match(valueStr);
                if (tempMatch.hasMatch()) {
                    double temperature = toPercentByThreshold(tempMatch.captured(1).toDouble(), thresholdT);
                    if (!tempTimeSet.contains(timeValue)) {
                        minTemp = qMin(minTemp, temperature);
                        maxTemp = qMax(maxTemp, temperature);
                        m_tempSeries->append(timeValue, temperature);
                        tempTimeSet.insert(timeValue);
                        tempDataCount++;
                        validDataCount++;
                        qDebug() << "  从字符串中提取到温度:" << temperature;
                    }
                }

                QRegularExpressionMatch humMatch = humRegex.match(valueStr);
                if (humMatch.hasMatch()) {
                    double humidity = toPercentByThreshold(humMatch.captured(1).toDouble(), thresholdH);
                    if (!humidityTimeSet.contains(timeValue)) {
                        minHumidity = qMin(minHumidity, humidity);
                        maxHumidity = qMax(maxHumidity, humidity);
                        m_humiditySeries->append(timeValue, humidity);
                        humidityTimeSet.insert(timeValue);
                        humidityDataCount++;
                        validDataCount++;
                        qDebug() << "  从字符串中提取到湿度:" << humidity;
                    }
                }
                
                QRegularExpressionMatch concMatch = concRegex.match(valueStr);
                if (concMatch.hasMatch()) {
                    double concentration = toPercentByThreshold(concMatch.captured(1).toDouble(), thresholdG);
                    if (!concentrationTimeSet.contains(timeValue)) {
                        minConcentration = qMin(minConcentration, concentration);
                        maxConcentration = qMax(maxConcentration, concentration);
                        m_concentrationSeries->append(timeValue, concentration);
                        concentrationTimeSet.insert(timeValue);
                        concentrationDataCount++;
                        validDataCount++;
                        qDebug() << "  从字符串中提取到浓度:" << concentration;
                    }
                }
            }
        }
    }
    // 处理传入的JSON对象数据
    else if (!jsonObject.isEmpty()) {
        // 处理data.list格式的历史数据
        if (jsonObject.contains("data") && jsonObject["data"].isObject()) {
            QJsonObject dataObj = jsonObject["data"].toObject();
            if (dataObj.contains("list") && dataObj["list"].isArray()) {
                QJsonArray listArray = dataObj["list"].toArray();
                qDebug() << "找到data.list数组，长度:" << listArray.size();
                
                for (const auto value : listArray) {
                    if (value.isObject()) {
                        QJsonObject itemObj = value.toObject();
                        
                        // 提取时间和数值
                        if (itemObj.contains("time") && itemObj["time"].isDouble() &&
                            itemObj.contains("value") && itemObj["value"].isString()) {
                            
                            qint64 timeValue = static_cast<qint64>(itemObj["time"].toDouble());
                            QDateTime itemTime = QDateTime::fromMSecsSinceEpoch(timeValue);
                            
                            QString valueStr = itemObj["value"].toString();
                            qDebug() << "数据项时间:" << itemTime.toString() << "值:" << valueStr;
                            
                            QStringList dataList = valueStr.split(",");
                            
                            double temperature = 0.0;
                            double humidity = 0.0;
                            double concentration = 0.0;
                            bool hasTemperature = false;
                            bool hasHumidity = false;
                            bool hasConcentration = false;
                            
                            for (const auto &dataItem : dataList) {
                                QStringList keyValue = dataItem.split(":");
                                if (keyValue.size() == 2) {
                                    QString key = keyValue[0].trimmed().toLower(); // 转为小写并去空格
                                    QString val = keyValue[1].trimmed();
                                    
                                    // 支持多种可能的字段名
                                    if (key == "t") {
                                        temperature = toPercentByThreshold(val.toDouble(), thresholdT);
                                        hasTemperature = true;
                                        tempDataCount++;
                                        qDebug() << "  找到温度数据:" << temperature;
                                    } else if (key == "h") {
                                        humidity = toPercentByThreshold(val.toDouble(), thresholdH);
                                        hasHumidity = true;
                                        humidityDataCount++;
                                        qDebug() << "  找到湿度数据:" << humidity;
                                    } else if (key == "g") {
                                        concentration = toPercentByThreshold(val.toDouble(), thresholdG);
                                        hasConcentration = true;
                                        concentrationDataCount++;
                                        qDebug() << "  找到浓度数据:" << concentration;
                                    }
                                }
                            }
                            
                            bool hasValidData = hasTemperature || hasHumidity || hasConcentration;
                            if (hasValidData) {
                                // 更新数据范围
                                if (earliestTime == -1 || timeValue < earliestTime) earliestTime = timeValue;
                                if (latestTime == -1 || timeValue > latestTime) latestTime = timeValue;
                                
                                // 只更新有数据的范围
                                if (hasTemperature && !tempTimeSet.contains(timeValue)) {
                                    minTemp = qMin(minTemp, temperature);
                                    maxTemp = qMax(maxTemp, temperature);
                                    m_tempSeries->append(timeValue, temperature);
                                    tempTimeSet.insert(timeValue);
                                }

                                if (hasHumidity && !humidityTimeSet.contains(timeValue)) {
                                    minHumidity = qMin(minHumidity, humidity);
                                    maxHumidity = qMax(maxHumidity, humidity);
                                    m_humiditySeries->append(timeValue, humidity);
                                    humidityTimeSet.insert(timeValue);
                                }
                                
                                if (hasConcentration && !concentrationTimeSet.contains(timeValue)) {
                                    minConcentration = qMin(minConcentration, concentration);
                                    maxConcentration = qMax(maxConcentration, concentration);
                                    m_concentrationSeries->append(timeValue, concentration);
                                    concentrationTimeSet.insert(timeValue);
                                }
                                
                                validDataCount++;
                            }
                        }
                    }
                }
            }
        }
        // 处理data数组格式的实时数据
        else if (jsonObject.contains("data") && jsonObject["data"].isArray()) {
            QJsonArray dataArray = jsonObject["data"].toArray();
            qDebug() << "找到data数组，长度:" << dataArray.size();
            
            qint64 timeValue = QDateTime::currentDateTime().toMSecsSinceEpoch();
            double temperature = 0.0;
            double humidity = 0.0;
            double concentration = 0.0;
            bool hasTemperature = false;
            bool hasHumidity = false;
            bool hasConcentration = false;
            
            for (const auto value : dataArray) {
                if (value.isObject()) {
                    QJsonObject itemObj = value.toObject();
                    
                    if (itemObj.contains("identifier") && itemObj["identifier"].toString() == "message" && 
                        itemObj.contains("value") && itemObj["value"].isString()) {
                        
                        QString messageValue = itemObj["value"].toString();
                        qDebug() << "消息值:" << messageValue;
                        
                        QStringList dataList = messageValue.split(",");
                        
                        for (const auto &dataItem : dataList) {
                            QStringList keyValue = dataItem.split(":");
                            if (keyValue.size() == 2) {
                                QString key = keyValue[0].trimmed().toLower();
                                QString val = keyValue[1].trimmed();
                                
                                // 支持多种可能的字段名
                                if (key == "t") {
                                    temperature = toPercentByThreshold(val.toDouble(), thresholdT);
                                    hasTemperature = true;
                                    tempDataCount++;
                                    qDebug() << "  找到温度数据:" << temperature;
                                } else if (key == "h") {
                                    humidity = toPercentByThreshold(val.toDouble(), thresholdH);
                                    hasHumidity = true;
                                    humidityDataCount++;
                                    qDebug() << "  找到湿度数据:" << humidity;
                                } else if (key == "g") {
                                    concentration = toPercentByThreshold(val.toDouble(), thresholdG);
                                    hasConcentration = true;
                                    concentrationDataCount++;
                                    qDebug() << "  找到浓度数据:" << concentration;
                                }
                            }
                        }
                    }
                }
            }
            
            if (hasTemperature || hasHumidity || hasConcentration) {
                // 更新数据范围
                if (earliestTime == -1 || timeValue < earliestTime) earliestTime = timeValue;
                if (latestTime == -1 || timeValue > latestTime) latestTime = timeValue;
                
                // 添加数据点
                if (hasTemperature && !tempTimeSet.contains(timeValue)) {
                    minTemp = qMin(minTemp, temperature);
                    maxTemp = qMax(maxTemp, temperature);
                    m_tempSeries->append(timeValue, temperature);
                    tempTimeSet.insert(timeValue);
                }

                if (hasHumidity && !humidityTimeSet.contains(timeValue)) {
                    minHumidity = qMin(minHumidity, humidity);
                    maxHumidity = qMax(maxHumidity, humidity);
                    m_humiditySeries->append(timeValue, humidity);
                    humidityTimeSet.insert(timeValue);
                }
                
                if (hasConcentration && !concentrationTimeSet.contains(timeValue)) {
                    minConcentration = qMin(minConcentration, concentration);
                    maxConcentration = qMax(maxConcentration, concentration);
                    m_concentrationSeries->append(timeValue, concentration);
                    concentrationTimeSet.insert(timeValue);
                }
                
                validDataCount++;
            }
        }
    }
    
    qDebug() << "有效数据点数:" << validDataCount;
    qDebug() << "温度数据点数:" << tempDataCount;
    qDebug() << "湿度数据点数:" << humidityDataCount;
    qDebug() << "浓度数据点数:" << concentrationDataCount;
    qDebug() << "最终温度系列点数:" << m_tempSeries->count();
    qDebug() << "最终湿度系列点数:" << m_humiditySeries->count();
    qDebug() << "最终浓度系列点数:" << m_concentrationSeries->count();
    
    // 确保数据点按时间排序
    if (m_tempSeries->count() > 1) {
        QList<QPointF> tempPoints = m_tempSeries->points();
        std::sort(tempPoints.begin(), tempPoints.end(), [](const QPointF &a, const QPointF &b) {
            return a.x() < b.x();
        });
        m_tempSeries->replace(tempPoints);
    }

    if (m_humiditySeries->count() > 1) {
        QList<QPointF> humidityPoints = m_humiditySeries->points();
        std::sort(humidityPoints.begin(), humidityPoints.end(), [](const QPointF &a, const QPointF &b) {
            return a.x() < b.x();
        });
        m_humiditySeries->replace(humidityPoints);
    }
    
    if (m_concentrationSeries->count() > 1) {
        QList<QPointF> concentrationPoints = m_concentrationSeries->points();
        std::sort(concentrationPoints.begin(), concentrationPoints.end(), [](const QPointF &a, const QPointF &b) {
            return a.x() < b.x();
        });
        m_concentrationSeries->replace(concentrationPoints);
    }
    
    // 设置时间轴范围
    if (earliestTime != -1 && latestTime != -1) {
        qDebug() << "设置时间轴范围: 最早:" << QDateTime::fromMSecsSinceEpoch(earliestTime).toString() 
                << "最晚:" << QDateTime::fromMSecsSinceEpoch(latestTime).toString();
        
        // 获取时间轴
        QList<QAbstractAxis*> axes = m_chart->axes();
        QDateTimeAxis *axisX = nullptr;
        QValueAxis *axisY = nullptr;
        
        for (auto *axis : axes) {
            if (qobject_cast<QDateTimeAxis*>(axis)) {
                axisX = static_cast<QDateTimeAxis*>(axis);
            } else if (qobject_cast<QValueAxis*>(axis)) {
                axisY = static_cast<QValueAxis*>(axis);
            }
        }
        
        if (axisX) {
            // 为了更好的显示效果，给时间范围增加10%的边距
            qint64 margin = static_cast<qint64>((latestTime - earliestTime) * 0.1); // 10%的边距，显式转换
            
            axisX->setMin(QDateTime::fromMSecsSinceEpoch(earliestTime - margin));
            axisX->setMax(QDateTime::fromMSecsSinceEpoch(latestTime + margin));
        }
        
        if (axisY) {
            axisY->setRange(0, 100);
            applyYAxisTickStep(m_yTickStepPercent);
        }
        
        // 更新统计信息
        updateStats(validDataCount, earliestTime, latestTime);
    } else {
        qDebug() << "没有足够的数据来设置坐标轴范围";
        updateStats(0, 0, 0);
    }
    
    // 如果数据为空，可能需要请求更多数据
    if (m_tempSeries->count() == 0 && m_humiditySeries->count() == 0 && m_concentrationSeries->count() == 0) {
        qDebug() << "无法从现有数据中提取有效图表数据";
    }
}

// 事件过滤器处理图表交互事件
bool ChartDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_chartView) {
        // 处理鼠标点击事件
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                // 优化坐标转换，避免不必要的计算
                QPointF point = m_chartView->mapToScene(mouseEvent->pos());
                
                // 检查点击是否在图表区域内
                if (m_chart->plotArea().contains(m_chart->mapFromScene(point))) {
                    QPointF chartPoint = m_chart->mapFromScene(point);
                    
                    // 调试输出
                    qDebug() << "图表点击坐标:" << chartPoint;
                    
                    // 处理点击事件
                    onChartClicked(chartPoint);
                    
                    // 返回true表示事件已处理
                    return true;
                }
            }
        }
    }
    // 返回false让事件继续传播
    return QDialog::eventFilter(watched, event);
}

void ChartDialog::adjustPrecision()
{
    // 在 20% 和 10% 两档精度间切换
    if (m_yTickStepPercent == 20) {
        applyYAxisTickStep(10);
    } else {
        applyYAxisTickStep(20);
    }
}

void ChartDialog::continueCurrentDrawing()
{
    m_drawingStopped = false;
    emit continueDrawingRequested();
    close();
}

void ChartDialog::stopCurrentDrawing()
{
    m_drawingStopped = true;
    emit stopDrawingRequested();
    close();
}
 
void ChartDialog::toggleTempSeries(bool visible)
{
    m_tempSeries->setVisible(visible);
}

void ChartDialog::toggleHumiditySeries(bool visible)
{
    m_humiditySeries->setVisible(visible);
}

void ChartDialog::toggleConcentrationSeries(bool visible)
{
    m_concentrationSeries->setVisible(visible);
}
 
void ChartDialog::exportChart()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出图表", ".",
                                                 "PNG图片 (*.png);;JPEG图片 (*.jpg);;BMP图片 (*.bmp);;所有文件 (*)");
    
    if (!fileName.isEmpty()) {
        // 获取图表的图像
        QPixmap pixmap = m_chartView->grab();
        
        // 保存图像
        if (pixmap.save(fileName)) {
            QMessageBox::information(this, "成功", "图表已成功导出！");
        } else {
            QMessageBox::warning(this, "错误", "图表导出失败！");
        }
    }
}

// 处理窗口关闭事件
void ChartDialog::closeEvent(QCloseEvent *event)
{
    // 简化closeEvent，只接受关闭事件
    // 主要的资源清理工作由析构函数处理，避免重复释放和纯虚函数调用错误
    
    // 接受关闭事件
    event->accept();
}

void ChartDialog::updateStats(int dataCount, qint64 startTime, qint64 endTime)
{
    if (!ui) {
        return;
    }

    if (startTime <= 0 || endTime <= 0) {
        ui->dataCountLabel->setText("日期: -- | 数据点数量: 0");
        ui->timeRangeLabel->setText("时间范围: --");
        return;
    }

    const QDateTime startDt = QDateTime::fromMSecsSinceEpoch(startTime);
    const QDateTime endDt = QDateTime::fromMSecsSinceEpoch(endTime);
    QString dateText;
    if (startDt.date() == endDt.date()) {
        dateText = startDt.toString("yyyy-MM-dd");
    } else {
        dateText = startDt.toString("yyyy-MM-dd") + " ~ " + endDt.toString("yyyy-MM-dd");
    }

    ui->dataCountLabel->setText(QString("日期: %1 | 数据点数量: %2").arg(dateText).arg(dataCount));
    ui->timeRangeLabel->setText(
        QString("时间范围: %1 - %2")
            .arg(startDt.toString("HH:mm:ss"))
            .arg(endDt.toString("HH:mm:ss"))
    );
}
