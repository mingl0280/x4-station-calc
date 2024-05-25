#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>

#include <common.h>
#include <interfaces/i_load_factory_func.h>

class GameVFS;

/**
 * @brief	Macros in game.
 */
class GameMacros :
    public ILoadFactoryFunc<GameMacros,
                            ::std::shared_ptr<GameVFS>,
                            ::std::function<void(const QString &)>> {
    LOAD_FUNC(GameMacros,
              ::std::shared_ptr<GameVFS>,
              ::std::function<void(const QString &)>);

  private:
    QMap<QString, QString> m_macros; ///< Macros

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameMacros(::std::shared_ptr<GameVFS>             vfs,
               ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief	Get macro.
     *
     * @return	Value of macro.
     */
    QString macro(const QString &id);

    /**
     * @brief		Destructor.
     */
    virtual ~GameMacros();

  protected:
    /**
     * @brief		Start element callback in root.
     *
     * @param[in]	loader			XML loader.
     * @param[in]	context			Context.
     * @param[in]	name			Name of the element.
     * @param[in]	attr			Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRoot(XMLLoader &                   loader,
                              XMLLoader::Context &          context,
                              const QString &               name,
                              const QMap<QString, QString> &attr);

    /**
     * @brief		Start element callback in index.
     *
     * @param[in]	loader			XML loader.
     * @param[in]	context			Context.
     * @param[in]	name			Name of the element.
     * @param[in]	attr			Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInIndex(XMLLoader &                   loader,
                               XMLLoader::Context &          context,
                               const QString &               name,
                               const QMap<QString, QString> &attr);
};

#include <game_data/game_vfs.h>
