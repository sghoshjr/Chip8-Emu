#include "qdebugstream.h"

ThreadLogStream::ThreadLogStream(std::ostream &stream, QTextEdit *text_edit) : m_stream(stream) {
    log_window = text_edit;
    m_old_buf = stream.rdbuf();
    stream.rdbuf(this);
}

ThreadLogStream::~ThreadLogStream() {
    if (!m_string.empty()) {
        log_window->append(m_string.c_str());
    }

    m_stream.rdbuf(m_old_buf);
}

ThreadLogStream::int_type ThreadLogStream::overflow(int_type v) {
    if (v == '\n') {
        log_window->append(m_string.c_str());
        m_string.erase(m_string.begin(), m_string.end());
    } else
        m_string += v;

    return v;
}

std::streamsize ThreadLogStream::xsputn(const char *p, std::streamsize n) {
    m_string.append(p, p + n);

    long pos = 0;
    while (pos != static_cast<long>(std::string::npos)) {
        pos = m_string.find('\n');
        if (pos != static_cast<long>(std::string::npos)) {
            std::string tmp(m_string.begin(), m_string.begin() + pos);
            log_window->append(tmp.c_str());
            m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
        }
    }

    return n;
}
