#include <QtCore/QDebug>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegularExpression>

#include <common.h>
#include <game_data/game_races.h>
#include <locale/string_table.h>

/// Player races.
QSet<QString> GameRaces::_playerRaces = {
    "argon",
    //"boron",
    "paranid", "split", "teladi",
    //"terran"
};

/**
 * @brief		Constructor.
 */
GameRaces::GameRaces(::std::shared_ptr<GameVFS>             vfs,
                     ::std::shared_ptr<GameTexts>           texts,
                     ::std::function<void(const QString &)> setTextFunc)
{
    setTextFunc(STR("STR_LOADING_RACES"));
    qDebug() << "Loading races...";

    // Open file.
    ::std::shared_ptr<GameVFS::FileReader> file
        = vfs->open("/libraries/races.xml");
    if (file == nullptr) {
        return;
    }
    QByteArray       data = file->readAll();
    QXmlStreamReader reader(data);

    // Parse file
    auto context = XMLLoader::Context::create();
    context->setOnStartElement(
        ::std::bind(&GameRaces::onStartElementInRoot, this,
                    ::std::placeholders::_1, ::std::placeholders::_2,
                    ::std::placeholders::_3, ::std::placeholders::_4));
    XMLLoader loader;
    loader["texts"] = texts;
    loader.parse(reader, ::std::move(context));

    this->setInitialized();
}

/**
 * @brief	Get race information.
 */
const GameRaces::Race &GameRaces::race(const QString &id)
{
    return m_races[id];
}

/**
 * @brief	Get player races.
 */
const QSet<QString> &GameRaces::playerRaces()
{
    return _playerRaces;
}

/**
 * @brief		Destructor.
 */
GameRaces::~GameRaces() {}

/**
 * @brief		Start element callback in root.
 */
bool GameRaces::onStartElementInRoot(XMLLoader &loader,
                                     XMLLoader::Context &,
                                     const QString &name,
                                     const QMap<QString, QString> &)
{
    if (name == "races") {
        auto context = XMLLoader::Context::create();
        context->setOnStartElement(
            ::std::bind(&GameRaces::onStartElementInRaces, this,
                        ::std::placeholders::_1, ::std::placeholders::_2,
                        ::std::placeholders::_3, ::std::placeholders::_4));
        loader.pushContext(::std::move(context));
    } else {
        loader.pushContext(XMLLoader::Context::create());
    }
    return true;
}

/**
 * @brief		Start element callback in races.
 */
bool GameRaces::onStartElementInRaces(XMLLoader &loader,
                                      XMLLoader::Context &,
                                      const QString &               name,
                                      const QMap<QString, QString> &attr)
{
    ::std::shared_ptr<GameTexts> texts
        = ::std::any_cast<::std::shared_ptr<GameTexts>>(loader["texts"]);
    if (name == "race" && attr.find("id") != attr.end()
        && attr.find("name") != attr.end()
        && attr.find("description") != attr.end()) {
        Race race = {
            attr["id"],          //< ID.
            attr["name"],        //< Name.
            attr["description"], //< Description.
        };

        m_races[race.id] = race;

        qDebug() << QString("Race \"%1\" loaded, name = \"%2\", "
                            "description = \"%3\".")
                        .arg(race.id)
                        .arg(texts->text(race.name))
                        .arg(texts->text(race.description))
                        .toStdString()
                        .c_str();
    }
    loader.pushContext(XMLLoader::Context::create());
    return true;
}
