#pragma once

#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QLabel>

#include <save/save.h>
#include <ui/controls/scroll_area_auto_fill.h>

/**
 * @brief   New factory wizard.
 */
class NewFactoryWizardCentralWidget;

/**
 * @brief   New factory wizard.
 */
class NewFactoryWizard : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief   Product infomation.
     */
    struct ProductInfo {
        QString ware;       ///< Ware.
        quint64 production; ///< Production.
    };

    /**
     * @brief   Workforce infomation.
     */
    struct WorkforceInfo {
        QString race;       ///< Race.
        quint32 percentage; ///< Percentage.
    };

  private:
    /**
     * @brief   Wizard status.
     */
    enum class WizardStatus {
        None,            ///< None.
        SelectProduct,   ///< Select product.
        SetRace,         ///< Set race.
        SetIntermediate, ///< Set intermediate.
        Finish,          ///< Finish.
    };

  private:
    QVBoxLayout *m_layout;   ///< Widget layout.
    QLabel *     m_lblTitle; ///< Label title,

    ScrollAreaAutoFill *m_clientArea; ///< Client area.

    QHBoxLayout *m_layoutButton; ///< Button layout.
    QPushButton *m_btnBack;      ///< Button "Back".
    QPushButton *m_btnNext;      ///< Button "Next".
    QPushButton *m_btnFinish;    ///< Button "Finish".
    QPushButton *m_btnCancel;    ///< Button "Cancel".

    ::std::shared_ptr<Save> m_result; ///< Result.

    WizardStatus m_status; ///< Current status.

    NewFactoryWizardCentralWidget *m_centralWidget; ///< Central widget.

    QSet<QString>              m_products;         ///< Products.
    QSet<QString>              m_resource;         ///< Resources.
    QMap<QString, ProductInfo> m_selectedProducts; ///< Selected products.

    QMap<QString, WorkforceInfo> m_workforce;   ///< Workforce information.
    QSet<QString>                m_resources;   ///< Resources.
    QVector<QString> m_orderOfProductionMethod; ///< Order of production method.

  private:
    /**
     * @brief       Constructor.
     *
     * @param[in]   parent      Parent widget.
     */
    NewFactoryWizard(QWidget *parent);

    /**
     * @brief       Run wizard.
     *
     * @return      If a new save is generated, the method returns the new save.
     *              If not, \c nullptr is returned.
     */
    ::std::shared_ptr<Save> exec();

  public:
    /**
     * @brief       Show wizard.
     *
     * @param[in]   parent      Parent widget.
     *
     * @return      If a new save is generated, the method returns the new save.
     *              If not, \c nullptr is returned.
     */
    static ::std::shared_ptr<Save> showWizard(QWidget *parent = nullptr);

  public:
    /**
     * @brief       Destructor.
     */
    virtual ~NewFactoryWizard();

  private:
    /**
     * @brief       Switch to select product status.
     */
    void switchToSelectProduct();

    /**
     * @brief       Switch to set workforce status.
     */
    void switchToSetRace();

    /**
     * @brief       Switch to set intermediate status.
     */
    void switchToSetIntermediate();

    /**
     * @brief       Switch to finish.
     */
    void switchToFinish();

    /**
     * @brief       Close central widget when button \"Back\" clicked.
     */
    void closeCentralWidgetOnBtnBack();

    /**
     * @brief       Close central widget when button \"Next\" clicked.
     */
    void closeCentralWidgetOnBtnNext();

  public slots:
    /**
     * @brief       Set the enable status of button "Next".
     *
     * @param[in]   enabled     \c true if enabled, otherwise returns \c
     * false.
     */
    void setNextBtnEnabled(bool enabled);

  private slots:
    /**
     * @brief   On button "Back" clicked.
     */
    void onBtnBackClicked();

    /**
     * @brief   On button "Next" clicked.
     */
    void onBtnNextClicked();

    /**
     * @brief   On button "Finish" clicked.
     */
    void onBtnFinishClicked();

    /**
     * @brief   On button "Cancel" clicked.
     */
    void onBtnCancelClicked();
};