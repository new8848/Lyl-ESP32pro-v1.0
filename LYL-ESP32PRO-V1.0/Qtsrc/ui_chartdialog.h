/********************************************************************************
** Form generated from reading UI file 'chartdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHARTDIALOG_H
#define UI_CHARTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <qchartview.h>

QT_BEGIN_NAMESPACE

class Ui_ChartDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *controlPanel;
    QHBoxLayout *horizontalLayout;
    QPushButton *adjustPrecisionButton;
    QSpacerItem *horizontalSpacer;
    QCheckBox *showTempCheckBox;
    QCheckBox *showHumidityCheckBox;
    QCheckBox *showConcentrationCheckBox;
    QGroupBox *chartContainer;
    QVBoxLayout *verticalLayout_2;
    QChartView *chartView;
    QGroupBox *infoPanel;
    QHBoxLayout *horizontalLayout_2;
    QLabel *dataCountLabel;
    QLabel *timeRangeLabel;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *continueDrawingButton;
    QPushButton *stopDrawingButton;
    QPushButton *exportChartButton;

    void setupUi(QDialog *ChartDialog)
    {
        if (ChartDialog->objectName().isEmpty())
            ChartDialog->setObjectName(QString::fromUtf8("ChartDialog"));
        ChartDialog->resize(900, 700);
        QIcon icon;
        icon.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        ChartDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(ChartDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        controlPanel = new QGroupBox(ChartDialog);
        controlPanel->setObjectName(QString::fromUtf8("controlPanel"));
        horizontalLayout = new QHBoxLayout(controlPanel);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        adjustPrecisionButton = new QPushButton(controlPanel);
        adjustPrecisionButton->setObjectName(QString::fromUtf8("adjustPrecisionButton"));
        adjustPrecisionButton->setIcon(icon);

        horizontalLayout->addWidget(adjustPrecisionButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        showTempCheckBox = new QCheckBox(controlPanel);
        showTempCheckBox->setObjectName(QString::fromUtf8("showTempCheckBox"));
        showTempCheckBox->setChecked(true);

        horizontalLayout->addWidget(showTempCheckBox);

        showHumidityCheckBox = new QCheckBox(controlPanel);
        showHumidityCheckBox->setObjectName(QString::fromUtf8("showHumidityCheckBox"));
        showHumidityCheckBox->setChecked(true);

        horizontalLayout->addWidget(showHumidityCheckBox);

        showConcentrationCheckBox = new QCheckBox(controlPanel);
        showConcentrationCheckBox->setObjectName(QString::fromUtf8("showConcentrationCheckBox"));
        showConcentrationCheckBox->setChecked(true);

        horizontalLayout->addWidget(showConcentrationCheckBox);


        verticalLayout->addWidget(controlPanel);

        chartContainer = new QGroupBox(ChartDialog);
        chartContainer->setObjectName(QString::fromUtf8("chartContainer"));
        verticalLayout_2 = new QVBoxLayout(chartContainer);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        chartView = new QChartView(chartContainer);
        chartView->setObjectName(QString::fromUtf8("chartView"));
        chartView->setMinimumSize(QSize(0, 400));
        chartView->setRenderHint(QPainter::Antialiasing);

        verticalLayout_2->addWidget(chartView);


        verticalLayout->addWidget(chartContainer);

        infoPanel = new QGroupBox(ChartDialog);
        infoPanel->setObjectName(QString::fromUtf8("infoPanel"));
        horizontalLayout_2 = new QHBoxLayout(infoPanel);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        dataCountLabel = new QLabel(infoPanel);
        dataCountLabel->setObjectName(QString::fromUtf8("dataCountLabel"));

        horizontalLayout_2->addWidget(dataCountLabel);

        timeRangeLabel = new QLabel(infoPanel);
        timeRangeLabel->setObjectName(QString::fromUtf8("timeRangeLabel"));

        horizontalLayout_2->addWidget(timeRangeLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        continueDrawingButton = new QPushButton(infoPanel);
        continueDrawingButton->setObjectName(QString::fromUtf8("continueDrawingButton"));

        horizontalLayout_2->addWidget(continueDrawingButton);

        stopDrawingButton = new QPushButton(infoPanel);
        stopDrawingButton->setObjectName(QString::fromUtf8("stopDrawingButton"));

        horizontalLayout_2->addWidget(stopDrawingButton);

        exportChartButton = new QPushButton(infoPanel);
        exportChartButton->setObjectName(QString::fromUtf8("exportChartButton"));

        horizontalLayout_2->addWidget(exportChartButton);


        verticalLayout->addWidget(infoPanel);

        verticalLayout->setStretch(1, 1);

        retranslateUi(ChartDialog);

        QMetaObject::connectSlotsByName(ChartDialog);
    } // setupUi

    void retranslateUi(QDialog *ChartDialog)
    {
        ChartDialog->setWindowTitle(QApplication::translate("ChartDialog", "\346\225\260\346\215\256\347\233\221\346\216\247\345\233\276\350\241\250", nullptr));
        controlPanel->setTitle(QApplication::translate("ChartDialog", "\345\233\276\350\241\250\346\216\247\345\210\266", nullptr));
        adjustPrecisionButton->setText(QApplication::translate("ChartDialog", "\350\260\203\346\225\264", nullptr));
        showTempCheckBox->setText(QApplication::translate("ChartDialog", "\346\230\276\347\244\272\346\270\251\345\272\246", nullptr));
        showHumidityCheckBox->setText(QApplication::translate("ChartDialog", "\346\230\276\347\244\272\346\271\277\345\272\246", nullptr));
        showConcentrationCheckBox->setText(QApplication::translate("ChartDialog", "\346\230\276\347\244\272\346\265\223\345\272\246", nullptr));
        chartContainer->setTitle(QString());
        infoPanel->setTitle(QApplication::translate("ChartDialog", "\346\225\260\346\215\256\347\273\237\350\256\241", nullptr));
        dataCountLabel->setText(QApplication::translate("ChartDialog", "\346\225\260\346\215\256\347\202\271\346\225\260\351\207\217: 0", nullptr));
        timeRangeLabel->setText(QApplication::translate("ChartDialog", "\346\227\266\351\227\264\350\214\203\345\233\264: --", nullptr));
        continueDrawingButton->setText(QApplication::translate("ChartDialog", "\347\273\247\347\273\255\347\273\230\345\210\266", nullptr));
        stopDrawingButton->setText(QApplication::translate("ChartDialog", "\347\273\223\346\235\237\347\273\230\345\210\266", nullptr));
        exportChartButton->setText(QApplication::translate("ChartDialog", "\345\257\274\345\207\272\345\233\276\350\241\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChartDialog: public Ui_ChartDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTDIALOG_H
