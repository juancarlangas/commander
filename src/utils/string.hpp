#pragma once

#include <string>
#include <QString>

namespace Juanca {
	std::string to_lowercase(const std::string& str) noexcept;
	std::string no_accent(const std::string& input) noexcept;
	QString remove_accents(const QString &input) noexcept;
	QString removeLastLetter(const QString& input) noexcept;
}


