#include "graphview.h"

/* graphicsView */

/**
 * @brief graphicsView::graphicsView
 * @param _leftTopx
 * @param _leftTopy
 * @param _width
 * @param _height
 * @param parent 对象树的父亲指针
 */
graphicsView::graphicsView(qint16 _leftTopx, qint16 _leftTopy, qint16 _width, qint16 _height, QWidget* parent):
    QGraphicsView (parent),
    leftTopx(_leftTopx),
    leftTopy(_leftTopy),
    width(_width),
    height(_height),
    currentVexColor(QColor(144, 200, 180)),
    defaultVexColor(QColor(144, 200, 180)),
    HighlightVexColor(QColor(0xe9e299)),
    defaultVexRadius(15)
{
    this->move(leftTopx, leftTopy);
    this->resize(width, height);
    this->setStyleSheet("border-radius: 20px; background-color: white;");
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform); // 抗锯齿
    this->setMouseTracking(true);   // 不按下时也追踪鼠标

    // 绑定graphicsScene
    graphicsScene = new QGraphicsScene(this);
    graphicsScene->setSceneRect(0, 0, width, height);
    this->setScene(graphicsScene);
}

graphicsView::~graphicsView()
{
}

/**
 * @brief graphicsView::addVex 向画布上添加结点
 * @param position 结点位置
 * @return 对应的可视化结点指针
 */
inline graphicsVexItem* graphicsView::addVex(QPointF position)
{
    graphicsVexItem* newvex = new graphicsVexItem(defaultVexRadius, position, currentVexColor, (currentVexColor == defaultVexColor ? HighlightVexColor : defaultVexColor), vexNum);
    graphicsScene->addItem(newvex);
    connect(newvex, &graphicsVexItem::startNewVex, this, &graphicsView::handleNewVexCreate);
    connect(newvex, &graphicsVexItem::startNewThread, this, &graphicsView::handleNewThreadCreate);
    ++vexNum;      
    return newvex;
}

/**
 * @brief graphicsView::removeThread 移除可视化线索（不清空结构信息）
 */
inline void graphicsView::removeThread()
{
    graphicsThreadItem* thread;
    while(!threads.empty()){
        thread = threads.back();
        threads.pop_back();
        graphicsScene->removeItem(thread);
        delete thread;
    }
}

/**
 * @brief graphicsView::mousePressEvent 监测鼠标点击 用于生成新结点
 * @param e 鼠标事件
 */
void graphicsView::mousePressEvent(QMouseEvent *e)
{
    if(vexNum == 0){
        graphicsVexItem* root = addVex(e->localPos());
        binTree = new binaryTree(root);
        emit tipsChanged("Click the node with left/right button to create a correspoding child node.");
        emit leafNodeNumChanged(binTree->countLeafNode());
    } 
    else if(isNewVexCreating && getDistance(curParentNode->getPosition(), e->localPos()) > 3 * defaultVexRadius){
        isNewVexCreating = false;
        setCursor(Qt::ArrowCursor);
        graphicsVexItem* newvex = addVex(e->localPos());

        if(isLeftChild)
            curParentNode->leftChild = newvex;
        else
            curParentNode->rightChild = newvex;

        curEdge->setPen(curEdge->defaultPen);
        emit leafNodeNumChanged(binTree->countLeafNode());

        emit tipsChanged("Continue to click a node with left/right button to create a left/right child.");
    }
    else{
        // 传递mousePress事件，以供item捕捉
        QGraphicsView::mousePressEvent(e);
    }
}

/**
 * @brief graphicsView::mouseMoveEvent 检测鼠标移动 用于拖拽边
 * @param e
 */
void graphicsView::mouseMoveEvent(QMouseEvent *e)
{
    if(isNewVexCreating){
        curEdge->setLine(QLine(curParentNode->getPosition().toPoint(), e->localPos().toPoint()));
        emit tipsChanged("Drag and click again to create a new node.");
    }
}


/**
 * @brief graphicsView::handleNewVexCreate 处理拖拽出新结点
 * @param parentNode 其对应的父亲结点
 * @param _isLeftChild 左/右结点
 */
void graphicsView::handleNewVexCreate(graphicsVexItem* parentNode, bool _isLeftChild)
{
    // 要求不在遍历状态
    if(!isTraversal){
        curParentNode = parentNode;
        curEdge = new graphicsEdgeItem(parentNode, parentNode->getPosition().toPoint());
        curEdge->setZValue(-1);
        graphicsScene->addItem(curEdge);
        isNewVexCreating = true;
        setCursor(Qt::CrossCursor);
        isLeftChild = _isLeftChild;
    }
}

/**
 * @brief graphicsView::handleNewThreadCreate 可视化绘制新线索
 * @param start 当前结点
 * @param end 前驱/后继结点
 * @param position 左/右孩子处
 */
void graphicsView::handleNewThreadCreate(graphicsVexItem *start, graphicsVexItem *end, enum THREAD_POSITION position)
{
    if(start && end){
        graphicsThreadItem* newThread = new graphicsThreadItem(start, end, position);
        graphicsScene->addItem(newThread);
        threads.push_back(newThread);
    }
}

/**
 * @brief graphicsView::handleStartTraversal
 */
void graphicsView::handleStartTraversal()
{
    if(vexNum){
        emit traversalStart();
        isTraversal = true;     // 开始遍历 禁用添加结点

        // 清除之前的线索
        removeThread();     // 清除线索的可视化部分
        binTree->clearThreadedTree();   // 清楚树结构中穿好的线索

        if(isThreaded){
            emit tipsChanged("Creating a threaded binary tree...");
            switch (traversalMode) {
                case binaryTree::PREORDER_TRAVERSAL:
                    binTree->createThreadedTree(traversalMode);
                    emit tipsChanged("The threaded binary tree creation is completed.");
                    waitForSeconds(2);
                    emit tipsChanged("Executing threaded binary tree traversal...");
                    binTree->preOrderTraversal_Thr();
                    break;
                case binaryTree::INORDER_TRAVERSAL:
                    binTree->createThreadedTree(traversalMode);
                    emit tipsChanged("The threaded binary tree creation is completed.");
                    waitForSeconds(2);
                    emit tipsChanged("Executing threaded binary tree traversal...");
                    binTree->inOrderTraversal_Thr();
                    break;
                default:
                    binTree->createThreadedTree(traversalMode);
                    emit tipsChanged("The threaded binary tree creation is completed.");
                    currentVexColor = (currentVexColor == defaultVexColor ? HighlightVexColor : defaultVexColor);   // 反转结点颜色记录
            }
            emit tipsChanged("The traversal is done. Choose different mode to try again.");
        }
        else{
            emit tipsChanged("Executing binary tree traversal...");
            switch (traversalMode) {
                case binaryTree::PREORDER_TRAVERSAL:
                    binTree->preOrderTraversal();
                    break;
                case binaryTree::INORDER_TRAVERSAL:
                    binTree->inOrderTraversal();
                    break;
                case binaryTree::POSTORDER_TRAVERSAL:
                    binTree->postOrderTraversal();
                    break;
            }
            currentVexColor = (currentVexColor == defaultVexColor ? HighlightVexColor : defaultVexColor);   // 反转结点颜色记录
            emit tipsChanged("The traversal is done. Choose different mode to try again.");
        }

        emit traversalEnd();
        isTraversal = false;    // 结束遍历，允许添加结点
    }
}

/**
 * @brief graphicsView::handleModeChanged
 * @param mode 前/中/后序
 */
void graphicsView::handleModeChanged(int mode)
{
    traversalMode = mode;
    emit traversalModeChanged(traversalMode, isThreaded);
}

/**
 * @brief graphicsView::handleThreadStateChanged 处理勾选/取消勾选“thread”
 * @param state 是否线索化
 */
void graphicsView::handleThreadStateChanged(int state)
{
    isThreaded = state;
    emit traversalModeChanged(traversalMode, isThreaded);
}

/**
 * @brief graphicsView::handleClearCanvas
 */
void graphicsView::handleClearCanvas()
{
    graphicsScene->clear();
    vexNum = 0;
    threads.clear();    // 清空记录的thread，防止再次删除
    currentVexColor = defaultVexColor;      // 恢复为默认颜色
    emit leafNodeNumChanged(0);
    emit tipsChanged("Click the canvas to create a root node.");
}


/* 二叉树结点：graphicsVexItem */

graphicsVexItem::graphicsVexItem(qreal _radius, QPointF _position, QColor _color, QColor _color2, qint16 _id, QGraphicsItem* parent):
    QGraphicsEllipseItem(_position.x() - _radius, _position.y() - _radius, 2 * _radius, 2 * _radius, parent),
    radius(_radius),
    position(_position),
    color(_color),
    color2(_color2),
    id(_id),
    name("V" + QString::number(id))
{
    nameTag = new QGraphicsSimpleTextItem(this);
    nameTag->setPos(position + QPointF(radius, - radius - QFontMetrics(nameFont).height()));
    nameTag->setFont(nameFont);
    nameTag->setText(name);
    nameTag->setZValue(this->zValue());
    nameTag->setBrush(Qt::transparent);
    nameTag->setFlags(QGraphicsItem::ItemIsSelectable);

    setBrush(this->color);
    setPen(QPen(Qt::NoPen));;
    this->popOutAnimation(true);
}

/**
 * @brief graphicsVexItem::mousePressEvent 用于监听点击拖拽子结点
 * @param e
 */
void graphicsVexItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    bool isLeftChild = (e->button() == Qt::LeftButton);
    if((isLeftChild && !leftChild) || (!isLeftChild && !rightChild))
        emit startNewVex(this, isLeftChild);
    this->popOutAnimation(false);
}

graphicsVexItem* graphicsVexItem::getLeftChild() const
{
    return this->leftChild;
}

graphicsVexItem* graphicsVexItem::getRightChild() const
{
    return this->rightChild;
}

enum binaryTreeNode::TAG graphicsVexItem::getLeftChildTag() const
{
    return this->leftChildTag;
}

enum binaryTreeNode::TAG graphicsVexItem::getRightChildTag() const
{
    return this->rightChildTag;
}

void graphicsVexItem::setLeftChild(binaryTreeNode* _leftChild, enum binaryTreeNode::TAG tag)
{
    this->leftChild = dynamic_cast<graphicsVexItem *>(_leftChild);
    this->leftChildTag = tag;
    // qDebug() << "set" << (leftChild ? leftChild->name : "null") << "as the left" << (tag == binaryTreeNode::LINK ? "link" : "thread") << "of"<<name;
    emit startNewThread(this, leftChild, THREAD_POSITION::LEFT);
}

void graphicsVexItem::setRightChild(binaryTreeNode* _rightChild, enum binaryTreeNode::TAG tag)
{
    this->rightChild = dynamic_cast<graphicsVexItem *>(_rightChild);
    this->rightChildTag = tag;
    // qDebug() << "set" << (rightChild ? rightChild->name : "null") << "as the right" << (tag == binaryTreeNode::LINK ? "link" : "thread") << "of"<<name;
    emit startNewThread(this, rightChild, THREAD_POSITION::RIGHT);
}

void graphicsVexItem::visit()
{
    // qDebug() << "visit" << name << "(left child" << (leftChild ? leftChild->name : "null") << "right child" << (rightChild ? rightChild->name : "null") << ")";

    // 高亮与原色来回交替
    if(this->brush() == color)
        this->setBrush(color2);
    else
        this->setBrush(color);
    this->popOutAnimation();
}

// 新建结点时的弹出动画
void graphicsVexItem::popOutAnimation(bool withNameTag)
{
    QTimeLine* timeLine = new QTimeLine(300, this);
    timeLine->setFrameRange(0, 1000);
    QEasingCurve curve = QEasingCurve::InBounce;

    connect(timeLine, &QTimeLine::frameChanged, timeLine, [=](qint16 frame){
        qreal curProgress = curve.valueForProgress(frame / 1000.0);
        qreal curRadius = radius + 5 - 5 * curProgress;
        if(withNameTag)
            nameTag->setBrush(QColor(0, 0, 0, int(curProgress * 0xFF)));
        this->setRect(QRectF(position.x() - curRadius, position.y() - curRadius, curRadius * 2, curRadius * 2));
    });

    timeLine->start();
}

// 获取结点半径
qreal graphicsVexItem::getRadius() const
{
    return radius;
}

// 获取结点中心位置
QPointF graphicsVexItem::getPosition() const
{
    return position;
}

// 获取结点名字
QString graphicsVexItem::getName() const
{
    return name;
}

graphicsVexItem::~graphicsVexItem()
{
}



/* 二叉树边：graphicsEdgeItem */

graphicsEdgeItem::graphicsEdgeItem(graphicsVexItem* start, graphicsVexItem* end, QGraphicsItem* parent):
    QGraphicsLineItem (QLine(start->getPosition().toPoint(), end->getPosition().toPoint()), parent)
{
    setPen(defaultPen);
}

graphicsEdgeItem::graphicsEdgeItem(graphicsVexItem* start, QPointF end, QGraphicsItem* parent):
    QGraphicsLineItem (QLine(start->getPosition().toPoint(), end.toPoint()), parent)
{
    setPen(defaultDashPen);
}

graphicsEdgeItem::~graphicsEdgeItem()
{
}


/* 二叉树线索：graphicsThreadItem */

graphicsThreadItem::graphicsThreadItem(graphicsVexItem* _start, graphicsVexItem* _end, enum THREAD_POSITION _position, QGraphicsItem* parent):
    QGraphicsItem (parent),
    end(_end->getPosition()),
    position(_position)
{
    // 设置起始出发点
    if(position == THREAD_POSITION::LEFT)
        start.setX(_start->getPosition().x() - _start->getRadius() * M_SQRT1_2);
    else
        start.setX(_start->getPosition().x() + _start->getRadius() * M_SQRT1_2);
    start.setY(_start->getPosition().y() + _start->getRadius() * M_SQRT1_2);


    // 计算控制点的位置
    QPointF middle = (start + end) / 2;
    qreal k = -(start.x() - end.x()) / (start.y() - end.y());
    qreal edge = getDistance(start, end) / 4;
    qreal angle = qAtan(qAbs(k));

    qreal deltaY, deltaX;
    deltaY =  - qSin(angle) * edge;
    if(k > 0)
        deltaX =  - qCos(angle) * edge;
    else
        deltaX = qCos(angle) * edge;

    // 曲线上凸
    if((k > 0 && position == THREAD_POSITION::LEFT) || (k < 0 && position == THREAD_POSITION::RIGHT)){
        controlPoint.setX(middle.x() + deltaX);
        controlPoint.setY(middle.y() + deltaY);
    }
    // 曲线下凹
    else {
        controlPoint.setX(middle.x() - deltaX);
        controlPoint.setY(middle.y() - deltaY);
    }

    // 置于最底层
    this->setZValue(-2);
}

QRectF graphicsThreadItem::boundingRect() const
{
    // 为箭头预留空间
    qreal minx = qMin(start.x(), qMin(end.x(), controlPoint.x())) - 50;
    qreal maxx = qMax(start.x(), qMax(end.x(), controlPoint.x())) + 50;
    qreal miny = qMin(start.y(), qMin(end.y(), controlPoint.y())) - 50;
    qreal maxy = qMax(start.y(), qMax(end.y(), controlPoint.y())) + 50;
    return QRectF(QPointF(minx, miny), QPointF(maxx, maxy));
}

void graphicsThreadItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 画弧线
    QPainterPath painterPath;
    painter->setPen(QPen(QColor(144, 200, 180, 64), 3, Qt::DashLine));
    painterPath.moveTo(start);
    painterPath.quadTo(controlPoint, end);

    painter->drawPath(painterPath);

    // 画箭头
    painter->setPen(QPen(QColor(144, 200, 180, 64), 3, Qt::SolidLine));
    QPointF arrowSide1 = end + (start - end) / getDistance(start, end) * 40, arrowSide2;
    qreal rotationAngle = qRadiansToDegrees(60.0);
    // 逆时针旋转的情况
    if((position == LEFT && start.y() < end.y()) || (position == RIGHT && start.y() > end.y()))
        rotationAngle = - rotationAngle;
    arrowSide2.setX((arrowSide1.x() - end.x()) * cos(rotationAngle) - (arrowSide1.y() - end.y()) * sin(rotationAngle) + end.x());
    arrowSide2.setY((arrowSide1.x() - end.x()) * sin(rotationAngle) + (arrowSide1.y() - end.y()) * cos(rotationAngle) + end.y());
    painter->drawLine(end, arrowSide1);
    painter->drawLine(end, arrowSide2);
}

/**
 * @brief getDistance 获取两点在画布上的距离
 * @param p1
 * @param p2
 * @return 距离
 */
qreal getDistance(const QPointF& p1, const QPointF& p2)
{
    return qSqrt(QPointF::dotProduct((p2 - p1), (p2 - p1)));
}
