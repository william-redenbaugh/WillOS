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

/**
 * @returns When we have run out of reentrant locks
*/
#define OS_RET_MAX_RENTRANT (-10)

/**
 @returns When we have a deadlock in our resource sharing
*/
#define OS_RET_DEADLOCK (-11)

/**
 * @returns When there are no more resources(outside of memory) left
*/
#define OS_RET_NO_MORE_RESOURCES (-12)

/**
 * @returns When we don't own the resources that we are trying to access
*/
#define OS_RET_NOT_OWNED (-13)


/**
 * @returns when we have an unsupported feature
*/
#define OS_RET_UNSUPPORTED_FEATURES (-14)

/**
 * @returns when the OS is null
*/
#define OS_RET_NULL_PTR (-15)

/**
 * @brief Converts POSIX Error messages to our OS Error messages
*/
int posix_os_ret_error_conv(int ret);
#endif