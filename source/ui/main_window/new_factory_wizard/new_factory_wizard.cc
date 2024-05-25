#include <QtCore/QEventLoop>
#include <QtWidgets/QApplication>
#include <QScreen>

#include <game_data/game_data.h>
#include <locale/string_table.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_finish_widget.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_product_widget.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_resource_widget.h>
#include <ui/main_window/new_factory_wizard/new_factory_wizard_set_race_widget.h>

/**
 * @brief       Constructor.
 */
NewFactoryWizard::NewFactoryWizard(QWidget *parent) :
    QWidget(parent), m_status(WizardStatus::None)
{
    this->setWindowFlags(Qt::Dialog);
    this->setWindowTitle(STR("STR_TITLE_NEW_FACTORY_WIZARD"));

    m_layout = new QVBoxLayout(this);
    this->setLayout(m_layout);

    // Title.
    m_lblTitle = new QLabel(this);
    m_layout->addWidget(m_lblTitle);

    // Client area.
    m_clientArea = new ScrollAreaAutoFill(this);
    m_layout->addWidget(m_clientArea);

    // Buttons.
    m_layoutButton = new QHBoxLayout();
    m_layout->addLayout(m_layoutButton);

    m_layoutButton->addStretch();

    m_btnBack = new QPushButton(STR("STR_BTN_BACK"));
    m_layoutButton->addWidget(m_btnBack);
    m_btnBack->setEnabled(false);
    this->connect(m_btnBack, &QPushButton::clicked, this,
                  &NewFactoryWizard::onBtnBackClicked);

    m_btnNext = new QPushButton(STR("STR_BTN_NEXT"));
    m_layoutButton->addWidget(m_btnNext);
    this->connect(m_btnNext, &QPushButton::clicked, this,
                  &NewFactoryWizard::onBtnNextClicked);

    m_btnFinish = new QPushButton(STR("STR_BTN_FINISH"));
    m_layoutButton->addWidget(m_btnFinish);
    m_btnFinish->setVisible(false);
    this->connect(m_btnFinish, &QPushButton::clicked, this,
                  &NewFactoryWizard::onBtnFinishClicked);

    m_btnCancel = new QPushButton(STR("STR_BTN_CANCEL"));
    m_layoutButton->addWidget(m_btnCancel);
    this->connect(m_btnCancel, &QPushButton::clicked, this,
                  &NewFactoryWizard::onBtnCancelClicked);

    QRect windowRect = QApplication::primaryScreen()->geometry();

    windowRect.setWidth(windowRect.width() / 4 * 3);
    windowRect.setHeight(windowRect.height() / 4 * 3);
    windowRect.setX(windowRect.width() / 8);
    windowRect.setY(windowRect.height() / 8);

    this->setFixedSize(windowRect.size());

    // Scan products.
    auto wares = GameData::instance()->wares();
    for (auto stationModule :
         GameData::instance()->stationModules()->modules()) {
        if (stationModule->playerModule) {
            auto propertyIter = stationModule->properties.find(
                GameStationModules::Property::SupplyProduct);
            if (propertyIter != stationModule->properties.end()) {
                ::std::shared_ptr<GameStationModules::SupplyProduct> property
                    = ::std::static_pointer_cast<
                        GameStationModules::SupplyProduct>(*propertyIter);
                m_products.insert(property->product);
            }
        }
    }

    // Scan races.
    auto      races     = GameData::instance()->races();
    QCollator collator  = StringTable::instance()->collator();
    QString   firstRace = *(races->playerRaces().begin());
    for (auto &race : races->playerRaces()) {
        m_workforce[race] = {race, 0};
        if (collator.compare(race, firstRace) < 0) {
            firstRace = race;
        }
        m_orderOfProductionMethod.push_back(race);
    }
    m_workforce[firstRace].percentage = 100;

    // Initialize.
    this->switchToSelectProduct();
}

/**
 * @brief       Run wizard.
 */
::std::shared_ptr<Save> NewFactoryWizard::exec()
{
    this->setWindowModality(Qt::ApplicationModal);
    this->show();
    QEventLoop *eventLoop = new QEventLoop(this);
    eventLoop->exec();

    return m_result;
}

/**
 * @brief       Show wizard.
 */
::std::shared_ptr<Save> NewFactoryWizard::showWizard(QWidget *parent)
{
    NewFactoryWizard *wizard = new NewFactoryWizard(parent);
    auto              ret    = wizard->exec();
    delete wizard;
    return ret;
}

/**
 * @brief   Set the enable status of button "Next".
 */
void NewFactoryWizard::setNextBtnEnabled(bool enabled)
{
    switch (m_status) {
        case WizardStatus::SelectProduct:
        case WizardStatus::SetRace:
        case WizardStatus::SetIntermediate:
            m_btnNext->setEnabled(enabled);
            break;

        default:
            m_btnNext->setEnabled(false);
    }
}

/**
 * @brief       Destructor.
 */
NewFactoryWizard::~NewFactoryWizard() {}

/**
 * @brief       Switch to select product status.
 */
void NewFactoryWizard::switchToSelectProduct()
{
    m_btnNext->setEnabled(true);
    m_btnNext->setVisible(true);
    m_btnBack->setVisible(true);
    m_btnBack->setEnabled(false);
    m_btnFinish->setVisible(false);
    m_btnFinish->setEnabled(false);

    m_centralWidget = new NewFactoryWizardProductWidget(this, m_products,
                                                        m_selectedProducts);
    m_clientArea->setWidget(m_centralWidget);
    m_status = WizardStatus::SelectProduct;

    m_lblTitle->setText(m_centralWidget->windowTitle());
}

/**
 * @brief       Switch to select workforce status.
 */
void NewFactoryWizard::switchToSetRace()
{
    m_btnNext->setEnabled(true);
    m_btnNext->setVisible(true);
    m_btnBack->setVisible(true);
    m_btnBack->setEnabled(true);
    m_btnFinish->setVisible(false);
    m_btnFinish->setEnabled(false);

    m_centralWidget = new NewFactoryWizardSetRaceWidget(
        m_workforce, m_orderOfProductionMethod, this);
    m_clientArea->setWidget(m_centralWidget);
    m_status = WizardStatus::SetRace;

    m_lblTitle->setText(m_centralWidget->windowTitle());
}

/**
 * @brief       Switch to select intermediate status.
 */
void NewFactoryWizard::switchToSetIntermediate()
{
    m_btnNext->setEnabled(true);
    m_btnNext->setVisible(true);
    m_btnBack->setVisible(true);
    m_btnBack->setEnabled(true);
    m_btnFinish->setVisible(false);
    m_btnFinish->setEnabled(false);

    m_centralWidget = new NewFactoryWizardResourceWidget(
        this, m_selectedProducts, m_workforce, m_orderOfProductionMethod,
        m_resources);
    m_clientArea->setWidget(m_centralWidget);
    m_status = WizardStatus::SetIntermediate;

    m_lblTitle->setText(m_centralWidget->windowTitle());
}

/**
 * @brief       Switch to show result status.
 */
void NewFactoryWizard::switchToFinish()
{
    m_btnNext->setEnabled(false);
    m_btnNext->setVisible(false);
    m_btnBack->setVisible(true);
    m_btnBack->setEnabled(true);
    m_btnFinish->setVisible(true);
    m_btnFinish->setEnabled(true);

    m_centralWidget = new NewFactoryWizardFinishWidget(this);
    m_clientArea->setWidget(m_centralWidget);
    m_status = WizardStatus::Finish;

    m_lblTitle->setText(m_centralWidget->windowTitle());
}

/**
 * @brief       Close central widget when button \"Back\" clicked.
 */
void NewFactoryWizard::closeCentralWidgetOnBtnBack()
{
    if (m_centralWidget != nullptr) {
        m_centralWidget->onNextStep();
        m_centralWidget->close();
        delete m_centralWidget;
        m_centralWidget = nullptr;
    }
}

/**
 * @brief       Close central widget when button \"Next\" clicked.
 */
void NewFactoryWizard::closeCentralWidgetOnBtnNext()
{
    if (m_centralWidget != nullptr) {
        m_centralWidget->onPrevStep();
        m_centralWidget->close();
        delete m_centralWidget;
        m_centralWidget = nullptr;
    }
}

/**
 * @brief   On button "Back" clicked.
 */
void NewFactoryWizard::onBtnBackClicked()
{
    switch (m_status) {
        case WizardStatus::SetRace:
            this->closeCentralWidgetOnBtnBack();
            this->switchToSelectProduct();
            break;

        case WizardStatus::SetIntermediate:
            this->closeCentralWidgetOnBtnBack();
            this->switchToSetRace();
            break;

        case WizardStatus::Finish:
            this->closeCentralWidgetOnBtnBack();
            this->switchToSetIntermediate();
            break;

        case WizardStatus::SelectProduct:
        default:
            break;
    }
}

/**
 * @brief   On button "Next" clicked.
 */
void NewFactoryWizard::onBtnNextClicked()
{
    switch (m_status) {
        case WizardStatus::SelectProduct:
            this->closeCentralWidgetOnBtnNext();
            this->switchToSetRace();
            break;

        case WizardStatus::SetRace:
            this->closeCentralWidgetOnBtnNext();
            this->switchToSetIntermediate();
            break;

        case WizardStatus::SetIntermediate:
            this->closeCentralWidgetOnBtnNext();
            this->switchToFinish();
            break;

        case WizardStatus::Finish:
        default:
            break;
    }
}

/**
 * @brief   On button "Finish" clicked.
 */
void NewFactoryWizard::onBtnFinishClicked()
{
    this->close();
}

/**
 * @brief   On button "Cancel" clicked.
 */
void NewFactoryWizard::onBtnCancelClicked()
{
    this->close();
}
