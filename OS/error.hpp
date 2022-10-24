#ifndef _ERROR_H
#define _ERROR_H

/**
 * @brief When the function returns normally
 *
 */
#define MK_OK (0)

/**
 * @brief When we have an internal error.
 */
#define MK_INT_ERR (-1)

/**
 * @brief When invalid data has been entered into the system
 */
#define MK_INVALID_PARAM (-2)

/**
 * @brief Some type of I/O error has occured during the operation
 */
#define MK_IO_ERR (-3)

/**
 * @brief no more memory/space error. Allocated memory pool is exhausted
 */
#define MK_LOW_MEM_ERR (-4)

/**
 * @brief When we haven't initialized the thread/statemachine to handle the function called yet
 */
#define MK_NOT_INITED (-5)

/**
 * @brief When we no longer have anymore elements in our list
 *
 */
#define MK_LIST_EMPTY (-6)

#endif