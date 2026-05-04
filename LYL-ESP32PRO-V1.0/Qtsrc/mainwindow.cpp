#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chartdialog.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include <QUrlQuery>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QByteArray>
#include <QStringList>
#include <QPainter>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QSettings>
#include <QProcess>
#include <QCoreApplication>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    networkManager(nullptr),
    m_chartDialog(nullptr),
    m_queryLatestInProgress(false),
    m_latestQueryReply(nullptr),
    m_autoRefreshTimer(nullptr),
    m_historyModeActive(false),
    m_responseInfoLockedAfterClear(false),
    m_thresholdT(100),
    m_thresholdH(100),
    m_thresholdG(5000),
    m_hasLatestTemperature(false),
    m_hasLatestHumidity(false),
    m_hasLatestConcentration(false),
    m_latestTemperature(0.0),
    m_latestHumidity(0.0),
    m_latestConcentration(0.0),
    m_thresholdAutoAlarmOn(false),
    m_thresholdAutoControlInitialized(false),
    m_hasChartDrawStartTime(false),
    m_chartDrawStartTime(),
    m_resumeChartSession(false)
{
    ui->setupUi(this);

    // 读取阈值配置（重启后保持）
    QSettings settings("myendchart", "thresholds");
    m_thresholdT = qBound(0, settings.value("T", 100).toInt(), 100);
    m_thresholdH = qBound(20, settings.value("H", 100).toInt(), 100);
    m_thresholdG = qBound(0, settings.value("G", 5000).toInt(), 5000);

    // 防止UI文件中的连接和connectSlotsByName重复连接，导致按钮点击触发两次
    disconnect(ui->queryButton, nullptr, this, nullptr);
    disconnect(ui->queryHistoryButton, nullptr, this, nullptr);
    disconnect(ui->clearButton, nullptr, this, nullptr);
    disconnect(ui->refreshButton, nullptr, this, nullptr);
    disconnect(ui->exportButton, nullptr, this, nullptr);
    disconnect(ui->openFanButton, nullptr, this, nullptr);
    disconnect(ui->closeFanButton, nullptr, this, nullptr);
    disconnect(ui->openLightButton, nullptr, this, nullptr);
    disconnect(ui->closeLightButton, nullptr, this, nullptr);
    disconnect(ui->drawChartButton, nullptr, this, nullptr);

    // 统一使用显式连接，只连接一次
    connect(ui->queryButton, &QPushButton::clicked, this, &MainWindow::on_queryButton_clicked);
    connect(ui->queryHistoryButton, &QPushButton::clicked, this, &MainWindow::on_queryHistoryButton_clicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::on_clearButton_clicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::on_refreshButton_clicked);
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::on_exportButton_clicked);
    connect(ui->openFanButton, &QPushButton::clicked, this, &MainWindow::on_openFanButton_clicked);
    connect(ui->closeFanButton, &QPushButton::clicked, this, &MainWindow::on_closeFanButton_clicked);
    connect(ui->openLightButton, &QPushButton::clicked, this, &MainWindow::on_openLightButton_clicked);
    connect(ui->closeLightButton, &QPushButton::clicked, this, &MainWindow::on_closeLightButton_clicked);
    connect(ui->drawChartButton, &QPushButton::clicked, this, &MainWindow::on_drawChartButton_clicked);

    // 主页面菜单：关于
    QMenu *aboutMenu = ui->menuBar->addMenu("帮助");
    QAction *aboutAction = aboutMenu->addAction("关于");
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::information(
            this,
            "关于",
            "作者：李玉路\n"
            "学院：智能制造学院\n"
            "声明：此软件最总解释权归个人所有\n"
            "电话：18130132281\n"
        );
    });
    
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onNetworkReplyReceived);

    // 每1秒自动刷新一次实时数据
    m_autoRefreshTimer = new QTimer(this);
    m_autoRefreshTimer->setInterval(1000);
    connect(m_autoRefreshTimer, &QTimer::timeout, this, [this]() {
        queryLatestData(false);
    });
    m_autoRefreshTimer->start();

    // 启动后先拉取一次最新数据
    queryLatestData(true);
}

MainWindow::~MainWindow()
{
    if (networkManager) {
        delete networkManager;
        networkManager = nullptr;
    }
    
    if (m_chartDialog) {
        delete m_chartDialog;
        m_chartDialog = nullptr;
    }
    
    delete ui;
}

//发送设备属性设置的POST请求
void MainWindow::sendDevicePropertySet(const QString &command, const QString &operationType) {
    // 存储当前操作类型
    currentOperation = operationType;
    
    // 随机生成id（使用当前时间戳加上随机数）
    QString randomId = QString::number(QDateTime::currentMSecsSinceEpoch()) + 
                       QString::number(qrand() % 1000);
    
    // 创建JSON请求体
    QJsonObject jsonObj;
    jsonObj["id"] = randomId;
    jsonObj["version"] = "1.0";
    jsonObj["product_id"] = "83bcAx554p";
    jsonObj["device_name"] = "Linux_my_client_start";
    
    // 创建params对象
    QJsonObject paramsObj;
    paramsObj["command"] = command;
    jsonObj["params"] = paramsObj;
    
    // 转换为JSON字符串
    QJsonDocument jsonDoc(jsonObj);
    QByteArray postData = jsonDoc.toJson();
    
    // 创建请求对象
    QUrl url("http://iot-api.heclouds.com/thingmodel/set-device-property");
    QNetworkRequest request(url);
    
    // 设置HTTP头信息
    request.setRawHeader("Authorization", QString("version=2018-10-31&res=products%2F83bcAx554p&et=1797929077&method=sha1&sign=GWTUG3SkNpqmWc1wsQmDtLJGCqw%3D").toUtf8());
    request.setRawHeader("User-Agent", QString("Apifox/1.0.0 (https://apifox.com)").toUtf8());
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "*/*");
    
    // 发送POST请求
    qDebug() << "发送设备属性设置请求，ID: " << randomId << " Command: " << command;
    qDebug() << "请求数据: " << postData;
    networkManager->post(request, postData);
}

int MainWindow::toPercentByThreshold(double value, int threshold) const
{
    if (threshold <= 0) {
        return 0;
    }

    const double ratio = (value / static_cast<double>(threshold)) * 100.0;
    return qBound(0, static_cast<int>(ratio), 100);
}

void MainWindow::applyThresholdToProgressBars()
{
    if (m_hasLatestTemperature) {
        ui->temperatureProgressBar->setValue(toPercentByThreshold(m_latestTemperature, m_thresholdT));
    }
    if (m_hasLatestHumidity) {
        ui->humidityProgressBar->setValue(toPercentByThreshold(m_latestHumidity, m_thresholdH));
    }
    if (m_hasLatestConcentration) {
        ui->concentrationProgressBar->setValue(toPercentByThreshold(m_latestConcentration, m_thresholdG));
    }
}

QList<QPair<QDateTime, QByteArray>> MainWindow::buildChartHistorySinceStart() const
{
    if (!m_hasChartDrawStartTime) {
        return responseHistory;
    }

    QList<QPair<QDateTime, QByteArray>> filtered;
    for (const auto &item : responseHistory) {
        if (item.first >= m_chartDrawStartTime) {
            filtered.append(item);
        }
    }
    return filtered;
}

void MainWindow::handleThresholdAutoControl(bool temperatureOk, double tempValue,
                                            bool humidityOk, double humValue,
                                            bool concentrationOk, double concValue)
{
    // 至少有一个有效数值时才执行
    if (!temperatureOk && !humidityOk && !concentrationOk) {
        return;
    }

    const bool thresholdExceeded =
        (temperatureOk && tempValue > m_thresholdT) ||
        (humidityOk && humValue > m_thresholdH) ||
        (concentrationOk && concValue > m_thresholdG);

    // 首次评估时也强制下发一次，确保设备状态与阈值规则一致
    if (!m_thresholdAutoControlInitialized) {
        m_thresholdAutoControlInitialized = true;
        m_thresholdAutoAlarmOn = !thresholdExceeded; // 取反以复用下面的状态切换逻辑
    }

    // 状态无变化时不重复下发命令
    if (thresholdExceeded == m_thresholdAutoAlarmOn) {
        return;
    }

    m_thresholdAutoAlarmOn = thresholdExceeded;

    if (thresholdExceeded) {
        // 任一超限：打开风扇、灯
        sendDevicePropertySet("00001", "打开风扇");
        sendDevicePropertySet("00002", "打开灯");
        ui->statusBar->showMessage("阈值超限联动：已打开风扇、灯", 2500);
    } else {
        // 全部恢复：关闭风扇、灯
        sendDevicePropertySet("01001", "关闭风扇");
        sendDevicePropertySet("01002", "关闭灯");
        ui->statusBar->showMessage("阈值恢复联动：已关闭风扇、灯", 2500);
    }
}

//最新数据查询
void MainWindow::on_queryButton_clicked()
{
    queryLatestData(true);
}

void MainWindow::queryLatestData(bool userInitiated)
{
    // 仅在用户手动触发时切回实时模式、恢复自动刷新并解除“清除锁定”
    if (userInitiated) {
        m_historyModeActive = false;
        m_responseInfoLockedAfterClear = false;
        if (m_autoRefreshTimer && !m_autoRefreshTimer->isActive()) {
            m_autoRefreshTimer->start();
        }
    }

    if (m_queryLatestInProgress || (m_latestQueryReply && m_latestQueryReply->isRunning())) {
        ui->statusBar->showMessage("正在查询最新属性，请稍候...", 2000);
        return;
    }

    QUrl url("http://iot-api.heclouds.com/thingmodel/query-device-property");
    QUrlQuery query;
    query.addQueryItem("product_id", "83bcAx554p");
    query.addQueryItem("device_name", "Linux_my_client_start");
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QString("version=2018-10-31&res=products%2F83bcAx554p&et=1797929077&method=sha1&sign=GWTUG3SkNpqmWc1wsQmDtLJGCqw%3D").toUtf8());
    request.setRawHeader("User-Agent", QString("Apifox/1.0.0").toUtf8());
    request.setRawHeader("Accept", QString("*/*").toUtf8());
    request.setRawHeader("Content-Type", QString("application/json").toUtf8());
    request.setRawHeader("Connection", "close");

    m_queryLatestInProgress = true;
    ui->queryButton->setEnabled(false);
    ui->refreshButton->setEnabled(false);
    m_latestQueryReply = networkManager->get(request);
}



// 绘制图表按钮点击槽函数
void MainWindow::on_drawChartButton_clicked()
{
    // 查看图表后仍保持实时模式，避免实时数据停止更新
    m_historyModeActive = false;
    if (m_autoRefreshTimer && !m_autoRefreshTimer->isActive()) {
        m_autoRefreshTimer->start();
    }

    // 默认从当前时刻开始；仅在“继续绘制”后保留上次会话起点
    if (!m_resumeChartSession || !m_hasChartDrawStartTime) {
        m_chartDrawStartTime = QDateTime::currentDateTime();
        m_hasChartDrawStartTime = true;
    }

    queryLatestData(false);

    qDebug() << "on_drawChartButton_clicked()方法被调用，准备显示图表对话框";
    
    // 检查是否已经存在ChartDialog实例
    if (!m_chartDialog) {
        qDebug() << "创建新的ChartDialog实例";
        // 创建图表对话框
        m_chartDialog = new ChartDialog(this);
        
        // 连接信号和槽
        connect(m_chartDialog, &ChartDialog::requestMoreData, this, &MainWindow::onRequestMoreData);
        connect(m_chartDialog, &ChartDialog::chartUpdated, this, &MainWindow::onChartUpdated);
        connect(m_chartDialog, &ChartDialog::continueDrawingRequested, this, &MainWindow::onContinueDrawingRequested);
        connect(m_chartDialog, &ChartDialog::stopDrawingRequested, this, &MainWindow::onStopDrawingRequested);
    } else {
        qDebug() << "ChartDialog实例已存在，直接更新数据";
    }

    // 显示并聚焦图表窗口
    m_chartDialog->show();
    m_chartDialog->raise();
    m_chartDialog->activateWindow();

    m_chartDialog->startNewDrawingSession();
    
    // 仅绘制从“本次开始时间”之后的数据
    m_chartDialog->setHistoryData(buildChartHistorySinceStart());
    
    // 更新图表数据
    m_chartDialog->updateChart();

    // 进入图表后，除非再次点击“继续绘制”，否则下次默认新会话
    m_resumeChartSession = false;
    qDebug() << "图表对话框已显示并更新";
}

// 响应图表更新的槽函数
void MainWindow::onChartUpdated()
{
    qDebug() << "接收到图表已更新信号";
    ui->statusBar->showMessage("图表数据已更新", 2000);
}

void MainWindow::onContinueDrawingRequested()
{
    // 继续绘制：退出图表界面，但保留本次会话起点，下一次进入继续接着画
    m_resumeChartSession = true;
    ui->statusBar->showMessage("已退出图表，继续绘制模式已保留", 2000);
}

void MainWindow::onStopDrawingRequested()
{
    // 结束绘制：退出图表界面，并结束会话，下一次进入从当前时间重新开始
    m_resumeChartSession = false;
    m_hasChartDrawStartTime = false;
    ui->statusBar->showMessage("已结束绘制，下一次将从当前时间重新绘制", 2500);
}

// 响应请求更多数据的槽函数
void MainWindow::onRequestMoreData(const QDateTime &startTime, const QDateTime &endTime)
{
    qDebug() << "接收到请求更多数据信号，开始时间:" << startTime.toString() << "结束时间:" << endTime.toString();
    // 计算时间戳
    QString startTimeStr = QString::number(startTime.toUTC().toMSecsSinceEpoch());
    QString endTimeStr = QString::number(endTime.toUTC().toMSecsSinceEpoch());
    
    // 设置请求URL和查询参数
    QUrl url("http://iot-api.heclouds.com/thingmodel/query-device-property-history");
    QUrlQuery query;
    query.addQueryItem("product_id", "83bcAx554p");
    query.addQueryItem("device_name", "Linux_my_client_start");
    query.addQueryItem("identifier", "message");
    query.addQueryItem("start_time", startTimeStr);
    query.addQueryItem("end_time", endTimeStr);
    url.setQuery(query);
    
    // 创建请求对象并设置请求头
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QString("version=2018-10-31&res=products%2F83bcAx554p&et=1766194167&method=sha1&sign=jeCE7c%2FVakmItcHkaByyEx8llcI%3D").toUtf8());
    request.setRawHeader("User-Agent", QString("OneNET Client/1.0").toUtf8());
    
    // 发送GET请求
    networkManager->get(request);
    
    ui->statusBar->showMessage("正在加载更多历史数据...", 3000);
}

//网络响应处理函数
void MainWindow::onNetworkReplyReceived(QNetworkReply *reply)
{
    const QString replyPath = reply->request().url().path();
    const bool isLatestQueryReply = (replyPath == "/thingmodel/query-device-property");
    const bool isHistoryQueryReply = (replyPath == "/thingmodel/query-device-property-history");

    auto resetLatestQueryState = [this]() {
        m_queryLatestInProgress = false;
        ui->queryButton->setEnabled(true);
        ui->refreshButton->setEnabled(true);
        m_latestQueryReply = nullptr;
    };

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "请求错误:" << reply->errorString();
        if (isLatestQueryReply || reply == m_latestQueryReply) {
            resetLatestQueryState();
        }
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    qDebug() << "响应数据:" << responseData;
    
    // 将响应时间和数据添加到历史记录
    responseHistory.append(QPair<QDateTime, QByteArray>(QDateTime::currentDateTime(), responseData));
    
    // JSON解析
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if (!jsonDoc.isNull() && jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();
            QString displayText;
            
            // 处理POST响应格式：提取code值并显示结果
            if (jsonObj.contains("code")) {
                int code = jsonObj["code"].toInt();
                
                // 只处理特定的四个设备控制操作
                QStringList validOperations = {"打开风扇", "关闭风扇", "打开灯", "关闭灯"};
                if (validOperations.contains(currentOperation)) {
                    // 获取当前时间
                    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
                    QString statusText;
                    
                    // 显示具体的操作结果
                    if (code == 0) {
                        statusText = currentTime + "：" + currentOperation + "：成功";
                    } else {
                        statusText = currentTime + "：" + currentOperation + "：失败";
                    }
                    
                    // 在现有文本基础上追加新内容，实现记录功能
                    QString existingText = ui->deviceStatusTextEdit->toPlainText();
                    if (!existingText.isEmpty()) {
                        ui->deviceStatusTextEdit->setText(existingText + "\n" + statusText);
                    } else {
                        ui->deviceStatusTextEdit->setText(statusText);
                    }
                }
                
                // 重置当前操作类型，避免影响后续操作
                currentOperation.clear();
            }
        
 // 检查是否是历史数据响应格式 (data.list)
        if (jsonObj.contains("data") && jsonObj["data"].isObject()) {
            QJsonObject dataObj = jsonObj["data"].toObject();
            if (dataObj.contains("list") && dataObj["list"].isArray()) {
                QJsonArray listArray = dataObj["list"].toArray();
                
                // 遍历所有历史数据项
                foreach (const QJsonValue &value, listArray) {
                    if (value.isObject()) {
                        QJsonObject itemObj = value.toObject();
                        
                        // 提取时间戳并转换为中文日期时间格式
                        if (itemObj.contains("time") && itemObj["time"].isDouble()) {
                            qint64 timestamp = static_cast<qint64>(itemObj["time"].toDouble());
                            QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestamp);
                            QString chineseDateTime = dateTime.toString("yyyy年MM月dd日 HH:mm:ss");
                            
                            // 提取温度、湿度和浓度
                            QString temperature = "";
                            QString humidity = "";
                            QString concentration = "";
                            if (itemObj.contains("value") && itemObj["value"].isString()) {
                                QString valueStr = itemObj["value"].toString();
                                QStringList dataList = valueStr.split(",");
                                
                                foreach (const QString &dataItem, dataList) {
                                    QStringList keyValue = dataItem.split(":");
                                    if (keyValue.size() == 2) {
                                        QString key = keyValue[0].trimmed().toLower();
                                        QString val = keyValue[1].trimmed();
                                        
                                        if (key == "t") {
                                            temperature = val;
                                        } else if (key == "h") {
                                            humidity = val;
                                        } else if (key == "g") {
                                            concentration = val;
                                        }
                                    }
                                }
                            }
                            
                            // 添加到显示文本
                            displayText += "时间: " + chineseDateTime + "\n";
                            displayText += "温度: " + temperature + " °C\n";
                            displayText += "湿度: " + humidity + " %\n";
                            displayText += "浓度: " + concentration + "\n";
                            displayText += "设备状态: 正常\n";
                            displayText += "------------------------\n";
                            
                            // 注意：根据需求，历史数据查询时不更新温度和浓度标签
                        }
                    }
                }
            }
        } else if (jsonObj.contains("data") && jsonObj["data"].isArray()) {
            // 保持原有逻辑以兼容实时数据响应
            QJsonArray dataArray = jsonObj["data"].toArray();
            
            bool timeExtracted = false;
            QString chineseDateTime;
            QString temperature;
            QString humidity;
            QString concentration;
            
            foreach (const QJsonValue &value, dataArray) {
                if (value.isObject()) {
                    QJsonObject itemObj = value.toObject();
                    
                    if (!timeExtracted && itemObj.contains("time") && itemObj["time"].isDouble()) {
                        qint64 timestamp = static_cast<qint64>(itemObj["time"].toDouble());
                        QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestamp);
                        chineseDateTime = dateTime.toString("yyyy年MM月dd日 HH:mm:ss");
                        qDebug() << "中文时间:" << chineseDateTime;
                        timeExtracted = true;
                    }
                    
                    if (itemObj.contains("identifier") && itemObj["identifier"].toString() == "message" && 
                        itemObj.contains("value") && itemObj["value"].isString()) {
                        QString messageValue = itemObj["value"].toString();
                        QStringList dataList = messageValue.split(",");
                        
                        foreach (const QString &dataItem, dataList) {
                            QStringList keyValue = dataItem.split(":");
                            if (keyValue.size() == 2) {
                                QString key = keyValue[0].trimmed().toLower();
                                QString val = keyValue[1].trimmed();
                                
                                if (key == "t") {
                                    temperature = val;
                                    qDebug() << "温度:" << value << "°C";
                                } else if (key == "h") {
                                    humidity = val;
                                    qDebug() << "湿度:" << value << "%";
                                } else if (key == "g") {
                                    concentration = val;
                                    qDebug() << "浓度:" << value;
                                }
                            }
                        }
                    }
                }
            }
            
            // 以新格式显示实时数据
            displayText = "时间: " + chineseDateTime + "\n";
            displayText += "温度: " + temperature + " °C\n";
            displayText += "湿度: " + humidity + " %\n";
            displayText += "浓度: " + concentration + "\n";
            displayText += "设备状态: 正常";
            
            // 更新标签
            ui->temperatureLabel->setText(" 温度：" + temperature + " °C");
            ui->humidityLabel->setText(" 湿度：" + humidity + " %");
            ui->concentrationLabel->setText(" 浓度：" + concentration);
            
            // 更新进度条
            bool temperatureOk = false;
            bool humidityOk = false;
            bool concentrationOk = false;
            double tempValue = temperature.toDouble(&temperatureOk);
            double humValue = humidity.toDouble(&humidityOk);
            double concValue = concentration.toDouble(&concentrationOk);
            
            if (temperatureOk) {
                m_latestTemperature = tempValue;
                m_hasLatestTemperature = true;
            }

            if (humidityOk) {
                m_latestHumidity = humValue;
                m_hasLatestHumidity = true;
            }
            
            if (concentrationOk) {
                m_latestConcentration = concValue;
                m_hasLatestConcentration = true;
            }

            applyThresholdToProgressBars();

            // 阈值联动：任一超限即联动风扇、灯
            handleThresholdAutoControl(temperatureOk, tempValue,
                                       humidityOk, humValue,
                                       concentrationOk, concValue);
        }
        
        // 将提取的数据显示到UI控件上
        if (!displayText.isEmpty() && (!isLatestQueryReply || !m_historyModeActive) && !m_responseInfoLockedAfterClear) {
            ui->responseInfoTextEdit->setText(displayText);
        }

        if (isHistoryQueryReply) {
            ui->statusBar->showMessage("历史数据查询完成", 2000);
        }
    }
    
    if (isLatestQueryReply || reply == m_latestQueryReply) {
        resetLatestQueryState();
    }

    // 自动重绘：图表窗口打开时，收到新数据后自动刷新图表
    if (m_chartDialog && m_chartDialog->isVisible()) {
        m_chartDialog->setHistoryData(buildChartHistorySinceStart());
        m_chartDialog->updateChart();
    }

    reply->deleteLater();
}


//历史数据请求函数
void MainWindow::on_queryHistoryButton_clicked()
{
    // 进入历史查询模式，暂停自动刷新，避免实时结果覆盖历史显示
    m_historyModeActive = true;
    m_responseInfoLockedAfterClear = false;
    if (m_autoRefreshTimer && m_autoRefreshTimer->isActive()) {
        m_autoRefreshTimer->stop();
    }

    // 清空响应文本框
    ui->responseInfoTextEdit->clear();
    
    // 更新状态为正在查询历史数据
    qDebug() << "正在查询历史数据"; // 简单的状态更新
    
    // 计算时间戳
    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime threeDaysAgo = currentTime.addDays(-3);
    QString startTime = QString::number(threeDaysAgo.toUTC().toMSecsSinceEpoch());
    QString endTime = QString::number(currentTime.toUTC().toMSecsSinceEpoch());
    
    // 设置请求URL和查询参数
    QUrl url("http://iot-api.heclouds.com/thingmodel/query-device-property-history");
    QUrlQuery query;
    query.addQueryItem("product_id", "83bcAx554p");
    query.addQueryItem("device_name", "Linux_my_client_start");
    query.addQueryItem("identifier", "message");
    query.addQueryItem("start_time", startTime);
    query.addQueryItem("end_time", endTime);
    url.setQuery(query);
    
    // 创建请求对象并设置请求头
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QString("version=2018-10-31&res=products%2F83bcAx554p&et=1797929077&method=sha1&sign=GWTUG3SkNpqmWc1wsQmDtLJGCqw%3D").toUtf8());
    request.setRawHeader("User-Agent", QString("OneNET Client/1.0").toUtf8());
    
    // 发送GET请求
    networkManager->get(request);
}

//清除按钮
void MainWindow::on_clearButton_clicked()
{
    // 清空响应文本框
    ui->responseInfoTextEdit->clear();
    m_responseInfoLockedAfterClear = true;

    // 清空设备状态记录
    ui->deviceStatusTextEdit->clear();

    // 清除后恢复到实时模式，避免历史模式下自动刷新被暂停
    m_historyModeActive = false;
    if (m_autoRefreshTimer && !m_autoRefreshTimer->isActive()) {
        m_autoRefreshTimer->start();
    }

    ui->statusBar->showMessage("已清除显示内容", 1500);
}


//刷新按钮
void MainWindow::on_refreshButton_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("阈值设置");
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QFormLayout *layout = new QFormLayout(&dialog);

    QSpinBox *tSpin = new QSpinBox(&dialog);
    tSpin->setRange(0, 100);
    tSpin->setValue(m_thresholdT);

    QSpinBox *hSpin = new QSpinBox(&dialog);
    hSpin->setRange(20, 100);
    hSpin->setValue(qBound(20, m_thresholdH, 100));

    QSpinBox *gSpin = new QSpinBox(&dialog);
    gSpin->setRange(0, 5000);
    gSpin->setValue(m_thresholdG);

    layout->addRow("T:", tSpin);
    layout->addRow("H:", hSpin);
    layout->addRow("G:", gSpin);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "确认重启",
            "继续后当前程序会自动重启，确定继续吗？",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::Yes
        );

        if (reply != QMessageBox::Yes) {
            return;
        }

        m_thresholdT = tSpin->value();
        m_thresholdH = hSpin->value();
        m_thresholdG = gSpin->value();

        // 阈值更新后立刻按当前最新值重判一次，避免“已不超限但设备仍在运行”
        handleThresholdAutoControl(m_hasLatestTemperature, m_latestTemperature,
                       m_hasLatestHumidity, m_latestHumidity,
                       m_hasLatestConcentration, m_latestConcentration);

        QSettings settings("myendchart", "thresholds");
        settings.setValue("T", m_thresholdT);
        settings.setValue("H", m_thresholdH);
        settings.setValue("G", m_thresholdG);
        settings.sync();

        const QString program = QCoreApplication::applicationFilePath();
        const QStringList arguments = QCoreApplication::arguments();

        if (QProcess::startDetached(program, arguments)) {
            QCoreApplication::quit();
            return;
        }

        ui->statusBar->showMessage(
            QString("阈值已保存，但自动重启失败，请手动重启程序生效：T=%1, H=%2, G=%3")
                .arg(m_thresholdT)
                .arg(m_thresholdH)
                .arg(m_thresholdG),
            4000
        );
    }
}

//打开风扇按钮
void MainWindow::on_openFanButton_clicked()
{
    // 发送打开风扇的命令
       sendDevicePropertySet("00001", "打开风扇");
}

//关闭风扇按钮
void MainWindow::on_closeFanButton_clicked()
{
    //发送关闭风扇的命令
       sendDevicePropertySet("01001", "关闭风扇");
}

//打开灯按钮
void MainWindow::on_openLightButton_clicked()
{
    //发送打开灯的命令
       sendDevicePropertySet("00002", "打开灯");
}

//关闭灯按钮
void MainWindow::on_closeLightButton_clicked()
{
    //发送关闭灯的命令
       sendDevicePropertySet("01002", "关闭灯");
}

//导出历史记录格式
void MainWindow::on_exportButton_clicked()
{
    // 设置默认文件名和路径
    QString defaultFileName = "united.js";
    
    // 检查responseHistory是否为空
    if (responseHistory.isEmpty()) {
        // 显示警告并请求用户确认是否继续保存
        QMessageBox::StandardButton reply = QMessageBox::warning(
            this,
            tr("警告"),
            tr("无效的数据，需要继续保存吗？"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No  // 默认选择No
        );
        
        // 如果用户选择不继续保存，则直接返回
        if (reply != QMessageBox::Yes) {
            return;
        }
    }
    
    // 打开文件对话框，让用户选择保存位置
    QString filePath = QFileDialog::getSaveFileName(
        this,                       // 父窗口
        tr("保存历史记录"),         // 对话框标题
        QDir::homePath() + "/" + defaultFileName,  // 默认路径和文件名
        tr("JavaScript文件 (*.js);;所有文件 (*.*)")  // 文件类型过滤器
    );
    
    // 如果用户取消了对话框，则不执行后续操作
    if (filePath.isEmpty()) {
        return;
    }
    
    // 将原始响应数据转换为JSON格式
    QJsonArray historyArray;
    
    for (int i = 0; i < responseHistory.size(); ++i) {
        const QPair<QDateTime, QByteArray> &record = responseHistory.at(i);
        
        QJsonObject historyItem;
        historyItem["timestamp"] = record.first.toString("yyyy-MM-dd HH:mm:ss");
        historyItem["raw_data"] = QString::fromUtf8(record.second);
        
        // 尝试解析JSON，如果是有效的JSON，则添加解析后的对象
        QJsonDocument jsonDoc = QJsonDocument::fromJson(record.second);
        if (!jsonDoc.isNull()) {
            if (jsonDoc.isObject()) {
                historyItem["parsed_json"] = jsonDoc.object();
            } else if (jsonDoc.isArray()) {
                historyItem["parsed_json"] = jsonDoc.array();
            }
        }
        
        historyArray.append(historyItem);
    }
    
    // 创建包含所有历史记录的JSON对象
    QJsonObject rootObject;
    rootObject["export_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    rootObject["total_records"] = responseHistory.size();
    rootObject["raw_response_history"] = historyArray;
    
    // 将JSON对象转换为字符串
    QJsonDocument jsonDoc(rootObject);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Indented);  // 使用缩进格式使文件更易读
    
    // 保存到文件
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonData);
        file.close();
        
        // 显示保存成功的消息
        QMessageBox::information(this, tr("成功"), tr("历史记录已成功导出到文件：") + filePath);
    } else {
        // 显示保存失败的消息
        QMessageBox::critical(this, tr("错误"), tr("无法保存文件：") + file.errorString());
    }
}
