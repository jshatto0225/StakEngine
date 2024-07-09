#pragma once

#include <stdio.h>

namespace Log
{

/**
 * @brief Function to initialize console and file logging
 */
void Init();

/**
 * @brief Function to shutdown console and file logging
 */
void Shutdown();

namespace Core
{

/**
 * @brief Function to log at trace level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Trace(const char *fmt, ...);

/**
 * @brief Function to log at info level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Info(const char *fmt, ...);

/**
 * @brief Function to log at warn level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Warn(const char *fmt, ...);

/**
 * @brief Function to log at error level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Error(const char *fmt, ...);

/**
 * @brief Function to log at critical level
 * Logs from the core to the core log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Critical(const char *fmt, ...);

} // namespace Core

/**
 * @brief Function to log at trace level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Trace(const char *fmt, ...);

/**
 * @brief Function to log at info level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Info(const char *fmt, ...);

/**
 * @brief Function to log at warn level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Warn(const char *fmt, ...);

/**
 * @brief Function to log at error level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Error(const char *fmt, ...);

/**
 * @brief Function to log at critical level
 * Logs from the client to the client log file
 *
 * @param fmt Format of the message
 * @param ... variadic arguments to fill the Format
 */
void Critical(const char *fmt, ...);

} // namespace Log
