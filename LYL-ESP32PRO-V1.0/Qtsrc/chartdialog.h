#ifndef CHARTDIALOG_H
#define CHARTDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QDateTimeAxis>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QJsonObject>
#include <QFileDialog>

// 使用Qt Charts命名空间
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class ChartDialog;
}

class ChartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChartDialog(QWidget *parent = nullptr);
    ~ChartDialog();
    
    // 设置历史数据
    void setHistoryData(const QList<QPair<QDateTime, QByteArray>>& historyData);
    
    // 更新图表
    void updateChart();

    // 开启新的绘制会话（恢复自动绘制）
    void startNewDrawingSession();

    // 是否已结束当前绘制
    bool isDrawingStopped() const;
    
signals:
    // 添加信号，用于在需要更多数据时通知主窗口
    void requestMoreData(const QDateTime& startTime, const QDateTime& endTime);
    
    // 添加信号，通知主窗口图表已更新
    void chartUpdated();
    void continueDrawingRequested();
    void stopDrawingRequested();

public slots:
    // 准备图表数据
    void prepareChartData();
    void prepareChartData(const QJsonObject &jsonObject);
    void adjustPrecision();
    void continueCurrentDrawing();
    void stopCurrentDrawing();
    void toggleTempSeries(bool visible);
    void toggleHumiditySeries(bool visible);
    void toggleConcentrationSeries(bool visible);
    void exportChart();

private:
    Ui::ChartDialog *ui;                    // UI对象指针
    QList<QPair<QDateTime, QByteArray>> m_historyData;  // 存储历史数据
    QChart *m_chart;                        // 图表对象
    QLineSeries *m_tempSeries;              // 温度数据系列
    QLineSeries *m_humiditySeries;          // 湿度数据系列
    QLineSeries *m_concentrationSeries;     // 浓度数据系列
    QChartView *m_chartView;                // 图表视图指针
    QValueAxis *m_axisX;                    // X轴
    QValueAxis *m_axisY;                    // Y轴
    int m_yTickStepPercent;                 // Y轴刻度步长（百分比）
    bool m_drawingStopped;                  // 是否已结束当前绘制
    
    // 初始化图表
    void initChart();
    void applyYAxisTickStep(int stepPercent);

protected:
    // 重写事件过滤器方法
    bool eventFilter(QObject *watched, QEvent *event) override;
    
    // 重写关闭事件处理方法
    void closeEvent(QCloseEvent *event) override;
    
private slots:
    // 添加槽函数，用于接收并处理新数据
    void handleNewData(const QList<QPair<QDateTime, QByteArray>>& newData);
    
    // 图表区域点击事件处理
    void onChartClicked(const QPointF &point);
    
    // 更新统计信息
    void updateStats(int dataCount, qint64 startTime, qint64 endTime);
};

#endif // CHARTDIALOG_H