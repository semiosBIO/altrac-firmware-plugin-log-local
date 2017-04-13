#include "log-local.h"

LogLocalHandler::LogLocalHandler(uint16_t _localLogSave, LogLevel level, const LogCategoryFilters &filters) : LogHandler(level, filters), localLogSave(_localLogSave) {
    LogManager::instance()->addHandler(this);
}

// this is where we can save the logging data
void LogLocalHandler::log(String message, LogLevel level) {

    if (firstRun)
    {
        LogLocalHandler::recall();
        firstRun = false;
    }

    if (level > LOG_LEVEL_WARN)
    {
        LogLocalHandler::errorFlagSet(1);
        LogLocalHandler::save();
    }

    uint8_t messageLength = message.length();
    if (messageLength > logFile.size)
    {
        return;
    }
    for (int i = 0; i < messageLength; ++i)
    {
        if (++logFile.position >= logFile.size)
        {
            logFile.position = 0;
        }
        logFile.log[logFile.position] = message[i];
    }
    if (++logFile.position >= logFile.size)
    {
        logFile.position = 0;
    }
    logFile.log[logFile.position] = '\n';
}

LogLocalHandler::~LogLocalHandler() {
    LogManager::instance()->removeHandler(this);
}


// The floowing methods are taken from Particle FW, specifically spark::StreamLogHandler.
// See https://github.com/spark/firmware/blob/develop/wiring/src/spark_wiring_logging.cpp
const char* LogLocalHandler::LogLocalHandler::extractFileName(const char *s) {
    const char *s1 = strrchr(s, '/');
    if (s1) {
        return s1 + 1;
    }
    return s;
}

const char* LogLocalHandler::extractFuncName(const char *s, size_t *size) {
    const char *s1 = s;
    for (; *s; ++s) {
        if (*s == ' ') {
            s1 = s + 1; // Skip return type
        } else if (*s == '(') {
            break; // Skip argument types
        }
    }
    *size = s - s1;
    return s1;
}

uint8_t LogLocalHandler::errorFlagSet(uint8_t state) {
    uint8_t protectedState = logFile.errorFlag;
    if (state >= 0 && state <= 1)
    {
        protectedState = state;
    }
    logFile.errorFlag = protectedState;
    return logFile.errorFlag;
}
uint8_t LogLocalHandler::errorFlagState() {
    return logFile.errorFlag;
}
uint8_t LogLocalHandler::errorFlagClear() {
    LogLocalHandler::errorFlagSet(0);
    LogLocalHandler::save();
    return 0;
}

uint16_t LogLocalHandler::logLength() {
    return logFile.size;
}
uint8_t LogLocalHandler::returnValue(uint8_t returnPosition) {
    if (returnPosition >= logFile.size || returnPosition < 0)
    {
        return 0;
    }
    return logFile.log[returnPosition];
}

void LogLocalHandler::save() {
    EEPROM.put(localLogSave, logFile);
}
void LogLocalHandler::recall() {
    EEPROM.get(localLogSave, logFile);
    if (logFile.size > LOG_LOCAL_ARRAY_SIZE)
    {
        logFile.size = LOG_LOCAL_ARRAY_SIZE;
    }
}

// this is where the incoming message gets taken in and then sent
void LogLocalHandler::logMessage(const char *msg, LogLevel level, const char *category, const LogAttributes &attr) {

    String s;

    if (category) {
        s.concat(category);
        s.concat(",");
    }

    // Source file
    if (attr.has_file) {
        s = extractFileName(attr.file); // Strip directory path
        s.concat(s); // File name
        if (attr.has_line) {
            s.concat(":");
            s.concat(String(attr.line)); // Line number
        }
        if (attr.has_function) {
            s.concat(", ");
        } else {
            s.concat(": ");
        }
    }

    // Function name
    if (attr.has_function) {
        size_t n = 0;
        s = extractFuncName(attr.function, &n); // Strip argument and return types
        s.concat(s);
        s.concat("(): ");
    }

    // Level
    s.concat(level / 10);
    s.concat(",");

    // Message
    if (msg) {
        s.concat(msg);
    }

    // Additional attributes
    if (attr.has_code || attr.has_details) {
        s.concat(" [");
        // Code
        if (attr.has_code) {
            s.concat(String::format("code = %p" , (intptr_t)attr.code));
        }
        // Details
        if (attr.has_details) {
            if (attr.has_code) {
                s.concat(", ");
            }
            s.concat("details = ");
            s.concat(attr.details);
        }
        s.concat(']');
    }

    log(s, level);
}