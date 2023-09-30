#pragma once

#include <map>
#include <string>
#include <vector>

namespace flconc {

    class StringUtil {

    public:
        static void SplitStrToMap(
                const std::string &str, const std::string &split_str,
                const std::string &joiner, std::map<std::string, std::string> &res);

        static void SplitStrToVector(
                const std::string &str, const std::string &split_str,
                std::vector<std::string> &res);

    };

}

