#pragma once

#include <memory>

#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QVBoxLayout>

#include <common/generic_string.h>
#include <common/multi_threading.h>
#include <save/save.h>
#include <ui/main_window/editor_widget/group_item.h>
#include <ui/main_window/editor_widget/module_item.h>
#include <ui/main_window/editor_widget/warning_widget.h>
#include <ui/main_window/info_widget/info_widget.h>
#include <ui/main_window/main_window.h>
#include <ui/main_window/station_modules_widget/station_modules_widget.h>

/**
 * @brief		Save editor widget.
 */
class EditorWidget : public QWidget {
    Q_OBJECT;

  private:
    class Operation;
    template<typename T, typename... Args>
    class OperationBase;
    class AddGroupOperation;
    class AddModuleOperation;
    class ChangeModuleAmountOperation;
    class MoveGroupOperation;
    class MoveModuleOperation;
    class PasteGroupOperation;
    class PasteModuleOperation;
    class RemoveOperation;
    class RenameGroupOperation;

  private:
    /**
     * @brief   Module information.
     */
    struct ModuleInfo {
        ModuleItem *      moduleItem;   ///< Item.
        ModuleItemWidget *moduleWidget; ///< Widget.
    };

    /**
     * @brief   Summary information.
     */
    struct SummaryInfo {
        quint64 hull            = 0; ///< Hull.
        quint64 explosionDamage = 0; ///< Explosion damage.
        struct {
            quint64 sLaunchTube = 0; ///< S launch tube..
            quint64 mLaunchTube = 0; ///< M launch tube.
            quint64 mTurret     = 0; ///< M turret.
            quint64 lTurret     = 0; ///< L turret.
        } weapons;                   ///< Weapons.
        struct {
            quint64 mShield = 0; ///< M shield.
            quint64 lShield = 0; ///< L shield.
        } shields;               ///< Shields
        struct {
            quint64 container = 0; ///< Container.
            quint64 solid     = 0; ///< Solid.
            quint64 liquid    = 0; ///< Liquid.
        } Storage;                 ///< Storage.
        struct {
            quint64 m_sDock   = 0; ///< S dock.
            quint64 m_mDock   = 0; ///< M dock.
            quint64 m_lDock   = 0; ///< L dock.
            quint64 m_xlDock  = 0; ///< XL dock.
            quint64 m_lXLDock = 0; ///< L/XL dock.
        } dockingBay;              ///< Docking bay.
        struct {
            quint64 m_sShipCargo = 0;               ///< S ship cargo.
            quint64 m_mShipCargo = 0;               ///< M ship cargo.
        } ShipStorage;                              ///< Ship storage.
        qint64                       workforce;     ///< Workforce.
        QMap<QString, Range<qint64>> resources;     ///< Resources.
        QMap<QString, Range<qint64>> intermediates; ///< Intermediates.
        QMap<QString, Range<qint64>> products;      ///< Products.

        struct {
            bool requireContainerStorage
                = false;                       ///< Require container storage.
            bool requireSolidStorage  = false; ///< Require solid storage.
            bool requireLiquidStorage = false; ///< Require liquid storage.
        } requirements;                        ///< Requirements.
    };

  private:
    StationModulesWidget *m_stationModulesWidget; ///< Station modules widget.
    InfoWidget *          m_infoWidget;           ///< Info widget.

    ::std::shared_ptr<Save>  m_save;            ///< Save file.
    int                      m_savedUndoCount;  ///< Undo stack size when saved.
    MainWindow::FileActions *m_fileActions;     ///< File actions.
    MainWindow::EditActions *m_editActions;     ///< Edit actions.
    BackgroundTask *         m_backgroundTasks; ///< Background tasks.

    QVBoxLayout *m_layout; ///< Layout.

    QVector<WarningWidget *> m_widgetsWarningInfos; ///< Warning informations.
    QTreeWidget *            m_treeEditor;          ///< Editor.

    // Operation stack.
    QVector<::std::shared_ptr<Operation>> m_undoStack; ///< Undo stack.
    QVector<::std::shared_ptr<Operation>> m_redoStack; ///< Redo stack.

    // Items
    QTreeWidgetItem *m_itemGroups; ///< Station module groups.

    QTreeWidgetItem *m_itemSummary;         ///< Summary.
    QTreeWidgetItem *m_itemHull;            ///< Hull.
    QTreeWidgetItem *m_itemExplosionDamage; ///< Explosion damage.
    QTreeWidgetItem *m_itemWeapons;         ///< Weapons.
    QTreeWidgetItem *m_itemShields;         ///< Shields.
    QTreeWidgetItem *m_itemStorage;         ///< Storage.
    QTreeWidgetItem *m_itemDockingbay;      ///< Docking bay.
    QTreeWidgetItem *m_itemWorkforce;       ///< Workforce.
    QTreeWidgetItem *m_itemResources;       ///< Resources.
    QTreeWidgetItem *m_itemIntermediates;   ///< Intermediates.
    QTreeWidgetItem *m_itemProducts;        ///< Products.

  private:
    static QMap<QString, EditorWidget *> _opendFiles; ///< Opened files.

  public:
    /**
     * @brief       Get editor widget by path.
     *
     * @param[in]   path        Path.
     *
     * @return      If the file is opened, the editor widget of the file is
     *              returned, otherwise returns \c nullptr.
     */
    static EditorWidget *getEditorWidgetByPath(const QString &path);

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	save			        Save file.
     * @param[in]	fileActions		        File actions.
     * @param[in]	editActions		        Edit actions.
     * @param[in]	infoWidget              Info widget.
     * @param[in]	stationModulesWidget    Station modules widget.
     * @param[in]	parent			        Parent.
     */
    EditorWidget(::std::shared_ptr<Save>  save,
                 MainWindow::FileActions *fileActions,
                 MainWindow::EditActions *editActions,
                 InfoWidget *             infoWidget,
                 StationModulesWidget *   stationModulesWidget,
                 QMdiSubWindow *          parent);

    /**
     * @brief	Destructors.
     */
    virtual ~EditorWidget();

  public:
    /**
     * @brief	Close save file.
     *
     * @return	\c true if the file closed, otherwise returns false.
     */
    bool closeSave();

    /**
     * @brief	Load groups.
     */
    void loadGroups();

  signals:
    /**
     * @brief       Emit when enable status of button "Add to Station" should
     *              be changed,
     *
     * @param[in]   status      Enable status.
     */
    void addToStationStatusChaged(bool status);

  private slots:
    /**
     * @brief		Do operation.
     *
     * @param[in]	operation		Operation.
     */
    void doOperation(::std::shared_ptr<Operation> operation);

    /**
     * @brief       Update window title.
     */
    void updateTitle();

    /**
     * @brief       Update button "Save" status.
     */
    void updateSaveStatus();

    /**
     * @brief       Update button "Add to Station" status.
     */
    void updateAddToStationStatus();

    /**
     * @brief       Update undo/redo action status.
     */
    void updateUndoRedoStatus();

    /**
     * @brief       Update cut/copy/remove action status.
     */
    void updateCutCopyRemoveStatus();

    /**
     * @brief       Update paste action status.
     */
    void updatePasteStatus();

    /**
     * @brief       Update move button status.
     *
     * @param[in]   item        Group item.
     * @param[in]   itemWidget  Item widge.
     */
    void updateGroupMoveButtonStatus(GroupItem *      item,
                                     GroupItemWidget *itemWidget = nullptr);

    /**
     * @brief       Update move button status.
     *
     * @param[in]   item        Module item.
     * @param[in]   itemWidget  Item widget.
     */
    void updateModuleMoveButtonStatus(ModuleItem *      item,
                                      ModuleItemWidget *itemWidget = nullptr);

    /**
     * @brief       Update summary.
     */
    void updateSummary();

  private:
    /**
     * @brief		Close event.
     *
     * @param[in]	event		Event.
     */
    virtual void closeEvent(QCloseEvent *event) override;

  public slots:
    /**
     * @brief	    Add module.
     *
     * @param[in]   macros       Macros of the modules.
     */
    void addModules(const QStringList &macros);

    /**
     * @brief		Create new group.
     */
    void newGroup();

    /**
     * @brief		Undo.
     */
    void undo();

    /**
     * @brief		Redo.
     */
    void redo();

    /**
     * @brief		Cut.
     */
    void cut();

    /**
     * @brief		Copy.
     */
    void copy();

    /**
     * @brief		Paste.
     */
    void paste();

    /**
     * @brief		Remove.
     */
    void remove();

    /**
     * @brief		Remove group item.
     *
     * @param[in]   item    Item.
     */
    void removeGroupItem(GroupItem *item);

    /**
     * @brief		Remove module item.
     *
     * @param[in]   item    Item.
     */
    void removeModuleItem(ModuleItem *item);

    /**
     * @brief		Save.
     */
    void save();

    /**
     * @brief		Save as.
     */
    void saveAs();

    /**
     * @brief		Change language.
     */
    void onLanguageChanged();

  private slots:
    /**
     * @brief	Clear all warning informations.
     */
    void clearInfo();

    /**
     * @brief		Show error information.
     *
     * @param[in]	id		String id.
     */
    void addWarning(QString id);

    /**
     * @brief		Called when item changed.
     *
     * @param[in]	item	Item.
     * @param[in]	column	Column.
     */
    void onItemChanged(QTreeWidgetItem *item, int column);

    /**
     * @brief		Called when item double clicked.
     *
     * @param[in]	item	Item.
     * @param[in]	column	Column.
     */
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);

    /**
     * @brief	    On amount changed.
     *
     * @param[in]   oldAmount       Old amount.
     * @param[in]   newAmount       New amount.
     * @param[in]	moduleItem	    Module item.
     */
    void onChangeAmount(quint64     oldAmount,
                        quint64     newAmount,
                        ModuleItem *moduleItem);

    /**
     * @brief		Called when move up button of a group item clicked.
     *
     * @param[in]	item	Item.
     */
    void onGroupMoveUp(GroupItem *item);

    /**
     * @brief		Called when move down button of a group item clicked.
     *
     * @param[in]	item	Item.
     */
    void onGroupMoveDown(GroupItem *item);

    /**
     * @brief		Called when move up button of a module item clicked.
     *
     * @param[in]	item	Item.
     */
    void onModuleMoveUp(ModuleItem *item);

    /**
     * @brief		Called when move up button of a module item clicked.
     *
     * @param[in]	item	Item.
     */
    void onModuleMoveDown(ModuleItem *item);

    /**
     * @brief		Called when request a context menu.
     *
     * @param[in]	pos     Position.
     */
    void onCustomContextMenuRequested(const QPoint &pos);

  public:
    /**
     * @brief       Active editor widget.
     */
    void active();
};

#include <ui/main_window/editor_widget/operation.h>
