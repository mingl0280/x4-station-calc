#pragma once

#include <functional>
#include <memory>

#include <QtCore/QMap>
#include <QtCore/QSet>

#include <common.h>
#include <interfaces/i_load_factory_func.h>

#include <game_data/game_vfs.h>

#include <game_data/game_components.h>
#include <game_data/game_macros.h>
#include <game_data/game_texts.h>
#include <game_data/game_wares.h>

class GameVFS;

class GameMacros;
class GameTexts;
class GameWares;

/**
 * @brief	Station modules in game.
 */
class GameStationModules :
    public ILoadFactoryFunc<GameStationModules,
                            ::std::shared_ptr<GameVFS>,
                            ::std::shared_ptr<GameMacros>,
                            ::std::shared_ptr<GameTexts>,
                            ::std::shared_ptr<GameWares>,
                            ::std::shared_ptr<GameComponents>,
                            ::std::function<void(const QString &)>> {
    LOAD_FUNC(GameStationModules,
              ::std::shared_ptr<GameVFS>,
              ::std::shared_ptr<GameMacros>,
              ::std::shared_ptr<GameTexts>,
              ::std::shared_ptr<GameWares>,
              ::std::shared_ptr<GameComponents>,
              ::std::function<void(const QString &)>);

  public:
    /**
     * @brief	Station module property.
     */
    struct Property {
        /**
         * @brief		Property type.
         */
        enum Type {
            MTurret,          ///< Has M turret.
            MShield,          ///< Has M shield.
            LTurret,          ///< Has L turret.
            LShield,          ///< Has L shield.
            SDock,            ///< Has S docking bay.
            SShipCargo,       ///< Has S ship cargo.
            MDock,            ///< Has M docking bay.
            MShipCargo,       ///< Has M ship cargo.
            LDock,            ///< Has L docking bay.
            XLDock,           ///< Has XL docking bay.
            LXLDock,          ///< Has L/XL docking bay.
            SLaunchTube,      ///< Has S launch tube.
            MLaunchTube,      ///< Has M launch tube.
            SupplyWorkforce,  ///< Supply workforce.
            RequireWorkforce, ///< Require workforce.
            SupplyProduct,    ///< Supply product.
            Cargo,            ///< Has cargo.
        };
        Type type; ///< Property type.

        /**
         * @brief		Constructor.
         *
         * @param[in]	propertyType	Property type.
         */
        Property(Type propertyType) : type(propertyType) {}

        /**
         * @brief		Desctructor.
         */
        virtual ~Property() {};
    };

    /**
     * @brief	Informations of station modules.
     */
    struct StationModule {
        /**
         * @brief	Class of station modules.
         */
        enum class StationModuleClass {
            Unknow,           ///< Unknow.
            BuildModule,      ///< Build module.
            ConnectionModule, ///< Connection module.
            DefenceModule,    ///< Defence module.
            Dockarea,         ///< Dockarea.
            Habitation,       ///< Habitation,
            Production,       ///< Production,
            Storage,          ///< Storage,
            Processing,       ///< ProcessingModule,
            Welfare,          ///< Welfare Module,
            Radar,            ///< Radar Module,

        };

        QString            macro;           ///< Macro of the module.
        QString            component;       ///< Component of the module.
        GameTexts::IDPair  name;            ///< Module name.
        StationModuleClass moduleClass;     ///< Class of station module.
        bool               playerModule;    ///< Player can build or not.
        GameTexts::IDPair  description;     ///< Module desctiption.
        bool               racialLimited;   ///< The module is racial limited.
        QSet<QString>      races;           ///< Module race.
        quint32            hull;            ///< Hull.
        quint32            explosiondamage; ///< Explosion damage.
        QMap<Property::Type, ::std::shared_ptr<Property>>
            properties; ///< Properties.
    };

    /**
     * @brief     Has M turret.
     */
    struct HasMTurret : public Property {
        quint64 count; ///< Count of the turret.

        /**
         * @brief		Constructor.
         */
        HasMTurret() : Property(Property::Type::MTurret), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMTurret() {}
    };

    /**
     * @brief     Has M shield.
     */
    struct HasMShield : public Property {
        quint64 count; ///< Count of the shield.

        /**
         * @brief		Constructor.
         */
        HasMShield() : Property(Property::Type::MShield), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMShield() {}
    };

    /**
     * @brief     Has L turret.
     */
    struct HasLTurret : public Property {
        quint64 count; ///< Count of the turret.

        /**
         * @brief		Constructor.
         */
        HasLTurret() : Property(Property::Type::LTurret), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasLTurret() {}
    };

    /**
     * @brief     Has L shield.
     */
    struct HasLShield : public Property {
        quint64 count; ///< Count of the shield.

        /**
         * @brief		Constructor.
         */
        HasLShield() : Property(Property::Type::LShield), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasLShield() {}
    };

    /**
     * @brief     Has S docking bay.
     */
    struct HasSDock : public Property {
        quint64 count; ///< Count of the docking bay.

        /**
         * @brief		Constructor.
         */
        HasSDock() : Property(Property::Type::SDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasSDock() {}
    };

    /**
     * @brief     Has S ship capacity.
     */
    struct HasSShipCargo : public Property {
        quint64 capacity; ///< Ship capacity.

        /**
         * @brief		Constructor.
         */
        HasSShipCargo() : Property(Property::Type::SShipCargo), capacity(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasSShipCargo() {}
    };

    /**
     * @brief     Has M docking bay.
     */
    struct HasMDock : public Property {
        quint64 count; ///< Count of the docking bay.

        /**
         * @brief		Constructor.
         */
        HasMDock() : Property(Property::Type::MDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMDock() {}
    };

    /**
     * @brief     Has M ship capacity.
     */
    struct HasMShipCargo : public Property {
        quint64 capacity; ///< Ship capacity.

        /**
         * @brief		Constructor.
         */
        HasMShipCargo() : Property(Property::Type::MShipCargo), capacity(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMShipCargo() {}
    };

    /**
     * @brief     Has L docking bay.
     */
    struct HasLDock : public Property {
        quint64 count; ///< Count of the L docking bay.

        /**
         * @brief		Constructor.
         */
        HasLDock() : Property(Property::Type::LDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasLDock() {}
    };

    /**
     * @brief     Has XL docking bay.
     */
    struct HasXLDock : public Property {
        quint64 count; ///< Count of the XL docking bay.

        /**
         * @brief		Constructor.
         */
        HasXLDock() : Property(Property::Type::XLDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasXLDock() {}
    };

    /**
     * @brief     Has L/XL docking bay.
     */
    struct HasLXLDock : public Property {
        quint64 count; ///< Count of the L/XL docking bay.

        /**
         * @brief		Constructor.
         */
        HasLXLDock() : Property(Property::Type::LXLDock), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasLXLDock() {}
    };

    /**
     * @brief     Has S launch tube.
     */
    struct HasSLaunchTube : public Property {
        quint64 count; ///< Count of the S launch tube.

        /**
         * @brief		Constructor.
         */
        HasSLaunchTube() : Property(Property::Type::SLaunchTube), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasSLaunchTube() {}
    };

    /**
     * @brief     Has M launch tube.
     */
    struct HasMLaunchTube : public Property {
        quint64 count; ///< Count of the M launch tube.

        /**
         * @brief		Constructor.
         */
        HasMLaunchTube() : Property(Property::Type::MLaunchTube), count(0) {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasMLaunchTube() {}
    };

    /**
     * @brief     Support workforce.
     */
    struct SupplyWorkforce : public Property {
        quint64 workforce; ///< Workforce can supply.
        ::std::shared_ptr<GameWares::ProductionInfo>
            supplyInfo; ///< Supply informations.

        /**
         * @brief		Constructor.
         */
        SupplyWorkforce() :
            Property(Property::Type::SupplyWorkforce), workforce(0)
        {}

        /**
         * @brief		Destructor.
         */
        virtual ~SupplyWorkforce() {}
    };

    /**
     * @brief     Require workforce.
     */
    struct RequireWorkforce : public Property {
        quint64 workforce; ///< Workforce required.

        /**
         * @brief		Constructor.
         */
        RequireWorkforce() :
            Property(Property::Type::RequireWorkforce), workforce(0)
        {}

        /**
         * @brief		Destructor.
         */
        virtual ~RequireWorkforce() {}
    };

    /**
     * @brief     Supply product.
     */
    struct SupplyProduct : public Property {
        QString product; ///< ID of the product.
        ::std::shared_ptr<GameWares::ProductionInfo>
            productionInfo; ///< Production information.

        /**
         * @brief		Constructor.
         */
        SupplyProduct() : Property(Property::Type::SupplyProduct) {}

        /**
         * @brief		Destructor.
         */
        virtual ~SupplyProduct() {}
    };

    /**
     * @brief     Has cargo.
     */
    struct HasCargo : public Property {
        GameWares::TransportType cargoType; ///< Type of cargo.
        quint64                  cargoSize; ///< Cargo size(m^3).

        /**
         * @brief		Constructor.
         */
        HasCargo() :
            Property(Property::Type::Cargo),
            cargoType(GameWares::TransportType::Unknow), cargoSize(0)
        {}

        /**
         * @brief		Destructor.
         */
        virtual ~HasCargo() {}
    };

    /**
     * @brief	Temporart docking bay information.
     */
    struct TmpDockingBayInfo {
        quint64 count;                   ///< Count of the docking bay.
        quint64 capacity;                ///< Ship capacity.
        enum { S, M, L, XL, L_XL } type; ///< Type.
    };

  private:
    QVector<::std::shared_ptr<StationModule>> m_modules; ///< Station modules.
    QMap<QString, ::std::shared_ptr<StationModule>>
        m_modulesIndex; ///< Station modules index.
    QMap<QString, QVector<::std::shared_ptr<StationModule>>>
        m_componentTmpIndex; ///< Temporart component index.
    static QMap<QString, StationModule::StationModuleClass>
        _classMap; ///< Station module class map.

  protected:
    /**
     * @brief		Constructor.
     *
     * @param[in]	vfs				Virtual filesystem of the game.
     * @param[in]	macros			Game macros.
     * @param[in]	texts			Game texts.
     * @param[in]	wares			Game wares.
     * @param[in]	components		Game components.
     * @param[in]	setTextFunc		Callback to set text.
     */
    GameStationModules(::std::shared_ptr<GameVFS>             vfs,
                       ::std::shared_ptr<GameMacros>          macros,
                       ::std::shared_ptr<GameTexts>           texts,
                       ::std::shared_ptr<GameWares>           wares,
                       ::std::shared_ptr<GameComponents>      components,
                       ::std::function<void(const QString &)> setTextFunc);

  public:
    /**
     * @brief		Get modules.
     *
     * @return		Modules.
     */
    const QVector<::std::shared_ptr<StationModule>> &modules() const;

    /**
     * @brief		Get modules.
     *
     * @return		Modules.
     */
    ::std::shared_ptr<StationModule> module(const QString &macro);

    /**
     * @brief		Destructor.
     */
    virtual ~GameStationModules();

  private:
    /**
     * @brief		Start element callback in group.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRootOfModuleGroups(XMLLoader &         loader,
                                            XMLLoader::Context &context,
                                            const QString &     name,
                                            const QMap<QString, QString> &attr);

    /**
     * @brief		Start element callback in groups.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInGroupsOfModuleGroups(
        XMLLoader &                   loader,
        XMLLoader::Context &          context,
        const QString &               name,
        const QMap<QString, QString> &attr);

    /**
     * @brief		Start element callback in group.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool
        onStartElementInGroupOfModuleGroups(XMLLoader &         loader,
                                            XMLLoader::Context &context,
                                            const QString &     name,
                                            const QMap<QString, QString> &attr);

    /**
     * @brief		Load macro.
     *
     * @param[in]	macro		Macro to load.
     * @param[in]	vfs			Virtual filesystem of the game.
     * @param[in]	macros		Game macros.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     * @param[in]	components	Game components.
     */
    void loadMacro(const QString &                   macro,
                   ::std::shared_ptr<GameVFS>        vfs,
                   ::std::shared_ptr<GameMacros>     macros,
                   ::std::shared_ptr<GameTexts>      texts,
                   ::std::shared_ptr<GameWares>      wares,
                   ::std::shared_ptr<GameComponents> components);

    /**
     * @brief		Start element callback in module macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRootOfModuleMacro(XMLLoader &         loader,
                                           XMLLoader::Context &context,
                                           const QString &     name,
                                           const QMap<QString, QString> &attr);

    /**
     * @brief		Start element callback in module macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool
        onStartElementInMacrosOfModuleMacro(XMLLoader &         loader,
                                            XMLLoader::Context &context,
                                            const QString &     name,
                                            const QMap<QString, QString> &attr);

    /**
     * @brief		Start element callback in module macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInMacroOfModuleMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in module macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInPropertiesOfModuleMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in module macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInBuildOfModuleMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in module macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInSetsOfModuleMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in module macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInProductionOfModuleMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in module macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInConnectionsOfModuleMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in module macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInConnectionOfModuleMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Load macro in connections of module macro.
     *
     * @param[in]	macro		Maro to load.
     * @param[in]	module		Station module.
     * @param[in]	vfs			Virtual filesystem of the game.
     * @param[in]	macros		Game macros.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     * @param[in]	components	Game components.
     */
    void loadConnectionMacro(const QString &                   macro,
                             ::std::shared_ptr<StationModule>  module,
                             ::std::shared_ptr<GameVFS>        vfs,
                             ::std::shared_ptr<GameMacros>     macros,
                             ::std::shared_ptr<GameTexts>      texts,
                             ::std::shared_ptr<GameWares>      wares,
                             ::std::shared_ptr<GameComponents> components);

    /**
     * @brief		Start element callback in connection macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRootOfConnectionMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in connection macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInMacrosOfConnectionMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in connection macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInMacroOfConnectionMacro(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in connection macro.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     * @param[in]	info		Docking bay information.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInnPropertiesOfConnectionMacro(
        XMLLoader &                          loader,
        XMLLoader::Context &                 context,
        const QString &                      name,
        const QMap<QString, QString> &       attr,
        ::std::shared_ptr<StationModule>     module,
        ::std::shared_ptr<TmpDockingBayInfo> info);

    /**
     * @brief		Load component.
     *
     * @param[in]	component	Component to load.
     * @param[in]	module		Station module.
     * @param[in]	vfs			Virtual filesystem of the game.
     * @param[in]	macros		Game macros.
     * @param[in]	texts		Game texts.
     * @param[in]	wares		Game wares.
     * @param[in]	components	Game components.
     */
    void loadComponent(const QString &                   component,
                       ::std::shared_ptr<StationModule>  module,
                       ::std::shared_ptr<GameVFS>        vfs,
                       ::std::shared_ptr<GameMacros>     macros,
                       ::std::shared_ptr<GameTexts>      texts,
                       ::std::shared_ptr<GameWares>      wares,
                       ::std::shared_ptr<GameComponents> components);

    /**
     * @brief		Start element callback in module component.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInRootOfModuleComponent(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in module component.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInComponentsOfModuleComponent(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in module component.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInComponentOfModuleComponent(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);

    /**
     * @brief		Start element callback in module component.
     *
     * @param[in]	loader		XML loader.
     * @param[in]	context		Context.
     * @param[in]	name		Name of the element.
     * @param[in]	attr		Attributes.
     * @param[in]	module		Station module.
     *
     * @return		Return \c true if the parsing should be continued.
     *				otherwise returns \c false.
     */
    bool onStartElementInConnectionsOfModuleComponent(
        XMLLoader &                      loader,
        XMLLoader::Context &             context,
        const QString &                  name,
        const QMap<QString, QString> &   attr,
        ::std::shared_ptr<StationModule> module);
};
