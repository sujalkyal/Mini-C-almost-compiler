#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <vector>
#include <cstdint>

class SourceLocation {
public:
    SourceLocation(const std::string& filename = "", uint32_t line = 0, uint32_t column = 0)
        : filename(filename), line(line), column(column) {}
    
    std::string filename;
    uint32_t line;
    uint32_t column;
};

enum class DiagnosticType {
    Error,
    Warning,
    Note
};

class ErrorReporter {
public:
    ErrorReporter() = default;
    ~ErrorReporter();
    
    bool init(const std::string& src_filename);
    
    void error(const SourceLocation& loc, const char* format, ...);
    void warning(const SourceLocation& loc, const char* format, ...);
    void note(const SourceLocation& loc, const char* format, ...);
    
    int getErrorCount() const;
    
    void cleanup();

private:
    std::string current_file;
    int error_count = 0;
    std::vector<std::string> source_lines;
    
    void printSourceLine(const SourceLocation& loc);
    void reportDiagnostic(DiagnosticType type, const SourceLocation& loc, const char* format, va_list args);
};

// Global error reporter instance
extern ErrorReporter errorReporter;

#endif // ERROR_H