#include "string.hpp"

#include <unordered_map>
#include <algorithm>
#include <QString>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

std::string Juanca::to_lowercase(const std::string& str) noexcept {/*{{{*/
    std::string result {str};
    std::transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c){ return std::tolower(c); });
    return result;
}/*}}}*/

std::string Juanca::no_accent(const std::string& input) noexcept {/*{{{*/
    // Mapping of accented characters to their unaccented counterparts
    static const std::unordered_map<std::string, char> accent_map = {
        {"\u00C1", 'A'},
		{"\u00C9", 'E'},
		{"\u00CD", 'I'},
		{"\u00D3", 'O'},
		{"\u00DA", 'U'},
		{"\u00D1", 'N'},
        {"\u00E1", 'a'},
		{"\u00E9", 'e'},
		{"\u00ED", 'i'},
		{"\u00F3", 'o'},
		{"\u00FA", 'u'},
		{"\u00F1", 'n'}};

    std::string result;
    result.reserve(input.size()); // Reserve space to avoid multiple allocations

    for (size_t i = 0; i < input.size(); ++i) {
        std::string utf8_char = input.substr(i, 1);

        // Check for multibyte UTF-8 characters (e.g., á, é, í, ó, ú, ñ)
        if ((input[i] & 0xC0) == 0xC0) {
            utf8_char = input.substr(i, 2);
            ++i; // Skip the next byte as it is part of the current character
        }

        auto it = accent_map.find(utf8_char);
        if (it != accent_map.end()) {
            result.push_back(it->second);
        } else if (utf8_char != "\'") {
            result.append(utf8_char);
        }
    }

    return result;
}/*}}}*/

QString Juanca::remove_accents(const QString &input) noexcept {/*{{{*/
    // Normalize the string to NFD (Canonical Decomposition)
    QString normalizedString = input.normalized(QString::NormalizationForm_D);

    // Regular expression to match combining diacritical marks
    QRegularExpression re(QStringLiteral("[\\u0300-\\u036f]"));

    // Remove all diacritical marks
    QString result = normalizedString.remove(re);

    // Handle specific characters like ñ to n manually
    result.replace(QStringLiteral("ñ"), QStringLiteral("n"));
    result.replace(QStringLiteral("Ñ"), QStringLiteral("N"));

    return result;
}/*}}}*/

QString Juanca::removeLastLetter(const QString& input) noexcept {/*{{{*/
    if (input.isEmpty()) {
        return input;  // Return empty string if input is empty
    }

    QString result = input;
    result.chop(1);  // Remove the last character

    return result;
}/*}}}*/
