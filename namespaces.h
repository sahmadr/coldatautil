/**
 * @file        namespaces.h
 *
 * @project     ColDataUtil
 * @version     0.4
 *
 * @author      Syed Ahmad Raza (git@ahmads.org)
 *
 * @brief       All the includes, namespaces and aliases.
 *
 */

#ifndef NAMESPACES_H
#define NAMESPACES_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include <set>
#include <limits>
#include <algorithm>
#include <iterator>
#include <cstddef>
#include <cmath>

using   std::string, std::vector, std::set, std::tuple, std::unordered_map,
        std::numeric_limits, std::tie, std::ios_base, std::to_string,
        std::cout, std::endl, std::flush, std::setw, std::left, std::right,
        std::stoi, std::stod,
        std::ifstream, std::ofstream, std::getline, std::streampos,
        std::invalid_argument, std::logic_error, std::runtime_error;

using stringV = std::string_view;
using calcType = double(*)(int, size_t, size_t);

enum class Delimitation { undefined, spaced, delimited, spacedAndDelimited };

//----------------------------------------------------------------------------//
//**************************** ColData Namespace *****************************//
//----------------------------------------------------------------------------//

namespace ColData {
    // inline constexpr int maxRowsToCheckInt = 1000;
    // inline constexpr int delimiterLenLimit = 3;
    class DoubleV;
    class IntV;
    const tuple<Delimitation, int, size_t, tuple<bool, size_t, size_t>, IntV*,
        vector<DoubleV*>&> loadData(const string& fileName, const string& dlm);
    bool isNumberLine(stringV lineStr, string dlm);
    tuple<streampos, streampos> findLinePositions(ifstream& iFile,
        const string& dlm);
    tuple<string, Delimitation> parseHeaderLine(ifstream& iFile,
        const string& dlm, const streampos headerLinePos);
    tuple<int, vector<string>> identifyColumnHeaders(string headerLine,
        const string& dlm, const Delimitation headerDlmType);
    set<int> findTimestepColCandidates(int dataColTotal,
        vector<string> colNames);
    Delimitation parseColumnData(ifstream& iFile, const string& dlm,
        const streampos dataLinePos);
    void classifyColumns(ifstream& iFile, const string& dlm,
        const Delimitation dataDlmType, const streampos dataLinePos,
        const int colTotal, set<int>& timestepColCandidates);
    void createVectors(const vector<string>& colNames);
    size_t populateVectors(ifstream& iFile, const string& dlm,
        const int dataColTotal, const Delimitation dataDlmType,
        const streampos dataLinePos);
}

//----------------------------------------------------------------------------//
//**************************** CmdArgs Namespace *****************************//
//----------------------------------------------------------------------------//

namespace CmdArgs {
    enum class Option { delimiter, fileIn, calculation, column, row, timestep,
        cycle, fileOut, printData, fileData, help, version };
    enum class CalcId { findMin, findMax, findAbsMin, findAbsMax, findMean,
        findQuadraticMean, findCubicMean };
    enum class CycleInit { first, last };
    class Args;
    class Delimiter;
    class FileIn;
    class Calc;
    class Column;
    class Row;
    class Timestep;
    class Cycle;
    class FileOut;
    class PrintData;
    class FileData;
    class Help;
    class Version;
    extern const unordered_map<string, Option> mapStrToOption;
    extern const unordered_map<string, CalcId> mapStrToCalc;
    extern const unordered_map<string, CycleInit> mapStrToCycInit;
}

//----------------------------------------------------------------------------//
//**************************** CalcFnc Namespace *****************************//
//----------------------------------------------------------------------------//

namespace CalcFnc {
    template<typename T> const unordered_map<calcType, string> mapCalcToStr;
    template<typename T> double findMax(const T column,
        const size_t rowBgn, const size_t rowEnd);
    template<typename T> double findMin(const T column,
        const size_t rowBgn, const size_t rowEnd);
    template<typename T> double findAbsMax(const T column,
        const size_t rowBgn, const size_t rowEnd);
    template<typename T> double findAbsMin(const T column,
        const size_t rowBgn, const size_t rowEnd);
    template<typename T> double findMean(const T column,
        const size_t rowBgn, const size_t rowEnd);
    template<typename T> double findQuadraticMean(const T column,
        const size_t rowBgn, const size_t rowEnd);
    template<typename T> double findCubicMean(const T column,
        const size_t rowBgn, const size_t rowEnd);
}

//----------------------------------------------------------------------------//
//***************************** Output Namespace *****************************//
//----------------------------------------------------------------------------//

namespace Output {
    void output(CmdArgs::Args* argsP);
    void printInputDataInfo(const string& fileInName, const int dataColTotal,
        const size_t dataRowTotal, const Delimitation dataDlmType,
        const tuple<bool, size_t, size_t> dataTimestepRange);
    void printer(
        const tuple<size_t, size_t> rowRange,
        const bool timestepConsistent, const tuple<size_t,size_t> timestepRange,
        const vector<int>& doubleColSet,
        const vector<CmdArgs::CalcId>& calcIdSet);
    void filer(const string& fileOutName, const string& fileInName,
        const tuple<size_t, size_t> rowRange,
        const bool timestepConsistent, const tuple<size_t,size_t> timestepRange,
        const vector<int>& doubleColSet,
        const vector<CmdArgs::CalcId>& calcIdSet);
    void dataPrinter(const string& dlm, const size_t dataRowTotal);
    void dataFiler(const string& fileName, const string& dlm,
        const size_t dataRowTotal);
    template<typename T> const unordered_map<CmdArgs::CalcId, calcType>
        mapCalcIdToCalc;
}

#endif