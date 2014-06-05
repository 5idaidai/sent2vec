/*
	Command line parser

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2012-01-04

	Copyright 2010-2012 Steffen Rendle, see license.txt for more information
*/

#ifndef CMDLINE_H_
#define CMDLINE_H_

#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <string>
using namespace std;

typedef unsigned int uint;


std::vector<std::string> tokenize(const std::string& str, const std::string& delimiter);

class CMDParseError
{
    int errorid;

    public:
        CMDParseError(int errorid);
        CMDParseError( CMDParseError& myExp);
        //CMDParseError( CMDParseError myExp);
};

class CMDLine {
	protected:
		std::map< std::string, std::string > help;
		std::map< std::string, std::string > value;
		bool parse_name(std::string& s);

	public:
		std::string delimiter;

		CMDLine(int argc, char **argv);

		void setValue(std::string parameter, std::string value);

		bool hasParameter(std::string parameter);

		void print_help();

		const std::string& registerParameter(const std::string& parameter, const std::string& help);

		void checkParameters();

		const std::string& getValue(const std::string& parameter);

		const std::string& getValue(const std::string& parameter, const std::string& default_value);

		const double getValue(const std::string& parameter, const double& default_value);

		const int getValue(const std::string& parameter, const int& default_value);

		const uint getValue(const std::string& parameter, const uint& default_value);

        std::vector<int> getIntValues(const std::string& parameter);

		std::vector<std::string> getStrValues(const std::string& parameter);

		std::vector<double> getDblValues(const std::string& parameter);

		std::vector<uint> getUIntValues(const std::string& parameter);

};


#endif /*CMDLINE_H_*/
