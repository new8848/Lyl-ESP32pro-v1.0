/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *leftLayout;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_8;
    QTextEdit *responseInfoTextEdit;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_4;
    QLineEdit *productIdLineEdit;
    QLineEdit *deviceNameLineEdit;
    QVBoxLayout *centerLayout;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_5;
    QLabel *temperatureLabel;
    QProgressBar *temperatureProgressBar;
    QGroupBox *groupBox_8;
    QVBoxLayout *verticalLayout_9;
    QLabel *humidityLabel;
    QProgressBar *humidityProgressBar;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_6;
    QLabel *concentrationLabel;
    QProgressBar *concentrationProgressBar;
    QGroupBox *groupBox_7;
    QVBoxLayout *verticalLayout_7;
    QPushButton *queryButton;
    QPushButton *queryHistoryButton;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *exportButton;
    QPushButton *refreshButton;
    QPushButton *clearButton;
    QVBoxLayout *rightLayout;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout;
    QTextEdit *deviceStatusTextEdit;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *openFanButton;
    QPushButton *closeFanButton;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *openLightButton;
    QPushButton *closeLightButton;
    QPushButton *drawChartButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1440, 820);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(16);
        horizontalLayout->setContentsMargins(10, 10, 10, 10);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        leftLayout = new QVBoxLayout();
        leftLayout->setSpacing(16);
        leftLayout->setObjectName(QString::fromUtf8("leftLayout"));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(360, 280));
        verticalLayout_8 = new QVBoxLayout(groupBox_2);
        verticalLayout_8->setSpacing(8);
        verticalLayout_8->setContentsMargins(10, 10, 10, 10);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        responseInfoTextEdit = new QTextEdit(groupBox_2);
        responseInfoTextEdit->setObjectName(QString::fromUtf8("responseInfoTextEdit"));
        responseInfoTextEdit->setMinimumSize(QSize(340, 220));
        responseInfoTextEdit->setReadOnly(true);

        verticalLayout_8->addWidget(responseInfoTextEdit);


        leftLayout->addWidget(groupBox_2);

        groupBox_4 = new QGroupBox(centralWidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setMinimumSize(QSize(350, 260));
        verticalLayout_4 = new QVBoxLayout(groupBox_4);
        verticalLayout_4->setSpacing(8);
        verticalLayout_4->setContentsMargins(10, 10, 10, 10);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        productIdLineEdit = new QLineEdit(groupBox_4);
        productIdLineEdit->setObjectName(QString::fromUtf8("productIdLineEdit"));
        productIdLineEdit->setReadOnly(true);

        verticalLayout_4->addWidget(productIdLineEdit);

        deviceNameLineEdit = new QLineEdit(groupBox_4);
        deviceNameLineEdit->setObjectName(QString::fromUtf8("deviceNameLineEdit"));
        deviceNameLineEdit->setReadOnly(true);

        verticalLayout_4->addWidget(deviceNameLineEdit);


        leftLayout->addWidget(groupBox_4);


        horizontalLayout->addLayout(leftLayout);

        centerLayout = new QVBoxLayout();
        centerLayout->setSpacing(16);
        centerLayout->setObjectName(QString::fromUtf8("centerLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(16);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        groupBox_5 = new QGroupBox(centralWidget);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        verticalLayout_5 = new QVBoxLayout(groupBox_5);
        verticalLayout_5->setSpacing(8);
        verticalLayout_5->setContentsMargins(10, 10, 10, 10);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        temperatureLabel = new QLabel(groupBox_5);
        temperatureLabel->setObjectName(QString::fromUtf8("temperatureLabel"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        temperatureLabel->setFont(font);
        temperatureLabel->setAlignment(Qt::AlignCenter);
        temperatureLabel->setMinimumSize(QSize(200, 100));

        verticalLayout_5->addWidget(temperatureLabel);

        temperatureProgressBar = new QProgressBar(groupBox_5);
        temperatureProgressBar->setObjectName(QString::fromUtf8("temperatureProgressBar"));
        temperatureProgressBar->setMinimum(0);
        temperatureProgressBar->setMaximum(100);
        temperatureProgressBar->setValue(25);
        temperatureProgressBar->setTextVisible(true);
        temperatureProgressBar->setOrientation(Qt::Horizontal);

        verticalLayout_5->addWidget(temperatureProgressBar);


        horizontalLayout_2->addWidget(groupBox_5);

        groupBox_8 = new QGroupBox(centralWidget);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        verticalLayout_9 = new QVBoxLayout(groupBox_8);
        verticalLayout_9->setSpacing(8);
        verticalLayout_9->setContentsMargins(10, 10, 10, 10);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        humidityLabel = new QLabel(groupBox_8);
        humidityLabel->setObjectName(QString::fromUtf8("humidityLabel"));
        humidityLabel->setFont(font);
        humidityLabel->setAlignment(Qt::AlignCenter);
        humidityLabel->setMinimumSize(QSize(200, 100));

        verticalLayout_9->addWidget(humidityLabel);

        humidityProgressBar = new QProgressBar(groupBox_8);
        humidityProgressBar->setObjectName(QString::fromUtf8("humidityProgressBar"));
        humidityProgressBar->setMinimum(0);
        humidityProgressBar->setMaximum(100);
        humidityProgressBar->setValue(40);
        humidityProgressBar->setTextVisible(true);
        humidityProgressBar->setOrientation(Qt::Horizontal);

        verticalLayout_9->addWidget(humidityProgressBar);


        horizontalLayout_2->addWidget(groupBox_8);

        groupBox_6 = new QGroupBox(centralWidget);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        verticalLayout_6 = new QVBoxLayout(groupBox_6);
        verticalLayout_6->setSpacing(8);
        verticalLayout_6->setContentsMargins(10, 10, 10, 10);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        concentrationLabel = new QLabel(groupBox_6);
        concentrationLabel->setObjectName(QString::fromUtf8("concentrationLabel"));
        concentrationLabel->setFont(font);
        concentrationLabel->setAlignment(Qt::AlignCenter);
        concentrationLabel->setMinimumSize(QSize(200, 100));

        verticalLayout_6->addWidget(concentrationLabel);

        concentrationProgressBar = new QProgressBar(groupBox_6);
        concentrationProgressBar->setObjectName(QString::fromUtf8("concentrationProgressBar"));
        concentrationProgressBar->setMinimum(0);
        concentrationProgressBar->setMaximum(100);
        concentrationProgressBar->setValue(50);
        concentrationProgressBar->setTextVisible(true);
        concentrationProgressBar->setOrientation(Qt::Horizontal);

        verticalLayout_6->addWidget(concentrationProgressBar);


        horizontalLayout_2->addWidget(groupBox_6);


        centerLayout->addLayout(horizontalLayout_2);

        groupBox_7 = new QGroupBox(centralWidget);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        verticalLayout_7 = new QVBoxLayout(groupBox_7);
        verticalLayout_7->setSpacing(8);
        verticalLayout_7->setContentsMargins(10, 10, 10, 10);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        queryButton = new QPushButton(groupBox_7);
        queryButton->setObjectName(QString::fromUtf8("queryButton"));
        queryButton->setMinimumSize(QSize(0, 40));

        verticalLayout_7->addWidget(queryButton);

        queryHistoryButton = new QPushButton(groupBox_7);
        queryHistoryButton->setObjectName(QString::fromUtf8("queryHistoryButton"));
        queryHistoryButton->setMinimumSize(QSize(0, 40));

        verticalLayout_7->addWidget(queryHistoryButton);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(16);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        exportButton = new QPushButton(groupBox_7);
        exportButton->setObjectName(QString::fromUtf8("exportButton"));
        exportButton->setMinimumSize(QSize(0, 35));

        horizontalLayout_3->addWidget(exportButton);

        refreshButton = new QPushButton(groupBox_7);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setMinimumSize(QSize(100, 40));

        horizontalLayout_3->addWidget(refreshButton);

        clearButton = new QPushButton(groupBox_7);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));
        clearButton->setMinimumSize(QSize(100, 40));

        horizontalLayout_3->addWidget(clearButton);


        verticalLayout_7->addLayout(horizontalLayout_3);


        centerLayout->addWidget(groupBox_7);


        horizontalLayout->addLayout(centerLayout);

        rightLayout = new QVBoxLayout();
        rightLayout->setSpacing(16);
        rightLayout->setObjectName(QString::fromUtf8("rightLayout"));
        groupBox_3 = new QGroupBox(centralWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout = new QVBoxLayout(groupBox_3);
        verticalLayout->setSpacing(16);
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        deviceStatusTextEdit = new QTextEdit(groupBox_3);
        deviceStatusTextEdit->setObjectName(QString::fromUtf8("deviceStatusTextEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(deviceStatusTextEdit->sizePolicy().hasHeightForWidth());
        deviceStatusTextEdit->setSizePolicy(sizePolicy);
        deviceStatusTextEdit->setMinimumSize(QSize(340, 220));
        deviceStatusTextEdit->setReadOnly(true);

        verticalLayout->addWidget(deviceStatusTextEdit);

        groupBox = new QGroupBox(groupBox_3);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setSpacing(8);
        verticalLayout_2->setContentsMargins(10, 10, 10, 10);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(12);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        openFanButton = new QPushButton(groupBox);
        openFanButton->setObjectName(QString::fromUtf8("openFanButton"));
        openFanButton->setMinimumSize(QSize(150, 40));

        horizontalLayout_6->addWidget(openFanButton);

        closeFanButton = new QPushButton(groupBox);
        closeFanButton->setObjectName(QString::fromUtf8("closeFanButton"));
        closeFanButton->setMinimumSize(QSize(150, 40));

        horizontalLayout_6->addWidget(closeFanButton);


        verticalLayout_2->addLayout(horizontalLayout_6);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(16);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        openLightButton = new QPushButton(groupBox);
        openLightButton->setObjectName(QString::fromUtf8("openLightButton"));
        openLightButton->setMinimumSize(QSize(150, 40));

        horizontalLayout_4->addWidget(openLightButton);

        closeLightButton = new QPushButton(groupBox);
        closeLightButton->setObjectName(QString::fromUtf8("closeLightButton"));
        closeLightButton->setMinimumSize(QSize(150, 40));

        horizontalLayout_4->addWidget(closeLightButton);


        verticalLayout_2->addLayout(horizontalLayout_4);

        drawChartButton = new QPushButton(groupBox);
        drawChartButton->setObjectName(QString::fromUtf8("drawChartButton"));
        drawChartButton->setMinimumSize(QSize(0, 40));

        verticalLayout_2->addWidget(drawChartButton);


        verticalLayout->addWidget(groupBox);


        rightLayout->addWidget(groupBox_3);


        horizontalLayout->addLayout(rightLayout);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1280, 26));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "OneNET\350\256\276\345\244\207\345\261\236\346\200\247\346\237\245\350\257\242", nullptr));
        centralWidget->setStyleSheet(QApplication::translate("MainWindow", "QWidget#centralWidget { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1e1e1e, stop:1 #2d2d2d); }\n"
"\n"
"/* \351\200\232\347\224\250GroupBox\346\240\267\345\274\217 */\n"
"QGroupBox { \n"
"    border: 1px solid #404040; \n"
"    border-radius: 8px; \n"
"    margin-top: 15px; \n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2a2a2a, stop:1 #353535);\n"
"    border: 1px solid qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3a3a3a, stop:1 #454545);\n"
"}\n"
"\n"
"QGroupBox::title { \n"
"    subcontrol-origin: margin; \n"
"    left: 10px; \n"
"    padding: 0 8px 0 8px; \n"
"    background-color: #333333; \n"
"    color: #ffffff; \n"
"    font-weight: bold;\n"
"    font-size: 13px;\n"
"    border-radius: 4px;\n"
"    border: 1px solid #606060;\n"
"}\n"
"\n"
"/* \345\265\214\345\245\227GroupBox\346\240\267\345\274\217 */\n"
"QGroupBox QGroupBox {\n"
"    border: 1px solid #454545;\n"
"    border-radius: 6px;\n"
"    margin-top: 12px;\n"
"    background-color: qlineargrad"
                        "ient(x1:0, y1:0, x2:0, y2:1, stop:0 #252525, stop:1 #303030);\n"
"    border: 1px solid qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #303030, stop:1 #3a3a3a);\n"
"}\n"
"\n"
"QGroupBox QGroupBox::title {\n"
"    font-size: 12px;\n"
"    color: #ffffff;\n"
"    padding: 0 6px 0 6px;\n"
"    background-color: #3a3a3a;\n"
"}\n"
"\n"
"/* \346\214\211\351\222\256\346\240\267\345\274\217 */\n"
"QPushButton {\n"
"    border-radius: 6px;\n"
"    padding: 8px 16px;\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"    color: white;\n"
"    border: none;\n"
"    background-color: #0078d7;\n"
"    outline: none;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #1089ff;\n"
"    border: 1px solid rgba(0, 120, 215, 0.5);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #0066c0;\n"
"    border: 1px solid rgba(0, 120, 215, 0.3);\n"
"}\n"
"\n"
"/* \346\214\211\351\222\256\347\211\271\345\256\232\351\242\234\350\211\262 */\n"
"QPushButton#queryButton { background-color: #0078d7; }\n"
"QPushButton"
                        "#queryButton:hover { background-color: #1089ff; border: 1px solid rgba(0, 120, 215, 0.5); }\n"
"QPushButton#queryHistoryButton { background-color: #107c10; }\n"
"QPushButton#queryHistoryButton:hover { background-color: #148c14; border: 1px solid rgba(16, 124, 16, 0.5); }\n"
"QPushButton#exportButton { background-color: #005a9e; }\n"
"QPushButton#exportButton:hover { background-color: #0066b3; border: 1px solid rgba(0, 90, 158, 0.5); }\n"
"QPushButton#refreshButton { background-color: #ffb900; color: #333333; }\n"
"QPushButton#refreshButton:hover { background-color: #ffc600; border: 1px solid rgba(255, 185, 0, 0.5); }\n"
"QPushButton#clearButton { background-color: #d13438; }\n"
"QPushButton#clearButton:hover { background-color: #e24549; border: 1px solid rgba(209, 52, 56, 0.5); }\n"
"QPushButton#openFanButton { background-color: #1a9c1a; color: #ffffff; }\n"
"QPushButton#openFanButton:hover { background-color: #1eaf1e; border: 1px solid rgba(26, 156, 26, 0.5); }\n"
"QPushButton#closeFanButton { background-colo"
                        "r: #e14448; color: #ffffff; }\n"
"QPushButton#closeFanButton:hover { background-color: #f25559; border: 1px solid rgba(225, 68, 72, 0.5); }\n"
"QPushButton#openLightButton { background-color: #ffc900; color: #ffffff; }\n"
"QPushButton#openLightButton:hover { background-color: #ffda00; border: 1px solid rgba(255, 201, 0, 0.5); }\n"
"QPushButton#closeLightButton { background-color: #777777; color: #ffffff; }\n"
"QPushButton#closeLightButton:hover { background-color: #888888; border: 1px solid rgba(119, 119, 119, 0.5); }\n"
"\n"
"/* \346\226\207\346\234\254\346\241\206\346\240\267\345\274\217 */\n"
"QLineEdit, QTextEdit {\n"
"    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3a3a3a, stop:1 #404040);\n"
"    border: 1px solid #505050;\n"
"    border-radius: 6px;\n"
"    padding: 8px 10px;\n"
"    color: #e0e0e0;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QLineEdit:focus, QTextEdit:focus {\n"
"    border-color: #0096ff;\n"
"    background-color: #454545;\n"
"    border-width: 2px;\n"
"    border-st"
                        "yle: outset;\n"
"}\n"
"\n"
"/* \346\240\207\347\255\276\346\240\267\345\274\217 */\n"
"QLabel {\n"
"    color: #cccccc;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QLabel#temperatureLabel {\n"
"    color: #33adff;\n"
"    font-size: 24px;\n"
"    font-weight: bold;\n"
"    background-color: rgba(51, 173, 255, 0.1);\n"
"    border: 1px solid rgba(51, 173, 255, 0.3);\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"QLabel#humidityLabel {\n"
"    color: #4cd964;\n"
"    font-size: 24px;\n"
"    font-weight: bold;\n"
"    background-color: rgba(76, 217, 100, 0.1);\n"
"    border: 1px solid rgba(76, 217, 100, 0.3);\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"QLabel#concentrationLabel {\n"
"    color: #ff8555;\n"
"    font-size: 24px;\n"
"    font-weight: bold;\n"
"    background-color: rgba(255, 133, 85, 0.1);\n"
"    border: 1px solid rgba(255, 133, 85, 0.3);\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"/* \346\273\221\345\235\227\346\240\267\345\274\217 */\n"
"QSlider::groove:horizontal {\n"
"    border: 1px solid #404040;\n"
""
                        "    height: 8px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #404040, stop:1 #505050);\n"
"    margin: 2px 0;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QSlider::handle:horizontal {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1a88ff, stop:1 #0066cc);\n"
"    border: 1px solid #0055aa;\n"
"    width: 20px;\n"
"    margin: -6px 0;\n"
"    border-radius: 10px;\n"
"    border-style: outset;\n"
"}\n"
"\n"
"QSlider::handle:horizontal:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #3399ff, stop:1 #1a75ff);\n"
"    border: 2px solid #0055aa;\n"
"    border-style: inset;\n"
"}\n"
"\n"
"/* \350\277\233\345\272\246\346\235\241\346\240\267\345\274\217 */\n"
"QProgressBar#temperatureProgressBar {\n"
"    border: 1px solid #404040;\n"
"    border-radius: 4px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #404040, stop:1 #505050);\n"
"    text-align: center;\n"
"}\n"
"\n"
"QProgressBar#temperatureProgressBar::chunk {\n"
"    backg"
                        "round: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #33adff, stop:1 #1a75ff);\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QProgressBar#humidityProgressBar {\n"
"    border: 1px solid #404040;\n"
"    border-radius: 4px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #404040, stop:1 #505050);\n"
"    text-align: center;\n"
"}\n"
"\n"
"QProgressBar#humidityProgressBar::chunk {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #4cd964, stop:1 #2ea043);\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QProgressBar#concentrationProgressBar {\n"
"    border: 1px solid #404040;\n"
"    border-radius: 4px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #404040, stop:1 #505050);\n"
"    text-align: center;\n"
"}\n"
"\n"
"QProgressBar#concentrationProgressBar::chunk {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ff8555, stop:1 #e56600);\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"/* \347\212\266\346\200\201\346\240\217\346\240\267\345\274\217 */\n"
""
                        "QStatusBar {\n"
"    background-color: #333333;\n"
"    border-top: 1px solid #505050;\n"
"    color: #cccccc;\n"
"}\n"
"\n"
"/* \350\217\234\345\215\225\346\240\217\346\240\267\345\274\217 */\n"
"QMenuBar {\n"
"    background-color: #333333;\n"
"    border-bottom: 1px solid #505050;\n"
"    color: #cccccc;\n"
"}\n"
"\n"
"QMenuBar::item {\n"
"    background-color: transparent;\n"
"    color: #cccccc;\n"
"    padding: 8px 12px;\n"
"}\n"
"\n"
"QMenuBar::item:selected {\n"
"    background-color: #454545;\n"
"    color: #ffffff;\n"
"}\n"
"    ", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "\345\223\215\345\272\224\344\277\241\346\201\257", nullptr));
        groupBox_2->setStyleSheet(QString());
        responseInfoTextEdit->setStyleSheet(QString());
        groupBox_4->setTitle(QApplication::translate("MainWindow", "\350\256\276\345\244\207\344\277\241\346\201\257", nullptr));
        groupBox_4->setStyleSheet(QString());
        productIdLineEdit->setText(QApplication::translate("MainWindow", "83bcAx554p", nullptr));
        productIdLineEdit->setStyleSheet(QString());
        deviceNameLineEdit->setText(QApplication::translate("MainWindow", "Linux_my_client_start", nullptr));
        deviceNameLineEdit->setStyleSheet(QString());
        groupBox_5->setTitle(QApplication::translate("MainWindow", "\346\270\251\345\272\246\345\256\236\346\227\266\346\230\276\347\244\272", nullptr));
        groupBox_5->setStyleSheet(QString());
        temperatureLabel->setText(QApplication::translate("MainWindow", "\346\270\251\345\272\246\346\230\276\347\244\272", nullptr));
        temperatureLabel->setStyleSheet(QString());
        temperatureProgressBar->setStyleSheet(QString());
        groupBox_8->setTitle(QApplication::translate("MainWindow", "\346\271\277\345\272\246\345\256\236\346\227\266\346\230\276\347\244\272", nullptr));
        groupBox_8->setStyleSheet(QString());
        humidityLabel->setText(QApplication::translate("MainWindow", "\346\271\277\345\272\246\346\230\276\347\244\272", nullptr));
        humidityLabel->setStyleSheet(QString());
        humidityProgressBar->setStyleSheet(QString());
        groupBox_6->setTitle(QApplication::translate("MainWindow", "\346\265\223\345\272\246\345\256\236\346\227\266\346\230\276\347\244\272", nullptr));
        groupBox_6->setStyleSheet(QString());
        concentrationLabel->setText(QApplication::translate("MainWindow", "\346\265\223\345\272\246\346\230\276\347\244\272", nullptr));
        concentrationLabel->setStyleSheet(QString());
        concentrationProgressBar->setStyleSheet(QString());
        groupBox_7->setTitle(QApplication::translate("MainWindow", "\346\225\260\346\215\256\346\216\247\345\210\266", nullptr));
        groupBox_7->setStyleSheet(QString());
        queryButton->setText(QApplication::translate("MainWindow", "\346\237\245\350\257\242\346\234\200\346\226\260\345\261\236\346\200\247", nullptr));
        queryButton->setStyleSheet(QString());
        queryHistoryButton->setText(QApplication::translate("MainWindow", "\346\237\245\350\257\242\345\216\206\345\217\262\346\225\260\346\215\256", nullptr));
        queryHistoryButton->setStyleSheet(QString());
        exportButton->setText(QApplication::translate("MainWindow", "\345\257\274\345\207\272\345\216\206\345\217\262\350\256\260\345\275\225", nullptr));
        exportButton->setStyleSheet(QString());
        refreshButton->setText(QApplication::translate("MainWindow", "\351\230\210\345\200\274\350\256\276\347\275\256", nullptr));
        refreshButton->setStyleSheet(QString());
        clearButton->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244", nullptr));
        clearButton->setStyleSheet(QString());
        groupBox_3->setTitle(QApplication::translate("MainWindow", "\350\256\276\345\244\207\346\216\247\345\210\266", nullptr));
        groupBox_3->setStyleSheet(QString());
        deviceStatusTextEdit->setStyleSheet(QString());
        groupBox->setTitle(QApplication::translate("MainWindow", "\346\216\247\345\210\266\346\214\211\351\222\256", nullptr));
        groupBox->setStyleSheet(QString());
        openFanButton->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\351\243\216\346\211\207", nullptr));
        openFanButton->setStyleSheet(QString());
        closeFanButton->setText(QApplication::translate("MainWindow", "\345\205\263\351\227\255\351\243\216\346\211\207", nullptr));
        closeFanButton->setStyleSheet(QString());
        openLightButton->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\347\201\257", nullptr));
        openLightButton->setStyleSheet(QString());
        closeLightButton->setText(QApplication::translate("MainWindow", "\345\205\263\351\227\255\347\201\257", nullptr));
        closeLightButton->setStyleSheet(QString());
        drawChartButton->setText(QApplication::translate("MainWindow", "\347\273\230\345\210\266\345\233\276\350\241\250", nullptr));
        drawChartButton->setStyleSheet(QApplication::translate("MainWindow", "background-color: #626681; color: white;", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
