#pragma once

#include <interfaces/i_create_factory_func.h>

class EditorWidget;

/**
 * @brief	Operation.
 */
class Operation {
  protected:
    EditorWidget *m_editorWidget; ///< Editor widget.

  protected:
    /**
     * @brief       Constructor.
     *
     * @param[in]   editorWidget      Editor widget.
     */
    Operation(EditorWidget *editorWidget) : m_editorWidget(editorWidget) {}

  public:
    /**
     * @brief       Get editor widget.
     *
     * @return      Editor widget.
     */
    inline EditorWidget *editorWidget()
    {
        return m_editorWidget;
    }

    /**
     * @brief	Do operation.
     *
     * @return	On success, the method will return \c true, otherwise returns
     *			\c false.
     */
    virtual bool doOperation() = 0;

    /**
     * @brief	Undo operation.
     */
    virtual void undoOperation() = 0;

    /**
     * @brief	Destructor.
     */
    virtual ~Operation() {};
};

/**
 * @brief	Base class of all operations.
 *
 * @tpatam	T		Type of operation.
 * @tpatam	Args	Types of parameters of constructor.
 */
template<typename T, typename... Args>
class OperationBase :
    virtual public Operation,
    virtual public ICreateFactoryFunc<T(Args...)> {
  public:
    /**
     * @brief		Destructor.
     */
    virtual ~OperationBase() {};
};

#include <ui/main_window/editor_widget/editor_widget.h>

#include <ui/main_window/editor_widget/operation/rename_group_operation.h>
