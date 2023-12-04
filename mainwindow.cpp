#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 原窗口隐藏
    setWindowFlag(Qt::FramelessWindowHint);         // 无边框
    setAttribute(Qt::WA_TranslucentBackground);     // 透明背景


    // 窗口绘制
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0);
    shadow->setColor(QColor(0, 0, 0, 100));
    ui->mainWindowFillet->setGraphicsEffect(shadow);


    // 画布界面
    graphicsView *view = new graphicsView(75, 195, 780, 650, this);
    view->show();
    connect(view, &graphicsView::tipsChanged, this, &MainWindow::handleTipsChanged);
    connect(view, &graphicsView::leafNodeNumChanged, this, &MainWindow::handleLeafNodeNumChanged);
    connect(view, &graphicsView::traversalModeChanged, this, &MainWindow::handleTraversalModeChanged);
    connect(view, &graphicsView::traversalStart, this, &MainWindow::handleTraversalStart);
    connect(view, &graphicsView::traversalEnd, this, &MainWindow::handleTraversalEnd);

    // 右侧栏
    QWidget* rightBar = new QWidget(this);
    rightBar->setFixedSize(370, 700);
    rightBar->move(895, 165);
    QGridLayout* layOut = new QGridLayout();

    QString headingStyle = "max-height:50px; font-size:40px; font-family:'corbel';";
    QString buttonStyle = "min-height:60px; border-radius:10px; font-size:30px; font-family:'corbel'; background-color:rgba(144, 200, 180, 0.5);";

    QLabel* labelTips = new QLabel("Tips");
    labelTips->setStyleSheet(headingStyle);
    labelTipsContent = new QLabel("Click the canvas to create a root node.");
    labelTipsContent->setWordWrap(true);
    labelTipsContent->setAlignment(Qt::AlignTop);
    labelTipsContent->setFixedWidth(370);
    labelTipsContent->setStyleSheet("max-height: 120px; font-size:25px; font-family:'corbel light';");

    QLabel* labelLeafNode = new QLabel("Leaf Node:");
    labelLeafNode->setStyleSheet(headingStyle);
    labelLeafNodeNumContent = new QLabel("0");
    labelLeafNodeNumContent->setStyleSheet("max-height: 80px; font-size:40px; font-family:'corbel light';");

    QLabel* labelSettings = new QLabel("Settings");
    labelSettings->setStyleSheet(headingStyle);
    QLabel* labelOrder = new QLabel("order");
    labelOrder->setStyleSheet("font-size:22px; font-family:'corbel';");

    QComboBox* comboBox = new QComboBox();
    comboBox->setStyleSheet("min-height:50px; font-size:22px; font-family:'corbel light';");
    comboBox->addItem("Preorder Traversal");
    comboBox->addItem("Inorder Traversal");
    comboBox->addItem("Postorder Traversal");
    comboBox->setView(new QListView());
    setStyleSheet("QComboBox QAbstractItemView::item{height:30px;}");
    connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), view, &graphicsView::handleModeChanged);

    buttonStart = new QPushButton(QString("Start Traversal"));
    buttonStart->setStyleSheet(buttonStyle);
    buttonStart->setCursor(Qt::PointingHandCursor);
    connect(buttonStart, &QPushButton::clicked, view, &graphicsView::handleStartTraversal);

    QLabel* labelThreaded = new QLabel("thread");
    labelThreaded->setStyleSheet("font-size:22px; font-family:'corbel';");
    QCheckBox* checkBox = new QCheckBox();
    connect(checkBox, &QCheckBox::stateChanged, view, &graphicsView::handleThreadStateChanged);

    buttonClear = new QPushButton(QString("Clear"));
    buttonClear->setCursor(Qt::PointingHandCursor);
    buttonClear->setStyleSheet(buttonStyle);
    connect(buttonClear, &QPushButton::clicked, view, &graphicsView::handleClearCanvas);

    layOut->addWidget(labelTips, 0, 0, 1, 4);
    layOut->addWidget(labelTipsContent, 1, 0, 1, 4);
    layOut->addWidget(labelLeafNode, 2, 0, 1, 3);
    layOut->addWidget(labelLeafNodeNumContent, 2, 3, 1, 2);
    layOut->addWidget(labelSettings, 3, 0, 1, 4);
    layOut->addWidget(labelOrder, 4, 0, 1, 1);
    layOut->addWidget(comboBox, 4, 1, 1, 3);
    layOut->addWidget(labelThreaded, 5, 0, 1, 1);
    layOut->addWidget(checkBox, 5, 1, 1, 1);
    layOut->addWidget(buttonStart, 6, 0, 1, 4);
    layOut->addWidget(buttonClear, 7, 0, 1, 4);
    rightBar->setLayout(layOut);

    QIcon minIcon(":image/minimize.png");
    QPushButton* minButton = new QPushButton(minIcon, "", this);
    minButton->setFixedSize(50, 50);
    minButton->setStyleSheet("background-color:rgba(0,0,0,0)");
    minButton->setIconSize(QSize(50, 50));
    minButton->setCursor(Qt::PointingHandCursor);
    minButton->move(1160, 42);
    minButton->show();
    connect(minButton, &QPushButton::clicked, this, &MainWindow::showMinimized);

    QIcon closeIcon(":image/close.png");
    QPushButton* closeButton = new QPushButton(closeIcon, "", this);
    closeButton->setFixedSize(50, 50);
    closeButton->setStyleSheet("background-color:rgba(0,0,0,0)");
    closeButton->setIconSize(QSize(50, 50));
    closeButton->setCursor(Qt::PointingHandCursor);
    closeButton->move(1230, 42);
    closeButton->show();
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::close);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    // 拖动区域限制
    if(e->button() == Qt::LeftButton && e->localPos().y() < 195){
        QPoint mousePst = e->globalPos();
        mouseOffset = mousePst - geometry().topLeft();
        isValidDragging = true;
    }
    QMainWindow::mousePressEvent(e);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    QPoint mousePst = e->globalPos();
    QPoint newWindowPst = mousePst - mouseOffset;
    if(isValidDragging)
        move(newWindowPst);
    QMainWindow::mouseMoveEvent(e);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    isValidDragging = false;
    QMainWindow::mouseReleaseEvent(e);
}

void MainWindow::handleTipsChanged(const QString& tips)
{
    labelTipsContent->setText(tips);
}

void MainWindow::handleLeafNodeNumChanged(qint16 leafNode)
{
    labelLeafNodeNumContent->setText(QString::number(leafNode));
}

void MainWindow::handleTraversalModeChanged(int traverseOrder, bool isThreaded)
{
    if(!isThreaded)
        buttonStart->setText("Start Traversal");
    else if(traverseOrder == binaryTree::POSTORDER_TRAVERSAL)
        buttonStart->setText("Create Tree");
    else
        buttonStart->setText("Create && Traverse");
}

void MainWindow::handleTraversalStart()
{
    buttonStart->setEnabled(false);
    buttonClear->setEnabled(false);
    buttonStart->setCursor(Qt::ForbiddenCursor);
    buttonClear->setCursor(Qt::ForbiddenCursor);
}

void MainWindow::handleTraversalEnd()
{
    buttonStart->setEnabled(true);
    buttonClear->setEnabled(true);
    buttonStart->setCursor(Qt::PointingHandCursor);
    buttonClear->setCursor(Qt::PointingHandCursor);
}
