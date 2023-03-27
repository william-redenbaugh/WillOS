#ifndef _OS_ERROR_H
#define _OS_ERROR_H

/**
 * @returns When the operation completed successfully
*/
#define OS_RET_OK (0)

/**
 * @brief When an internal error has occoured 
*/
#define OS_RET_INT_ERR (-1)

/**
 * @returns When we have invalid parameters
*/
#define OS_RET_INVALID_PARAM (-2)

/**
 * @returns When there's an IO error during the operations
*/
#define OS_RET_IO_ERROR (-3)

/**
 * @returns No more memory available
*/
#define OS_RET_LOW_MEM_ERROR (-5)

/**
 * @returns When the system is not initialized
*/
#define OS_RET_NOT_INITIALIZED (-6)

/**
 * @returns When the list is empty!
*/
#define OS_RET_LIST_EMPTY (-7)

/**
 * @returns When there's some undefined error that we can't process
*/
#define OS_RET_UNDEFINED_ERROR (-8)

/**
 * @returns When there's a timeout during the operation
*/
#define OS_RET_TIMEOUT (-9)
#endif