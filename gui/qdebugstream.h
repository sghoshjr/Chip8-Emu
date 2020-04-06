#ifndef GUI_QDEBUGSTREAM_H
#define GUI_QDEBUGSTREAM_H

#include <iostream>
#include <string>
#include <streambuf>
#include <QTextEdit>
#include <QString>

class ThreadLogStream : public std::basic_streambuf<char> {
   public:
    ThreadLogStream(std::ostream &stream, QTextEdit *text_edit);
    ~ThreadLogStream();

   protected:
    virtual int_type overflow(int_type v);
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

   private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;

    QTextEdit *log_window;
};

#endif  // GUI_QDEBUGSTREAM_H