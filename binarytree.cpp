#include "binarytree.h"


// 暂停等待seconds秒
void waitForSeconds(qreal seconds)
{
    QEventLoop loop;
    QTimer::singleShot(seconds * 1000, &loop, &QEventLoop::quit);
    loop.exec();
}

binaryTreeNode::~binaryTreeNode()
{
}

/**
 * @brief binaryTree::binaryTree 传入对应的根节点指针构造二叉树
 * @param _root 根节点指针
 */
binaryTree::binaryTree(binaryTreeNode* _root):
    root(_root)
{
}

/**
 * @brief binaryTree::countLeafNode 供外部调用，统计叶子结点数
 * @return 二叉树的叶子结点数
 */
qint16 binaryTree::countLeafNode()
{
    return countLeafNode(root);
}

/**
 * @brief binaryTree::countLeafNode 函数内部递归的统计叶子结点数
 * @param cur 当前遍历的结点
 * @return 当前结点为根结点的二叉树的叶子结点数
 */
qint16 binaryTree::countLeafNode(binaryTreeNode* cur)
{
    if(!cur)
        return 0;
    else if(!cur->getLeftChild() && !cur->getRightChild())
        return 1;

    return countLeafNode(cur->getLeftChild()) + countLeafNode(cur->getRightChild());
}

/**
 * @brief binaryTree::preOrderTraversal
 * @param withDelay 是否延迟动画
 */
void binaryTree::preOrderTraversal(bool withDelay)
{
    QStack<binaryTreeNode*> s;
    binaryTreeNode* p;

    s.push(root);	//根指针进栈

    while(!s.empty()) {
        while((p = s.top())) {
            p = s.pop();    //根元素出栈
            p->visit();
            if(withDelay)
                waitForSeconds(0.5);
            s.push(p->getRightChild());	//右子树先进栈
            s.push(p->getLeftChild());	//左子树再进栈
        }
        s.pop();        //空指针退栈
    }
}

/**
 * @brief binaryTree::inOrderTraversal
 * @param withDelay 是否延迟动画
 */
void binaryTree::inOrderTraversal(bool withDelay)
{
    QStack<binaryTreeNode*> s;
    binaryTreeNode* p;

    s.push(root);	// 根指针进栈

    while(!s.empty()) {
        while((p = s.top())){
            s.push(p->getLeftChild());
        }

        s.pop();    // 空指针

        if(!s.empty()){
            p = s.pop();
            p->visit();
            if(withDelay)
                waitForSeconds(0.5);
            s.push(p->getRightChild());
        }
    }
}

/**
 * @brief binaryTree::postOrderTraversal
 * @param withDelay 是否延迟动画
 */
void binaryTree::postOrderTraversal(bool withDelay)
{
    QStack<binaryTreeNode*> s;
    binaryTreeNode* p, * pre = nullptr;

    s.push(root);	// 根指针进栈

    while(!s.empty()) {
        while((p = s.top())){
            s.push(p->getLeftChild());
        }

        s.pop();    // 空指针

        if(!s.empty()){
            p = s.top();
            if(!p->getRightChild() || p->getRightChild() == pre){
                s.pop();
                p->visit();
                if(withDelay)
                    waitForSeconds(0.5);
                pre = p;
                s.push(nullptr);    // 压一个空指针，因为上面要弹出
            }
            else{
                s.push(p->getRightChild());
            }
        }
    }
}

/**
 * @brief binaryTree::preOrderTraversal_Thr 线索化遍历前序二叉树
 * @param withDelay 是否延迟动画
 */
void binaryTree::preOrderTraversal_Thr(bool withDelay)
{
    binaryTreeNode* p = root;
    while(p){
        while(p->getLeftChildTag() == binaryTreeNode::LINK){
            p->visit();
            if(withDelay)
                waitForSeconds(0.5);
            p = p->getLeftChild();
        }
        p->visit();
        if(withDelay)
            waitForSeconds(0.5);
        p = p->getRightChild();
    }
}

/**
 * @brief binaryTree::inOrderTraversal_Thr 线索化遍历中序二叉树
 * @param withDelay 是否延迟动画
 */
void binaryTree::inOrderTraversal_Thr(bool withDelay)
{
    binaryTreeNode* p = root;
    while(p){
        while(p->getLeftChildTag() == binaryTreeNode::LINK)
            p = p->getLeftChild();
        p->visit();
        if(withDelay)
            waitForSeconds(0.5);
        while(p->getRightChildTag() == binaryTreeNode::THREAD && p->getRightChild()){
            p = p->getRightChild();
            p->visit();
            if(withDelay)
                waitForSeconds(0.5);
        }
        p = p->getRightChild();
    }
}

/**
 * @brief binaryTree::createThreadedTree 递归线索化
 * @param mode 前/中/后续
 * @param cur 当前遍历的结点
 * @param withDelay 是否延迟动画
 */
void binaryTree::createThreadedTree(int mode, binaryTreeNode* cur, bool withDelay)
{
    if(cur){
        cur->visit();
        if(withDelay)
            waitForSeconds(0.5);
        switch(mode){
            case PREORDER_TRAVERSAL:
                threading(cur);
                if(cur->getLeftChildTag() == binaryTreeNode::LINK)
                    createThreadedTree(mode, cur->getLeftChild(), withDelay);
                if(cur->getRightChildTag() == binaryTreeNode::LINK)
                    createThreadedTree(mode, cur->getRightChild(), withDelay);
                break;
            case INORDER_TRAVERSAL:
                createThreadedTree(mode, cur->getLeftChild(), withDelay);
                threading(cur);
                createThreadedTree(mode, cur->getRightChild(), withDelay);
                break;
            case POSTORDER_TRAVERSAL:
                createThreadedTree(mode, cur->getLeftChild(), withDelay);
                createThreadedTree(mode, cur->getRightChild(), withDelay);
                threading(cur);
                break;
        }
    }
}

/**
 * @brief binaryTree::createThreadedTree 与上一函数重载，供外部调用
 * @param mode 前/中/后续
 * @param withDelay 是否延迟动画
 */
void binaryTree::createThreadedTree(int mode, bool withDelay)
{
    pre = nullptr;

    if(root){
        createThreadedTree(mode, root, withDelay);
        if(pre && !pre->getRightChild())
            pre->setRightChild(nullptr, binaryTreeNode::THREAD);
    }
}

/**
 * @brief binaryTree::clearThreadedTree
 * @param cur 当前遍历到的结点
 */
void binaryTree::clearThreadedTree(binaryTreeNode* cur)
{
    // 递归的清除线索
    if(cur){
        if(cur->getLeftChildTag() == binaryTreeNode::LINK)
            clearThreadedTree(cur->getLeftChild());
        else
            cur->setLeftChild(nullptr, binaryTreeNode::LINK);
        if(cur->getRightChildTag() == binaryTreeNode::LINK)
            clearThreadedTree(cur->getRightChild());
        else
            cur->setRightChild(nullptr, binaryTreeNode::LINK);
    }
}

/**
 * @brief binaryTree::clearThreadedTree 供外部调用，清除线索
 */
void binaryTree::clearThreadedTree()
{
    clearThreadedTree(root);
}

/**
 * @brief binaryTree::threading 处理某结点的线索化
 * @param cur 当前处理的结点
 */
inline void binaryTree::threading(binaryTreeNode *cur)
{
    if(!cur->getLeftChild())
        cur->setLeftChild(pre, binaryTreeNode::THREAD);

    if(pre && !pre->getRightChild())
        pre->setRightChild(cur, binaryTreeNode::THREAD);

    pre = cur;
}
