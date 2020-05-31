#pragma once

#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>

#include <ui/main_window/info_widget/info_widget.h>
#include <ui/main_window/station_modules_widget/station_modules_widget.h>

/**
 * @brief	Main window.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

  private:
    // Menu
    QMenuBar *m_mainMenu; ///< Main menu.

    // File menu
    QMenu *   m_menuFile;        ///< Menu "File".
    QToolBar *m_toolbarFile;     ///< Toolbar "File".
    QAction * m_acionFileNew;    ///< Menu "File->New".
    QAction * m_acionFileOpen;   ///< Menu "File->Open".
    QAction * m_acionFileSave;   ///< Menu "File->Save".
    QAction * m_acionFileSaveAs; ///< Menu "File->Save As".
    QAction * m_acionFileClose;  ///< Menu "File->Close".
    QAction * m_acionFileExit;   ///< Menu "File->Exit".

    // Edit menu
    QMenu *   m_menuEdit;          ///< Menu "Edit".
    QToolBar *m_toolbarEdit;       ///< Toolbar "Edit".
    QAction * m_acionEditNewGroup; ///< Menu "Edit->New Group".
    QAction * m_acionEditUndo;     ///< Menu "Edit->Undo".
    QAction * m_acionEditRedo;     ///< Menu "Edit->Redo".
    QAction * m_acionEditCut;      ///< Menu "Edit->Cut".
    QAction * m_acionEditCopy;     ///< Menu "Edit->Copy".
    QAction * m_acionEditPaste;    ///< Menu "Edit->Paste".
    QAction * m_acionEditRemove;   ///< Menu "Edit->Remove".

    // Setting menu
    QMenu *  m_menuSettings;           ///< Menu "Settings".
    QMenu *  m_menuSettingsLanguage;   ///< Menu "Settings->Language".
    QAction *m_actionSetttingGamePath; ///< Menu "Settings->Game Path".

    // View menu
    QMenu *  m_menuView;                 ///< Menu "View".
    QAction *m_actionViewStationModules; ///< Menu "View->Station Modules".
    QAction *m_actionViewInfo;           ///< Menu "View->Info".

    // Help menu
    QMenu *  m_menuHelp;        ///< Menu "Help".
    QAction *m_helpAbout;       ///< Menu "Help->About".
    QAction *m_helpCheckUpdate; ///< Menu "Help->Check Update".

    // Dock widgets
    // Station module widget.
    StationModulesWidget *m_stationModulesWidget; ///< Station modules widget.

    // Info widget.
    InfoWidget *m_infoWidget; ///< Information widget.

    // Central widget
    QTabWidget *m_centralWidget; ///< Central widget.

  public:
    /**
     * @brief		Constructor of main window.
     */
    MainWindow();

    /**
     * @brief		Destructor of main window.
     */
    virtual ~MainWindow();

  private:
    /**
     * @brief	Initialize Menu.
     */
    void initMenuToolBar();

    /**
     * @brief		Close event.
     *
     * @param[in]	event		Event.
     */
    virtual void closeEvent(QCloseEvent *event) override;

  private slots:
    /**
     * @brief		Open file.
     *
     * @param[in]	path		Path of file.
     */
    void open(QString path);

    /**
     * @brief		Active window.
     */
    void active();

  protected slots:
    /**
     * @brief		Change language.
     */
    void onLanguageChanged();
};