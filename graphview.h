#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QComboBox>
#include <QMouseEvent>
#include <QTimeLine>
#include <QPainter>
#include <QBrush>
#include <QVector>
#include <QFont>
#include <QPen>
#include <QColor>
#include <QDebug>
#include <QtMath>
#include "binarytree.h"

// 二叉树显示的画布
class graphicsView;

// 二叉树的可视化结点
class graphicsVexItem;

// 二叉树的可视化边
class graphicsEdgeItem;

// 二叉树的可视化（线索）线
class graphicsThreadItem;

// 线索是左还是右结点（用于绘制）
enum THREAD_POSITION  { LEFT, RIGHT };


// 二叉树显示的画布
class graphicsView: public QGraphicsView
{
    Q_OBJECT

private:          
    // 画布基础信息
    qint16 leftTopx, leftTopy;
    qint16 width, height;
    QGraphicsScene* graphicsScene;

    binaryTree* binTree = nullptr;
    qint16 vexNum = 0;                  // 已有的结点数量
    bool isNewVexCreating = false;      // 是否在创建新的结点
    bool isTraversal = false;           // 是否正在遍历（此时禁止拖拽）
    bool isLeftChild;                   // 是否创建的是左子树（左键左子树，右键右子树）
    graphicsVexItem* curParentNode;     // 若在创建新的结点，则要记录其双亲
    graphicsEdgeItem* curEdge;          // 拖动时绘制的边

    int traversalMode = 0;                  // 遍历模式
    bool isThreaded = false;                // 是否线索化
    QVector<graphicsThreadItem *> threads;  // 存储所有线索以单独清除
    QColor currentVexColor;                 // 当前的颜色（交替）

    // 默认配置
    const QColor defaultVexColor;
    const QColor HighlightVexColor;
    const qreal defaultVexRadius;

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

public:
    graphicsView(qint16 _leftTopx = 0, qint16 _leftTopy = 0, qint16 _width = 780, qint16 _height = 640, QWidget* parent = nullptr);
    ~graphicsView() Q_DECL_OVERRIDE;
    graphicsVexItem* addVex(QPointF position);
    void removeThread();
    void handleNewVexCreate(graphicsVexItem* parentNode, bool _isLeftChild);
    void handleNewThreadCreate(graphicsVexItem* start, graphicsVexItem* end, enum THREAD_POSITION position);
    void handleStartTraversal();
    void handleModeChanged(int mode = 0);
    void handleThreadStateChanged(int state);
    void handleClearCanvas();

signals:
    void tipsChanged(const QString& tipsContent);
    void leafNodeNumChanged(qint16 leafNodeNum);
    void traversalModeChanged(int traverseOrder, bool isThreaded);
    void traversalStart();
    void traversalEnd();
};


// 二叉树的可视化结点
class graphicsVexItem: public QObject, public QGraphicsEllipseItem, public binaryTreeNode
{
    Q_OBJECT

    friend class graphicsView;

private:
    // 基础信息
    qreal radius;
    QPointF position;
    QColor color, color2;   // 两种颜色交替

    // 编号与名称
    qint16 id;  // 结点的编号（唯一）
    QString name;

    // NameTag
    QGraphicsSimpleTextItem* nameTag;
    QFont nameFont = QFont("Corbel", 13, QFont::Normal, true);

    // 左右孩子及tag
    graphicsVexItem* leftChild = nullptr, * rightChild = nullptr;
    enum binaryTreeNode::TAG leftChildTag = binaryTreeNode::LINK;
    enum binaryTreeNode::TAG rightChildTag = binaryTreeNode::LINK;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e) Q_DECL_OVERRIDE;

public:
    graphicsVexItem(qreal _radius, QPointF _position, QColor _color, QColor _color2, qint16 _id, QGraphicsItem* parent = nullptr);
    ~graphicsVexItem() Q_DECL_OVERRIDE;

    // 弹出动画
    void popOutAnimation(bool withNameTag = false);

    // 获取基本信息
    qreal getRadius() const;
    QPointF getPosition() const;
    QString getName() const;

    // 对基类虚方法的继承
    virtual graphicsVexItem* getLeftChild() const Q_DECL_OVERRIDE;
    virtual graphicsVexItem* getRightChild() const Q_DECL_OVERRIDE;
    virtual enum TAG getLeftChildTag() const Q_DECL_OVERRIDE;
    virtual enum TAG getRightChildTag() const Q_DECL_OVERRIDE;
    virtual void setLeftChild(binaryTreeNode* leftChild, enum binaryTreeNode::TAG tag) Q_DECL_OVERRIDE;
    virtual void setRightChild(binaryTreeNode* rightChild, enum binaryTreeNode::TAG tag) Q_DECL_OVERRIDE;
    virtual void visit() Q_DECL_OVERRIDE;

signals:
    void startNewVex(graphicsVexItem* parentNode, bool isLeftChild);
    void startNewThread(graphicsVexItem* start, graphicsVexItem* end, enum THREAD_POSITION position);
};


// 二叉树的可视化边
class graphicsEdgeItem: public QObject, public QGraphicsLineItem
{
    Q_OBJECT

    friend class graphicsView;

    QPointF start, end;
    const QPen defaultPen = QPen(QColor(0xB8BBC1), 2, Qt::SolidLine, Qt::RoundCap);
    const QPen defaultDashPen = QPen(QColor(0xD0D2D7), 2, Qt::DashLine, Qt::RoundCap);

public:
    graphicsEdgeItem(graphicsVexItem* start, graphicsVexItem* end, QGraphicsItem* parent = nullptr);
    graphicsEdgeItem(graphicsVexItem* start, QPointF end, QGraphicsItem* parent = nullptr);
    ~graphicsEdgeItem();
};


// 二叉树的可视化（线索）线
class graphicsThreadItem: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    friend class graphicsView;

public:
    graphicsThreadItem(graphicsVexItem* _start, graphicsVexItem* _end, enum THREAD_POSITION position, QGraphicsItem* parent = nullptr);
    // 必须继承的虚方法
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) Q_DECL_OVERRIDE;

private:
    // 起始终止位置，以及贝塞尔曲线的控制点
    QPointF start, end, controlPoint;
    // 线索是左/右
    enum THREAD_POSITION position;
};


qreal getDistance(const QPointF& p1, const QPointF& p2);

#endif // GRAPHVIEW_H
