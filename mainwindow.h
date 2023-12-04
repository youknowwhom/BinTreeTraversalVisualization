#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QColor>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QCheckBox>
#include <QDebug>
#include <QListView>
#include "binarytree.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() Q_DECL_OVERRIDE;

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

    // 处理graphicView传来的信号
    void handleTipsChanged(const QString& tips);
    void handleLeafNodeNumChanged(qint16 leafNode);
    void handleTraversalModeChanged(int traverseOrder, bool isThreaded);
    void handleTraversalStart();
    void handleTraversalEnd();

private:
    Ui::MainWindow *ui;


    // 拖拽窗口
    bool isValidDragging = false;   // 是否有效区域内在拖拽窗口
    QPoint mouseOffset;             // 鼠标按下时距离窗口左上角的偏移

    // 右侧栏的控件
    QLabel* labelTipsContent;           // 提示信息内容
    QLabel* labelLeafNodeNumContent;    // 叶子结点个数
    QPushButton* buttonStart, * buttonClear;

};

#endif // MAINWINDOW_H
