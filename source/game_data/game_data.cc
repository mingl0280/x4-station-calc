#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <config.h>
#include <game_data/game_data.h>
#include <game_data/game_texts.h>
#include <locale/string_table.h>

/**
 * @brief		Constructor.
 */
GameData::GameData(SplashWidget* splash) : QObject(nullptr)
{
    while (true) {
        splash->setText(STR("STR_CHECKING_GAME_PATH"));

        // Check game path
        m_gamePath = Config::instance()->getString("/gamePath", "");
        QMap<QString, GameVFS::CatFileInfo> catFiles;
        if (!checkGamePath(m_gamePath, catFiles)) {
            if (splash->callFunc(::std::function<bool()>(
                ::std::bind(&GameData::askGamePath, this)))) {
                continue;
            }
            else {
                return;
            }
        }

        // Load vfs
        splash->setText(STR("STR_LOADING_VFS"));
        ::std::shared_ptr<GameVFS> vfs = GameVFS::create(
            m_gamePath, catFiles,
            [&](const QString& s) -> void {
                splash->setText(STR("STR_LOADING_VFS") + "\n" + s);
            },
            [&](const QString& s) -> void {
                splash->callFunc(::std::function<void()>([&]() -> void {
                    QMessageBox::critical(splash, STR("STR_ERROR"), s);
                    }));
            });
        if (vfs == nullptr) {
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load text
        ::std::shared_ptr<GameTexts> texts
            = GameTexts::load(vfs, [&](const QString& s) -> void {
            splash->setText(STR("STR_LOADING_TEXTS") + "\n" + s);
                });

        if (texts == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                STR("STR_FAILED_LOAD_STRINGS"));
                }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load game macros
        ::std::shared_ptr<GameMacros> macros
            = GameMacros::load(vfs, [&](const QString& s) -> void {
            splash->setText(s);
                });

        if (macros == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                STR("STR_FAILED_LOAD_MACROS"));
                }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load game components
        ::std::shared_ptr<GameComponents> components
            = GameComponents::load(vfs, [&](const QString& s) -> void {
            splash->setText(s);
                });

        if (components == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                STR("STR_FAILED_LOAD_COMPONENTS"));
                }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load game races
        ::std::shared_ptr<GameRaces> races
            = GameRaces::load(vfs, texts, [&](const QString& s) -> void {
            splash->setText(s);
                });

        if (races == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                STR("STR_FAILED_LOAD_RACES"));
                }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load game wares
        ::std::shared_ptr<GameWares> wares
            = GameWares::load(vfs, texts, [&](const QString& s) -> void {
            splash->setText(s);
                });

        if (wares == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                STR("STR_FAILED_LOAD_WARES"));
                }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Load station modules
        ::std::shared_ptr<GameStationModules> stationModules
            = GameStationModules::load(vfs, macros, texts, wares, components,
                [&](const QString& s) -> void {
                    splash->setText(s);
                });

        if (stationModules == nullptr) {
            splash->callFunc(::std::function<void()>([&]() -> void {
                QMessageBox::critical(splash, STR("STR_ERROR"),
                STR("STR_FAILED_LOAD_STATION_MODULES"));
                }));
            Config::instance()->setString("/gamePath", "");
            continue;
        }

        // Set value
        m_vfs = vfs;
        m_texts = texts;
        m_macros = macros;
        m_components = components;
        m_races = races;
        m_wares = wares;
        m_stationModules = stationModules;

        break;
    }

    this->setInitialized();
}

/**
 * @brief		Check path of game.
 */
bool GameData::checkGamePath(const QString& path)
{
    QMap<QString, GameVFS::CatFileInfo> catFiles;
    return this->checkGamePath(path, catFiles);
}

/**
 * @brief		Check path of game.
 */
bool GameData::setGamePath(const QString& path)
{
    QMap<QString, GameVFS::CatFileInfo> catFiles;

    if (!checkGamePath(path, catFiles)) {
        return false;
    }

    Config::instance()->setString("/gamePath", path);
    return true;
}

/**
 * @brief Destructor.
 */
GameData::~GameData() {}

/**
 * @brief		Check path of game.
 */
bool GameData::checkGamePath(const QString& path,
    QMap<QString, GameVFS::CatFileInfo>& catFiles)
{
    // Prepare
    QDir                                dir(path);
    bool                                execFound = false;
    QMap<QString, GameVFS::CatFileInfo> catsFound;
    qDebug() << "Checking game path...";

    // List files.
    // Filters
    QRegularExpression::PatternOption options;


    static QRegularExpression execFilter(QRegularExpression::anchoredPattern("x4|x4\\.exe"), QRegularExpression::PatternOption::CaseInsensitiveOption);

    //execFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    static QRegularExpression catFilter(QRegularExpression::anchoredPattern("\\d+\\.cat"), QRegularExpression::PatternOption::CaseInsensitiveOption);
    //catFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    static QRegularExpression extCatFilter(QRegularExpression::anchoredPattern("ext_\\d+\\.cat"), QRegularExpression::PatternOption::CaseInsensitiveOption);
    //extCatFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    static QRegularExpression datFilter(QRegularExpression::anchoredPattern("\\w+\\.dat"), QRegularExpression::PatternOption::CaseInsensitiveOption);
    //datFilter.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    // Main
    for (auto& f :
        dir.entryInfoList(QDir::Filter::NoFilter, QDir::SortFlag::Name)) {
        if (execFilter.match(f.fileName()).hasMatch() && !f.isDir()) {
            qDebug() << "Game executable file :" << f.fileName() << ".";
            execFound = true;
        }
        else if (catFilter.match(f.fileName()).hasMatch() && !f.isDir()) {
            qDebug() << "Found cat file file :" << f.fileName() << ".";
            QString key = f.fileName().left(f.fileName().size() - 4);
            auto    iter = catsFound.find(key);
            if (iter == catsFound.end()) {
                catsFound[key] = GameVFS::CatFileInfo();
            }
            catsFound[key].cat = f.fileName();
        }
        else if (datFilter.match(f.fileName()).hasMatch() && !f.isDir()) {
            qDebug() << "Found dat file file :" << f.fileName() << ".";
            QString key = f.fileName().left(f.fileName().size() - 4);
            auto    iter = catsFound.find(key);
            if (iter == catsFound.end()) {
                catsFound[key] = GameVFS::CatFileInfo();
            }
            catsFound[key].dat = f.fileName();
        }
        else if (f.fileName() == "extensions" && f.isDir()) {
            // Extensions
            QDir extensionsDir(f.absoluteFilePath());
            for (auto& modEntry : extensionsDir.entryInfoList(
                QDir::Filter::NoFilter, QDir::SortFlag::Name)) {
                if (modEntry.isDir() && modEntry.fileName() != "."
                    && modEntry.fileName() != "..") {
                    QDir modDir(modEntry.absoluteFilePath());
                    for (auto& modFile : modDir.entryInfoList(
                        QDir::Filter::NoFilter, QDir::SortFlag::Name)) {
                        if (extCatFilter.match(modFile.fileName()).hasMatch()
                            && !modFile.isDir()) {
                            QString filename = QString("extensions/%1/%2")
                                                   .arg(modEntry.fileName(), modFile.fileName());
                            qDebug()
                                << "Found cat file file :" << filename << ".";
                            QString key = filename.left(filename.size() - 4);
                            auto    iter = catsFound.find(key);
                            if (iter == catsFound.end()) {
                                catsFound[key] = GameVFS::CatFileInfo();
                            }
                            catsFound[key].cat = filename;
                        }
                        else if (datFilter.match(modFile.fileName()).hasMatch()
                            && !modFile.isDir()) {
                            QString filename = QString("extensions/%1/%2")
                                                   .arg(modEntry.fileName(),modFile.fileName());
                            qDebug()
                                << "Found dat file file :" << filename << ".";
                            QString key = filename.left(filename.size() - 4);
                            auto    iter = catsFound.find(key);
                            if (iter == catsFound.end()) {
                                catsFound[key] = GameVFS::CatFileInfo();
                            }
                            catsFound[key].dat = filename;
                        }
                    }
                }
            }
        }
    }

    // Check result
    if (!execFound) {
        qDebug() << "Missing main program.";
        qDebug() << "Failed.";
        return false;
    }

    if (catsFound.size() < MIN_CAT_FILE_NUM) {
        qDebug() << "Number of cat files si not enough.";
        qDebug() << "Failed.";
        return false;
    }

    for (auto& key : catsFound.keys()) {
        if (catsFound[key].cat == "" || catsFound[key].dat == "") {
            catsFound.remove(key);
        }
    }

    qDebug() << "OK.";
    catFiles = ::std::move(catsFound);
    return true;
}

/**
 * @brief		Ask game path.
 */
bool GameData::askGamePath()
{
    QFileDialog fileDialog(nullptr, STR("STR_TITLE_SELECT_GAME_PATH"),
        m_gamePath, "*");
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::FileMode::Directory);
    fileDialog.setFilter(QDir::Filter::Dirs | QDir::Filter::Hidden
        | QDir::Filter::System);
    if (fileDialog.exec() != QDialog::DialogCode::Accepted
        || fileDialog.selectedFiles().empty()) {
        return false;
    }
    QString str = QDir(fileDialog.selectedFiles()[0]).absolutePath();
    qDebug() << "Selected:" << str;

    m_gamePath = str;
    Config::instance()->setString("/gamePath", m_gamePath);

    return true;
}

/*
 * @brief	Get game VFS.
 */
::std::shared_ptr<GameVFS> GameData::vfs()
{
    return m_vfs;
}

/*
 * @brief	Get game texts.
 */
::std::shared_ptr<GameTexts> GameData::texts()
{
    return m_texts;
}

/*
 * @brief	Get game macros.
 */
::std::shared_ptr<GameMacros> GameData::macros()
{
    return m_macros;
}

/*
 * @brief	Get game components.
 */
::std::shared_ptr<GameComponents> GameData::components()
{
    return m_components;
}

/*
 * @brief	Get game races.
 */
::std::shared_ptr<GameRaces> GameData::races()
{
    return m_races;
}

/*
 * @brief	Get game wares
 */
::std::shared_ptr<GameWares> GameData::wares()
{
    return m_wares;
}

/*
 * @brief	Get station modules.
 */
::std::shared_ptr<GameStationModules> GameData::stationModules()
{
    return m_stationModules;
}
