/*
 * moda.h
 *
 *  Created on: 24 июля 2013 г.
 *      Author: klen
 */

#ifndef __MODA_H__
#define __MODA_H__


#include <vector>
#include <algorithm>

//---------------------------------------------------------------------------------------------
template <typename T> class TModaFilterContainer
{
        private:
        protected:
        public:

    std::vector<T> vals  ; // входная последовательность
    std::vector<T> gist  ; // копия входной последовательности для сортировки и поиска моды

    TModaFilterContainer () {} ;
    TModaFilterContainer (size_t size) { vals.resize(size) ; } ;
    ~TModaFilterContainer() {} ;
    void Add(T val) {vals.push_front(val); }
    void Remove ()  {vals.pop_back(); }
    void Clear() { vals.clear(); }
    void Resize(size_t size) {vals.resize(size); }
    void Update(T val) { vals.pop_back(); vals.insert(vals.begin(), val); }
};

//---------------------------------------------------------------------------
template <typename T> class TMedianFilterSort : public TModaFilterContainer<T>
{
  public:
  TMedianFilterSort():TModaFilterContainer<T>() {} ;
  TMedianFilterSort(size_t size):TModaFilterContainer<T>(size) {} ;
  ~TMedianFilterSort() {} ;

  T Find()
     {
         this->gist = this->vals ;
         div_t divresult;
         divresult = div( this->gist.size() , 2 ) ;
         size_t n = divresult.quot ;
         sort(this->gist.begin(), this->gist.end()) ;
         if ( divresult.rem )
           return this->vals[n] ;
         else
           return (this->vals[n-1] + this->vals[n]) / static_cast<T>(2) ;
     }

};
//--------------------------------------------------------------------------
template <typename T> class TMedianFilterNth : public TModaFilterContainer<T>
{
  public:
  TMedianFilterNth():TModaFilterContainer<T>() {} ;
  TMedianFilterNth(size_t size):TModaFilterContainer<T>(size) {} ;
  ~TMedianFilterNth() {} ;

  T Find()
     {
         this->gist = this->vals ;
         size_t n = this->gist.size() / 2 ;
         nth_element(this->gist.begin(), this->gist.begin() + n, this->gist.end()) ;
         return this->gist[n] ;
     }
};

// EXAMPLE
#if 0

#include "moda.h"

typedef TMedianFilterNth<int16_t> median_t ;
volatile int16_t mediana ;


median_t m(8) ;

mediana = m.Find() ;

m.Update(8) ;
mediana = m.Find() ;
m.Update(9) ;
mediana = m.Find() ;
m.Update(10) ;
mediana = m.Find() ;
m.Update(8) ;
mediana = m.Find() ;
m.Update(10) ;
mediana = m.Find() ;
m.Update(11) ;
mediana = m.Find() ;
m.Update(0) ;
mediana = m.Find() ;
m.Update(1) ;
mediana = m.Find() ;
m.Update(2) ;
mediana = m.Find() ;
m.Clear() ;

// результат работы:

0 0 0 0 0 0 0 0
mediana:0
8 0 0 0 0 0 0 0
mediana:0
9 8 0 0 0 0 0 0
mediana:0
10 9 8 0 0 0 0 0
mediana:0
8 10 9 8 0 0 0 0
mediana:8
10 8 10 9 8 0 0 0
mediana:8
11 10 8 10 9 8 0 0
mediana:9
0 11 10 8 10 9 8 0
mediana:9
1 0 11 10 8 10 9 8
mediana:9
2 1 0 11 10 8 10 9
mediana:9

#endif


#endif /* __MODA_H__ */
