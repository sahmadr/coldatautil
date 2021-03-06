/**
 * @version     ColDataUtil 1.5
 * @author      Syed Ahmad Raza (git@ahmads.org)
 * @copyright   GPLv3+: GNU Public License version 3 or later
 *
 * @file        output.cpp
 * @brief       Perform calculations and output results.
 */

#include "output.h"

//----------------------------------------------------------------------------//
//*************************** Main output function ***************************//
//----------------------------------------------------------------------------//
/*
 * Perform the output operations based on user input.
 */
void Output::output(CmdArgs::Args* argsP) {
    if (argsP->getHelpP()) {
        ifstream hF(argsP->getHelpP()->getHelpFileName());
        if (hF.is_open()) { cout << hF.rdbuf() << endl; }
        return;
    }
    if (argsP->getVersionP()) {
        cout << argsP->getVersionP()->getMsg();
        return;
    }
    printInputDataInfo(
        argsP->getFileInP()->getFileLocation(),
        argsP->getColumnP()->getDataColTotal(),
        argsP->getRowP()->getDataRowTotal(),
        argsP->getFileInP()->getDataDlmType(),
        argsP->getTimestepP()->getDataTimestepIVP(),
        argsP->getColumnP()->getDataDoubleVSetP()
    );

    // cout.setf(ios_base::scientific);
    cout.precision(numeric_limits<double>::max_digits10);

    if (argsP->getCycleP() || argsP->getCalcP()) {
        if (!argsP->getFileOutP()) {
            printer(
                argsP->getRowP()->getRange(),
                argsP->getTimestepP()->isTimestepConsistent(),
                argsP->getTimestepP()->getRange(),
                argsP->getColumnP()->getDataDoubleColSet(),
                argsP->getCalcP()->getCalcIdSet(),
                argsP->getCycleP(),
                argsP->getCalcP()
            );
        }
        else {
            for (const string& fileOut : argsP->getFileOutP()->getFileLocSet()){
                filer(
                    fileOut, argsP->getFileInP()->getFileLocation(),
                    argsP->getRowP()->getRange(),
                    argsP->getTimestepP()->isTimestepConsistent(),
                    argsP->getTimestepP()->getRange(),
                    argsP->getColumnP()->getDataDoubleColSet(),
                    argsP->getCalcP()->getCalcIdSet(),
                    argsP->getCycleP(),
                    argsP->getCalcP()
                );
                cout<< "\nThe calculation data output has been written to \""
                    << fileOut << "\"" << endl;
            }
        }
        if (argsP->getCycleP() && !argsP->getCycleP()->getFileName().empty()) {
            cyclePeaksFiler(argsP->getCycleP());
            cout<< "\nThe cycle data output has been written to \""
                << argsP->getCycleP()->getFileName() << "\"" << endl;
        }
    }
    if (argsP->getFourierP()) {
        fourierCalc(argsP->getFourierP());
    }
    if (argsP->getPrintDataP()) {
        dataPrinter(
            argsP->getPrintDataP()->getDelimiter(),
            argsP->getRowP()->getRange(),
            argsP->getTimestepP()->isTimestepConsistent(),
            argsP->getTimestepP()->getDataTimestepIVP(),
            argsP->getColumnP()->getDataDoubleColSet()
        );
    }
    if (argsP->getFileDataP()) {
        dataFiler(
            argsP->getFileDataP()->getFileName(),
            argsP->getFileDataP()->getDelimiter(),
            argsP->getRowP()->getRange(),
            argsP->getTimestepP()->isTimestepConsistent(),
            argsP->getTimestepP()->getDataTimestepIVP(),
            argsP->getColumnP()->getDataDoubleColSet()
        );
        cout<< "\nThe file data output has been written to \""
            << argsP->getFileDataP()->getFileName() << "\"" << endl;
    }
    cout<<endl;
}

//----------------------------------------------------------------------------//
//******************** Printing information on loaded data *******************//
//----------------------------------------------------------------------------//
/*
 * Print the names of all the vector columns; first integers and then doubles.
 */
void Output::printInputDataInfo(const string& fileInName,
        const int dataColTotal, const size_t dataRowTotal,
        const Delimitation dataDlmType, const ColData::IntV* dataTimestepIVP,
        const vector<ColData::DoubleV*> dataDoubleVSetP) {
    cout<< left << '\n' << string(55, '=') << "\n "
        << "Input file: " << fileInName << '\n' << string(55, '=') << "\n\n"
        << setw(20) << " Total columns:" << dataColTotal << '\n'
        << setw(20) << " Total rows:" << dataRowTotal
        << '\n' << setw(20) << " Data delimitation:"
        << ((dataDlmType == Delimitation::delimited) ? "delimiter" :
            ((dataDlmType == Delimitation::spacedAndDelimited) ?
                "whitespace and delimiter" : "whitespace")) << '\n' << endl;

    if (dataTimestepIVP && get<0>(dataTimestepIVP->getTimestepRange())) {
        cout<< " Timestep column:\n" << string(30, '-') << '\n'
            << setw(3) << right << dataTimestepIVP->getColNo() << ". "
            << left << dataTimestepIVP->getColName() << '\n'
            << "\n Available timestep range is from "
            << get<1>(dataTimestepIVP->getTimestepRange()) << " to "
            << get<2>(dataTimestepIVP->getTimestepRange()) << ".\n\n";
    }
    else {
        cout<< " No consistent timestep column was found.\n\n";
    }
    cout<< " Data columns:\n"
        << string(30, '-') << "\n";
    for (DoubleV* dVP : dataDoubleVSetP) {
        cout<< setw(3) << right << dVP->getColNo() << ". "
            << left << dVP->getColName() << '\n';
    }
    cout<< '\n' << string(55, '=') << '\n';
}

//----------------------------------------------------------------------------//
//***************** Printing and filing calculation results ******************//
//----------------------------------------------------------------------------//
/*
 * Perform all the selected operations on all the selected columns and print the
 * results to the terminal.
 */
void Output::printer(
        const tuple<size_t, size_t> rowRange,
        const bool timestepConsistent,
        const tuple<size_t,size_t> timestepRange,
        const vector<int>& doubleColSet,
        const vector<CmdArgs::CalcId>& calcIdSet,
        const CmdArgs::Cycle* cycleP, const CmdArgs::Calc* calcP) {
    size_t rBgn, rEnd, tBgn, tEnd;
    tie(rBgn, rEnd) = rowRange;
    tie(tBgn, tEnd) = timestepRange;

    // Print heading
    cout<< " Calculation results\n";
    if (timestepConsistent) {
        cout<< "\n Timesteps         => "
            << to_string(tBgn) << " to " << to_string(tEnd);
    }
    cout<< "\n Rows              => "
        << to_string(rBgn) << " to " << to_string(rEnd);
    if (cycleP) {
        int cycleCount{cycleP->getInputCount()};
        ColData::CycleData cData{cycleP->getCalcCycleData()};
        DoubleV* cycleColDVP{DoubleV::getOnePFromCol(cycleP->getCycleColNo())};

        cout<< "\n Column for cycles => " << cycleColDVP->getColName()
            << "\n Center for cycles => " << cycleP->getCenter()
            << "\n Number of cycles  => " << cycleCount;
        if (cycleCount > 0 ) {
            cout<< '\n'
                << "\n Crests mean       => "   << cData.crestsMean
                << "\n Troughs mean      => "   << cData.troughsMean
                << '\n'
                << "\n Peaks maximum     => " << cData.peaks[0]
                << "\n Peaks mean        => "   << cData.peaksMean;
            if (cData.peaks.size()/3 > 0) {
                cout<< "\n 1/3rd peaks mean  => " << cData.peaksOneThirdMean;
                if (cData.peaks.size()/10 > 0) {
                    cout<< "\n 1/10th peaks mean => "<< cData.peaksOneTenthMean;
                }
            }
            if (cycleP->getFrequency() >= 0.0) {
                cout<< "\n\n Cycle frequency   => " << cycleP->getFrequency();
            }
        }
    }
    cout<< '\n' << string(55, '=') << endl;

    if (calcP) {
        if (cycleP && cycleP->getInputCount()==0) {
            throw logic_error(errorCycleInvalidForCalc);
        }
        for (const int colNo : doubleColSet) {
            // Print subheadings
            cout<< "\n " << DoubleV::getOnePFromCol(colNo)->getColName() << '\n'
                << string(55, '-') << '\n';

            // Print calculation results
            for (const CmdArgs::CalcId id : calcIdSet) {
                calcType calc{mapCalcIdToCalc<int>.at(id)};
                cout<< ' ' << left << setw(22)
                    << CalcFnc::mapCalcToStr<int>.at(calc)
                    << " = " << calc(colNo, rBgn, rEnd) << '\n';
            }
        }
        cout<< '\n' << string(55, '=') << endl;
    }
}

/*
 * Perform all the selected operations on all the selected columns and print the
 * results to the terminal.
 */
void Output::filer(const string& fileOutName,  const string& fileInName,
        const tuple<size_t, size_t> rowRange,
        const bool timestepConsistent,
        const tuple<size_t,size_t> timestepRange,
        const vector<int>& doubleColSet,
        const vector<CmdArgs::CalcId>& calcIdSet,
        const CmdArgs::Cycle* cycleP, const CmdArgs::Calc* calcP) {
    size_t rBgn, rEnd, tBgn, tEnd;
    tie(rBgn, rEnd) = rowRange;
    tie(tBgn, tEnd) = timestepRange;

    // Open file and set properties
    ofstream fOut;
    fOut.open(fileOutName, ios_base::app);
    if(!fOut) { throw runtime_error(errorOutputFile); }

    // File heading
    fOut<< "Input file: " << fileInName
        << "\nCalculation results";
    if (timestepConsistent) {
        fOut<< "\nTimesteps         => "
            << to_string(tBgn) << " to " << to_string(tEnd);
    }
    fOut<< "\nRows              => "
        << to_string(rBgn) << " to " << to_string(rEnd);
    if (cycleP) {
        int cycleCount{cycleP->getInputCount()};
        ColData::CycleData cData{cycleP->getCalcCycleData()};
        DoubleV* cycleColDVP{DoubleV::getOnePFromCol(cycleP->getCycleColNo())};

        fOut<< "\nColumn for cycles => " << cycleColDVP->getColName()
            << "\nCenter for cycles => " << cycleP->getCenter()
            << "\nNumber of cycles  => " << cycleCount;

        if (cycleCount > 0 ) {
            fOut<< '\n'
                << "\nCrests mean       => "   << cData.crestsMean
                << "\nTroughs mean      => "   << cData.troughsMean
                << '\n'
                << "\nPeaks maximum     => " << cData.peaks[0]
                << "\nPeaks mean        => "   << cData.peaksMean;
            if (cData.peaks.size()/3 > 0) {
                fOut<< "\n1/3rd peaks mean  => " << cData.peaksOneThirdMean;
                if (cData.peaks.size()/10 > 0) {
                    fOut<< "\n1/10th peaks mean => "<< cData.peaksOneTenthMean;
                }
            }
            if (cycleP->getFrequency() >= 0.0) {
                fOut<< "\n\nCycle frequency   => " << cycleP->getFrequency();
            }
        }
    }
    fOut<< '\n' << string(70, '`') << '\n';

    if (calcP) {
        if (cycleP && cycleP->getInputCount()==0) {
            throw logic_error(errorCycleInvalidForCalc);
        }
        fOut.setf(ios_base::scientific);
        fOut.precision(numeric_limits<double>::max_digits10);

        // File subheadings
        fOut << "Calculations\\Columns,";
        for (const int colNo : doubleColSet) {
            fOut<< DoubleV::getOnePFromCol(colNo)->getColName() << ',';
        }
        // File calculation results
        for (const CmdArgs::CalcId id : calcIdSet) {
            calcType calc{mapCalcIdToCalc<int>.at(id)};
            fOut << '\n' << CalcFnc::mapCalcToStr<int>.at(calc) << ',';
            for (const int colNo : doubleColSet) {
                fOut << calc(colNo, rBgn, rEnd) << ',';
            }
        }
        fOut<< '\n' << string(70, '`') << '\n';
    }
    fOut<< '\n' << string(70, '`') << '\n';
    fOut.close();
}

void Output::cyclePeaksFiler(const CmdArgs::Cycle* cycleP) {
    ColData::CycleData cData{cycleP->getCalcCycleData()};
    size_t peaksSize{cData.peaks.size()};
    size_t crestsAndTroughsSize{cData.crests.size()};
    ofstream fOut{cycleP->getFileName()};

    fOut<< "Peaks (sorted),Crests (unsorted),Troughs (unsorted)\n";
    fOut.precision(numeric_limits<double>::max_digits10);
    for (size_t r=0; r<peaksSize; ++r) {
        fOut<< cData.peaks[r] << ',';
        if (r<crestsAndTroughsSize) {
            fOut<< cData.crests[r] << ',' << cData.troughs[r];
        }
        fOut<< '\n';
    }
    fOut.close();
}

//----------------------------------------------------------------------------//
//******************* Filing Fast Fourier Transform results ******************//
//----------------------------------------------------------------------------//
/*
 * File the results of Fast Fourier Transform.
 */
#include "fftw3.h"

void Output::fourierCalc(const CmdArgs::Fourier* fourierP) {
    const size_t
        rowBgn{get<0>(fourierP->getRowRange())},
        rowEnd{get<1>(fourierP->getRowRange())},
        sampleFreq{rowEnd - rowBgn + 1},        // Fs
        signalLen{sampleFreq},                  // L
        outputLen{(signalLen/2)+1};
    DoubleV* fourierColDVP{DoubleV::getOnePFromCol(fourierP->getColNo())};
    vector<double> fourierColData{fourierColDVP->getData()};
    vector<std::complex<double>> fftData;
    vector<double> fftMag;
    fftData.reserve(signalLen);
    fftMag.reserve(outputLen);

    fftw_plan plan{
        fftw_plan_dft_1d(
            signalLen,
            reinterpret_cast<fftw_complex*>(&fftData.data()[0]),
            reinterpret_cast<fftw_complex*>(&fftData.data()[0]),
            FFTW_FORWARD,
            FFTW_ESTIMATE
        )
    };

    for (size_t r=0; r<signalLen; ++r) {
        using namespace std::complex_literals;
        fftData.emplace_back(0i);
        reinterpret_cast<double(&)[2]>(fftData[r])[0]
            = fourierColData[rowBgn+r];
    }

    fftw_execute(plan);

    double signalLenInv{1.0/signalLen};
    double outputLenInv{1.0/outputLen};
    // double outputLenInv{1.0/(signalLen/2)};  // Matches with Teclplot
    // Output the data
    for (size_t r=0; r<outputLen; ++r) {
        fftMag.emplace_back(2.*std::abs(fftData[r])*signalLenInv);
    }
    if (!fourierP->getFileName().empty()) {
        fourierFiler(fourierP->getFileName(), outputLen, outputLenInv,
            fftData, fftMag);
    }

    // Print partial results
    cout<< '\n' << " FFT partial results (sorted by magnitude)\n"
        << " Column for FFT => "
        << DoubleV::getOnePFromCol(fourierP->getColNo())->getColName()
        << '\n' << string(55, '=') << "\n\n "
        << setw(30) << "Frequency"
        << setw(30) << "Magnitude"
        // << setw(30) << "Phase"
        << "\n\n ";
    for (int n=0; n<fftValuesToPrint; ++n) {
        vector<double>::iterator maxValueIt{
            std::max_element(fftMag.begin(), fftMag.end())
        };
        std::ptrdiff_t maxIndex{std::distance(fftMag.begin(), maxValueIt)};

        cout<< setw(30) << static_cast<double>(maxIndex)*outputLenInv*100.
            << setw(30) << *maxValueIt
            // << setw(30) << std::arg(fftData[maxIndex])
            << "\n ";
        fftMag[maxIndex] = 0.0;
    }
    cout<< '\n' << string(55, '=') << '\n';
    if (!fourierP->getFileName().empty()) {
        cout<< "\nThe FFT data output has been written to \""
            << fourierP->getFileName() << "\"" << endl;
    }

    fftw_destroy_plan(plan);
}

void Output::fourierFiler(const string& fileOutName, const size_t outputLen,
        const double outputLenInv, const vector<std::complex<double>>& fftData,
        const vector<double>& fftMag) {
    ofstream fOut{fileOutName};
    fOut<< "Frequency,Magnitude,Phase\n";
    fOut.precision(numeric_limits<double>::max_digits10);
    for (size_t r=0; r<outputLen; ++r) {
        fOut<< static_cast<double>(r)*outputLenInv*100. << ','
            << fftMag[r] << ','
            << std::arg(fftData[r]) << '\n';
    }
    fOut.close();
}

//----------------------------------------------------------------------------//
//********************* Printing and filing loaded data **********************//
//----------------------------------------------------------------------------//
/*
 * Print the data of all the vector columns; first integers and then doubles.
 */
void Output::dataPrinter(const string& dlm,
        const tuple<size_t, size_t> rowRange,
        const bool timestepConsistent, const ColData::IntV* dataTimestepIVP,
        const vector<int>& doubleColSet) {
    // cout.setf(ios_base::scientific);
    cout.precision(numeric_limits<double>::max_digits10);

// for (const int colNo : doubleColSet) {
//             // Print subheadings
//             cout<< "\n " << DoubleV::getOnePFromCol(colNo)->getColName()

    // Print the header line
    cout << '\n';
    if (timestepConsistent) {
        cout << dataTimestepIVP->getColName() << dlm;
    }
    for (const int colNo : doubleColSet) {
        cout << DoubleV::getOnePFromCol(colNo)->getColName() << dlm;
    }
    cout << '\n';

    // Print the data
    auto [rowBgn, rowEnd] = rowRange;
    for (size_t row=rowBgn; row<=rowEnd; ++row) {
        if (timestepConsistent) {
            cout << dataTimestepIVP->getData()[row] << dlm;
        }
        for (const int colNo : doubleColSet) {
            cout << DoubleV::getOnePFromCol(colNo)->getData()[row] << dlm;
        }
        cout << '\n';
    }
    cout << flush;
}

/*
 * File the data of all the vector columns; first integers and then doubles.
 */
void Output::dataFiler(const string& fileName, const string& dlm,
        const tuple<size_t, size_t> rowRange,
        const bool timestepConsistent, const ColData::IntV* dataTimestepIVP,
        const vector<int>& doubleColSet) {
    ofstream fOut{fileName};
    // fOut.setf(ios_base::scientific);
    fOut.precision(numeric_limits<double>::max_digits10);

    // File the header line
    if (timestepConsistent) {
        fOut << dataTimestepIVP->getColName() << dlm;
    }
    for (const int colNo : doubleColSet) {
        fOut << DoubleV::getOnePFromCol(colNo)->getColName() << dlm;
    }
    fOut << '\n';

    // File the data
    auto [rowBgn, rowEnd] = rowRange;
    for (size_t row=rowBgn; row<=rowEnd; ++row) {
        if (timestepConsistent) {
            fOut << dataTimestepIVP->getData()[row] << dlm;
        }
        for (const int colNo : doubleColSet) {
            fOut << DoubleV::getOnePFromCol(colNo)->getData()[row] << dlm;
        }
        fOut << '\n';
    }
    fOut << flush;
    fOut.close();
}
