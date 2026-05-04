#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QByteArray>
#include <QNetworkReply>
#include <QMessageBox>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QPointer>
#include <QTimer>

// 包含图表对话框头文件
#include "chartdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //最新数据查询按钮
    void on_queryButton_clicked();

    //历史查询按钮
    void on_queryHistoryButton_clicked();

    //清除按钮
    void on_clearButton_clicked();

    //刷新按钮
    void on_refreshButton_clicked();

    //打开风扇按钮
    void on_openFanButton_clicked();

    //关闭风扇按钮
    void on_closeFanButton_clicked();

    //打开灯按钮
    void on_openLightButton_clicked();

    //关闭灯按钮
    void on_closeLightButton_clicked();
    
    //导出历史记录按钮
    void on_exportButton_clicked();
    
    //绘制图表按钮
    void on_drawChartButton_clicked();
    
    // 新增：处理ChartDialog发送的信号的槽函数
    void onRequestMoreData(const QDateTime& startTime, const QDateTime& endTime);
    void onChartUpdated();
    void onContinueDrawingRequested();
    void onStopDrawingRequested();

private:
    // 网络响应处理函数
    void onNetworkReplyReceived(QNetworkReply *reply);

    // 查询最新数据（userInitiated=true 表示用户手动触发）
    void queryLatestData(bool userInitiated);

    // 按阈值换算百分比（0~100）
    int toPercentByThreshold(double value, int threshold) const;

    // 按当前阈值刷新实时进度条
    void applyThresholdToProgressBars();

    // 获取从图表起始时刻开始的数据
    QList<QPair<QDateTime, QByteArray>> buildChartHistorySinceStart() const;

    // 阈值超限联动（任一超限时开风扇+开灯，恢复后关闭）
    void handleThresholdAutoControl(bool temperatureOk, double tempValue,
                                    bool humidityOk, double humValue,
                                    bool concentrationOk, double concValue);

    // 发送设备属性设置的POST请求
    void sendDevicePropertySet(const QString &command, const QString &operationType = "");
    private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    QString currentOperation; // 存储当前操作类型
    ChartDialog *m_chartDialog; // 图表对话框实例，避免重复创建
    bool m_queryLatestInProgress; // 防止“查询最新属性”重复触发
    QPointer<QNetworkReply> m_latestQueryReply; // 跟踪最新属性查询请求
    QTimer *m_autoRefreshTimer; // 实时数据自动刷新定时器（5秒）
    bool m_historyModeActive; // 历史查询模式，避免实时刷新覆盖历史结果
    bool m_responseInfoLockedAfterClear; // 清除后锁定响应信息显示，直到用户手动查询
    int m_thresholdT; // T阈值（0~100）
    int m_thresholdH; // H阈值（20~100）
    int m_thresholdG; // G阈值（0~5000）
    bool m_hasLatestTemperature;
    bool m_hasLatestHumidity;
    bool m_hasLatestConcentration;
    double m_latestTemperature;
    double m_latestHumidity;
    double m_latestConcentration;
    bool m_thresholdAutoAlarmOn;
    bool m_thresholdAutoControlInitialized;
    bool m_hasChartDrawStartTime;
    QDateTime m_chartDrawStartTime;
    bool m_resumeChartSession;
    
    // 存储原始响应数据的列表，保存响应时间和原始数据
    QList<QPair<QDateTime, QByteArray>> responseHistory;
};

#endif // MAINWINDOW_H