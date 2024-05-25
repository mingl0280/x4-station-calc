#pragma once

#include <QtCore/QMap>
#include <QAction>
#include <QActionGroup>
#include <QtWidgets/QMenu>

#include <locale/string_table.h>

/**
 * @brief	Language menu.
 */
class LanguageMenu : public QMenu {
    Q_OBJECT
  protected:
    QMap<QAction *, QString> m_localeMap;   ///< Map action and value.
    QActionGroup *           m_actionGroup; ///< Action group of the languages.

  public:
    /**
     * @brief		Constructor.
     *
     * @param[in]	parent		Parent.
     */
    LanguageMenu(QWidget *parent = nullptr);

    /**
     * @brief		Destructor.
     */
    virtual ~LanguageMenu();

  private slots:
    /**
     * @brief		Selected language changed.
     *
     * @param[in]	action		Selected action.
     */
    void onSelectedChanged(QAction *action);
};
