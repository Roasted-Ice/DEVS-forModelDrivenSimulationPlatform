/*************************************************************************
 * DEVS++: DEVS Open Source in C++                                       *
 * Copyright (C) 2005-2009  Moon Ho Hwang <moonho.hwang@gmail.com>       *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU Lesser General Public            *
 * License as published by the Free Software Foundation; either          *
 * version 2.1 of the License, or (at your option) any later version.    *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 * Lesser General Public License for more details.                       *
 *                                                                       *
 * You should have received a copy of the GNU Lesser General Public      *
 * License along with this library; if not, write to the Free Software   *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, *
 * USA.                                                                  *
 * This RGN.h file is copied from ADEVS (2004) version of Jim Nutaro     *
 * whose email is nutaro@ece.arizona.edu.                                *
**************************************************************************/
#pragma once
 
#include "DEVS_Export.h"
#include <stdlib.h>

namespace DEVSpp {
 
/**
The random_seq class is an abstract interface to a random sequence
generator.
*/
class DEVSpp_EXP random_seq 
{
	public:
		/// Set the seed for the random number generator
		virtual void set_seed(unsigned long seed) = 0;
		/// Get the next double uniformly distributed in [0, 1]
		virtual double next_dbl() = 0;
		/// Copy the random number generator
		virtual random_seq* copy() const = 0;
		/// Destructor
		virtual ~random_seq(){}
};

/**
The mtrand class is a random number generator based on a GNU implementation
of the Mersenne Twister (see RGN.cpp for more info).  Each copy
of the object has its own state, and so multiple copies
will produce independent random number streams.
*/
class DEVSpp_EXP mtrand: public random_seq 
{
	public:
		/// Create a generator with an unsigned long seed 
		mtrand(unsigned long seed = 1);
		/// Create a generator with an arbitrary length seed
		mtrand(const unsigned long* seed_array); 
		/// Copy constructor performs a deep copy
		mtrand(const mtrand& src);
		/// Assignment operator does a deep copy
		const mtrand& operator=(const mtrand& src);
		/// Set the seed
		void set_seed(unsigned long seed);
		/// Create a copy of this mtrand
		random_seq* copy() const;
		/// Get the next unsigned long value.
		unsigned long next_long();
		/// Get the next value in the range [0,1]
		double next_dbl();
		/// Destructor
		~mtrand ();
	private:	
		// Wrappers around the original function calls
		void sgenrand(unsigned long);
		void lsgenrand(const unsigned long*);
		unsigned long genrand();
		// The array for the state vector
		unsigned long* mt;
		int mti;
		unsigned long init_seed;
};

/**
The crand class provides random number sequences using the standard
C rand() function.  Since the underlying stream generator is rand(),
the state of the random number generator can not be captured with
a call to copy.  Also, each instance of the object samples the same
random number sequence.
*/
class crand: public random_seq 
{
	public:
		/// Create a generator with the default seed
		crand(){}
		/// Create a generator with the given seed
		crand(unsigned long seed) { srand (seed); }
		/// Set the seed for the random number generator
		void set_seed(unsigned long seed) { srand (seed); }
		/// Get the next double uniformly distributed in [0, 1]
		double next_dbl() { return (double)rand()/(double)RAND_MAX; } 
		/// Copy the random number generator
		random_seq* copy() const { return new crand (); }
		/// Destructor
		~crand(){}
};

/**
The rv class provides a random variable based on a selectable
implementation.  By default, this implementation is the Mersenne
Twister (see mtrand).
*/
class DEVSpp_EXP rv 
{
	public:
		/// Create a random variable with seed of the current time
		rv ();
		/// Create a random variable with the default implementation.
		rv (unsigned long seed);
		/**
		Create a random variable with the desired implementation.  The
		implementation class is adopted by the rv.
		*/
		rv(random_seq* rand);
		/// Copy constructor relies on copy method of underlying stream.
		rv(const rv& src);
		/// Assignment operator relies on copy method of underlying stream.
		const rv& operator=(const rv& src);
		/// See the random number generator implementation
		void set_seed(unsigned long seed);
		/// Sample a uniform distribution in the range [a, b]
		double uniform(double a, double b);
		/// Sample a triangular distribution with (a, b, c) where c: mode in [a, b]
		double triangular(double a, double b, double c);
		/// Sample an exponential distribution with mean m
		double exp(double m);
		/**
		Sample a normally distributed random variable with mean m and 
		standard deviation s.
		*/
		double normal(double m, double s);
		/// Destructor
		~rv();
	private:	
		random_seq* _impl;
};

}// end of namespace of DEVS
