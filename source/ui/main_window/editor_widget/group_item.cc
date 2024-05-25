#include <ui/main_window/editor_widget/group_item.h>

/**
 * @brief		Constructor.
 */
GroupItem::GroupItem(::std::shared_ptr<SaveGroup> group) : m_group(group)
{
    // Style.
    this->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemIsSelectable
                   | Qt::ItemIsEditable);
    this->updateGroupName();
}

/**
 * @brief		Get group.
 */
::std::shared_ptr<SaveGroup> GroupItem::group()
{
    return m_group;
}

/**
 * @brief		Update group name.
 */
void GroupItem::updateGroupName()
{
    this->setText(0, m_group->name());
}

/**
 * @brief		Destructor.
 */
GroupItem::~GroupItem() {}

/**
 * @brief       Appends the child item to the list of children.
 */
bool GroupItem::addChild(ModuleItem *child)
{
    if (m_macroMap.find(child->module()->module()) == m_macroMap.end()) {
        this->QTreeWidgetItem::addChild(child);
        m_macroMap[child->module()->module()] = child;
        return true;
    } else {
        return false;
    }
}

/**
 * @brief       Returns the item at the given index in the list of the
 *              item's children.
 */
ModuleItem *GroupItem::child(int index) const
{
    return dynamic_cast<ModuleItem *>(this->QTreeWidgetItem::child(index));
}

/**
 * @brief       Returns the item which macro of the module is \c macro in
 *              the list of the item's children.
 */
ModuleItem *GroupItem::child(const QString &macro) const
{
    auto iter = m_macroMap.find(macro);

    if (iter == m_macroMap.end()) {
        return nullptr;
    } else {
        return *iter;
    }
}

/**
 * @brief       Get the index of the child.
 */
int GroupItem::indexOfChild(ModuleItem *child) const
{
    return this->QTreeWidgetItem::indexOfChild(child);
}

/**
 * @brief       Insert child.
 */
bool GroupItem::insertChild(int index, ModuleItem *child)
{
    if (m_macroMap.find(child->module()->module()) == m_macroMap.end()) {
        this->QTreeWidgetItem::insertChild(index, child);
        m_macroMap[child->module()->module()] = child;
        return true;
    } else {
        return false;
    }
}

/**
 * @brief       Remove child.
 */
void GroupItem::removeChild(ModuleItem *child)
{
    m_macroMap.remove(child->module()->module());
    this->QTreeWidgetItem::removeChild(child);
}

/**
 * @brief       Take child.
 */
ModuleItem *GroupItem::takeChild(int index)
{
    ModuleItem *ret
        = dynamic_cast<ModuleItem *>(this->QTreeWidgetItem::takeChild(index));

    if (ret != nullptr) {
        m_macroMap.remove(ret->module()->module());
    }

    return ret;
}

/**
 * @brief		Constructor.
 */
GroupItemWidget::GroupItemWidget(GroupItem *item) : m_item(item)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    m_layout = new QHBoxLayout();
    this->setLayout(m_layout);

    m_btnUp = new SquareButton(QIcon(":/Icons/Up.png"));
    m_layout->addWidget(m_btnUp, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnUp, &QPushButton::clicked, this,
                  &GroupItemWidget::onBtnUpClicked);

    m_btnDown = new SquareButton(QIcon(":/Icons/Down.png"));
    m_layout->addWidget(m_btnDown, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnDown, &QPushButton::clicked, this,
                  &GroupItemWidget::onBtnDownClicked);

    m_btnRemove = new SquareButton(QIcon(":/Icons/EditRemove.png"));
    m_layout->addWidget(m_btnRemove, Qt::AlignmentFlag::AlignLeft);
    this->connect(m_btnRemove, &QPushButton::clicked, this,
                  &GroupItemWidget::onBtnRemoveClicked);

    m_layout->addStretch();
    int margin_top = 0, margin_bottom = 0;
    m_layout->getContentsMargins(nullptr, &margin_top, nullptr, &margin_bottom);
    this->setMaximumHeight(this->fontMetrics().height()
                           + margin_top + margin_bottom);
}

/**
 * @brief		Destructor.
 */
GroupItemWidget::~GroupItemWidget() {}

/**
 * @brief		Set enable status of "up" button.
 */
void GroupItemWidget::setUpBtnEnabled(bool enabled)
{
    m_btnUp->setEnabled(enabled);
}

/**
 * @brief		Set enable status of "down" button.
 */
void GroupItemWidget::setDownBtnEnabled(bool enabled)
{
    m_btnDown->setEnabled(enabled);
}

/**
 * @brief	On "up" button clicked.
 */
void GroupItemWidget::onBtnUpClicked()
{
    emit this->btnUpClicked(m_item);
}

/**
 * @brief	On "down" button clicked.
 */
void GroupItemWidget::onBtnDownClicked()
{
    emit this->btnDownClicked(m_item);
}

/**
 * @brief	On "remove" button clicked.
 */
void GroupItemWidget::onBtnRemoveClicked()
{
    emit this->btnRemoveClicked(m_item);
}
