#include "error.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>

ErrorReporter errorReporter;

ErrorReporter::~ErrorReporter() {
    cleanup();
}

bool ErrorReporter::init(const std::string& src_filename) {
    current_file = src_filename;
    error_count = 0;
    source_lines.clear();
    
    std::ifstream file(src_filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        source_lines.push_back(line);
    }
    
    file.close();
    return true;
}

void ErrorReporter::printSourceLine(const SourceLocation& loc) {
    if (loc.line > 0 && loc.line <= source_lines.size()) {
        const std::string& line = source_lines[loc.line - 1];
        std::cerr << line << std::endl;
        
        // Print caret pointer
        for (uint32_t i = 0; i < loc.column; i++) {
            std::cerr << ' ';
        }
        std::cerr << "^" << std::endl;
    }
}

void ErrorReporter::reportDiagnostic(DiagnosticType type, const SourceLocation& loc, 
                                   const char* format, va_list args) {
    std::cerr << loc.filename << ":" << loc.line << ":" << loc.column << ": ";
    
    switch (type) {
        case DiagnosticType::Error:
            std::cerr << "error: ";
            error_count++;
            break;
        case DiagnosticType::Warning:
            std::cerr << "warning: ";
            break;
        case DiagnosticType::Note:
            std::cerr << "note: ";
            break;
    }
    
    // Format the message using vsnprintf
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    std::cerr << buffer << std::endl;
    
    printSourceLine(loc);
}

void ErrorReporter::error(const SourceLocation& loc, const char* format, ...) {
    va_list args;
    va_start(args, format);
    reportDiagnostic(DiagnosticType::Error, loc, format, args);
    va_end(args);
}

void ErrorReporter::warning(const SourceLocation& loc, const char* format, ...) {
    va_list args;
    va_start(args, format);
    reportDiagnostic(DiagnosticType::Warning, loc, format, args);
    va_end(args);
}

void ErrorReporter::note(const SourceLocation& loc, const char* format, ...) {
    va_list args;
    va_start(args, format);
    reportDiagnostic(DiagnosticType::Note, loc, format, args);
    va_end(args);
}

int ErrorReporter::getErrorCount() const {
    return error_count;
}

void ErrorReporter::cleanup() {
    source_lines.clear();
    current_file.clear();
}