#pragma once

#include <any>
#include <map>
#include <memory>
#include <vector>

#include <QtCore/QString>
#include <QtCore/QXmlStreamReader>
/**
 * @brief		XML parser using QXmlStreamReader.
 */
class XMLLoader {
public:
    /**
     * @brief		Reader context.
     */
    class Context;

protected:
    std::vector<std::unique_ptr<Context>> m_contextStack; ///< Contexts.
    std::map<QString, std::any>           m_values;       ///< Values.

public:
    /**
     * @brief	Constructor.
     */
    XMLLoader() = default;

    /**
     * @brief		Push context.
     *
     * @param[in]	context			Context to push.
     */
    void pushContext(std::unique_ptr<Context> context);

    /**
     * @brief	Parse XML.
     *
     * @param[in]	reader			XML SAX reader.
     * @param[in]	context			First context.
     *
     * @return		Returns \c true if the whole file has been parsed,
     *				otherwise returns \c false.
     */
    bool parse(QXmlStreamReader& reader, std::unique_ptr<Context> context);

    /**
     * @brief	Destructor.
     */
    virtual ~XMLLoader() = default;

    /**
     * @brief		Operator [].
     *
     * @param[in]	key		Key of the value.
     *
     * @return		Value.
     */
    std::any& operator[](const QString& key);

    // Rule of five additions:
    XMLLoader(const XMLLoader&) = delete;
    XMLLoader& operator=(const XMLLoader&) = delete;
    XMLLoader(XMLLoader&&) noexcept = default;
    XMLLoader& operator=(XMLLoader&&) noexcept = default;
};

#include <common/xml_loader_context.h>
