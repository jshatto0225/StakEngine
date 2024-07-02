#pragma once

#include <stdio.h>

/**
 * @brief Function to initialize console and file logging
 */
void LogInit();

/**
 * @brief Function to shutdown console and file logging
 */
void LogShutdown();

/**
 * @brief Function to log at trace level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogCoreTrace(const char *fmt, ...);

/**
 * @brief Function to log at info level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogCoreInfo(const char *fmt, ...);

/**
 * @brief Function to log at warn level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogCoreWarn(const char *fmt, ...);

/**
 * @brief Function to log at error level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogCoreError(const char *fmt, ...);

/**
 * @brief Function to log at critical level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogCoreCritical(const char *fmt, ...);

/**
 * @brief Function to log at trace level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogTrace(const char *fmt, ...);

/**
 * @brief Function to log at info level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogInfo(const char *fmt, ...);

/**
 * @brief Function to log at warn level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogWarn(const char *fmt, ...);

/**
 * @brief Function to log at error level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogError(const char *fmt, ...);

/**
 * @brief Function to log at critical level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void LogCritical(const char *fmt, ...);

