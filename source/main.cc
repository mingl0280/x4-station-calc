#include <cstdlib>
#include <cstring>


#include <QtWidgets/QApplication>
#include <QtGui/QFontDatabase>
#include <QtCore/QStandardPaths>

#include <common.h>
#include <config.h>
#include <game_data/game_data.h>
#include <global.h>
#include <locale/string_table.h>
#include <open_file_listener.h>
#include <ui/language_setting_dialog.h>
#include <ui/license_dialog.h>
#include <ui/main_window/main_window.h>
#include <ui/splash/splash_widget.h>

#if QT_VERSION_MAJOR>=6
#else
#include <QtCore/QTextCodec>
#endif

/**
 * @brief	Called when the first time to run.
 */
int firstRun()
{
    // Select language
    LanguageSettingDialog langDlg;
    if (langDlg.exec() != QDialog::DialogCode::Accepted) {
        return 1;
    }

    // Show license
    LicenseDialog licenseDlg;
    if (licenseDlg.exec() != QDialog::DialogCode::Accepted) {
        return 1;
    }

    return 0;
}

/**
 * @brief Pick a safe font.
 * 
 * @return QFont 
 */
QFont pickSafeFontForLocale()
{
    QLocale locale;
    QFontDatabase fontDb;
    QStringList availableFonts = fontDb.families();

    // Map language to preferred fonts
    const QMap<QLocale::Language, QStringList> languageFontMap = {
        { QLocale::Chinese,   { "microsoft yahei ui light", "SimSun", "Noto Sans SC", "microsoft yahei" } },
        { QLocale::Japanese,  { "Yu Gothic", "MS UI Gothic", "Noto Sans JP" } },
        { QLocale::Korean,    { "Malgun Gothic", "Noto Sans KR" } },
        { QLocale::Russian,   { "Segoe UI", "Arial" } },
        { QLocale::English,   { "Segoe UI", "Arial", "Helvetica" } },
        { QLocale::German,    { "Segoe UI", "Arial" } },
        { QLocale::French,    { "Segoe UI", "Arial" } },
        { QLocale::Arabic,    { "Segoe UI", "Arial", "Amiri" } },
        { QLocale::Hebrew,    { "Segoe UI", "Arial", "David" } },
        // Add more as needed...
    };

    QStringList preferredFonts = languageFontMap.value(locale.language(), {
        "Segoe UI", "Arial", "Helvetica", "Sans Serif"
    });

    QStringList normalizedAvailableFonts;
    for (const QString &f : availableFonts)
        normalizedAvailableFonts << f.toLower();

    for (const QString &f : normalizedAvailableFonts)
        qDebug() << f;

    for (const QString &fontName : preferredFonts)
    {
        if (normalizedAvailableFonts.contains(fontName.toLower()))
        {
            qDebug() << "Using font:" << fontName;
            return QFont(fontName, 10);
        }
    }

    qDebug() << "No preferred fonts found. Falling back to default.";
    return QApplication::font();
}


/**
 * @brief		Entery.
 *
 * @param[in]	argc		Count of arguments.
 * @param[in]	argv		Values of arguments.
 *
 * @return		Exit code.
 */
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

#if QT_VERSION_MAJOR < 6
    // Force UTF-8.
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif
    // High DPI support.
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    int          exitCode;
    int          fakeArgc   = 1;
    char *       fakeArgv[] = {argv[0], NULL};
    QApplication app(fakeArgc, fakeArgv);
    app.setFont(pickSafeFontForLocale());
    QFontDatabase fdb;
    qDebug() << "Fonts:" << fdb.families();
    qDebug() << "Fonts Location: " << QStandardPaths::writableLocation(QStandardPaths::FontsLocation);
    
    app.setApplicationName("X4 Station Calculator");
    

    // Initialize.
    if (Global::initialize(argc, argv, exitCode) == nullptr) {
        return exitCode;
    }

    if (Config::initialize() == nullptr) {
        return 1;
    }

    if (StringTable::initialize() == nullptr) {
        return 1;
    }

    if (OpenFileListener::initialize() == nullptr) {
        return 1;
    } else {
        if (OpenFileListener::instance()->opened()) {
            return 0;
        }
    }

    // Check if it is the first time to run.
    if (Config::instance()->getBool("/firstRun", true)) {
        exitCode = firstRun();
        if (exitCode != 0) {
            return exitCode;
        }
        Config::instance()->setBool("/firstRun", false);
    }

    // Show splash and load data.
    SplashWidget splash;
    int          ret = splash.exec([&]() -> int {
        // Load game data.
        if (GameData::initialize(&splash) == nullptr) {
            return 1;
        } else {
            return 0;
        }
    });
    if (ret != 0) {
        return ret;
    }

    // Show main window.
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
