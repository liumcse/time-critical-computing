#include <redblack.h>

void rbtree_init(RBTree *tree, int (*rbt_keycmp)(void *, void *))
{
    //	struct RBTree* tree = (struct RBTree*)OSMemGet(sizeof(struct RBTree));
    //	memset(tree, 0, sizeof(RBTree));

    //  tree = (struct RBTree){
    //    .rbt_keycmp = rbt_keycmp,
    //    .root = 0,
    tree->rbt_keycmp = rbt_keycmp;
    tree->root = 0;
    //  };
    //  tree->root =0;
    //  tree->rbt_keycmp = rbt_keycmp;
    //  return tree;
}

void _left_rotate(RBTree *tree, struct RBNode *node)
{
    struct RBNode *right_child = node->right;
    struct RBNode *parent = node->parent;

    node->right = right_child->left;
    if (!IS_NULL(right_child->left))
        right_child->left->parent = node;
    right_child->parent = parent;
    if (IS_NULL(parent))
        tree->root = right_child;
    else if (node == parent->left)
        parent->left = right_child;
    else
        parent->right = right_child;

    right_child->left = node;
    node->parent = right_child;
}

void _right_rotate(RBTree *tree, struct RBNode *node)
{
    struct RBNode *left_child = node->left;
    struct RBNode *parent = node->parent;

    node->left = (left_child == 0) ? 0 : left_child->right;
    if (!IS_NULL(left_child->right))
        left_child->right->parent = node;
    left_child->parent = parent;
    if (IS_NULL(parent))
        tree->root = left_child;
    else if (parent->left == node)
        parent->left = left_child;
    else
        parent->right = left_child;

    left_child->right = node;
    node->parent = left_child;
}

void _rbtree_insert_fixup(RBTree *tree, struct RBNode *node)
{
    struct RBNode *parent = 0;
    struct RBNode *uncle = 0;
    struct RBNode *tmp_node = 0;
    while (IS_RED(node))
    {
        parent = node->parent;
        if (!IS_RED(parent))
            break;
        struct RBNode *grandparent = parent->parent;
        if (parent == grandparent->left)
        {
            uncle = grandparent->right;
            if (IS_RED(uncle))
            {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                node = grandparent;
            }
            else
            {
                if (node == parent->right)
                {
                    _left_rotate(tree, parent);
                    tmp_node = node;
                    node = parent;
                    parent = tmp_node;
                }
                else
                {
                    parent->color = BLACK;
                    grandparent->color = RED;
                    _right_rotate(tree, grandparent);
                }
            }
        }
        else
        {
            uncle = grandparent->left;
            if (IS_RED(uncle))
            {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                node = grandparent;
            }
            else
            {
                if (node == parent->left)
                {
                    _right_rotate(tree, parent);
                    tmp_node = node;
                    node = parent;
                    parent = tmp_node;
                }
                else
                {
                    parent->color = BLACK;
                    grandparent->color = RED;
                    _left_rotate(tree, grandparent);
                }
            }
        }
    }

    tree->root->color = BLACK;
}

void _do_insert(RBTree *tree, struct RBNode *node)
{
    struct RBNode *tmp_node = 0;
    struct RBNode *walk_node = tree->root;
    int cmp;

    while (walk_node != 0)
    {
        tmp_node = walk_node;
        cmp = tree->rbt_keycmp(node->key, walk_node->key);
        walk_node = (cmp < 0) ? walk_node->left : walk_node->right;
    }

    node->parent = tmp_node;
    if (IS_NULL(tmp_node))
        tree->root = node;
    else if (tree->rbt_keycmp(node->key, tmp_node->key) < 0)
        tmp_node->left = node;
    else
        tmp_node->right = node;

    _rbtree_insert_fixup(tree, node);
}

void rbtree_insert(RBTree *tree, RBNode *new_node)
{
    //	struct RBNode* new_node = (struct RBNode*)memget(sizeof(struct RBNode));
    //	new_node->parent = 0;
    //	new_node->key = key;
    //	new_node->value = value;
    //	new_node->left = 0;
    //	new_node->color = RED;

    _do_insert(tree, new_node);
}

struct RBNode *_rbtree_minimum(struct RBNode *node)
{
    if (!IS_NULL(node))
    {
        while (node->left)
        {
            node = node->left;
        }

        return node;
    }
    return 0;
}

void _rbtree_transplant(RBTree *tree, struct RBNode *old_node,
                        struct RBNode *new_node)
{
    if (IS_NULL(old_node->parent))
        tree->root = new_node;
    else if (old_node == old_node->parent->left)
        old_node->parent->left = new_node;
    else
        old_node->parent->right = new_node;

    if (!IS_NULL(new_node))
    {
        new_node->parent = old_node->parent;
    }
}

void _rbtree_delete_fixup(RBTree *tree, struct RBNode *node,
                          struct RBNode *node_parent, int direction)
{
    while (node != tree->root && !IS_RED(node))
    {
        struct RBNode *brother = 0;
        struct RBNode *parent =
            (node == 0) ? node_parent : node->parent;
        if (node == parent->left || direction == LEFT)
        {
            brother = parent->right;
            if (IS_RED(brother))
            {
                brother->color = BLACK;
                parent->color = RED;
                _left_rotate(tree, parent);
                brother = parent->right;
            }

            if (!IS_RED(brother->left) && !IS_RED(brother->right))
            {
                brother->color = RED;
                node =
                    (node == 0) ? node_parent : node->parent;
            }
            else if (!IS_RED(brother->right))
            {
                brother->color = RED;
                brother->left->color = BLACK;
                _right_rotate(tree, brother);
                brother = parent->right;
            }
            else
            {
                parent->color = BLACK;
                brother->color = RED;
                brother->right->color = BLACK;
                _left_rotate(tree, parent);
                node = tree->root;
            }
        }
        else
        {
            brother = parent->left;

            if (IS_RED(brother))
            {
                brother->color = BLACK;
                parent->color = RED;
                _right_rotate(tree, parent);
                brother = parent->left;
            }

            if (!IS_RED(brother->right) && !IS_RED(brother->left))
            {
                brother->color = RED;
                node =
                    (node == 0) ? node_parent : node->parent;
            }
            else if (!IS_RED(brother->left))
            {
                brother->color = RED;
                brother->right->color = BLACK;
                _left_rotate(tree, brother);
                brother = parent->left;
            }
            else
            {
                parent->color = BLACK;
                brother->color = RED;
                brother->left->color = BLACK;
                _right_rotate(tree, parent);
                node = tree->root;
            }
        }
    }

    if (!IS_NULL(node))
        node->color = BLACK;
}

void _do_delete(RBTree *tree, struct RBNode *node)
{
    struct RBNode *walk_node = node;
    int tmp_color = walk_node->color;
    struct RBNode *need_fixup_node = 0;
    struct RBNode *need_fixup_node_parent = 0;
    int direction = 0;

    if (IS_NULL(node->left))
    {
        need_fixup_node = node->right;
        need_fixup_node_parent =
            IS_NULL(need_fixup_node) ? node->parent : node;
        direction = RIGHT;
        _rbtree_transplant(tree, node, need_fixup_node);
        if (IS_NULL(need_fixup_node))
            need_fixup_node_parent = node->parent;
    }
    else if (IS_NULL(node->right))
    {
        need_fixup_node = node->left;
        need_fixup_node_parent = node;
        direction = LEFT;
        _rbtree_transplant(tree, node, need_fixup_node);
    }
    else
    {
        walk_node = _rbtree_minimum(node->right);
        tmp_color = walk_node->color;
        need_fixup_node = walk_node->right;
        need_fixup_node_parent = walk_node;
        direction = RIGHT;
        if (walk_node->parent != node)
        {
            struct RBNode *tmp_node = walk_node->right;
            _rbtree_transplant(tree, walk_node, tmp_node);
            if (IS_NULL(need_fixup_node))
                need_fixup_node_parent = walk_node->parent;
            walk_node->right = node->right;
            walk_node->right->parent = walk_node;
        }
        _rbtree_transplant(tree, node, walk_node);
        walk_node->left = node->left;
        walk_node->left->parent = walk_node;
        walk_node->color = node->color;
    }
    if (tmp_color == BLACK)
        _rbtree_delete_fixup(tree, need_fixup_node,
                             need_fixup_node_parent, direction);
    free(node);
}

void *rbtree_del(RBTree *tree, void *key)
{
    struct RBNode *tmp_node = tree->root;
    while (!IS_NULL(tmp_node))
    {
        //		cmp = tree->rbt_keycmp(key, tmp_node->key);
        //		if (cmp > 0)
        //			tmp_node = tmp_node->right;
        //		else if (cmp < 0)
        //			tmp_node = tmp_node->left;
        //		else {
        //			_do_delete(tree, tmp_node);
        //			return key;
        //		}

        if (key > tmp_node->key)
            tmp_node = tmp_node->right;
        else if (key < tmp_node->key)
            tmp_node = tmp_node->left;
        else
        {
            _do_delete(tree, tmp_node);
            return key;
        }
    }

    return 0;
}