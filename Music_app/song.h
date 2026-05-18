#ifndef SONG_H
#define SONG_H

#include <QString>
#include <QDataStream>

// структура вынесена СПЕЦИАЛЬНО отдельно для удобства сериализации :)
struct Song
{
    QString title;
    QString author;
    int     year;

    Song() : year(2000) {}

    Song(const QString& title, const QString& author, int year)
        : title(title), author(author), year(year) {
    }
};

inline QDataStream& operator<<(QDataStream& out, const Song& s)
{
    out << s.title << s.author << s.year;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, Song& s)
{
    in >> s.title >> s.author >> s.year;
    return in;
}

#endif 
