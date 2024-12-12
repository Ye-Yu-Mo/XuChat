/**
 * @file logger.hpp
 * @brief 异步日志器初始化
 * 主要功能：
 * - 配置日志器名称为 "Asynclogger"
 * - 设置日志格式
 * - 指定日志类型为异步日志
 * - 将日志输出到标准输出
 *
 * 该类在全局范围内进行初始化，确保在使用日志功能之前已完成配置。
 */
#pragma once
#include "../third/Xulog/logs/Xulog.h"

namespace XuChat
{
    /**
     * @brief 日志器的智能指针类型
     */
    Xulog::Logger::ptr logger;

    /**
     * @class LogInit
     * @brief 日志初始化类，用于设置异步日志器
     *
     * 该类在创建时初始化异步日志器，配置日志器的名称、格式、类型和输出方式。
     */
    class LogInit
    {
    public:
        /**
         * @brief LogInit 构造函数
         *
         * 构造函数中创建并配置异步日志器，包括：
         * - 日志器名称为 "Asynclogger"
         * - 日志格式为 "[%d{%y-%m-%d|%H:%M:%S}][%f:%l][%p]%T%m%n"
         * - 日志类型为异步日志
         * - 输出至标准输出
         */
        LogInit()
        {
            std::unique_ptr<Xulog::LoggerBuilder> builder(new Xulog::GlobalLoggerBuild());
            builder->buildLoggerName("Asynclogger");
            builder->buildFormatter("[%d{%y-%m-%d|%H:%M:%S}][%f:%l][%p]%T%m%n");
            builder->buildLoggerType(Xulog::LoggerType::LOGGER_ASYNC);
            builder->buildSink<Xulog::StdoutSink>(Xulog::StdoutSink::Color::Enable);
            builder->build();
            logger = Xulog::getLogger("Asynclogger");
        }

    public:
    };

    LogInit log_init; ///< 在全局范围内初始化日志器
}
