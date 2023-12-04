#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <QStack>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

// 二叉树的结点类
class binaryTreeNode;

// 二叉树的类
class binaryTree;

// 二叉树的结点类（抽象类）
class binaryTreeNode
{
    friend class binaryTree;

public:
    enum TAG { LINK, THREAD };
    virtual binaryTreeNode* getLeftChild() const = 0 ;
    virtual binaryTreeNode* getRightChild() const = 0 ;
    virtual enum TAG getLeftChildTag() const = 0 ;
    virtual enum TAG getRightChildTag() const = 0 ;
    virtual void setLeftChild(binaryTreeNode* leftChild, enum binaryTreeNode::TAG tag) = 0;
    virtual void setRightChild(binaryTreeNode* rightChild, enum binaryTreeNode::TAG tag) = 0;
    virtual void visit() = 0;
    virtual ~binaryTreeNode() = 0;
};

// 二叉树类
class binaryTree
{
    binaryTreeNode* root = nullptr;      // 二叉树的根节点
    binaryTreeNode* pre = nullptr;       // 存储遍历时的前一个结点 用于线索化

public:
    // 三种遍历方式
    enum TRAVERSAL_MODE { PREORDER_TRAVERSAL, INORDER_TRAVERSAL, POSTORDER_TRAVERSAL };

    binaryTree(binaryTreeNode* _root);

    // 统计叶子结点数
    qint16 countLeafNode();

    // 遍历
    void preOrderTraversal(bool withDelay = true);
    void inOrderTraversal(bool withDelay = true);
    void postOrderTraversal(bool withDelay = true);
    void preOrderTraversal_Thr(bool withDelay = true);
    void inOrderTraversal_Thr(bool withDelay = true);

    // 线索化
    void createThreadedTree(int mode, bool withDelay = true);
    void clearThreadedTree();

private:
    // 以下函数用于内部实现递归 与public同名函数重载
    qint16 countLeafNode(binaryTreeNode* cur);
    void createThreadedTree(int mode, binaryTreeNode* cur, bool withDelay);
    void clearThreadedTree(binaryTreeNode* cur);

    // 实现某结点的threading
    void threading(binaryTreeNode* cur);    
};

void waitForSeconds(qreal seconds); // 等待seconds秒

#endif // BINARYTREE_H
