/**
 * @version     ColDataUtil 1.5
 * @author      Syed Ahmad Raza (git@ahmads.org)
 * @copyright   GPLv3+: GNU Public License version 3 or later
 *
 * @file        calcFnc.h
 * @brief       Functions for calculations on column data.
 */

#ifndef CALCFUNCTION_H
#define CALCFUNCTION_H

#include "namespaces.h"
#include "colData.h"

using namespace ColData;

/*
 * Find the maximum value of the given column using its Id number or name and
 * return the result.
 */
template<typename T>
double CalcFnc::findMax(const T column,
        const size_t rowBgn, const size_t rowEnd) {
    const vector<double> colData{DoubleV::getOnePFromCol(column)->getData()};
    double maxVal{colData[rowBgn]};
    for (size_t row=rowBgn+1; row<=rowEnd; ++row) {
        if (maxVal<colData[row]) { maxVal = colData[row]; }
    }
    return maxVal;
}

/*
 * Find the minimum value of the given column using its Id number or name and
 * return the result.
 */
template<typename T>
double CalcFnc::findMin(const T column,
        const size_t rowBgn, const size_t rowEnd) {
    const vector<double> colData{DoubleV::getOnePFromCol(column)->getData()};
    double minVal{colData[rowBgn]};
    for (size_t row=rowBgn+1; row<=rowEnd; ++row) {
        if (minVal>colData[row]) { minVal = colData[row]; }
    }
    return minVal;
}

/*
 * Find the absolute maximum value of the given column using its Id number or
 * name and return the result.
 */
template<typename T>
double CalcFnc::findAbsMax(const T column,
        const size_t rowBgn, const size_t rowEnd) {
    const vector<double> colData{DoubleV::getOnePFromCol(column)->getData()};
    double maxVal{std::abs(colData[rowBgn])};
    for (size_t row=rowBgn+1; row<=rowEnd; ++row) {
        if (maxVal<std::abs(colData[row])) { maxVal = std::abs(colData[row]); }
    }
    return maxVal;
}

/*
 * Find the absolute minimum value of the given column using its Id number or
 * name and return the result.
 */
template<typename T>
double CalcFnc::findAbsMin(const T column,
        const size_t rowBgn, const size_t rowEnd) {
    const vector<double> colData{DoubleV::getOnePFromCol(column)->getData()};
    double minVal{std::abs(colData[rowBgn])};
    for (size_t row=rowBgn+1; row<=rowEnd; ++row) {
        if (minVal>std::abs(colData[row])) { minVal = std::abs(colData[row]); }
    }
    return minVal;
}

/*
 * Find the mean of the given column using its Id number or name and return
 * the result.
 */
template<typename T>
double CalcFnc::findMean(const T column,
        const size_t rowBgn, const size_t rowEnd) {
    return DoubleV::getOnePFromCol(column)->getSum(rowBgn, rowEnd)
            /(rowEnd - rowBgn + 1);
}

/*
 * Find the quadratic mean (RMS) of the given column using its Id number or
 * name and return the result.
 */
template<typename T>
double CalcFnc::findRMS(const T column,
        const size_t rowBgn, const size_t rowEnd) {
    return std::sqrt(
                DoubleV::getOnePFromCol(column)->getSumOfSquares(rowBgn, rowEnd)
                    /(rowEnd - rowBgn + 1)
            );
}

/*
 * Find the fluctuation quadratic mean (RMS) of the given column using the mean
 * and its Id number or name and return the result.
 */
template<typename T>
double CalcFnc::findFluctuationRMS(const T column,
        const size_t rowBgn, const size_t rowEnd) {
    double mean {CalcFnc::findMean(column, rowBgn, rowEnd)};
    return
        std::sqrt(
            DoubleV::getOnePFromCol(column)->getSumOfSquaresOfDifference(
                    mean, rowBgn, rowEnd)
                /(rowEnd - rowBgn + 1)
        );
}

/*
 * Find the cubic mean of the given column using its Id number or
 * name and return the result.
 */
template<typename T>
double CalcFnc::findCubicMean(const T column,
        const size_t rowBgn, const size_t rowEnd) {
    return std::cbrt(
                DoubleV::getOnePFromCol(column)->getSumOfCubes(rowBgn, rowEnd)
                    /(rowEnd - rowBgn + 1)
            );
}

template<typename T> double findFourier(const T column,
        const size_t rowBgn, const size_t rowEnd);
/*
 *
 */
template<typename T>
double CalcFnc::findFourier(const T column,
        const size_t rowBgn, const size_t rowEnd) {
    return std::cbrt(
                DoubleV::getOnePFromCol(column)->getSumOfCubes(rowBgn, rowEnd)
                    /(rowEnd - rowBgn + 1)
            );
}

#endif