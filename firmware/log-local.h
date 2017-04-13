#ifndef LOG_LOCAL_H
#define LOG_LOCAL_H

#include "application.h"

#if (SYSTEM_VERSION < SYSTEM_VERSION_v061)
  #error This library requires FW version 0.6.1 and above.
#endif

#ifndef LOG_LOCAL_ARRAY_SIZE
  #define LOG_LOCAL_ARRAY_SIZE 200
#endif

class LogLocalHandler : public LogHandler {
  public:

    explicit LogLocalHandler(uint16_t _localLogSave = 0, LogLevel level = LOG_LEVEL_INFO, const LogCategoryFilters &filters = {});
    virtual ~LogLocalHandler();
    uint8_t errorFlagState();
    uint8_t errorFlagClear();

    uint16_t logLength();
    uint8_t returnValue(uint8_t position);

    void save();
    void recall();

  private:

    struct {
      uint8_t log[LOG_LOCAL_ARRAY_SIZE];
      uint16_t size = LOG_LOCAL_ARRAY_SIZE;  
    } logFile;

    uint8_t position;
    uint16_t localLogSave;

    uint8_t firstRun = true;

    uint8_t errorFlag;
    uint8_t errorFlagSet(uint8_t state);

    const char* extractFileName(const char *s);
    const char* extractFuncName(const char *s, size_t *size);
    void log(String message, LogLevel level);

  protected:

    virtual void logMessage(const char *msg, LogLevel level, const char *category, const LogAttributes &attr) override;
};

#endif