/************************************************************************
 * DEVS++: DEVS Open Source in C++                                      *
 * Copyright (C) 2005-2006  Moon Ho Hwang <moonho.hwang@gmail.com>      * 
 *                                                                      *
 * This library is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU Lesser General Public           *
 * License as published by the Free Software Foundation; either         *
 * version 2.1 of the License, or (at your option) any later version.   *
 *                                                                      *
 * This library is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     *
 * Lesser General Public License for more details.                      *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,*  
 * USA.                                                                 * 
*************************************************************************/
#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <DEVSpp/RNG.h>
#include <string>

using namespace DEVSpp;
using namespace std;

ofstream Fout;
#define NO_SAMPLES 10000

void init_sample_freq(int min, int max, map<int, int>& sample_freq)
{
	sample_freq.clear();
	for(int i=min; i< max; i++)
		sample_freq.insert(make_pair(i, 0));
}

void file_output(int min, int max,const string& fname, map<int, int>& sample_freq)
{
	Fout.open(fname.data(), ios_base::out);

	for(int i=min; i<max; i++)
		Fout << i <<" " << sample_freq[i] << endl;
}

 
//------------------------------- uniform pdf ---------------------------------
void uniform_pdf()
{
	rv randomvar;
	map<int, int> sample_freq;
	init_sample_freq(0, 20, sample_freq);

	for(int i=0; i< NO_SAMPLES; i++){
		double rv = randomvar.uniform(0,20);
		cout << "uniform[0,20]("<<i<<")="<<rv << endl;
		sample_freq[(int)rv] = sample_freq[(int)rv]+1;
	}

	file_output(0, 20, "uniform.txt", sample_freq);
}

//------------------------------- triangular pdf --------------------------------
void triangular_pdf()
{
	rv randomvar;
	map<int, int> sample_freq;
	init_sample_freq(0, 20, sample_freq);

	for(int i=0; i< NO_SAMPLES; i++){
		double rv = randomvar.triangular(0,20,15);
		cout << "triangular(0,20,15)("<<i<<")="<< rv << endl;
		sample_freq[(int)rv] = sample_freq[(int)rv]+1;
	}

	file_output(0, 20, "triangular.txt", sample_freq);
}

void exponential_pdf()
{
	rv randomvar;
	map<int, int> sample_freq;
	init_sample_freq(0, 1000, sample_freq);

	for(int i=0; i< NO_SAMPLES; i++){
		double rv = randomvar.exp(10);
		cout << "exp[0.10]("<<i<<")="<< rv << endl;
		sample_freq[(int)rv] = sample_freq[(int)rv]+1;
	}
	file_output(0, 1000, "exponential.txt", sample_freq);
}


void normal_pdf()
{
	rv randomvar;
	map<int, int> sample_freq;
	init_sample_freq(-100, 100, sample_freq);

	for(int i=0; i< NO_SAMPLES; i++){
		double rv = randomvar.normal(0,4) ;
		cout << "normal[30,4]("<<i<<")="<< rv << endl;
		sample_freq[(int)ceil(rv)] = sample_freq[(int)ceil(rv)]+1;
	}
	file_output(-100, 100, "normal.txt", sample_freq);
}

void main(void)
{
	//cout << 1.0 -::exp(-5.0/4.0) << endl; 
	//cout << "ceil(-0.3)=" << ceil(-0.3) << endl;
	//cout << "(int)-0.3=" << (int)-0.3 << endl;

	std::string str;
	do{
		cout << "[u]niform, [t]riangle, [e]xponential, [n]ormal, e[x]it >";
		cin >> str;
		if(str =="u")
			uniform_pdf();
		else if(str == "t")
			triangular_pdf();
		else if(str == "e")
			exponential_pdf();
		else if(str == "n")
			normal_pdf();
	}while(str!="x");
}

